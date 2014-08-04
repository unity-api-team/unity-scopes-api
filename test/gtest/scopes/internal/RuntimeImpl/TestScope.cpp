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
 * Authored by: Michi Henning <michi.henning@canonical.com>
 */

#include "TestScope.h"

using namespace std;
using namespace unity::scopes;

void TestScope::start(string const&)
{
}

SearchQueryBase::UPtr TestScope::search(CannedQuery const&, SearchMetadata const&)
{
    // Never called
    abort();
    return nullptr;
}

PreviewQueryBase::UPtr TestScope::preview(Result const&, ActionMetadata const&)
{
    // Never called
    abort();
    return nullptr;
}