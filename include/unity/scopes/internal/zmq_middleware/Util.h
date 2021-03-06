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

#pragma once

#include <zmqpp/socket.hpp>

#include <string>

namespace unity
{

namespace scopes
{

namespace internal
{

namespace zmq_middleware
{

void throw_if_bad_endpoint(std::string const& endpoint);

void safe_bind(zmqpp::socket& s, std::string const& endpoint);

} // namespace zmq_middleware

} // namespace internal

} // namespace scopes

} // namespace unity
