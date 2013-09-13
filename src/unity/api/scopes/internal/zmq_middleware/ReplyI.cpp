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

#include <unity/api/scopes/internal/zmq_middleware/ReplyI.h>

#include <unity/api/scopes/internal/zmq_middleware/capnproto/Reply.capnp.h>
#include <unity/api/scopes/internal/zmq_middleware/ObjectAdapter.h>
#include <unity/api/scopes/internal/zmq_middleware/ZmqReply.h>
#include <unity/api/scopes/internal/ReplyObject.h>

using namespace std;

namespace unity
{

namespace api
{

namespace scopes
{

namespace internal
{

namespace zmq_middleware
{

/*

interface Reply
{
    void push(string result);
    void finished();
};

*/

using namespace std::placeholders;

ReplyI::ReplyI(ReplyObject::SPtr const& ro) :
    ServantBase(ro, { { "push_", bind(&ReplyI::push_, this, _1, _2, _3) },
                      { "finished_", bind(&ReplyI::finished_, this, _1, _2, _3) } })
{
}

ReplyI::~ReplyI() noexcept
{
}

void ReplyI::push_(Current const&,
                   capnp::DynamicObject::Reader& in_params,
                   capnproto::Response::Builder&)
{
    auto req = in_params.as<capnproto::Reply::PushRequest>();
    auto result = req.getResult().cStr();
    auto delegate = dynamic_pointer_cast<ReplyObject>(del());
    delegate->push(result);
}

void ReplyI::finished_(Current const&,
                       capnp::DynamicObject::Reader&,
                       capnproto::Response::Builder&)
{
    auto delegate = dynamic_pointer_cast<ReplyObject>(del());
    delegate->finished();
}

} // namespace zmq_middleware

} // namespace internal

} // namespace scopes

} // namespace api

} // namespace unity
