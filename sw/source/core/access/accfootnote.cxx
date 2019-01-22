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

#include <com/sun/star/accessibility/AccessibleRole.hpp>
#include <unotools/accessiblestatesethelper.hxx>
#include <comphelper/servicehelper.hxx>
#include <cppuhelper/supportsservice.hxx>
#include <vcl/svapp.hxx>
#include <ftnfrm.hxx>
#include <fmtftn.hxx>
#include <txtftn.hxx>
#include <viewsh.hxx>
#include <accmap.hxx>
#include "accfootnote.hxx"
#include <strings.hrc>

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::accessibility;

const sal_Char sImplementationNameFootnote[] = "com.sun.star.comp.Writer.SwAccessibleFootnoteView";
const sal_Char sImplementationNameEndnote[] = "com.sun.star.comp.Writer.SwAccessibleEndnoteView";

SwAccessibleFootnote::SwAccessibleFootnote(
        std::shared_ptr<SwAccessibleMap> const& pInitMap,
        bool bIsEndnote,
        const SwFootnoteFrame *pFootnoteFrame ) :
    SwAccessibleContext( pInitMap,
        bIsEndnote ? AccessibleRole::END_NOTE : AccessibleRole::FOOTNOTE,
        pFootnoteFrame )
{
    const char* pResId = bIsEndnote ? STR_ACCESS_ENDNOTE_NAME
                                   : STR_ACCESS_FOOTNOTE_NAME;

    OUString sArg;
    const SwTextFootnote *pTextFootnote =
        static_cast< const SwFootnoteFrame *>( GetFrame() )->GetAttr();
    if( pTextFootnote )
    {
        const SwDoc *pDoc = GetShell()->GetDoc();
        sArg = pTextFootnote->GetFootnote().GetViewNumStr( *pDoc );
    }

    SetName(GetResource(pResId, &sArg));
}

SwAccessibleFootnote::~SwAccessibleFootnote()
{
}

OUString SAL_CALL SwAccessibleFootnote::getAccessibleDescription()
{
    SolarMutexGuard aGuard;

    ThrowIfDisposed();

    const char* pResId = AccessibleRole::END_NOTE == GetRole()
        ? STR_ACCESS_ENDNOTE_DESC
        : STR_ACCESS_FOOTNOTE_DESC ;

    OUString sArg;
    const SwTextFootnote *pTextFootnote =
        static_cast< const SwFootnoteFrame *>( GetFrame() )->GetAttr();
    if( pTextFootnote )
    {
        const SwDoc *pDoc = GetMap()->GetShell()->GetDoc();
        sArg = pTextFootnote->GetFootnote().GetViewNumStr( *pDoc );
    }

    return GetResource(pResId, &sArg);
}

OUString SAL_CALL SwAccessibleFootnote::getImplementationName()
{
    if( AccessibleRole::END_NOTE == GetRole() )
        return OUString(sImplementationNameEndnote);
    else
        return OUString(sImplementationNameFootnote);
}

sal_Bool SAL_CALL SwAccessibleFootnote::supportsService(const OUString& sTestServiceName)
{
    return cppu::supportsService(this, sTestServiceName);
}

Sequence< OUString > SAL_CALL SwAccessibleFootnote::getSupportedServiceNames()
{
    Sequence< OUString > aRet(2);
    OUString* pArray = aRet.getArray();
    if( AccessibleRole::END_NOTE == GetRole() )
        pArray[0] = "com.sun.star.text.AccessibleEndnoteView";
    else
        pArray[0] = "com.sun.star.text.AccessibleFootnoteView";
    pArray[1] = sAccessibleServiceName;
    return aRet;
}

Sequence< sal_Int8 > SAL_CALL SwAccessibleFootnote::getImplementationId()
{
    return css::uno::Sequence<sal_Int8>();
}

bool SwAccessibleFootnote::IsEndnote( const SwFootnoteFrame *pFootnoteFrame )
{
    const SwTextFootnote *pTextFootnote = pFootnoteFrame ->GetAttr();
    return pTextFootnote && pTextFootnote->GetFootnote().IsEndNote() ;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
