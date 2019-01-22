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
#include <rtl/ustring.hxx>
#include <rtl/uuid.h>
#include <framework/xmlsignaturetemplateimpl.hxx>

using namespace ::com::sun::star::uno ;
using ::com::sun::star::lang::XMultiServiceFactory ;
using ::com::sun::star::lang::XSingleServiceFactory ;

using ::com::sun::star::xml::wrapper::XXMLElementWrapper ;
using ::com::sun::star::xml::crypto::XXMLSignatureTemplate ;

XMLSignatureTemplateImpl::XMLSignatureTemplateImpl()
    :m_xTemplate( nullptr ),
     m_nStatus ( css::xml::crypto::SecurityOperationStatus_UNKNOWN )
{
}

XMLSignatureTemplateImpl::~XMLSignatureTemplateImpl() {
}

/* XXMLSignatureTemplate */
void SAL_CALL XMLSignatureTemplateImpl::setTemplate( const Reference< XXMLElementWrapper >& aTemplate )
{
    m_xTemplate = aTemplate ;
}

/* XXMLSignatureTemplate */
Reference< XXMLElementWrapper > SAL_CALL XMLSignatureTemplateImpl::getTemplate()
{
    return m_xTemplate ;
}

void SAL_CALL XMLSignatureTemplateImpl::setTarget( const css::uno::Reference< css::xml::wrapper::XXMLElementWrapper >& aXmlElement )
{
    targets.push_back( aXmlElement );
}

css::uno::Sequence< css::uno::Reference< css::xml::wrapper::XXMLElementWrapper > > SAL_CALL XMLSignatureTemplateImpl::getTargets()
{
    sal_Int32 length = targets.size();
    css::uno::Sequence< css::uno::Reference< css::xml::wrapper::XXMLElementWrapper > > aTargets (length);

    sal_Int32 i;

    for (i=0; i<length; i++)
    {
        aTargets[i] = targets[i];
    }

    return aTargets;
}

void SAL_CALL XMLSignatureTemplateImpl::setBinding(
    const css::uno::Reference< css::xml::crypto::XUriBinding >& aUriBinding )
{
    m_xUriBinding = aUriBinding;
}

css::uno::Reference< css::xml::crypto::XUriBinding > SAL_CALL XMLSignatureTemplateImpl::getBinding()
{
    return m_xUriBinding;
}

void SAL_CALL XMLSignatureTemplateImpl::setStatus(
    css::xml::crypto::SecurityOperationStatus status )
{
    m_nStatus = status;
}

css::xml::crypto::SecurityOperationStatus SAL_CALL XMLSignatureTemplateImpl::getStatus(  )
{
    return m_nStatus;
}

/* XServiceInfo */
OUString SAL_CALL XMLSignatureTemplateImpl::getImplementationName() {
    return impl_getImplementationName() ;
}

/* XServiceInfo */
sal_Bool SAL_CALL XMLSignatureTemplateImpl::supportsService( const OUString& serviceName) {
    Sequence< OUString > seqServiceNames = getSupportedServiceNames() ;
    const OUString* pArray = seqServiceNames.getConstArray() ;
    for( sal_Int32 i = 0 ; i < seqServiceNames.getLength() ; i ++ ) {
        if( *( pArray + i ) == serviceName )
            return true ;
    }
    return false ;
}

/* XServiceInfo */
Sequence< OUString > SAL_CALL XMLSignatureTemplateImpl::getSupportedServiceNames() {
    return impl_getSupportedServiceNames() ;
}

//Helper for XServiceInfo
Sequence< OUString > XMLSignatureTemplateImpl::impl_getSupportedServiceNames() {
    ::osl::Guard< ::osl::Mutex > aGuard( ::osl::Mutex::getGlobalMutex() ) ;
    Sequence<OUString> seqServiceNames { "com.sun.star.xml.crypto.XMLSignatureTemplate" };
    return seqServiceNames ;
}

OUString XMLSignatureTemplateImpl::impl_getImplementationName() {
    return OUString("com.sun.star.xml.security.framework.XMLSignatureTemplateImpl") ;
}

//Helper for registry
Reference< XInterface > SAL_CALL XMLSignatureTemplateImpl::impl_createInstance( const Reference< XMultiServiceFactory >&  ) {
    return Reference< XInterface >( *new XMLSignatureTemplateImpl ) ;
}
/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
