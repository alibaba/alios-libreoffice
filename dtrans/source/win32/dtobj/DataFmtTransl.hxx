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

#ifndef INCLUDED_DTRANS_SOURCE_WIN32_DTOBJ_DATAFMTTRANSL_HXX
#define INCLUDED_DTRANS_SOURCE_WIN32_DTOBJ_DATAFMTTRANSL_HXX

#include <com/sun/star/datatransfer/XDataFormatTranslator.hpp>
#include <com/sun/star/datatransfer/XTransferable.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <sal/types.h>
#include <rtl/ustring.hxx>

#if defined _MSC_VER
#pragma warning(push,1)
#endif
#if !defined WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <objidl.h>
#if defined _MSC_VER
#pragma warning(pop)
#endif

// declaration

class CFormatEtc;

class CDataFormatTranslator
{
public:
    explicit CDataFormatTranslator( const css::uno::Reference< css::uno::XComponentContext >& rxContext );

    CFormatEtc getFormatEtcFromDataFlavor( const css::datatransfer::DataFlavor& aDataFlavor ) const;
    css::datatransfer::DataFlavor getDataFlavorFromFormatEtc(
        const FORMATETC& aFormatEtc, LCID lcid = GetThreadLocale( ) ) const;

    static CFormatEtc SAL_CALL getFormatEtcForClipformat( CLIPFORMAT cf );
    static CFormatEtc SAL_CALL getFormatEtcForClipformatName( const OUString& aClipFmtName );
    static OUString SAL_CALL getClipboardFormatName( CLIPFORMAT aClipformat );

    static bool SAL_CALL isHTMLFormat( CLIPFORMAT cf );
    static bool SAL_CALL isTextHtmlFormat( CLIPFORMAT cf );
    static bool SAL_CALL isOemOrAnsiTextFormat( CLIPFORMAT cf );
    static bool SAL_CALL isUnicodeTextFormat( CLIPFORMAT cf );
    static bool SAL_CALL isTextFormat( CLIPFORMAT cf );

private:
    static OUString SAL_CALL getTextCharsetFromLCID( LCID lcid, CLIPFORMAT aClipformat );

private:
    css::uno::Reference< css::datatransfer::XDataFormatTranslator >   m_XDataFormatTranslator;
};

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
