/*
 * Copyright (C) 2014 Canonical Ltd
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
 * Authored by: Marcus Tomlinson <marcus.tomlinson@canonical.com>
 */

#ifndef UNITY_SCOPES_INTERNAL_MWSUBSCRIBER_H
#define UNITY_SCOPES_INTERNAL_MWSUBSCRIBER_H

#include <unity/util/DefinesPtrs.h>
#include <unity/util/NonCopyable.h>

namespace unity
{

namespace scopes
{

namespace internal
{

typedef std::function<void(std::string const& message)> SubscriberCallback;

class MWSubscriber
{
public:
    NONCOPYABLE(MWSubscriber);
    UNITY_DEFINES_PTRS(MWSubscriber);

    virtual ~MWSubscriber();

    virtual std::string endpoint() const = 0;
    virtual void set_message_callback(SubscriberCallback callback) = 0;

protected:
    MWSubscriber();
};

} // namespace internal

} // namespace scopes

} // namespace unity

#endif // UNITY_SCOPES_INTERNAL_MWSUBSCRIBER_H