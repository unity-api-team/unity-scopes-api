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

#include <unity/api/scopes/internal/ice_middleware/IceObjectProxy.h>

using namespace std;

namespace unity
{

namespace api
{

namespace scopes
{

namespace internal
{

namespace ice_middleware
{

IceObjectProxy::IceObjectProxy(IceMiddleware* mw_base, Ice::ObjectPrx const& p) noexcept :
    MWObjectProxy(mw_base),
    proxy_(p)
{
    assert(p);
}

IceObjectProxy::~IceObjectProxy() noexcept
{
}

IceMiddleware* IceObjectProxy::mw_base() const noexcept
{
    return dynamic_cast<IceMiddleware*>(MWObjectProxy::mw_base());
}

Ice::ObjectPrx IceObjectProxy::proxy() const noexcept
{
    return proxy_;
}

} // namespace ice_middleware

} // namespace internal

} // namespace scopes

} // namespace api

} // namespace unity
