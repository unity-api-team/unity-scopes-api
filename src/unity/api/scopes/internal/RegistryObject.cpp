/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Michi Henning <michi.henning@canonical.com>
 */

#include <unity/api/scopes/internal/RegistryObject.h>

#include <unity/api/scopes/ScopeExceptions.h>
#include <unity/UnityExceptions.h>

using namespace std;

namespace unity
{

namespace api
{

namespace scopes
{

namespace internal
{

RegistryObject::RegistryObject()
{
}

RegistryObject::~RegistryObject() noexcept
{
}

MWScopeProxy RegistryObject::find(std::string const& scope_name)
{
    // If the name is empty, it was sent as empty by the remote client.
    if (scope_name.empty())
    {
        throw unity::InvalidArgumentException("Registry: Cannot search for scope with empty name");
    }

    lock_guard<decltype(mutex_)> lock(mutex_);

    auto const& it = scopes_.find(scope_name);
    if (it == scopes_.end())
    {
        throw NotFoundException("Registry::find(): no such scope",  scope_name);
    }
    return it->second;
}

RegistryObject::MWScopeMap RegistryObject::list()
{
    lock_guard<decltype(mutex_)> lock(mutex_);
    return scopes_;
}

void RegistryObject::add(std::string const& scope_name, MWScopeProxy const& scope)
{
    // If the name is empty, it was sent as empty by the remote client.
    // TODO: also check for names containing a slash, because that won't work if we use
    //       the name for a socket in the file system.
    if (scope_name.empty())
    {
        throw unity::InvalidArgumentException("Registry: Cannot add scope with empty name");
    }

    lock_guard<decltype(mutex_)> lock(mutex_);

    auto const& pair = scopes_.insert(make_pair(scope_name, scope));
    if (!pair.second)
    {
        throw unity::LogicException("Registry: Scope \"" + scope_name + "\" is already in the registry");
    }
}

void RegistryObject::remove(std::string const& scope_name)
{
    // If the name is empty, it was sent as empty by the remote client.
    if (scope_name.empty())
    {
        throw unity::InvalidArgumentException("Registry: Cannot remove scope with empty name");
    }

    lock_guard<decltype(mutex_)> lock(mutex_);

    auto const& it = scopes_.find(scope_name);
    if (it == scopes_.end())
    {
        throw unity::LogicException("Registry: Scope \"" + scope_name + "\" is not in the registry");
    }
    scopes_.erase(it);
}

} // namespace internal

} // namespace scopes

} // namespace api

} // namespace unity
