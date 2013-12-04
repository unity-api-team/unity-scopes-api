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

#ifndef UNITY_API_SCOPES_ANNOTATIONOBJECTBASE_H
#define UNITY_API_SCOPES_ANNOTATIONOBJECTBASE_H

#include <unity/SymbolExport.h>
#include <scopes/Variant.h>
#include <scopes/Query.h>
#include <memory>

namespace unity
{

namespace api
{

namespace scopes
{

class PlacementHint;

namespace internal
{
class AnnotationObjectBaseImpl;
}

class UNITY_API AnnotationObjectBase
{
public:
    virtual Query canned_query() const = 0;
    PlacementHint placement_hint() const;
    virtual ~AnnotationObjectBase();
    VariantMap serialize() const;

private:
    AnnotationObjectBase(internal::AnnotationObjectBaseImpl* pimpl);
    std::shared_ptr<internal::AnnotationObjectBaseImpl> p;

    friend class Hyperlink;
};

} // namespace scopes

} // namespace api

} // namespace unity

#endif
