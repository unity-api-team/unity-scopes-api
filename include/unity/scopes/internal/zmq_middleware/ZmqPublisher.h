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

#ifndef UNITY_SCOPES_INTERNAL_ZMQMIDDLEWARE_ZMQPUBLISHER_H
#define UNITY_SCOPES_INTERNAL_ZMQMIDDLEWARE_ZMQPUBLISHER_H

#include <unity/scopes/internal/MWPublisher.h>

#include <zmqpp/context.hpp>

#include <thread>

namespace unity
{

namespace scopes
{

namespace internal
{

namespace zmq_middleware
{

class ZmqPublisher : public virtual MWPublisher
{
public:
    ZmqPublisher(zmqpp::context const* context, std::string const& endpoint, std::string const& topic);
    virtual ~ZmqPublisher();

    void send_message(std::string const& message) override;

private:
    zmqpp::context const* const context_;
    std::string const endpoint_;
    std::string const topic_;
    std::thread thread_;

    void publisher_thread();
};

} // namespace zmq_middleware

} // namespace internal

} // namespace scopes

} // namespace unity

#endif // UNITY_SCOPES_INTERNAL_ZMQMIDDLEWARE_ZMQPUBLISHER_H
