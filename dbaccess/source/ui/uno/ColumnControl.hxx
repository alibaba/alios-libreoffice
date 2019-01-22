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
#ifndef INCLUDED_DBACCESS_SOURCE_UI_UNO_COLUMNCONTROL_HXX
#define INCLUDED_DBACCESS_SOURCE_UI_UNO_COLUMNCONTROL_HXX

#include <toolkit/controls/unocontrol.hxx>
#include <apitools.hxx>

namespace com { namespace sun { namespace star { namespace uno {
    class XComponentContext;
} } } }

namespace dbaui
{
    class OColumnControl :  public UnoControl
    {
    private:
        css::uno::Reference< css::uno::XComponentContext> m_xContext;
    public:
        explicit OColumnControl(const css::uno::Reference< css::uno::XComponentContext>& rxContext);

        // UnoControl
        virtual OUString GetComponentServiceName() override;

        // XServiceInfo
        DECLARE_SERVICE_INFO();
        /// @throws css::uno::RuntimeException
        static OUString SAL_CALL getImplementationName_Static(  );
        /// @throws css::uno::RuntimeException
        static css::uno::Sequence< OUString > SAL_CALL getSupportedServiceNames_Static(  );
        static css::uno::Reference< css::uno::XInterface >
        SAL_CALL Create(const css::uno::Reference< css::lang::XMultiServiceFactory >&);

        // css::awt::XControl
        virtual void SAL_CALL createPeer(const css::uno::Reference< css::awt::XToolkit >& _rToolkit, const css::uno::Reference< css::awt::XWindowPeer >& Parent) override;
    };
}   // namespace dbaui

#endif // INCLUDED_DBACCESS_SOURCE_UI_UNO_COLUMNCONTROL_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
