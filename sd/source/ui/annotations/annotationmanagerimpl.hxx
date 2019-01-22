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

#ifndef INCLUDED_SD_SOURCE_UI_ANNOTATIONS_ANNOTATIONMANAGERIMPL_HXX
#define INCLUDED_SD_SOURCE_UI_ANNOTATIONS_ANNOTATIONMANAGERIMPL_HXX

#include <com/sun/star/document/XEventListener.hpp>
#include <com/sun/star/office/XAnnotationAccess.hpp>

#include <rtl/ustring.hxx>

#include <cppuhelper/basemutex.hxx>
#include <cppuhelper/compbase.hxx>

#include <ViewShellBase.hxx>

#include "annotationtag.hxx"

class SfxRequest;

namespace sd
{

typedef std::vector< rtl::Reference< AnnotationTag > > AnnotationTagVector;

namespace tools {
class EventMultiplexerEvent;
}

typedef ::cppu::WeakComponentImplHelper <
    css::document::XEventListener
    > AnnotationManagerImplBase;

class AnnotationManagerImpl : private ::cppu::BaseMutex, public AnnotationManagerImplBase
{
public:
    explicit AnnotationManagerImpl( ViewShellBase& rViewShellBase );

    void init();

    // WeakComponentImplHelper
    virtual void SAL_CALL disposing () override;

    // XEventListener
    virtual void SAL_CALL notifyEvent( const css::document::EventObject& Event ) override;
    virtual void SAL_CALL disposing( const css::lang::EventObject& Source ) override;

    void ExecuteAnnotation (SfxRequest const & rRequest);
    void GetAnnotationState (SfxItemSet& rItemSet);

    void ExecuteInsertAnnotation(SfxRequest const & rReq);
    void ExecuteDeleteAnnotation(SfxRequest const & rReq);
    void ExecuteEditAnnotation(SfxRequest const & rReq);
    void ExecuteReplyToAnnotation(SfxRequest const & rReq);

    void SelectNextAnnotation(bool bForeward);

    void SelectAnnotation( const css::uno::Reference< css::office::XAnnotation >& xAnnotation, bool bEdit = false );
    void GetSelectedAnnotation( css::uno::Reference< css::office::XAnnotation >& xAnnotation );

    void InsertAnnotation(const OUString& rText);
    void DeleteAnnotation( const css::uno::Reference< css::office::XAnnotation >& xAnnotation );
    void DeleteAnnotationsByAuthor( const OUString& sAuthor );
    void DeleteAllAnnotations();

    void ExecuteAnnotationContextMenu( const css::uno::Reference< css::office::XAnnotation >& xAnnotation, vcl::Window* pParent, const ::tools::Rectangle& rContextRect, bool bButtonMenu = false );

    static Color GetColorDark(sal_uInt16 aAuthorIndex);
    static Color GetColorLight(sal_uInt16 aAuthorIndex);
    static Color GetColor(sal_uInt16 aAuthorIndex);

    // callbacks
    void onTagSelected( AnnotationTag const & rTag );
    void onTagDeselected( AnnotationTag const & rTag );

    void onSelectionChanged();

    void addListener();
    void removeListener();

    void invalidateSlots();

    DECL_LINK(EventMultiplexerListener, tools::EventMultiplexerEvent&, void);
    DECL_LINK(UpdateTagsHdl, void *, void);

    void UpdateTags(bool bSynchron = false);
    void CreateTags();
    void DisposeTags();

    SdPage* GetNextPage( SdPage const * pPage, bool bForeward );

    SdPage* GetCurrentPage();

    SdDrawDocument* GetDoc() { return mpDoc; }

    void ShowAnnotations(bool bShow);

    // tdf#99388 and tdf#99712 flag to transport if the PopupMenu is active
    bool getPopupMenuActive() const { return mbPopupMenuActive; }
    void setPopupMenuActive(bool bNew) { mbPopupMenuActive = bNew; }

private:
    ViewShellBase& mrBase;
    SdDrawDocument* mpDoc;

    AnnotationTagVector maTagVector;

    css::uno::Reference< css::drawing::XDrawView > mxView;
    css::uno::Reference< css::office::XAnnotationAccess > mxCurrentPage;
    css::uno::Reference< css::office::XAnnotation > mxSelectedAnnotation;

    bool mbShowAnnotations;
    bool mbPopupMenuActive;
    ImplSVEvent * mnUpdateTagsEvent;
    vcl::Font maFont;

    css::uno::Reference<css::office::XAnnotation> GetAnnotationById(sal_uInt32 nAnnotationId);
};

OUString getAnnotationDateTimeString( const css::uno::Reference< css::office::XAnnotation >& xAnnotation );

SfxItemPool* GetAnnotationPool();

css::util::DateTime getCurrentDateTime();

}

#endif // INCLUDED_SD_SOURCE_UI_ANNOTATIONS_ANNOTATIONMANAGERIMPL_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
