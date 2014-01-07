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

#ifndef UNITY_SCOPES_REPLY_H
#define UNITY_SCOPES_REPLY_H

#include <unity/scopes/ObjectProxy.h>
#include <unity/scopes/ReplyProxyFwd.h>
#include <unity/scopes/Category.h>
#include <unity/scopes/CategoryRenderer.h>

namespace unity
{

namespace scopes
{

namespace internal
{
class QueryObject;
class ReplyImpl;
}

class CategorisedResult;
class Annotation;

/**
\brief Reply allows the results of a query to be sent to the source of the query.
*/

class UNITY_API Reply : public virtual ObjectProxy
{
public:
    Reply(Reply const&) = default;

    /**
    \brief Create and register a new Category. The category is automatically sent to the source of the query.
    \return Category instance
    */
    Category::SCPtr register_category(std::string const& id,
                                      std::string const& title,
                                      std::string const &icon,
                                      CategoryRenderer const& renderer_template = CategoryRenderer());

    /**
    \brief Register an existing category instance and send it to the source of the query.
    The purpose of this call is to register a category obtained via ReplyBase::push(Category::SCPtr) when aggregating
    results and categories from other scope(s).
    */
    void register_category(Category::SCPtr category);

    /**
    \brief Returns an instance of previously registered category.
    \return Category instance or nullptr if category hasn't been registered.
    */
    Category::SCPtr lookup_category(std::string const& id) const;

    // TODO: document return value from push()
    /**
    \brief Sends a single result to the source of a query.
    Any calls to push() after finished() was called are ignored.
    \return The return value is true if the result was accepted, false otherwise.
    A false return value is due to either finished() having been called earlier,
    or the client that sent the query having cancelled that query.
    */
    bool push(CategorisedResult const& result) const;

    bool push(Annotation const& annotation) const;

    /**
    \brief Informs the source of a query that the query results are complete.
    Calling finished() informs the source of a query that the final result for the query
    was sent, that is, that the query is complete.
    The scope application code is responsible for calling finished() once it has sent the
    final result for a query.
    Multiple calls to finished() and calls to error() after finished() was called are ignored.
    The destructor implicitly calls finished() if a Reply goes out of scope without
    a prior call to finished().
    */
    void finished() const;

    /**
    \brief Informs the source of a query that the query was terminated due to an error.
    Multiple calls to error() and calls to finished() after error() was called are ignored.
    \param ex An exception_ptr indicating the cause of the error. If ex is a `std::exception`,
              the return value of `what()` is made available to the query source. Otherwise,
              the query source receives `"unknown exception"`.
    */
    void error(std::exception_ptr ex) const;

    /**
    \brief Destroys a Reply.
    If a Reply goes out of scope without a prior call to finished(), the destructor implicitly calls finished().
    */
    virtual ~Reply() noexcept;

protected:
    Reply(internal::ReplyImpl* impl);         // Instantiated only by ReplyImpl
    friend class internal::ReplyImpl;

private:
    internal::ReplyImpl* fwd() const;

    std::shared_ptr<internal::QueryObject> qo; // Points at the corresponding QueryObject, so we can
                                               // forward cancellation.
};

} // namespace scopes

} // namespace unity

#endif
