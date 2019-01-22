/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

#ifndef INCLUDED_OOX_SOURCE_DRAWINGML_DIAGRAM_DATAMODELCONTEXT_HXX
#define INCLUDED_OOX_SOURCE_DRAWINGML_DIAGRAM_DATAMODELCONTEXT_HXX

#include <com/sun/star/drawing/XShapes.hpp>

#include <oox/core/contexthandler2.hxx>
#include "diagram.hxx"

namespace oox { namespace drawingml {

// CT_DataModel
class DataModelContext final : public ::oox::core::ContextHandler2
{
public:
    DataModelContext( ::oox::core::ContextHandler2Helper const & rParent, const DiagramDataPtr & pDataModelPtr );
    virtual ~DataModelContext() override;

    virtual ::oox::core::ContextHandlerRef onCreateContext( ::sal_Int32 Element, const ::oox::AttributeList& rAttribs ) override;

private:
    DiagramDataPtr mpDataModel;
};

} }

#endif // INCLUDED_OOX_SOURCE_DRAWINGML_DIAGRAM_DATAMODELCONTEXT_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
