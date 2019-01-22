/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
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
#ifndef INCLUDED_XMLSECURITY_SOURCE_XMLSEC_MSCRYPT_AKMNGR_HXX
#define INCLUDED_XMLSECURITY_SOURCE_XMLSEC_MSCRYPT_AKMNGR_HXX

#if !defined WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <wincrypt.h>

#include <xmlsec/xmlsec.h>
#include <xmlsec/keys.h>
#include <xmlsec/transforms.h>

xmlSecKeysMngrPtr xmlSecMSCryptoAppliedKeysMngrCreate();

int
xmlSecMSCryptoAppliedKeysMngrAdoptKeyStore(
    xmlSecKeysMngrPtr    mngr,
    HCERTSTORE keyStore
) ;

int
xmlSecMSCryptoAppliedKeysMngrAdoptTrustedStore(
    xmlSecKeysMngrPtr    mngr,
    HCERTSTORE trustedStore
) ;

int
xmlSecMSCryptoAppliedKeysMngrAdoptUntrustedStore(
    xmlSecKeysMngrPtr    mngr,
    HCERTSTORE untrustedStore
) ;

#endif // INCLUDED_XMLSECURITY_SOURCE_XMLSEC_MSCRYPT_AKMNGR_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
