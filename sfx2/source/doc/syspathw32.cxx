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

#include <sal/config.h>
#include <sal/types.h>
#include <o3tl/char16_t2wchar_t.hxx>

#ifdef _WIN32
#ifdef _MSC_VER
#pragma warning(disable:4917)
#endif

#undef WB_LEFT
#undef WB_RIGHT

#include <shlobj.h>

#include "syspathw32.hxx"

static bool SHGetSpecialFolderW32( int nFolderID, WCHAR* pszFolder, int nSize )
{
    LPITEMIDLIST    pidl;
    HRESULT         hHdl = SHGetSpecialFolderLocation( nullptr, nFolderID, &pidl );

    if( hHdl == NOERROR )
    {
        WCHAR *lpFolder = static_cast< WCHAR* >( HeapAlloc( GetProcessHeap(), 0, 16000 ));

        SHGetPathFromIDListW( pidl, lpFolder );
        wcsncpy( pszFolder, lpFolder, nSize );

        HeapFree( GetProcessHeap(), 0, lpFolder );
        IMalloc *pMalloc;
        if( NOERROR == SHGetMalloc(&pMalloc) )
        {
            pMalloc->Free( pidl );
            pMalloc->Release();
        }
    }
    return true;
}

#endif

bool GetUserTemplateLocation(sal_Unicode* pFolder, int nSize)
{
#ifdef _WIN32
    return SHGetSpecialFolderW32( CSIDL_TEMPLATES, o3tl::toW(pFolder), nSize );
#else
    (void)pFolder;
    (void)nSize;
    return false;
#endif
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
