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

#include <com/sun/star/ui/XDeck.hpp>
#include <com/sun/star/ui/XPanel.hpp>
#include "SlideBackground.hxx"
#include <TransitionPreset.hxx>
#include <sdresid.hxx>
#include <ViewShellBase.hxx>
#include <FrameView.hxx>
#include <DrawDocShell.hxx>
#include <SlideSorterViewShell.hxx>
#include <drawdoc.hxx>
#include <filedlg.hxx>
#include "PageMarginUtils.hxx"
#include <strings.hrc>
#include "DocumentHelper.hxx"
#include "MasterPagesSelector.hxx"
#include <DrawViewShell.hxx>
#include <DrawController.hxx>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <svtools/controldims.hxx>
#include <svx/colorbox.hxx>
#include <svx/gallery.hxx>
#include <svx/drawitem.hxx>
#include <unotools/pathoptions.hxx>
#include <vcl/msgbox.hxx>
#include <tools/urlobj.hxx>
#include <sfx2/sidebar/Theme.hxx>
#include <app.hrc>
#include <editeng/paperinf.hxx>
#include <svx/rulritem.hxx>
#include <svx/svxids.hrc>
#include <svx/xflclit.hxx>
#include <svx/xgrad.hxx>
#include <svx/xbitmap.hxx>
#include <svx/xflbckit.hxx>
#include <svx/xbtmpit.hxx>
#include <svx/xattr.hxx>
#include <svx/xflhtit.hxx>
#include <svx/svdpage.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/objface.hxx>
#include <svx/dlgutil.hxx>
#include <sfx2/tabdlg.hxx>
#include <sfx2/sidebar/Panel.hxx>
#include <algorithm>
#include <EventMultiplexer.hxx>
#include <vcl/salbtype.hxx>
#include <vcl/EnumContext.hxx>

#include <editeng/ulspitem.hxx>
#include <editeng/lrspitem.hxx>
#include <svl/itemset.hxx>

using namespace ::com::sun::star;

using ::com::sun::star::uno::Reference;

namespace sd { namespace sidebar {

enum eFillStyle
{
    NONE,
    SOLID,
    GRADIENT,
    HATCH,
    BITMAP,
    PATTERN
};

SlideBackground::SlideBackground(
    Window * pParent,
    ViewShellBase& rBase,
    const css::uno::Reference<css::frame::XFrame>& rxFrame,
    SfxBindings* pBindings
     ) :
    PanelLayout( pParent, "SlideBackgroundPanel", "modules/simpress/ui/sidebarslidebackground.ui", rxFrame ),
    mrBase( rBase ),
    maPaperSizeController(SID_ATTR_PAGE_SIZE, *pBindings, *this),
    maPaperOrientationController(SID_ATTR_PAGE, *pBindings, *this),
    maPaperMarginLRController(SID_ATTR_PAGE_LRSPACE, *pBindings, *this),
    maPaperMarginULController(SID_ATTR_PAGE_ULSPACE, *pBindings, *this),
    maBckColorController(SID_ATTR_PAGE_COLOR, *pBindings, *this),
    maBckGradientController(SID_ATTR_PAGE_GRADIENT, *pBindings, *this),
    maBckHatchController(SID_ATTR_PAGE_HATCH, *pBindings, *this),
    maBckBitmapController(SID_ATTR_PAGE_BITMAP, *pBindings, *this),
    maBckFillStyleController(SID_ATTR_PAGE_FILLSTYLE, *pBindings, *this),
    maBckImageController(SID_SELECT_BACKGROUND, *pBindings, *this),
    maDspBckController(SID_DISPLAY_MASTER_BACKGROUND, *pBindings, *this),
    maDspObjController(SID_DISPLAY_MASTER_OBJECTS, *pBindings, *this),
    maMetricController(SID_ATTR_METRIC, *pBindings, *this),
    maCloseMasterController(SID_CLOSE_MASTER_VIEW, *pBindings, *this),
    mpPageItem( new SvxPageItem(SID_ATTR_PAGE) ),
    mpColorItem(),
    mpGradientItem(),
    mpHatchItem(),
    mpBitmapItem(),
    mbEditModeChangePending(false),
    mxFrame(rxFrame),
    maContext(),
    maDrawOtherContext(vcl::EnumContext::Application::Draw, vcl::EnumContext::Context::DrawPage),
    maDrawMasterContext(vcl::EnumContext::Application::Draw, vcl::EnumContext::Context::MasterPage),
    maImpressOtherContext(vcl::EnumContext::Application::Impress, vcl::EnumContext::Context::DrawPage),
    maImpressMasterContext(vcl::EnumContext::Application::Impress, vcl::EnumContext::Context::MasterPage),
    maImpressHandoutContext(vcl::EnumContext::Application::Impress, vcl::EnumContext::Context::HandoutPage),
    maImpressNotesContext(vcl::EnumContext::Application::Impress, vcl::EnumContext::Context::NotesPage),
    mbTitle(false),
    mpPageLRMarginItem( new SvxLongLRSpaceItem( 0, 0, SID_ATTR_PAGE_LRSPACE ) ),
    mpPageULMarginItem( new SvxLongULSpaceItem( 0, 0, SID_ATTR_PAGE_ULSPACE ) ),
    m_nPageLeftMargin(0),
    m_nPageRightMargin(0),
    m_nPageTopMargin(0),
    m_nPageBottomMargin(0),
    maCustomEntry(),
    mpBindings(pBindings)
{
    get(mpPaperSizeBox,"paperformat");
    get(mpPaperOrientation, "orientation");
    get(mpMasterSlide, "masterslide");
    //let the listbox shrink to any size so the sidebar isn't forced to grow to
    //the size of the longest master slide name in the document
    mpMasterSlide->set_width_request(0);
    get(mpBackgroundLabel, "label3");
    get(mpFillAttr, "fillattr1");
    get(mpFillGrad, "fillattr2");
    get(mpFillStyle, "fillstyle");
    get(mpFillLB, "fillattr");
    get(mpInsertImage, "button2");
    get(mpDspMasterBackground, "displaymasterbackground");
    get(mpDspMasterObjects, "displaymasterobjects");
    get(mpCloseMaster, "closemasterslide");
    get(mpEditMaster, "masterslidebutton");
    get(mpMasterLabel, "masterlabel");
    get(mpMarginSelectBox, "marginLB");

    maCustomEntry = get<FixedText>("customlabel")->GetText();

    SfxViewFrame* pCurrent = SfxViewFrame::Current();
    if (pCurrent)
    {
        const SfxPoolItem* pItem = nullptr;
        pCurrent->GetBindings().GetDispatcher()->QueryState( SID_ATTR_PAGE_SIZE, pItem );
        if (pItem)
        {
            const SvxSizeItem* pSize = static_cast<const SvxSizeItem*>( pItem );
            m_aPageSize = pSize->GetSize();
        }
    }
    addListener();
    Initialize();
}

SlideBackground::~SlideBackground()
{
    disposeOnce();
}

bool SlideBackground::IsDraw()
{
    return ( maContext == maDrawMasterContext ||
             maContext == maDrawOtherContext );
}

bool SlideBackground::IsImpress()
{
    return ( maContext == maImpressMasterContext ||
             maContext == maImpressOtherContext ||
             maContext == maImpressHandoutContext ||
             maContext == maImpressNotesContext );
}

void SlideBackground::Initialize()
{
    mpPaperSizeBox->FillPaperSizeEntries( PaperSizeApp::Draw );
    mpPaperSizeBox->SetSelectHdl(LINK(this,SlideBackground,PaperSizeModifyHdl));
    mpPaperOrientation->SetSelectHdl(LINK(this,SlideBackground,PaperSizeModifyHdl));
    mpCloseMaster->SetClickHdl(LINK(this, SlideBackground, CloseMasterHdl));
    meUnit = maPaperSizeController.GetCoreMetric();

    mpMasterSlide->SetSelectHdl(LINK(this, SlideBackground, AssignMasterPage));

    mpFillStyle->SetSelectHdl(LINK(this, SlideBackground, FillStyleModifyHdl));
    mpFillLB->SetSelectHdl(LINK(this, SlideBackground, FillColorHdl));
    mpFillGrad->SetSelectHdl(LINK(this, SlideBackground, FillColorHdl));
    mpFillAttr->SetSelectHdl(LINK(this, SlideBackground, FillBackgroundHdl));

    ViewShell* pMainViewShell = mrBase.GetMainViewShell().get();
    if (pMainViewShell)
    {
        FrameView *pFrameView = pMainViewShell->GetFrameView();

        if ( pFrameView->GetViewShEditMode() ==  EditMode::Page )
        {
            SdPage* mpPage = pMainViewShell->getCurrentPage();
            populateMasterSlideDropdown();

            OUString aLayoutName( mpPage->GetLayoutName() );
            aLayoutName = aLayoutName.copy(0,aLayoutName.indexOf(SD_LT_SEPARATOR));
            mpMasterSlide->SelectEntry(aLayoutName);
        }
    }

    mpFillStyle->SelectEntryPos(static_cast< sal_Int32 >(NONE));

    mpDspMasterBackground->SetClickHdl(LINK(this, SlideBackground, DspBackground));
    mpDspMasterObjects->SetClickHdl(LINK(this,SlideBackground, DspObjects));

    //margins
    mpMarginSelectBox->SetSelectHdl(LINK(this, SlideBackground,ModifyMarginHdl));

    Update();
    UpdateMarginBox();
}

void SlideBackground::HandleContextChange(
    const vcl::EnumContext& rContext)
{
    if (maContext == rContext)
        return;
    maContext = rContext;

    if ( IsImpress() )
    {
        mpMasterLabel->SetText(SdResId(STR_MASTERSLIDE_LABEL));

        // margin selector is only for Draw
        mpMarginSelectBox->Hide();
        VclPtr<FixedText> pMarginLabel;
        get(pMarginLabel, "labelmargin");
        if ( pMarginLabel )
            pMarginLabel->Hide();

        if ( maContext == maImpressMasterContext )
        {
            mpCloseMaster->Show();
            mpEditMaster->Hide();
            mpMasterSlide->Disable();
            mpDspMasterBackground->Disable();
            mpDspMasterObjects->Disable();
            mpFillStyle->Show();
            mpBackgroundLabel->Show();
            mpInsertImage->Show();
        }
        else if ( maContext == maImpressHandoutContext )
        {
            mpCloseMaster->Hide();
            mpEditMaster->Hide();
            mpMasterSlide->Disable();
            mpDspMasterBackground->Disable();
            mpDspMasterObjects->Disable();
            mpFillStyle->Hide();
            mpBackgroundLabel->Hide();
            mpInsertImage->Hide();
        }
        else if (maContext == maImpressOtherContext)
        {
            mpCloseMaster->Hide();
            mpEditMaster->Show();
            mpMasterSlide->Enable();
            mpDspMasterBackground->Enable();
            mpDspMasterObjects->Enable();
            mpFillStyle->Show();
            mpBackgroundLabel->Show();
            mpInsertImage->Show();
        }
        else if (maContext == maImpressNotesContext)
        {
            mpCloseMaster->Hide();
            mpEditMaster->Hide();
            mpMasterSlide->Disable();
            mpDspMasterBackground->Disable();
            mpDspMasterObjects->Disable();
            mpFillStyle->Show();
            mpBackgroundLabel->Show();
            mpInsertImage->Hide();
        }
        // Need to do a relayouting, otherwise the panel size is not updated after show / hide controls
        sfx2::sidebar::Panel* pPanel = dynamic_cast<sfx2::sidebar::Panel*>(GetParent());
        if(pPanel)
            pPanel->TriggerDeckLayouting();
    }
    else if ( IsDraw() )
    {
        mpMasterLabel->SetText(SdResId(STR_MASTERPAGE_LABEL));
    }
}

void SlideBackground::Update()
{
    eFillStyle nPos = (eFillStyle)mpFillStyle->GetSelectedEntryPos();

    if(maContext == maImpressHandoutContext)
        nPos = NONE;

    SfxObjectShell* pSh = SfxObjectShell::Current();
    if (!pSh)
        return;

    switch(nPos)
    {
        case NONE:
        {
            mpFillLB->Hide();
            mpFillAttr->Hide();
            mpFillGrad->Hide();
        }
        break;
        case SOLID:
        {
            mpFillAttr->Hide();
            mpFillGrad->Hide();
            mpFillLB->Show();
            const Color aColor = GetColorSetOrDefault();
            mpFillLB->SelectEntry(aColor);
        }
        break;
        case GRADIENT:
        {
            mpFillLB->Show();
            mpFillAttr->Hide();
            mpFillGrad->Show();

            const XGradient xGradient = GetGradientSetOrDefault();
            const Color aStartColor = xGradient.GetStartColor();
            mpFillLB->SelectEntry(aStartColor);
            const Color aEndColor = xGradient.GetEndColor();
            mpFillGrad->SelectEntry(aEndColor);
        }
        break;

        case HATCH:
        {
            mpFillLB->Hide();
            const SvxHatchListItem aItem(*static_cast<const SvxHatchListItem*>(pSh->GetItem(SID_HATCH_LIST)));
            mpFillAttr->Show();
            mpFillAttr->Clear();
            mpFillAttr->Fill(aItem.GetHatchList());
            mpFillGrad->Hide();

            const OUString aHatchName = GetHatchingSetOrDefault();
            mpFillAttr->SelectEntry( aHatchName );
        }
        break;

        case BITMAP:
        case PATTERN:
        {
            mpFillLB->Hide();
            mpFillAttr->Show();
            mpFillAttr->Clear();
            mpFillGrad->Hide();
            OUString aName;
            if(nPos == BITMAP)
            {
                const SvxBitmapListItem aItem(*static_cast<const SvxBitmapListItem*>(pSh->GetItem(SID_BITMAP_LIST)));
                mpFillAttr->Fill(aItem.GetBitmapList());
                aName = GetBitmapSetOrDefault();
            }
            else if(nPos == PATTERN)
            {
                const SvxPatternListItem aItem(*static_cast<const SvxPatternListItem*>(pSh->GetItem(SID_PATTERN_LIST)));
                mpFillAttr->Fill(aItem.GetPatternList());
                aName = GetPatternSetOrDefault();
            }
            mpFillAttr->SelectEntry( aName );
        }
        break;
        default:
            break;
    }
}

void SlideBackground::UpdateMarginBox()
{
    m_nPageLeftMargin = mpPageLRMarginItem->GetLeft();
    m_nPageRightMargin = mpPageLRMarginItem->GetRight();
    m_nPageTopMargin = mpPageULMarginItem->GetUpper();
    m_nPageBottomMargin = mpPageULMarginItem->GetLower();

    if( IsNone(m_nPageLeftMargin, m_nPageRightMargin, m_nPageTopMargin, m_nPageBottomMargin) )
    {
        mpMarginSelectBox->SelectEntryPos(0);
        mpMarginSelectBox->RemoveEntry(maCustomEntry);
    }
    else if( IsNarrow(m_nPageLeftMargin, m_nPageRightMargin, m_nPageTopMargin, m_nPageBottomMargin) )
    {
        mpMarginSelectBox->SelectEntryPos(1);
        mpMarginSelectBox->RemoveEntry(maCustomEntry);
    }
    else if( IsModerate(m_nPageLeftMargin, m_nPageRightMargin, m_nPageTopMargin, m_nPageBottomMargin) )
    {
        mpMarginSelectBox->SelectEntryPos(2);
        mpMarginSelectBox->RemoveEntry(maCustomEntry);
    }
    else if( IsNormal075(m_nPageLeftMargin, m_nPageRightMargin, m_nPageTopMargin, m_nPageBottomMargin) )
    {
        mpMarginSelectBox->SelectEntryPos(3);
        mpMarginSelectBox->RemoveEntry(maCustomEntry);
    }
    else if( IsNormal100(m_nPageLeftMargin, m_nPageRightMargin, m_nPageTopMargin, m_nPageBottomMargin) )
    {
        mpMarginSelectBox->SelectEntryPos(4);
        mpMarginSelectBox->RemoveEntry(maCustomEntry);
    }
    else if( IsNormal125(m_nPageLeftMargin, m_nPageRightMargin, m_nPageTopMargin, m_nPageBottomMargin) )
    {
        mpMarginSelectBox->SelectEntryPos(5);
        mpMarginSelectBox->RemoveEntry(maCustomEntry);
    }
    else if( IsWide(m_nPageLeftMargin, m_nPageRightMargin, m_nPageTopMargin, m_nPageBottomMargin) )
    {
        mpMarginSelectBox->SelectEntryPos(6);
        mpMarginSelectBox->RemoveEntry(maCustomEntry);
    }
    else
    {
        if(mpMarginSelectBox->GetEntryPos(maCustomEntry) == LISTBOX_ENTRY_NOTFOUND)
            mpMarginSelectBox->InsertEntry(maCustomEntry);
        mpMarginSelectBox->SelectEntry(maCustomEntry);
    }
}

void SlideBackground::SetPanelTitle( const OUString& rTitle )
{
    Reference<frame::XController2> xController( mxFrame->getController(), uno::UNO_QUERY);
    if ( !xController.is() )
        return;

    Reference<ui::XSidebarProvider> xSidebarProvider( xController->getSidebar(), uno::UNO_QUERY );
    if ( !xSidebarProvider.is() )
        return;

    Reference<ui::XDecks> xDecks ( xSidebarProvider->getDecks(), uno::UNO_QUERY);
    if ( !xDecks.is() )
        return;

    Reference<ui::XDeck> xDeck ( xDecks->getByName("PropertyDeck"), uno::UNO_QUERY);
    if ( !xDeck.is() )
        return;

    Reference<ui::XPanels> xPanels ( xDeck->getPanels(), uno::UNO_QUERY);
    if ( !xPanels.is() )
        return;

    if (xPanels->hasByName("SlideBackgroundPanel"))
    {
        Reference<ui::XPanel> xPanel ( xPanels->getByName("SlideBackgroundPanel"), uno::UNO_QUERY);
        if ( !xPanel.is() )
            return;

        xPanel->setTitle( rTitle );
    }
}

void SlideBackground::addListener()
{
    Link<tools::EventMultiplexerEvent&,void> aLink( LINK(this, SlideBackground, EventMultiplexerListener) );
    mrBase.GetEventMultiplexer()->AddEventListener( aLink );
}

void SlideBackground::removeListener()
{
    Link<tools::EventMultiplexerEvent&,void> aLink( LINK(this, SlideBackground, EventMultiplexerListener) );
    mrBase.GetEventMultiplexer()->RemoveEventListener( aLink );
}

IMPL_LINK(SlideBackground, EventMultiplexerListener,
                tools::EventMultiplexerEvent&, rEvent, void)
{
    switch (rEvent.meEventId)
    {
        // add more events as per requirement
        // Master Page change triggers a shape change event. Solves sync problem.
        case EventMultiplexerEventId::ShapeChanged:
            populateMasterSlideDropdown();
            break;
        case EventMultiplexerEventId::EditModeNormal:
        case EventMultiplexerEventId::EditModeMaster:
            mbEditModeChangePending = true;
            break;
        case EventMultiplexerEventId::EditViewSelection:
        case EventMultiplexerEventId::EndTextEdit:
        {
            if (mbEditModeChangePending)
            {
                ViewShell* pMainViewShell = mrBase.GetMainViewShell().get();

                if (pMainViewShell)
                {
                    DrawViewShell* pDrawViewShell = static_cast<DrawViewShell*>(pMainViewShell);
                    EditMode eMode = pDrawViewShell->GetEditMode();

                    if ( eMode == EditMode::MasterPage)
                    {
                        if( IsImpress() )
                            SetPanelTitle(SdResId(STR_MASTERSLIDE_NAME));
                        else
                            SetPanelTitle(SdResId(STR_MASTERPAGE_NAME));
                    }
                    else // EditMode::Page
                    {
                        if( IsImpress() )
                            SetPanelTitle(SdResId(STR_SLIDE_NAME));
                        else
                            SetPanelTitle(SdResId(STR_PAGE_NAME));
                    }
                }
                mbEditModeChangePending = false;
            }
        }
        break;
        case EventMultiplexerEventId::CurrentPageChanged:
        {
            static const sal_uInt16 SidArray[] = {
                SID_ATTR_PAGE_COLOR,
                SID_ATTR_PAGE_GRADIENT,
                SID_ATTR_PAGE_HATCH,
                SID_ATTR_PAGE_BITMAP,
                SID_ATTR_PAGE_FILLSTYLE,
                SID_DISPLAY_MASTER_BACKGROUND,
                SID_DISPLAY_MASTER_OBJECTS,
                0 };
            updateMasterSlideSelection();
            GetBindings()->Invalidate( SidArray );
        }
        break;
        case EventMultiplexerEventId::ViewAdded:
        {
            if(!mbTitle)
            {
                if( IsDraw() )
                {
                    mpCloseMaster->Hide();
                    mpEditMaster->Hide();
                    if( maContext == maDrawMasterContext)
                        SetPanelTitle(SdResId(STR_MASTERPAGE_NAME));
                    else
                        SetPanelTitle(SdResId(STR_PAGE_NAME));
                }
                else if ( maContext == maImpressOtherContext || maContext == maImpressMasterContext )
                {
                    if( maContext == maImpressMasterContext )
                        SetPanelTitle(SdResId(STR_MASTERSLIDE_NAME));
                    else
                        SetPanelTitle(SdResId(STR_SLIDE_NAME));
                }
                else if ( maContext == maImpressNotesContext )
                {
                    mpMasterLabel->SetText(SdResId(STR_MASTERSLIDE_LABEL));
                    ViewShell* pMainViewShell = mrBase.GetMainViewShell().get();

                    if (pMainViewShell)
                    {
                        DrawViewShell* pDrawViewShell = static_cast<DrawViewShell*>(pMainViewShell);
                        if ( pDrawViewShell->GetEditMode() == EditMode::MasterPage)
                            SetPanelTitle(SdResId(STR_MASTERSLIDE_NAME));
                        else // EditMode::Page
                            SetPanelTitle(SdResId(STR_SLIDE_NAME));
                    }
                }
                mbTitle = true;
            }
        }
        break;
        default:
            break;
    }
}

void SlideBackground::populateMasterSlideDropdown()
{
    mpMasterSlide->Clear();
    ::sd::DrawDocShell* pDocSh = dynamic_cast<::sd::DrawDocShell*>( SfxObjectShell::Current() );
    SdDrawDocument* pDoc = pDocSh ? pDocSh->GetDoc() : nullptr;
    sal_uInt16 nCount = pDoc ? pDoc->GetMasterPageCount() : 0;
    for( sal_uInt16 nLayout = 0; nLayout < nCount; nLayout++ )
    {
        SdPage* pMaster = static_cast<SdPage*>(pDoc->GetMasterPage(nLayout));
        if( pMaster->GetPageKind() == PageKind::Standard)
        {
            OUString aLayoutName(pMaster->GetLayoutName());
            aLayoutName = aLayoutName.copy(0,aLayoutName.indexOf(SD_LT_SEPARATOR));
            mpMasterSlide->InsertEntry(aLayoutName);
        }
    }
    updateMasterSlideSelection();
}

void SlideBackground::updateMasterSlideSelection()
{
    ViewShell* pMainViewShell = mrBase.GetMainViewShell().get();
    SdPage* pPage = pMainViewShell ? pMainViewShell->getCurrentPage() : nullptr;
    if (pPage != nullptr && pPage->TRG_HasMasterPage())
    {
        SdrPage& rMasterPage (pPage->TRG_GetMasterPage());
        SdPage* pMasterPage = static_cast<SdPage*>(&rMasterPage);
        mpMasterSlide->SelectEntry(pMasterPage->GetName());
    }
}

void SlideBackground::dispose()
{
    removeListener();

    mpPaperSizeBox.clear();
    mpPaperOrientation.clear();
    mpMasterSlide.clear();
    mpBackgroundLabel.clear();
    mpFillAttr.clear();
    mpFillGrad.clear();
    mpFillStyle.clear();
    mpFillLB.clear();
    mpInsertImage.clear();
    mpMarginSelectBox.disposeAndClear();
    mpDspMasterBackground.clear();
    mpDspMasterObjects.clear();
    mpMasterLabel.clear();
    mpEditMaster.clear();
    mpCloseMaster.clear();

    m_pContainer.clear();

    maPaperSizeController.dispose();
    maPaperOrientationController.dispose();
    maPaperMarginLRController.dispose();
    maPaperMarginULController.dispose();
    maBckColorController.dispose();
    maBckGradientController.dispose();
    maBckHatchController.dispose();
    maBckBitmapController.dispose();
    maBckFillStyleController.dispose();
    maBckImageController.dispose();
    maDspBckController.dispose();
    maDspObjController.dispose();
    maMetricController.dispose();
    maCloseMasterController.dispose();

    mpPageItem.reset();
    mpColorItem.reset();
    mpHatchItem.reset();
    mpBitmapItem.reset();
    mpPageLRMarginItem.reset();
    mpPageULMarginItem.reset();
    PanelLayout::dispose();
}

void SlideBackground::ExecuteMarginLRChange(const long mnPageLeftMargin, const long mnPageRightMargin)
{
    mpPageLRMarginItem->SetLeft(mnPageLeftMargin);
    mpPageLRMarginItem->SetRight(mnPageRightMargin);
    GetBindings()->GetDispatcher()->ExecuteList( SID_ATTR_PAGE_LRSPACE, SfxCallMode::RECORD, { mpPageLRMarginItem.get() } );
}

void SlideBackground::ExecuteMarginULChange(const long mnPageTopMargin, const long mnPageBottomMargin)
{
    mpPageULMarginItem->SetUpper(mnPageTopMargin);
    mpPageULMarginItem->SetLower(mnPageBottomMargin);
    GetBindings()->GetDispatcher()->ExecuteList( SID_ATTR_PAGE_ULSPACE, SfxCallMode::RECORD, { mpPageULMarginItem.get() } );
}

Color SlideBackground::GetColorSetOrDefault()
{
   // Tango Sky Blue 1, to be consistent w/ area fill panel (b/c COL_AUTO for slides is transparent)
   if ( !mpColorItem )
        mpColorItem.reset( new XFillColorItem( OUString(), Color(0x72, 0x9f, 0xcf) ) );

   return mpColorItem->GetColorValue();
}

XGradient SlideBackground::GetGradientSetOrDefault()
{
    if( !mpGradientItem )
    {
        SfxObjectShell* pSh = SfxObjectShell::Current();
        const SvxGradientListItem aGradListItem(*static_cast<const SvxGradientListItem*>(pSh->GetItem(SID_GRADIENT_LIST)));
        const XGradient aGradient = aGradListItem.GetGradientList()->GetGradient(0)->GetGradient();
        const OUString aGradientName = aGradListItem.GetGradientList()->GetGradient(0)->GetName();

        mpGradientItem.reset( new XFillGradientItem( aGradientName, aGradient ) );
    }

    return mpGradientItem->GetGradientValue();
}

const OUString SlideBackground::GetHatchingSetOrDefault()
{
    if( !mpHatchItem )
    {
        SfxObjectShell* pSh = SfxObjectShell::Current();
        const SvxHatchListItem aHatchListItem(*static_cast<const SvxHatchListItem*>(pSh->GetItem(SID_HATCH_LIST)));
        const XHatch aHatch = aHatchListItem.GetHatchList()->GetHatch(0)->GetHatch();
        const OUString aHatchName = aHatchListItem.GetHatchList()->GetHatch(0)->GetName();

        mpHatchItem.reset( new XFillHatchItem( aHatchName, aHatch ) );
    }

    return mpHatchItem->GetName();
}

const OUString SlideBackground::GetBitmapSetOrDefault()
{
    if( !mpBitmapItem || mpBitmapItem->isPattern())
    {
        SfxObjectShell* pSh = SfxObjectShell::Current();
        const SvxBitmapListItem aBmpListItem(*static_cast<const SvxBitmapListItem*>(pSh->GetItem(SID_BITMAP_LIST)));
        const GraphicObject aGraphObj = aBmpListItem.GetBitmapList()->GetBitmap(0)->GetGraphicObject();
        const OUString aBmpName = aBmpListItem.GetBitmapList()->GetBitmap(0)->GetName();

        mpBitmapItem.reset( new XFillBitmapItem( aBmpName, aGraphObj ) );
    }

    return mpBitmapItem->GetName();
}

const OUString SlideBackground::GetPatternSetOrDefault()
{
    if( !mpBitmapItem || !(mpBitmapItem->isPattern()))
    {
        SfxObjectShell* pSh = SfxObjectShell::Current();
        const SvxPatternListItem aPtrnListItem(*static_cast<const SvxPatternListItem*>(pSh->GetItem(SID_PATTERN_LIST)));
        const GraphicObject aGraphObj = aPtrnListItem.GetPatternList()->GetBitmap(0)->GetGraphicObject();
        const OUString aPtrnName = aPtrnListItem.GetPatternList()->GetBitmap(0)->GetName();

        mpBitmapItem.reset( new XFillBitmapItem( aPtrnName, aGraphObj ) );
    }

    return mpBitmapItem->GetName();
}

void SlideBackground::DataChanged (const DataChangedEvent& /*rEvent*/)
{

}

void SlideBackground::NotifyItemUpdate(
    const sal_uInt16 nSID,
    const SfxItemState eState,
    const SfxPoolItem* pState,
    const bool)
{
    switch(nSID)
    {

        case SID_ATTR_PAGE_COLOR:
        {
            if(eState >= SfxItemState::DEFAULT)
            {
                mpFillStyle->SelectEntryPos(static_cast< sal_Int32 >(SOLID));
                mpColorItem.reset(pState ? static_cast< XFillColorItem* >(pState->Clone()) : nullptr);
                Update();
            }
        }
        break;

        case SID_ATTR_PAGE_HATCH:
        {
            if(eState >= SfxItemState::DEFAULT)
            {
                mpFillStyle->SelectEntryPos(static_cast< sal_Int32 >(HATCH));
                mpHatchItem.reset(pState ? static_cast < XFillHatchItem* >(pState->Clone()) : nullptr);
                Update();
            }
        }
        break;

        case SID_ATTR_PAGE_GRADIENT:
        {
            if(eState >= SfxItemState::DEFAULT)
            {
                mpFillStyle->SelectEntryPos(static_cast< sal_Int32>(GRADIENT));
                mpGradientItem.reset(pState ? static_cast< XFillGradientItem* >(pState->Clone()) : nullptr);
                Update();
            }
        }
        break;
        case SID_ATTR_PAGE_BITMAP:
        {
            if(eState >= SfxItemState::DEFAULT)
            {
                mpBitmapItem.reset(pState ? static_cast< XFillBitmapItem* >(pState->Clone()) : nullptr);
                if(mpBitmapItem)
                {
                    if(mpBitmapItem->isPattern())
                        mpFillStyle->SelectEntryPos(static_cast< sal_Int32 >(PATTERN));
                    else
                        mpFillStyle->SelectEntryPos(static_cast< sal_Int32 >(BITMAP));
                }
                else
                    mpFillStyle->SelectEntryPos(static_cast< sal_Int32 >(BITMAP));
                Update();
            }
        }
        break;

        case SID_ATTR_PAGE_FILLSTYLE:
        {
            const XFillStyleItem* pFillStyleItem = nullptr;
            if (eState >= SfxItemState::DEFAULT)
                pFillStyleItem = dynamic_cast< const XFillStyleItem* >(pState);
            if (pFillStyleItem)
            {
                css::drawing::FillStyle eXFS = pFillStyleItem->GetValue();
                switch(eXFS)
                {
                    case drawing::FillStyle_NONE:
                        mpFillStyle->SelectEntryPos(static_cast< sal_Int32 >(NONE));
                        break;
                    case drawing::FillStyle_SOLID:
                        mpFillStyle->SelectEntryPos(static_cast< sal_Int32 >(SOLID));
                        break;
                    case drawing::FillStyle_GRADIENT:
                        mpFillStyle->SelectEntryPos(static_cast< sal_Int32 >(GRADIENT));
                        break;
                    case drawing::FillStyle_HATCH:
                        mpFillStyle->SelectEntryPos(static_cast< sal_Int32 >(HATCH));
                        break;
                    case drawing::FillStyle_BITMAP:
                    {
                        if(mpBitmapItem->isPattern())
                            mpFillStyle->SelectEntryPos(static_cast< sal_Int32 >(PATTERN));
                        else
                            mpFillStyle->SelectEntryPos(static_cast< sal_Int32 >(BITMAP));
                    }
                    break;
                    default:
                        break;
                }
                Update();
            }
        }
        break;

        case SID_ATTR_PAGE_SIZE:
        {
            const SvxSizeItem* pSizeItem = nullptr;
            if (eState >= SfxItemState::DEFAULT)
                pSizeItem = dynamic_cast<const SvxSizeItem*>(pState);
            if (pSizeItem)
            {
                Size aPaperSize = pSizeItem->GetSize();
                if(mpPaperOrientation->GetSelectedEntryPos() == 0)
                   Swap(aPaperSize);

                Paper ePaper = SvxPaperInfo::GetSvxPaper(aPaperSize, meUnit);
                mpPaperSizeBox->SetSelection( ePaper );
            }
        }
        break;

        case SID_ATTR_PAGE:
        {
            const SvxPageItem* pPageItem = nullptr;
            if (eState >= SfxItemState::DEFAULT)
                pPageItem = dynamic_cast<const SvxPageItem*>(pState);
            if (pPageItem)
            {
                mpPageItem.reset(static_cast<SvxPageItem*>(pPageItem->Clone()));
                bool bIsLandscape = mpPageItem->IsLandscape();
                mpPaperOrientation->SelectEntryPos( bIsLandscape ? 0 : 1 );
            }
        }
        break;

        case SID_ATTR_PAGE_LRSPACE:
        {
            const SvxLongLRSpaceItem* pLRItem = nullptr;
            if (eState >= SfxItemState::DEFAULT)
                pLRItem = dynamic_cast<const SvxLongLRSpaceItem*>(pState);
            if (pLRItem)
            {
                mpPageLRMarginItem.reset( static_cast<SvxLongLRSpaceItem*>(pState->Clone()) );
                UpdateMarginBox();
            }
        }
        break;

        case SID_ATTR_PAGE_ULSPACE:
        {
            const SvxLongULSpaceItem* pULItem = nullptr;
            if (eState >= SfxItemState::DEFAULT)
                pULItem = dynamic_cast<const SvxLongULSpaceItem*>(pState);
            if (pULItem)
            {
                mpPageULMarginItem.reset( static_cast<SvxLongULSpaceItem*>(pState->Clone()) );
                UpdateMarginBox();
            }
        }
        break;

        case SID_DISPLAY_MASTER_BACKGROUND:
        {
            const SfxBoolItem* pBoolItem = nullptr;
            if (eState >= SfxItemState::DEFAULT)
                pBoolItem = dynamic_cast< const SfxBoolItem* >(pState);
            if (pBoolItem)
                mpDspMasterBackground->Check(pBoolItem->GetValue());
        }
        break;
        case SID_DISPLAY_MASTER_OBJECTS:
        {
            const SfxBoolItem* pBoolItem = nullptr;
            if (eState >= SfxItemState::DEFAULT)
                pBoolItem = dynamic_cast< const SfxBoolItem* >(pState);
            if (pBoolItem)
                mpDspMasterObjects->Check(pBoolItem->GetValue());
        }
        break;
        case SID_SELECT_BACKGROUND:
        {
            if(eState >= SfxItemState::DEFAULT)
            {
                mpFillStyle->SelectEntryPos(static_cast< sal_Int32 >(BITMAP));
                Update();
            }
        }
        break;
        default:
            break;
    }
}

IMPL_LINK_NOARG(SlideBackground, FillStyleModifyHdl, ListBox&, void)
{
    const eFillStyle nPos = (eFillStyle)mpFillStyle->GetSelectedEntryPos();
    Update();

    switch (nPos)
    {
        case NONE:
        {
            const XFillStyleItem aXFillStyleItem(drawing::FillStyle_NONE);
            GetBindings()->GetDispatcher()->ExecuteList(SID_ATTR_PAGE_FILLSTYLE, SfxCallMode::RECORD, { &aXFillStyleItem });
        }
        break;

        case SOLID:
        {
            const XFillColorItem aItem( OUString(), mpColorItem->GetColorValue() );
            GetBindings()->GetDispatcher()->ExecuteList(SID_ATTR_PAGE_COLOR, SfxCallMode::RECORD, { &aItem });
        }
        break;

        case GRADIENT:
        {
            const XFillGradientItem aItem( mpGradientItem->GetName(), mpGradientItem->GetGradientValue() );
            GetBindings()->GetDispatcher()->ExecuteList(SID_ATTR_PAGE_GRADIENT, SfxCallMode::RECORD, { &aItem });
        }
        break;

        case HATCH:
        {
            const XFillHatchItem aItem( mpHatchItem->GetName(), mpHatchItem->GetHatchValue() );
            GetBindings()->GetDispatcher()->ExecuteList(SID_ATTR_PAGE_HATCH, SfxCallMode::RECORD, { &aItem });
        }
        break;

        case BITMAP:
        case PATTERN:
        {
            const XFillBitmapItem aItem( mpBitmapItem->GetName(), mpBitmapItem->GetGraphicObject() );
            GetBindings()->GetDispatcher()->ExecuteList(SID_ATTR_PAGE_BITMAP, SfxCallMode::RECORD, { &aItem });
        }
        break;

        default:
        break;
    }
    mpFillStyle->Selected();
}

IMPL_LINK_NOARG(SlideBackground, PaperSizeModifyHdl, ListBox&, void)
{
    Paper ePaper =  mpPaperSizeBox->GetSelection();
    Size  aSize(SvxPaperInfo::GetPaperSize(ePaper, meUnit));

    if(mpPaperOrientation->GetSelectedEntryPos() == 0)
        Swap(aSize);

    mpPageItem->SetLandscape(mpPaperOrientation->GetSelectedEntryPos() == 0);
    SvxSizeItem aSizeItem(SID_ATTR_PAGE_SIZE, aSize);
    // Page/slide properties dialog (FuPage::ExecuteDialog and ::ApplyItemSet) misuses
    // SID_ATTR_PAGE_EXT1 to distinguish between Impress and Draw, as for whether to fit
    // objects to paper size. Until that is handled somehow better, we do the same here
    SfxBoolItem aFitObjs(SID_ATTR_PAGE_EXT1, IsImpress());

    GetBindings()->GetDispatcher()->ExecuteList(SID_ATTR_PAGE_SIZE, SfxCallMode::RECORD, { &aSizeItem, mpPageItem.get(), &aFitObjs});
}

IMPL_LINK_NOARG(SlideBackground, FillColorHdl, SvxColorListBox&, void)
{
    const drawing::FillStyle eXFS = (drawing::FillStyle)mpFillStyle->GetSelectedEntryPos();
    switch(eXFS)
    {
        case drawing::FillStyle_SOLID:
        {
            XFillColorItem aItem(OUString(), mpFillLB->GetSelectEntryColor());
            GetBindings()->GetDispatcher()->ExecuteList(SID_ATTR_PAGE_COLOR, SfxCallMode::RECORD, { &aItem });
        }
        break;
        case drawing::FillStyle_GRADIENT:
        {
            XGradient aGradient;
            aGradient.SetStartColor(mpFillLB->GetSelectEntryColor());
            aGradient.SetEndColor(mpFillGrad->GetSelectEntryColor());

            // the name doesn't really matter, it'll be converted to unique one eventually,
            // but it has to be non-empty
            XFillGradientItem aItem("gradient", aGradient);
            GetBindings()->GetDispatcher()->ExecuteList(SID_ATTR_PAGE_GRADIENT, SfxCallMode::RECORD, { &aItem });
        }
        break;
        default:
            break;
    }
}

IMPL_LINK_NOARG(SlideBackground, FillBackgroundHdl, ListBox&, void)
{
    const eFillStyle nFillPos = (eFillStyle)mpFillStyle->GetSelectedEntryPos();
    SfxObjectShell* pSh = SfxObjectShell::Current();
    switch(nFillPos)
    {

        case HATCH:
        {
            const SvxHatchListItem aHatchListItem(*static_cast<const SvxHatchListItem*>(pSh->GetItem(SID_HATCH_LIST)));
            sal_uInt16 nPos = mpFillAttr->GetSelectedEntryPos();
            XHatch aHatch = aHatchListItem.GetHatchList()->GetHatch(nPos)->GetHatch();
            const OUString aHatchName = aHatchListItem.GetHatchList()->GetHatch(nPos)->GetName();

            XFillHatchItem aItem(aHatchName, aHatch);
            GetBindings()->GetDispatcher()->ExecuteList(SID_ATTR_PAGE_HATCH, SfxCallMode::RECORD, { &aItem });
        }
        break;

        case BITMAP:
        case PATTERN:
        {
            sal_Int16 nPos = mpFillAttr->GetSelectedEntryPos();
            GraphicObject aBitmap;
            OUString aName;
            if( nFillPos == BITMAP )
            {
                SvxBitmapListItem aBitmapListItem(*static_cast<const SvxBitmapListItem*>(pSh->GetItem(SID_BITMAP_LIST)));
                aBitmap = aBitmapListItem.GetBitmapList()->GetBitmap(nPos)->GetGraphicObject();
                aName = aBitmapListItem.GetBitmapList()->GetBitmap(nPos)->GetName();
            }
            else if( nFillPos == PATTERN )
            {
                SvxPatternListItem aPatternListItem(*static_cast<const SvxPatternListItem*>(pSh->GetItem(SID_PATTERN_LIST)));
                aBitmap = aPatternListItem.GetPatternList()->GetBitmap(nPos)->GetGraphicObject();
                aName = aPatternListItem.GetPatternList()->GetBitmap(nPos)->GetName();
            }
            XFillBitmapItem aItem(aName, aBitmap);
            GetBindings()->GetDispatcher()->ExecuteList(SID_ATTR_PAGE_BITMAP, SfxCallMode::RECORD, { &aItem });
        }
        break;

        default:
            break;
    }
}

IMPL_LINK_NOARG(SlideBackground, AssignMasterPage, ListBox&, void)
{
    ::sd::DrawDocShell* pDocSh = dynamic_cast<::sd::DrawDocShell*>( SfxObjectShell::Current() );
    SdDrawDocument* pDoc = pDocSh ? pDocSh->GetDoc() : nullptr;
    if (!pDoc)
        return;
    sal_uInt16 nSelectedPage = SDRPAGE_NOTFOUND;
    for( sal_uInt16 nPage = 0; nPage < pDoc->GetSdPageCount(PageKind::Standard); nPage++ )
    {
        if (pDoc->GetSdPage(nPage,PageKind::Standard)->IsSelected())
        {
            nSelectedPage = nPage;
            break;
        }
    }
    OUString aLayoutName(mpMasterSlide->GetSelectedEntry());
    pDoc->SetMasterPage(nSelectedPage, aLayoutName, pDoc, false, false);
}

IMPL_LINK_NOARG(SlideBackground, CloseMasterHdl, Button*, void)
{
    GetBindings()->GetDispatcher()->Execute( SID_CLOSE_MASTER_VIEW, SfxCallMode::RECORD );
}

IMPL_LINK_NOARG(SlideBackground, DspBackground, Button*, void)
{
    bool IsChecked = mpDspMasterBackground->IsChecked();
    const SfxBoolItem aBoolItem(SID_DISPLAY_MASTER_BACKGROUND, IsChecked);
    GetBindings()->GetDispatcher()->ExecuteList(SID_DISPLAY_MASTER_BACKGROUND, SfxCallMode::RECORD, { &aBoolItem });
}

IMPL_LINK_NOARG(SlideBackground, DspObjects, Button*, void)
{
    bool IsChecked = mpDspMasterObjects->IsChecked();
    const SfxBoolItem aBoolItem(SID_DISPLAY_MASTER_OBJECTS,IsChecked);
    GetBindings()->GetDispatcher()->ExecuteList(SID_DISPLAY_MASTER_OBJECTS, SfxCallMode::RECORD, { &aBoolItem, &aBoolItem });
}

IMPL_LINK_NOARG( SlideBackground, ModifyMarginHdl, ListBox&, void )
{
    bool bApplyNewPageMargins = true;
    switch ( mpMarginSelectBox->GetSelectedEntryPos() )
    {
        case 0:
            SetNone(m_nPageLeftMargin, m_nPageRightMargin, m_nPageTopMargin, m_nPageBottomMargin);
            break;
        case 1:
            SetNarrow(m_nPageLeftMargin, m_nPageRightMargin, m_nPageTopMargin, m_nPageBottomMargin);
            break;
        case 2:
            SetModerate(m_nPageLeftMargin, m_nPageRightMargin, m_nPageTopMargin, m_nPageBottomMargin);
            break;
        case 3:
            SetNormal075(m_nPageLeftMargin, m_nPageRightMargin, m_nPageTopMargin, m_nPageBottomMargin);
            break;
        case 4:
            SetNormal100(m_nPageLeftMargin, m_nPageRightMargin, m_nPageTopMargin, m_nPageBottomMargin);
            break;
        case 5:
            SetNormal125(m_nPageLeftMargin, m_nPageRightMargin, m_nPageTopMargin, m_nPageBottomMargin);
            break;
        case 6:
            SetWide(m_nPageLeftMargin, m_nPageRightMargin, m_nPageTopMargin, m_nPageBottomMargin);
            break;
        default:
            bApplyNewPageMargins = false;
            break;
    }

    if(bApplyNewPageMargins)
    {
        ExecuteMarginLRChange(m_nPageLeftMargin, m_nPageRightMargin);
        ExecuteMarginULChange(m_nPageTopMargin, m_nPageBottomMargin);
    }
}

}}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
