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
#ifndef INCLUDED_XMLOFF_INC_XMLTEXTHEADERFOOTERCONTEXT_HXX
#define INCLUDED_XMLOFF_INC_XMLTEXTHEADERFOOTERCONTEXT_HXX


#include <xmloff/xmlictxt.hxx>

namespace com { namespace sun { namespace star {
    namespace text { class XTextCursor; }
    namespace beans { class XPropertySet; }
} } }

class XMLTextHeaderFooterContext: public SvXMLImportContext
{
    css::uno::Reference< css::text::XTextCursor > xOldTextCursor;
    css::uno::Reference< css::beans::XPropertySet > xPropSet;

    const OUString sOn;
    const OUString sShareContent;
    const OUString sShareContentFirst;
    const OUString sText;
    const OUString sTextFirst;
    const OUString sTextLeft;

    bool    bInsertContent : 1;
    bool    bLeft : 1;
    bool    bFirst : 1;

public:

    XMLTextHeaderFooterContext( SvXMLImport& rImport, sal_uInt16 nPrfx,
            const OUString& rLName,
            const css::uno::Reference< css::xml::sax::XAttributeList > & xAttrList,
            const css::uno::Reference< css::beans::XPropertySet > & rPageStylePropSet,
               bool bFooter, bool bLft, bool bFrst );

    virtual ~XMLTextHeaderFooterContext() override;

    virtual SvXMLImportContextRef CreateChildContext(
            sal_uInt16 nPrefix,
            const OUString& rLocalName,
            const css::uno::Reference< css::xml::sax::XAttributeList > & xAttrList ) override;

    virtual void EndElement() override;
};


#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
