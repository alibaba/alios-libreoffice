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

#ifndef INCLUDED_XMLSECURITY_SOURCE_XMLSEC_MSCRYPT_SANEXTENSION_MSCRYPTIMPL_HXX
#define INCLUDED_XMLSECURITY_SOURCE_XMLSEC_MSCRYPT_SANEXTENSION_MSCRYPTIMPL_HXX

#ifdef _MSC_VER
#pragma warning(push,1)
#endif
#if !defined WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <WinCrypt.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif
#include <sal/config.h>
#include <rtl/ustring.hxx>
#include <cppuhelper/factory.hxx>
#include <cppuhelper/implbase.hxx>
#include <com/sun/star/uno/Exception.hpp>
#include <com/sun/star/uno/SecurityException.hpp>
#include <com/sun/star/security/XCertificateExtension.hpp>
#include <com/sun/star/security/XSanExtension.hpp>
#include <com/sun/star/security/CertAltNameEntry.hpp>

class SanExtensionImpl : public ::cppu::WeakImplHelper<
    css::security::XSanExtension >
{
    private:
        bool m_critical ;
        css::uno::Sequence< sal_Int8 > m_xExtnId ;
        css::uno::Sequence< sal_Int8 > m_xExtnValue ;

        css::uno::Sequence< css::security::CertAltNameEntry > m_Entries;

    public:
        SanExtensionImpl() ;
        virtual ~SanExtensionImpl() override;

        //Methods from XCertificateExtension
        virtual sal_Bool SAL_CALL isCritical() override;

        virtual css::uno::Sequence< sal_Int8 > SAL_CALL getExtensionId() override;

        virtual css::uno::Sequence< sal_Int8 > SAL_CALL getExtensionValue() override;

        //Methods from XSanExtension

        virtual css::uno::Sequence< css::security::CertAltNameEntry > SAL_CALL getAlternativeNames() override;

        //Helper method
        void setCertExtn( unsigned char* value, unsigned int vlen, unsigned char* id, unsigned int idlen, bool critical ) ;
} ;

#endif // INCLUDED_XMLSECURITY_SOURCE_XMLSEC_MSCRYPT_SANEXTENSION_MSCRYPTIMPL_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
