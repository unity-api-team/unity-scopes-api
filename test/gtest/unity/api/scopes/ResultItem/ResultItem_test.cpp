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
 * Authored by: Pawel Stolowski <pawel.stolowski@canonical.com>
 */

#include <scopes/ResultItem.h>
#include <scopes/Category.h>
#include <scopes/internal/ResultItemImpl.h>
#include <gtest/gtest.h>

using namespace unity::api::scopes;
using namespace unity::api::scopes::internal;

// basic test of ResultIem setters and getters
TEST(ResultItem, basic)
{
    auto cat = std::shared_ptr<Category>(new Category("1"));
    ResultItem result(cat);
    result.set_uri("http://ubuntu.com");
    result.set_title("a title");
    result.set_icon("an icon");
    result.set_dnd_uri("http://canonical.com");
    result.add_metadata("foo", Variant("bar"));

    EXPECT_EQ("http://ubuntu.com", result.uri());
    EXPECT_EQ("a title", result.title());
    EXPECT_EQ("an icon", result.icon());
    EXPECT_EQ("http://canonical.com", result.dnd_uri());
    EXPECT_EQ("bar", (*result.variant_map())["foo"].get_string());
}

// test conversion to VariantMap
TEST(ResultItem, variant_map)
{
    auto cat = std::shared_ptr<Category>(new Category("1"));
    ResultItem result(cat);
    result.set_uri("http://ubuntu.com");
    result.set_title("a title");
    result.set_icon("an icon");
    result.set_dnd_uri("http://canonical.com");

    EXPECT_EQ("http://ubuntu.com", result.uri());
    EXPECT_EQ("a title", result.title());
    EXPECT_EQ("an icon", result.icon());
    EXPECT_EQ("http://canonical.com", result.dnd_uri());

    auto var = result.variant_map();
    EXPECT_EQ("http://ubuntu.com", (*var)["uri"].get_string());
    EXPECT_EQ("a title", (*var)["title"].get_string());
    EXPECT_EQ("an icon", (*var)["icon"].get_string());
    EXPECT_EQ("http://canonical.com", (*var)["dnd_uri"].get_string());
    EXPECT_EQ("1", (*var)["cat_id"].get_string());
}

// test conversion to VariantMap
TEST(ResultItem, from_variant)
{
    VariantMap vm;
    vm["uri"] = "http://ubuntu.com";
    vm["dnd_uri"] = "http://canonical.com";
    vm["title"] = "a title";
    vm["icon"] = "an icon";

    auto cat = std::shared_ptr<Category>(new Category("1"));
    ResultItem result(cat, vm);

    EXPECT_EQ("http://ubuntu.com", result.uri());
    EXPECT_EQ("a title", result.title());
    EXPECT_EQ("an icon", result.icon());
    EXPECT_EQ("http://canonical.com", result.dnd_uri());
}
