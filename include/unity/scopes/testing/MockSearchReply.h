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
 * Authored by: Thomas Voß <thomas.voss@canonical.com>
 */

#pragma once

#include <unity/scopes/CategorisedResult.h>
#include <unity/scopes/SearchReply.h>
#include <unity/scopes/Department.h>

#include <unity/scopes/testing/MockObject.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wctor-dtor-privacy"
#include <gmock/gmock.h>
#pragma GCC diagnostic pop

namespace unity
{

namespace scopes
{

namespace testing
{

/**
 \brief Mock for unity::scopes::SearchReply class.
 */
class MockSearchReply : public unity::scopes::SearchReply, public virtual MockObject
{
public:
/// @cond
    MockSearchReply() = default;

    // From Reply
    MOCK_METHOD0(finished, void());
    MOCK_METHOD1(error, void(std::exception_ptr));
    MOCK_METHOD1(info, void(OperationInfo const&));

    // From SearchReply
    MOCK_METHOD1(register_departments, void(Department::SCPtr const&));
    MOCK_METHOD4(register_category,
                 Category::SCPtr(std::string const&,
                                 std::string const&,
                                 std::string const&,
                                 CategoryRenderer const&));
    MOCK_METHOD5(register_category,
                 Category::SCPtr(std::string const&,
                                 std::string const&,
                                 std::string const&,
                                 CannedQuery const&,
                                 CategoryRenderer const&));
    MOCK_METHOD1(register_category, void(Category::SCPtr category));
    MOCK_METHOD1(lookup_category, Category::SCPtr(std::string const&));
    MOCK_METHOD1(push, bool(CategorisedResult const&));
    MOCK_METHOD2(push, bool(Filters const&, FilterState const&));
    MOCK_METHOD1(push, bool(Filters const&));
    MOCK_METHOD1(push, bool(experimental::Annotation const& annotation));
    MOCK_METHOD0(push_surfacing_results_from_cache, void());

/// @endcond
};


} // namespace testing

} // namespace scopes

} // namespace unity
