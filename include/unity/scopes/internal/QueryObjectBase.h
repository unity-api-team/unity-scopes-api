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

#ifndef UNITY_SCOPES_INTERNAL_QUERYOBJECTBASE_H
#define UNITY_SCOPES_INTERNAL_QUERYOBJECTBASE_H

#include <unity/scopes/internal/AbstractObject.h>
#include <unity/scopes/internal/MWReplyProxyFwd.h>

#include <atomic>
#include <mutex>

namespace unity
{

namespace scopes
{

namespace internal
{

class QueryObjectBase : public AbstractObject
{
public:
    UNITY_DEFINES_PTRS(QueryObjectBase);

    virtual void run(MWReplyProxy const& reply) noexcept = 0;
    virtual void cancel() = 0;
};

} // namespace internal

} // namespace scopes

} // namespace unity

#endif
