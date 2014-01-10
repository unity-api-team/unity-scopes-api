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

#include <scopes/ListenerBase.h>

#include <cassert>
#include <unordered_map>

using namespace std;

namespace unity
{

namespace api
{

namespace scopes
{

//! @cond

ListenerBase::ListenerBase()
{
}

ListenerBase::~ListenerBase() noexcept
{
}

SearchListener::SearchListener()
{
}

SearchListener::~SearchListener() noexcept
{
}

void SearchListener::push(Category::SCPtr /* category */)
{
    // Intentionally empty: "do nothing" default implementation.
}

void SearchListener::push(Annotation /* annotation */)
{
    // Intentionally empty: "do nothing" default implementation.
}

PreviewListener::PreviewListener()
{
}

PreviewListener::~PreviewListener() noexcept
{
}

ActivationListener::ActivationListener()
{
}

ActivationListener::~ActivationListener()
{
}

void ActivationListener::activation_response(ActivationResponse const& /* response */)
{
    // Intentionally empty: "do nothing" default implementation.
}
    
void ActivationListener::finished(Reason /* r */, std::string const& /* error_message */)
{
    // Intentionally empty: "do nothing" default implementation.
}

// Possibly overkill, but safer than using the enum as the index into an array,
// in case the enumeration is ever added to or the enumerators get re-ordered.

static unordered_map<int, char const*> const reasons =
{
    pair<int, char const*>(static_cast<int>(ListenerBase::Finished),  "finished"),
    pair<int, char const*>(static_cast<int>(ListenerBase::Cancelled), "cancelled"),
    pair<int, char const*>(static_cast<int>(ListenerBase::Error),     "error")
};

char const* to_string(ListenerBase::Reason reason)
{
    assert(reasons.find(static_cast<int>(reason)) != reasons.end());
    return reasons.find(static_cast<int>(reason))->second;
}

//! @endcond

} // namespace scopes

} // namespace api

} // namespace unity
