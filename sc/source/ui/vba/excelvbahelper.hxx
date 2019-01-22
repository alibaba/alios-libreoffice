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
#ifndef INCLUDED_SC_SOURCE_UI_VBA_EXCELVBAHELPER_HXX
#define INCLUDED_SC_SOURCE_UI_VBA_EXCELVBAHELPER_HXX

#include <vbahelper/vbahelper.hxx>
#include <docsh.hxx>
#include <com/sun/star/sheet/XDatabaseRange.hpp>
#include <com/sun/star/sheet/XUnnamedDatabaseRanges.hpp>
#include <com/sun/star/table/XCellRange.hpp>
#include <com/sun/star/sheet/XSheetCellRangeContainer.hpp>
#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <ooo/vba/XHelperInterface.hpp>

class ScCellRangesBase;

namespace ooo {
namespace vba {
namespace excel {

// nTabs empty means apply zoom to all sheets
void implSetZoom( const css::uno::Reference< css::frame::XModel >& xModel, sal_Int16 nZoom, std::vector< SCTAB >& nTabs );
void implnCopy( const css::uno::Reference< css::frame::XModel>& xModel );
void implnPaste ( const css::uno::Reference< css::frame::XModel>& xModel );
void implnCut( const css::uno::Reference< css::frame::XModel>& xModel );
void implnPasteSpecial( const css::uno::Reference< css::frame::XModel>& xModel, InsertDeleteFlags nFlags, ScPasteFunc nFunction, bool bSkipEmpty, bool bTranspose);
ScTabViewShell* getBestViewShell( const css::uno::Reference< css::frame::XModel>& xModel ) ;
ScDocShell* getDocShell( const css::uno::Reference< css::frame::XModel>& xModel ) ;
ScTabViewShell* getCurrentBestViewShell( const css::uno::Reference< css::uno::XComponentContext >& xContext );
SfxViewFrame* getViewFrame( const css::uno::Reference< css::frame::XModel >& xModel );

/// @throws css::uno::RuntimeException
css::uno::Reference< css::sheet::XUnnamedDatabaseRanges > GetUnnamedDataBaseRanges( const ScDocShell* pShell );

/// @throws css::uno::RuntimeException
css::uno::Reference< css::sheet::XDatabaseRange > GetAutoFiltRange( const ScDocShell* pShell, sal_Int16 nSheet );
/// @throws css::uno::RuntimeException
css::uno::Reference< ooo::vba::XHelperInterface > getUnoSheetModuleObj( const css::uno::Reference< css::sheet::XSpreadsheet >& xSheet );
/// @throws css::uno::RuntimeException
css::uno::Reference< ooo::vba::XHelperInterface > getUnoSheetModuleObj( const css::uno::Reference< css::sheet::XSheetCellRangeContainer >& xRanges );
/// @throws css::uno::RuntimeException
css::uno::Reference< ooo::vba::XHelperInterface > getUnoSheetModuleObj( const css::uno::Reference< css::table::XCellRange >& xRange );
/// @throws css::uno::RuntimeException
css::uno::Reference< ooo::vba::XHelperInterface > getUnoSheetModuleObj( const css::uno::Reference< css::table::XCell >& xCell );
/// @throws css::uno::RuntimeException
css::uno::Reference< ooo::vba::XHelperInterface > getUnoSheetModuleObj( const css::uno::Reference< css::frame::XModel >& xModel, SCTAB nTab );

/// @throws css::uno::RuntimeException
ScDocShell* GetDocShellFromRange( const css::uno::Reference< css::uno::XInterface >& xRange );
void setUpDocumentModules( const css::uno::Reference< css::sheet::XSpreadsheetDocument >& xDoc );

class ScVbaCellRangeAccess
{
public:
    static SfxItemSet* GetDataSet( ScCellRangesBase* pRangeObj );
};

// Extracts a implementation object ( via XUnoTunnel ) from an uno object.
// Will throw if unsuccessful.
/// @throws css::uno::RuntimeException
template < typename ImplObject >
    ImplObject* getImplFromDocModuleWrapper( const css::uno::Reference< css::uno::XInterface >& rxWrapperIf )
    {
        ImplObject* pObj = nullptr;
        css::uno::Reference< css::lang::XUnoTunnel >  xTunnel( rxWrapperIf, css::uno::UNO_QUERY );
        if ( xTunnel.is() )
            pObj = reinterpret_cast<ImplObject*>( xTunnel->getSomething(ImplObject::getUnoTunnelId()));
        if ( !pObj )
            throw css::uno::RuntimeException("Internal error, can't extract implementation object", rxWrapperIf );
        return pObj;
    }

} // namespace excel
} // namespace vba
} // namespace ooo

#endif

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
