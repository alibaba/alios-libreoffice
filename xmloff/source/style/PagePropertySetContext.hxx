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

#ifndef INCLUDED_XMLOFF_SOURCE_STYLE_PAGEPROPERTYSETCONTEXT_HXX
#define INCLUDED_XMLOFF_SOURCE_STYLE_PAGEPROPERTYSETCONTEXT_HXX

#include <xmloff/xmlprcon.hxx>

enum PageContextType
{
    Page,
    Header,
    Footer
};

class PagePropertySetContext : public SvXMLPropertySetContext
{
    PageContextType aType;

public:
    PagePropertySetContext( SvXMLImport& rImport, sal_uInt16 nPrfx,
                const OUString& rLName,
                 const css::uno::Reference< css::xml::sax::XAttributeList >& xAttrList,
                 sal_uInt32 nFam,
                 ::std::vector< XMLPropertyState > &rProps,
                 const rtl::Reference < SvXMLImportPropertyMapper > &rMap,
                 sal_Int32 nStartIndex, sal_Int32 nEndIndex,
                 const PageContextType aType );

    virtual ~PagePropertySetContext() override;

    using SvXMLPropertySetContext::CreateChildContext;
    virtual SvXMLImportContextRef CreateChildContext( sal_uInt16 nPrefix,
                                   const OUString& rLocalName,
                                   const css::uno::Reference< css::xml::sax::XAttributeList >& xAttrList,
                                   ::std::vector< XMLPropertyState > &rProperties,
                                   const XMLPropertyState& rProp) override;
};

#endif // INCLUDED_XMLOFF_SOURCE_STYLE_PAGEPROPERTYSETCONTEXT_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
