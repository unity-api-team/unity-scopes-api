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
 * Authored by: Pawel Stolowski <pawel.stolowski@canonical.com>
 */

#include <unity/scopes/SearchQueryBase.h>
#include <unity/scopes/PreviewQueryBase.h>
#include <unity/scopes/ScopeBase.h>
#include <unity/scopes/PreviewWidget.h>
#include <unity/scopes/SearchReply.h>

#define EXPORT __attribute__ ((visibility ("default")))

using namespace std;
using namespace unity::scopes;

class MyQuery : public SearchQueryBase
{
public:
    MyQuery(CannedQuery const& query) :
        query_(query)
    {
    }

    virtual void cancelled() override
    {
    }

    virtual void run(SearchReplyProxy const&) override
    {
    }

private:
    CannedQuery query_;
};

class MyPreview : public PreviewQueryBase
{
public:
    virtual void cancelled() override
    {
    }

    virtual void run(PreviewReplyProxy const&) override
    {
    }
};

class MyScope : public ScopeBase
{
public:
    virtual int start(string const&, RegistryProxy const&) override
    {
        return VERSION;
    }

    virtual void stop() override {}

    virtual SearchQueryBase::UPtr search(CannedQuery const& q, SearchMetadata const&) override
    {
        SearchQueryBase::UPtr query(new MyQuery(q));
        return query;
    }

    virtual PreviewQueryBase::UPtr preview(Result const&, ActionMetadata const&) override
    {
        PreviewQueryBase::UPtr preview(new MyPreview());
        return preview;
    }
};

extern "C"
{

    EXPORT
    unity::scopes::ScopeBase*
    // cppcheck-suppress unusedFunction
    UNITY_SCOPE_CREATE_FUNCTION()
    {
        return new MyScope;
    }

    EXPORT
    void
    // cppcheck-suppress unusedFunction
    UNITY_SCOPE_DESTROY_FUNCTION(unity::scopes::ScopeBase* scope_base)
    {
        delete scope_base;
    }
}