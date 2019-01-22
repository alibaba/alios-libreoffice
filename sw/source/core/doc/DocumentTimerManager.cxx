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
#include <DocumentTimerManager.hxx>

#include <doc.hxx>
#include <DocumentSettingManager.hxx>
#include <IDocumentFieldsAccess.hxx>
#include <IDocumentLayoutAccess.hxx>
#include <rootfrm.hxx>
#include <viewsh.hxx>
#include <unotools/lingucfg.hxx>
#include <unotools/linguprops.hxx>
#include <set>
#include <fldupde.hxx>
#include <sfx2/progress.hxx>
#include <viewopt.hxx>
#include <docsh.hxx>
#include <docfld.hxx>
#include <fldbas.hxx>

namespace sw
{

DocumentTimerManager::DocumentTimerManager( SwDoc& i_rSwdoc ) : m_rDoc( i_rSwdoc ),
                                                                mbStartIdleTimer( false ),
                                                                mIdleBlockCount( 0 ),
                                                                maDocIdle( i_rSwdoc )
{
    maDocIdle.SetPriority( TaskPriority::LOWEST );
    maDocIdle.SetInvokeHandler( LINK( this, DocumentTimerManager, DoIdleJobs) );
    maDocIdle.SetDebugName( "sw::DocumentTimerManager maDocIdle" );
}

void DocumentTimerManager::StartIdling()
{
    mbStartIdleTimer = true;
    if( !mIdleBlockCount )
        maDocIdle.Start();
}

void DocumentTimerManager::StopIdling()
{
    mbStartIdleTimer = false;
    maDocIdle.Stop();
}

void DocumentTimerManager::BlockIdling()
{
    maDocIdle.Stop();
    ++mIdleBlockCount;
}

void DocumentTimerManager::UnblockIdling()
{
    --mIdleBlockCount;
    if( !mIdleBlockCount && mbStartIdleTimer && !maDocIdle.IsActive() )
        maDocIdle.Start();
}

void DocumentTimerManager::StartBackgroundJobs()
{
    // Trigger DoIdleJobs(), asynchronously.
    if (!maDocIdle.IsActive()) //fdo#73165 if the timer is already running don't restart from 0
        maDocIdle.Start();
}

DocumentTimerManager::IdleJob DocumentTimerManager::GetNextIdleJob() const
{
    SwRootFrame* pTmpRoot = m_rDoc.getIDocumentLayoutAccess().GetCurrentLayout();
    if( pTmpRoot &&
        !SfxProgress::GetActiveProgress( m_rDoc.GetDocShell() ) )
    {
        SwViewShell* pShell(m_rDoc.getIDocumentLayoutAccess().GetCurrentViewShell());
        for(SwViewShell& rSh : pShell->GetRingContainer())
            if( rSh.ActionPend() )
                return IdleJob::Busy;

        if( pTmpRoot->IsNeedGrammarCheck() )
        {
            bool bIsOnlineSpell = pShell->GetViewOptions()->IsOnlineSpell();
            bool bIsAutoGrammar = false;
            SvtLinguConfig().GetProperty( OUString(
                        UPN_IS_GRAMMAR_AUTO ) ) >>= bIsAutoGrammar;

            if( bIsOnlineSpell && bIsAutoGrammar && m_rDoc.StartGrammarChecking( true ) )
                return IdleJob::Grammar;
        }

        for ( auto pLayout : m_rDoc.GetAllLayouts() )
        {
            if( pLayout->IsIdleFormat() )
                return IdleJob::Layout;
        }

        SwFieldUpdateFlags nFieldUpdFlag = m_rDoc.GetDocumentSettingManager().getFieldUpdateFlags(true);
        if( ( AUTOUPD_FIELD_ONLY == nFieldUpdFlag
                    || AUTOUPD_FIELD_AND_CHARTS == nFieldUpdFlag )
                && m_rDoc.getIDocumentFieldsAccess().GetUpdateFields().IsFieldsDirty() )
        {
            if( m_rDoc.getIDocumentFieldsAccess().GetUpdateFields().IsInUpdateFields()
                    || m_rDoc.getIDocumentFieldsAccess().IsExpFieldsLocked() )
                return IdleJob::Busy;
            return IdleJob::Fields;
        }
    }

    return IdleJob::None;
}

IMPL_LINK( DocumentTimerManager, DoIdleJobs, Timer*, pIdle, void )
{
#ifdef TIMELOG
    static ::rtl::Logfile* pModLogFile = 0;
    if( !pModLogFile )
        pModLogFile = new ::rtl::Logfile( "First DoIdleJobs" );
#endif

    IdleJob eJob = GetNextIdleJob();

    switch ( eJob )
    {
    case IdleJob::Grammar:
        m_rDoc.StartGrammarChecking();
        break;

    case IdleJob::Layout:
        for ( auto pLayout : m_rDoc.GetAllLayouts() )
            if( pLayout->IsIdleFormat() )
            {
                pLayout->GetCurrShell()->LayoutIdle();
                break;
            }
         break;

    case IdleJob::Fields:
    {
        SwViewShell* pShell( m_rDoc.getIDocumentLayoutAccess().GetCurrentViewShell() );
        SwRootFrame* pTmpRoot = m_rDoc.getIDocumentLayoutAccess().GetCurrentLayout();

        //  Action brackets!
        m_rDoc.getIDocumentFieldsAccess().GetUpdateFields().SetInUpdateFields( true );

        pTmpRoot->StartAllAction();

        // no jump on update of fields #i85168#
        const bool bOldLockView = pShell->IsViewLocked();
        pShell->LockView( true );

        m_rDoc.getIDocumentFieldsAccess().GetSysFieldType( SwFieldIds::Chapter )->ModifyNotification( nullptr, nullptr );  // ChapterField
        m_rDoc.getIDocumentFieldsAccess().UpdateExpFields( nullptr, false );  // Updates ExpressionFields
        m_rDoc.getIDocumentFieldsAccess().UpdateTableFields(nullptr);  // Tables
        m_rDoc.getIDocumentFieldsAccess().UpdateRefFields();  // References

        pTmpRoot->EndAllAction();

        pShell->LockView( bOldLockView );

        m_rDoc.getIDocumentFieldsAccess().GetUpdateFields().SetInUpdateFields( false );
        m_rDoc.getIDocumentFieldsAccess().GetUpdateFields().SetFieldsDirty( false );
        break;
    }

    case IdleJob::Busy:
        break;
    case IdleJob::None:
        break;
    }

    if ( IdleJob::None != eJob )
        pIdle->Start();

#ifdef TIMELOG
    if( pModLogFile && 1 != (long)pModLogFile )
        delete pModLogFile, static_cast<long&>(pModLogFile) = 1;
#endif
}

DocumentTimerManager::~DocumentTimerManager() {}

}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
