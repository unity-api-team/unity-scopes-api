/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Michi Henning <michi.henning@canonical.com>
 */

#include <unity/scopes/internal/Reaper.h>

#include <unity/UnityExceptions.h>

#include <cassert>
#include <sstream>

using namespace std;
using namespace unity::scopes::internal::reaper_private;

namespace unity
{

namespace scopes
{

namespace internal
{

mutex ReapItem::mutex_;

ReapItem::ReapItem(weak_ptr<Reaper> const& reaper, reaper_private::Reaplist::iterator it) :
    reaper_(reaper),
    it_(it),
    destroyed_(false)
{
}

ReapItem::~ReapItem() noexcept
{
    // If we go out of scope, we remove ourselves from the reaper list. This
    // ensures that no more callbacks will be sent.
    destroy();
}

void ReapItem::refresh() noexcept
{
    if (destroyed_.load())
    {
        return; // Calling refresh() after destroy() has no effect.
    }

    // If the reaper is still around, remove the entry from the list
    // and put it back on the front, updating the time stamp.
    weak_ptr<Reaper> wp_reaper;
    {
        lock_guard<mutex> lock(mutex_);
        wp_reaper = reaper_;
    }
    auto const reaper = wp_reaper.lock();
    if (reaper)
    {
        lock_guard<mutex> reaper_lock(reaper->mutex_);
        assert(it_ != reaper->list_.end());
        reaper_private::Item item(*it_);
        item.timestamp = chrono::steady_clock::now();
        reaper->list_.erase(it_);
        reaper->list_.push_front(item);
        it_ = reaper->list_.begin();
    }
    else
    {
        // The reaper has gone away, so we disable ourself.
        destroyed_.store(true);
    }
}

void ReapItem::destroy() noexcept
{
    if (destroyed_.exchange(true))  // Only the first call to destroy has any effect.
    {
        return;
    }

    weak_ptr<Reaper> wp_reaper;
    {
        lock_guard<mutex> lock(mutex_);
        wp_reaper = reaper_;
    }
    auto const reaper = wp_reaper.lock();
    if (reaper)
    {
        lock_guard<mutex> reaper_lock(reaper->mutex_);
        assert(it_ != reaper->list_.end());
        reaper->list_.erase(it_);
    }

#ifndef NDEBUG
    it_ = reaper->list_.end();
#endif
}

Reaper::Reaper(int reap_interval, int expiry_interval, DestroyPolicy p) :
    reap_interval_(chrono::seconds(reap_interval)),
    expiry_interval_(chrono::seconds(expiry_interval)),
    policy_(p),
    finish_(false)
{
    if (reap_interval < 1)
    {
        ostringstream s;
        s << "Reaper: invalid reap_interval (" << reap_interval << "). Interval must be > 0.";
        throw unity::InvalidArgumentException(s.str());
    }
    if (reap_interval > expiry_interval)
    {
        ostringstream s;
        s << "Reaper: reap_interval (" << reap_interval << ") must be <= expiry_interval (" << expiry_interval << ").";
        throw unity::LogicException(s.str());
    }
}

Reaper::~Reaper() noexcept
{
    destroy();
}

// Instantiate a new reaper. We call set_self() after instantiation so the reaper
// can keep a weak_ptr to itself. That weak_ptr in turn is passed to each ReapItem,
// so the ReapItem can manipulate the reap list. If the reaper goes out of scope
// before a ReapItem, the ReapItem will notice this and deactivate itself.

Reaper::SPtr Reaper::create(int reap_interval, int expiry_interval, DestroyPolicy p)
{
    SPtr reaper(new Reaper(reap_interval, expiry_interval, p));
    reaper->set_self();
    reaper->start();
    return reaper;
}

void Reaper::set_self() noexcept
{
    self_ = shared_from_this();
}

void Reaper::start()
{
    reap_thread_ = thread(&Reaper::reap_func, this);
}

void Reaper::destroy()
{
    // Let the reaper thread know that it needs to stop doing things
    {
        lock_guard<mutex> lock(mutex_);
        if (finish_)
        {
            return;
        }
        finish_ = true;
        do_work_.notify_one();
    }
    reap_thread_.join();
}

// Add a new entry to the reaper. If the entry is not refreshed within the expiry interval,
// the reaper removes the item from the list and calls cb to let the caller know about the expiry.

ReapItem::SPtr Reaper::add(ReaperCallback const& cb)
{
    if (!cb)
    {
        throw unity::InvalidArgumentException("Reaper: invalid null callback passed to add().");
    }

    lock_guard<mutex> lock(mutex_);

    if (finish_)
    {
        throw unity::LogicException("Reaper: cannot add item to destroyed reaper.");
    }

    // Put new Item at the head of the list.
    reaper_private::Reaplist::iterator ri;
    Item item(cb);
    list_.push_front(item); // LRU order
    ri = list_.begin();
    if (list_.size() == 1)
    {
        do_work_.notify_one();  // Wake up reaper thread
    }

    // Make a new ReapItem.
    assert(self_.lock());
    ReapItem::SPtr reap_item(new ReapItem(self_, ri));
    // Now that the ReapItem is created, we can set the disable callback
    ri->disable_reap_item = [reap_item] { reap_item->destroy(); };
    return reap_item;
}

size_t Reaper::size() const noexcept
{
    lock_guard<mutex> lock(mutex_);
    return list_.size();
}

// Reaper thread

void Reaper::reap_func()
{
    unique_lock<mutex> lock(mutex_);
    for (;;)
    {
        if (list_.empty())
        {
            // If no items are in the list, we wait until there is at least one item
            // in the list or we are told to finish. (While there is nothing
            // to reap, there is no point in waking up periodically only to find the list empty.)
            do_work_.wait(lock, [this] { return !list_.empty() || finish_; });
        }
        else
        {
            // There is at least one item on the list, we wait with a timeout.
            // The first-to-expire item is at the tail of the list. We sleep at least long enough
            // for that item to get a chance to expire. (There is no point in waking up earlier.)
            // But, if have just done a scan, we sleep for at least reap_interval_, so there is
            // at most one pass every reap_interval_.
            auto const now = chrono::steady_clock::now();
            auto const oldest_item_age = chrono::duration_cast<chrono::milliseconds>(now - list_.back().timestamp);
            auto const reap_interval = chrono::duration_cast<chrono::milliseconds>(reap_interval_);
            auto const sleep_interval = max(expiry_interval_ - oldest_item_age, reap_interval);
            auto const wakeup_time = now + sleep_interval;
            do_work_.wait_until(lock, wakeup_time, [this]{ return finish_; });
        }

        if (finish_ && policy_ == NoCallbackOnDestroy)
        {
            // If we are told to finish (which happens when the Reaper instance is destroyed),
            // if NoCallbackOnDestroy is set, we are done.
            return;
        }

        // We run along the list from the tail towards the head. For any entry on the list
        // that is too old, we copy it to a zombie list. We use a strictly less comparison for
        // the timestamp, so if the entry is exactly expiry_interval_ old, it will be reaped.
        reaper_private::Reaplist zombies;
        if (finish_ && policy_ == CallbackOnDestroy)
        {
            // Final pass for CallbackOnDestroy. We simply call back on everything.
            zombies.assign(list_.begin(), list_.end());
        }
        else
        {
            // finish_ may or may not be set here. If it is set, we still do one final
            // reaping pass before returning below so, if the reaper is destroyed
            // while there are still entries on it, expired entries have their callbacks
            // invoked at destruction time.
            auto const now = chrono::steady_clock::now();
            for (auto it = list_.rbegin(); it != list_.rend(); ++it)
            {
                if (now < it->timestamp + expiry_interval_)
                {
                    break;  // LRU order. Once we find an entry that's not expired, we can stop looking.
                }
                zombies.push_back(*it);
            }
        }

        // Callbacks are made outside the synchronization, so we can't deadlock if a
        // a callback invokes a method on the reaper or a ReapItem.
        lock.unlock();
        remove_zombies(zombies);    // noexcept
        lock.lock();

        if (finish_)
        {
            return;
        }
    }
}

void Reaper::remove_zombies(reaper_private::Reaplist const& zombies) noexcept
{
    for (auto item : zombies)
    {
        assert(item.disable_reap_item);
        item.disable_reap_item();           // Calls destroy() on the ReapItem, so it removes itself from list_

        assert(item.cb);
        try
        {
            item.cb();                      // Informs the caller that the item timed out.
        }
        catch (...)
        {
            // Ignore exceptions raised by the application's callback function.
        }
    }
}

} // namespace internal

} // namespace scopes

} // namespace unity
