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

#ifndef INCLUDED_SD_SOURCE_UI_INC_FUHHCONV_HXX
#define INCLUDED_SD_SOURCE_UI_INC_FUHHCONV_HXX

#include "fupoor.hxx"

class SdOutliner;

namespace sd {

class FuHangulHanjaConversion final : public FuPoor
{
public:

    static rtl::Reference<FuPoor> Create( ViewShell* pViewSh, ::sd::Window* pWin, ::sd::View* pView, SdDrawDocument* pDoc, SfxRequest& rReq );

    void StartConversion( LanguageType nSourceLanguage,  LanguageType nTargetLanguage,
                          const vcl::Font *pTargetFont, sal_Int32 nOptions, bool bIsInteractive );

    void StartChineseConversion();

    void ConvertStyles( LanguageType nTargetLanguage, const vcl::Font *pTargetFont );

private:
    virtual ~FuHangulHanjaConversion() override;

    SdOutliner*     pSdOutliner;
    bool            bOwnOutliner;

    FuHangulHanjaConversion (
        ViewShell* pViewSh,
        ::sd::Window* pWin,
        ::sd::View* pView,
        SdDrawDocument* pDoc,
        SfxRequest& rReq );

};

#endif // INCLUDED_SD_SOURCE_UI_INC_FUHHCONV_HXX

} // end of namespace sd

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
