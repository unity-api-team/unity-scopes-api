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

#ifndef UNITY_SCOPES_INTERNAL_REAPER_H
#define UNITY_SCOPES_INTERNAL_REAPER_H

#include <unity/util/DefinesPtrs.h>
#include <unity/util/NonCopyable.h>

#include <atomic>
#include <condition_variable>
#include <list>
#include <mutex>
#include <thread>

namespace unity
{

namespace scopes
{

namespace internal
{

typedef std::function<void()> ReaperCallback;

class ReapItem;

namespace reaper_private
{

struct Item final
{
    Item(ReaperCallback cb) :
        cb(cb),
        timestamp(std::chrono::steady_clock::now())
    {
    }

    Item(Item const&) = default;
    Item& operator=(Item const&) = default;

    ReaperCallback cb;                               // Called if timeout expires (application-supplied callback)
    std::chrono::steady_clock::time_point timestamp; // Last add() or refresh()
    std::weak_ptr<ReapItem> reap_item;               // Points back at owning ReapItem
};

typedef std::list<Item> Reaplist;

} // namespace reaper_private

class Reaper;

// Simple refresh class returned from Reaper::add().
// refresh() renews the timestamp of an entry.
// destroy() removes the entry from the reaper's list *without* invoking the callback.
// Calls to refresh() or destroy() after destroy() do nothing.
//
// It is safe to continue to call methods on a ReapItem even its reaper has gone out of scope.
// In that case, refresh() and destroy() do nothing.
//
// Letting a ReapItem go out of scope automatically calls destroy(), meaning that the item's callback
// is *not* invoked when a ReapItem is destroyed.

class ReapItem final
{
public:
    NONCOPYABLE(ReapItem);
    UNITY_DEFINES_PTRS(ReapItem);

    void refresh() noexcept; // Update time stamp on item to keep it alive. O(1) performance.
    void destroy() noexcept; // Removes this item from the reaper *without* invoking the callback. O(1) performance.

    ~ReapItem();

private:
    ReapItem(std::weak_ptr<Reaper> const& reaper,
             reaper_private::Reaplist::iterator it);    // Only Reaper can instantiate

    std::weak_ptr<Reaper> reaper_;                      // The reaper this item belongs with
    static std::mutex mutex_;                           // Protects weak_ptr
    reaper_private::Reaplist::iterator it_;             // Position in reap list
    std::atomic_bool destroyed_;

    friend class Reaper;
};

// Simple reaper class. It maintains a list of items in LRU order. The caller adds items to the
// list by calling add(), which returns a ReapItem. If the caller calls refresh() on the returned
// ReapItem within the expiry interval, the item remains in the list. If no refresh() was sent
// for the item within the expiry interval, the reaper removes the item and calls the callback
// function that was passed to add(). This lets the caller know that the item expired.
//
// It is safe to let a reaper go out of scope while there are still ReapItems for it. The methods
// on the ReapItem do nothing if they are called after the reaper is gone.

class Reaper final : public std::enable_shared_from_this<Reaper>
{
public:
    NONCOPYABLE(Reaper);
    UNITY_DEFINES_PTRS(Reaper);

    ~Reaper();

    enum DestroyPolicy { NoCallbackOnDestroy, CallbackOnDestroy };

    // Creates a new reaper. Intervals are in seconds. By default, if the reaper is destroyed while
    // there are entries in its list, the reaper will *not* invoke the callback from the destructor.
    // If CallbackOnDestroy is set, the destructor will call the callback of all entries that are
    // still on the list, whether they have expired or not.
    //
    // The reaper thread runs (at most) once every reap_interval seconds. Entries are reaped once
    // they are at least expiry_interval seconds old. Actual life time is in the range
    // [expiry_interval, expiry_interval + reap_interval].
    // For example, a reap interval of 1 second and expiry interval of 5 seconds means that the
    // callback for an entry will be made once the entry has not been refreshed for at least 5 seconds
    // and at most 6 seconds.
    // The reap interval must be less than or equal to the expiry interval. Otherwise, we would allow
    // entries to not be refreshed for longer than their expiry interval, then be refreshed again, and
    // not be reaped on the next pass.
    //
    // Reaping passes are O(m) complexity, where m is the number of expired items (not
    // the total number of items).
    static SPtr create(int reap_interval, int expiry_interval, DestroyPolicy p = NoCallbackOnDestroy);

    // Destroys the reaper and returns once any remaining items have been reaped (depending on the
    // destroy policy). The destructor implicitly calls destroy().
    void destroy();

    // Adds a new item to the reaper. The reaper calls cb once the item has not been refreshed for at
    // least expiry_interval seconds. O(1) performance.
    // The callback passed to add() must not block for any length of time. We have only one reaper
    // thread, which invokes the callback synchronously. (Invoking the callback asynchronously would be
    // too expensive because, in the worst case, we would use as many threads as there are expired
    // entries.)
    ReapItem::SPtr add(ReaperCallback const& cb);

    // Returns the number of items in the reaper list.
    // O(1) performance.
    size_t size() const noexcept;

private:
    Reaper(int reap_interval, int expiry_interval, DestroyPolicy p);
    void set_self() noexcept;
    void start();

    void reap_func();                       // Start function for reaper thread

    void remove_zombies(reaper_private::Reaplist const&) noexcept;   // Invokes callbacks for expired entries

    std::weak_ptr<Reaper> self_;            // We keep a weak reference to ourselves, to pass to each ReapItem.
    std::chrono::seconds reap_interval_;    // How frequently we look for entries to reap
    std::chrono::seconds expiry_interval_;  // How long before an entry times out
    DestroyPolicy policy_;                  // Whether to invoke callback on entries still present when reaper is destroyed
    reaper_private::Reaplist list_;         // Items in LRU order, most recently refreshed one at the front.

    mutable std::mutex mutex_;              // Protects list_. Also used by ReapItem to serialize updates to list_.

    std::thread reap_thread_;               // Reaper thread scans list_ and issues callbacks for timed-out entries
    std::condition_variable do_work_;       // Reaper thread waits on this
    bool finish_;                           // Set when reaper thread needs to terminate

    friend class ReapItem;
};

} // namespace internal

} // namespace scopes

} // namespace unity

#endif
