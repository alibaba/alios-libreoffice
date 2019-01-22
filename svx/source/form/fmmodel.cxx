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


#include <fmundo.hxx>
#include <fmdocumentclassification.hxx>
#include <fmcontrollayout.hxx>

#include <svx/fmmodel.hxx>
#include <svx/fmpage.hxx>
#include <svx/svdobj.hxx>

#include <sfx2/objsh.hxx>

#include <boost/optional.hpp>

using ::com::sun::star::uno::Reference;
using ::com::sun::star::container::XNameContainer;
using namespace svxform;


struct FmFormModelImplData
{
    rtl::Reference<FmXUndoEnvironment>  mxUndoEnv;
    bool                bOpenInDesignIsDefaulted;
    ::boost::optional< sal_Bool >
                            aControlsUseRefDevice;

    FmFormModelImplData()
        :bOpenInDesignIsDefaulted( true )
        ,aControlsUseRefDevice()
    {
    }
};

FmFormModel::FmFormModel(SfxItemPool* pPool, SfxObjectShell* pPers)
    : SdrModel(pPool, pPers)
    , m_pImpl(nullptr)
    , m_pObjShell(nullptr)
    , m_bOpenInDesignMode(false)
    , m_bAutoControlFocus(false)
{
    m_pImpl.reset( new FmFormModelImplData );
    m_pImpl->mxUndoEnv = new FmXUndoEnvironment(*this);
}

FmFormModel::FmFormModel(const OUString& rPath, SfxItemPool* pPool, SfxObjectShell* pPers)
    : SdrModel(rPath, pPool, pPers, false)
    , m_pImpl(nullptr)
    , m_pObjShell(nullptr)
    , m_bOpenInDesignMode(false)
    , m_bAutoControlFocus(false)
{
    m_pImpl.reset( new FmFormModelImplData );
    m_pImpl->mxUndoEnv = new FmXUndoEnvironment(*this);
}

FmFormModel::FmFormModel(const OUString& rPath, SfxItemPool* pPool, SfxObjectShell* pPers,
                         bool bUseExtColorTable)
    : SdrModel(rPath, pPool, pPers, bUseExtColorTable)
    , m_pImpl(nullptr)
    , m_pObjShell(nullptr)
    , m_bOpenInDesignMode(false)
    , m_bAutoControlFocus(false)
{
    m_pImpl.reset( new FmFormModelImplData );
    m_pImpl->mxUndoEnv = new FmXUndoEnvironment(*this);
}

FmFormModel::~FmFormModel()
{
    if (m_pObjShell && m_pImpl->mxUndoEnv->IsListening(*m_pObjShell))
        SetObjectShell(nullptr);

    ClearUndoBuffer();
    // minimum limit for undos
    SetMaxUndoActionCount(1);
}

SdrPage* FmFormModel::AllocPage(bool bMasterPage)
{
    return new FmFormPage(*this, bMasterPage);
}

void FmFormModel::InsertPage(SdrPage* pPage, sal_uInt16 nPos)
{
    // hack solange method intern
    if (m_pObjShell && !m_pImpl->mxUndoEnv->IsListening( *m_pObjShell ))
        SetObjectShell(m_pObjShell);

    SdrModel::InsertPage( pPage, nPos );
}

SdrPage* FmFormModel::RemovePage(sal_uInt16 nPgNum)
{
    FmFormPage* pToBeRemovedPage = dynamic_cast< FmFormPage* >( GetPage( nPgNum ) );
    OSL_ENSURE( pToBeRemovedPage, "FmFormModel::RemovePage: *which page*?" );

    if ( pToBeRemovedPage )
    {
        Reference< XNameContainer > xForms( pToBeRemovedPage->GetForms( false ), css::uno::UNO_QUERY );
        if ( xForms.is() )
            m_pImpl->mxUndoEnv->RemoveForms( xForms );
    }

    FmFormPage* pRemovedPage = static_cast<FmFormPage*>(SdrModel::RemovePage(nPgNum));
    OSL_ENSURE( pRemovedPage == pToBeRemovedPage, "FmFormModel::RemovePage: inconsistency!" );
    return pRemovedPage;
}

void FmFormModel::InsertMasterPage(SdrPage* pPage, sal_uInt16 nPos)
{
    // hack solange method intern
    if (m_pObjShell && !m_pImpl->mxUndoEnv->IsListening( *m_pObjShell ))
        SetObjectShell(m_pObjShell);

    SdrModel::InsertMasterPage(pPage, nPos);
}

SdrPage* FmFormModel::RemoveMasterPage(sal_uInt16 nPgNum)
{
    FmFormPage* pPage = static_cast<FmFormPage*>(SdrModel::RemoveMasterPage(nPgNum));

    if ( pPage )
    {
        Reference< XNameContainer > xForms( pPage->GetForms( false ), css::uno::UNO_QUERY );
        if ( xForms.is() )
            m_pImpl->mxUndoEnv->RemoveForms( xForms );
    }

    return pPage;
}


void FmFormModel::implSetOpenInDesignMode( bool _bOpenDesignMode )
{
    if( _bOpenDesignMode != m_bOpenInDesignMode )
    {
        m_bOpenInDesignMode = _bOpenDesignMode;

        if ( m_pObjShell )
            m_pObjShell->SetModified();
    }
    // no matter if we really did it or not - from now on, it does not count as defaulted anymore
    m_pImpl->bOpenInDesignIsDefaulted = false;
}


void FmFormModel::SetOpenInDesignMode( bool bOpenDesignMode )
{
    implSetOpenInDesignMode( bOpenDesignMode );
}


bool FmFormModel::OpenInDesignModeIsDefaulted( )
{
    return m_pImpl->bOpenInDesignIsDefaulted;
}


bool FmFormModel::ControlsUseRefDevice() const
{
    if ( !m_pImpl->aControlsUseRefDevice )
    {
        DocumentType eDocType = eUnknownDocumentType;
        if ( m_pObjShell )
            eDocType = DocumentClassification::classifyHostDocument( m_pObjShell->GetModel() );
        m_pImpl->aControlsUseRefDevice.reset( ControlLayouter::useDocumentReferenceDevice( eDocType ) );
    }
    return *m_pImpl->aControlsUseRefDevice;
}


void FmFormModel::SetAutoControlFocus( bool _bAutoControlFocus )
{
    if( _bAutoControlFocus != m_bAutoControlFocus )
    {
        m_bAutoControlFocus = _bAutoControlFocus;
        m_pObjShell->SetModified();
    }
}


void FmFormModel::SetObjectShell( SfxObjectShell* pShell )
{
    if (pShell == m_pObjShell)
        return;

    if (m_pObjShell)
    {
        m_pImpl->mxUndoEnv->EndListening( *this );
        m_pImpl->mxUndoEnv->EndListening( *m_pObjShell );
    }

    m_pObjShell = pShell;

    if (m_pObjShell)
    {
        m_pImpl->mxUndoEnv->SetReadOnly( m_pObjShell->IsReadOnly() || m_pObjShell->IsReadOnlyUI(), FmXUndoEnvironment::Accessor() );

        if (!m_pImpl->mxUndoEnv->IsReadOnly())
             m_pImpl->mxUndoEnv->StartListening(*this);

        m_pImpl->mxUndoEnv->StartListening( *m_pObjShell );
    }
}


FmXUndoEnvironment& FmFormModel::GetUndoEnv()
{
    return *m_pImpl->mxUndoEnv;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
