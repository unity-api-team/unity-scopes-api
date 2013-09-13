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

#include <unity/api/scopes/internal/zmq_middleware/ZmqReceiver.h>

#include <cassert>
#include <cstdint>
#include <stdexcept>

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

ZmqReceiver::ZmqReceiver(zmqpp::socket& s) :
    s_(s)
{
}

// Receive a message (as a single message or in parts) and convert to a capnp segment list.

kj::ArrayPtr<kj::ArrayPtr<capnp::word const> const> ZmqReceiver::receive()
{
    // Clear previously received content, if any.
    parts_.clear();
    copied_parts_.clear();
    segments_.clear();

    do
    {
        parts_.push_back(string());
        string& str = parts_.back();
        s_.receive(str);

        if (str.size() % sizeof(capnp::word) != 0)      // Received message must contain an integral number of words.
        {
            throw std::runtime_error("ZmqReceiver::receive(): impossible message size (" + to_string(str.size()) + ")");
        }
        auto num_words = str.size() / sizeof(capnp::word);
        char* buf = &str[0];

        if (reinterpret_cast<uintptr_t>(buf) % sizeof(capnp::word) == 0)
        {
            // String buffer is word-aligned, point directly at the start of the string.
            segments_.push_back(kj::ArrayPtr<capnp::word const>(reinterpret_cast<capnp::word const*>(buf), num_words));
        }
        else
        {
            // String buffer is not word-aligned, make a copy and point at that.
            unique_ptr<capnp::word[]> words(new capnp::word[num_words]);
            memcpy(words.get(), buf, str.size());
            segments_.push_back(kj::ArrayPtr<capnp::word const>(&words[0], num_words));
            copied_parts_.push_back(move(words));
        }
    }
    while (s_.has_more_parts());

    return kj::ArrayPtr<kj::ArrayPtr<capnp::word const>>(&segments_[0], segments_.size());
}

} // namespace zmq_middleware

} // namespace internal

} // namespace scopes

} // namespace api

} // namespace unity
