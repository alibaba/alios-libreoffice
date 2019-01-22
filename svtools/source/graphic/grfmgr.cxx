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

#include <algorithm>

#include <officecfg/Office/Common.hxx>
#include <osl/file.hxx>
#include <tools/vcompat.hxx>
#include <tools/fract.hxx>
#include <tools/helpers.hxx>
#include <unotools/ucbstreamhelper.hxx>
#include <unotools/tempfile.hxx>
#include <unotools/configmgr.hxx>
#include <vcl/svapp.hxx>
#include <vcl/cvtgrf.hxx>
#include <vcl/metaact.hxx>
#include <vcl/virdev.hxx>
#include <svtools/grfmgr.hxx>

#include <com/sun/star/container/XNameContainer.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <memory>

using com::sun::star::uno::Reference;
using com::sun::star::uno::XInterface;
using com::sun::star::uno::UNO_QUERY;
using com::sun::star::uno::Sequence;
using com::sun::star::container::XNameContainer;
using com::sun::star::beans::XPropertySet;

GraphicManager* GraphicObject::mpGlobalMgr = nullptr;

struct GrfSimpleCacheObj
{
    Graphic     maGraphic;
    GraphicAttr maAttr;

                GrfSimpleCacheObj( const Graphic& rGraphic, const GraphicAttr& rAttr ) :
                    maGraphic( rGraphic ), maAttr( rAttr ) {}
};

// unique increasing ID for being able to detect the GraphicObject with the
// oldest last data changes
static sal_uLong aIncrementingTimeOfLastDataChange = 1;

void GraphicObject::ImplAfterDataChange()
{
    // set unique timestamp ID of last data change
    mnDataChangeTimeStamp = aIncrementingTimeOfLastDataChange++;

    // check memory footprint of all GraphicObjects managed and evtl. take action
    mpGlobalMgr->ImplCheckSizeOfSwappedInGraphics(this);
}

GraphicObject::GraphicObject()
    : mbAutoSwapped(false)
    , mbIsInSwapIn(false)
    , mbIsInSwapOut(false)
{
    ImplEnsureGraphicManager();
    ImplAssignGraphicData();
    mpGlobalMgr->ImplRegisterObj(*this, maGraphic, nullptr, nullptr);
}

GraphicObject::GraphicObject(const Graphic& rGraphic)
    : maGraphic(rGraphic)
    , mbAutoSwapped(false)
    , mbIsInSwapIn(false)
    , mbIsInSwapOut(false)
{
    ImplEnsureGraphicManager();
    ImplAssignGraphicData();
    mpGlobalMgr->ImplRegisterObj(*this, maGraphic, nullptr, nullptr);
}

GraphicObject::GraphicObject(const GraphicObject& rGraphicObj)
    : maGraphic(rGraphicObj.GetGraphic())
    , maAttr(rGraphicObj.maAttr)
    , maPrefMapMode(rGraphicObj.maPrefMapMode)
    , maLink(rGraphicObj.maLink)
    , maUserData(rGraphicObj.maUserData)
    , mbAutoSwapped(false)
    , mbIsInSwapIn(false)
    , mbIsInSwapOut(false)
{
    ImplAssignGraphicData();
    mpGlobalMgr->ImplRegisterObj(*this, maGraphic, nullptr, &rGraphicObj);
    if( rGraphicObj.HasUserData() && rGraphicObj.IsSwappedOut() )
        SetSwapState();
}

GraphicObject::GraphicObject(const OString& rUniqueID)
    : mbAutoSwapped(false)
    , mbIsInSwapIn(false)
    , mbIsInSwapOut(false)
{
    ImplEnsureGraphicManager();

    // assign default properties
    ImplAssignGraphicData();

    mpGlobalMgr->ImplRegisterObj(*this, maGraphic, &rUniqueID, nullptr);

    // update properties
    ImplAssignGraphicData();
}

GraphicObject::~GraphicObject()
{
    mpGlobalMgr->ImplUnregisterObj( *this );

    if (!mpGlobalMgr->ImplHasObjects())
    {
        delete mpGlobalMgr;
        mpGlobalMgr = nullptr;
    }
}

void GraphicObject::ImplAssignGraphicData()
{
    maPrefSize = maGraphic.GetPrefSize();
    maPrefMapMode = maGraphic.GetPrefMapMode();
    mnSizeBytes = maGraphic.GetSizeBytes();
    meType = maGraphic.GetType();
    mbTransparent = maGraphic.IsTransparent();
    mbAnimated = maGraphic.IsAnimated();
    mbEPS = maGraphic.IsEPS();
    mnAnimationLoopCount = ( mbAnimated ? maGraphic.GetAnimationLoopCount() : 0 );

    // Handle evtl. needed AfterDataChanges
    ImplAfterDataChange();
}

void GraphicObject::ImplEnsureGraphicManager()
{
    if (mpGlobalMgr)
        return;

    sal_uLong nCacheSize = 20000;
    sal_uLong nMaxObjCacheSize = 20000;
    sal_uLong nTimeoutSeconds = 20000;
    if (!utl::ConfigManager::IsFuzzing())
    {
        try
        {
            nCacheSize = officecfg::Office::Common::Cache::GraphicManager::TotalCacheSize::get();
            nMaxObjCacheSize = officecfg::Office::Common::Cache::GraphicManager::ObjectCacheSize::get();
            nTimeoutSeconds = officecfg::Office::Common::Cache::GraphicManager::ObjectReleaseTime::get();
        }
        catch (...)
        {
        }
    }
    mpGlobalMgr = new GraphicManager(nCacheSize, nMaxObjCacheSize);
    mpGlobalMgr->SetCacheTimeout(nTimeoutSeconds);
}

void GraphicObject::ImplAutoSwapIn()
{
    if( !IsSwappedOut() )
        return;

    {
        mbIsInSwapIn = true;

        if( maGraphic.SwapIn() )
            mbAutoSwapped = false;
        else
        {
            SvStream* pStream = GetSwapStream();

            if( GRFMGR_AUTOSWAPSTREAM_NONE != pStream )
            {
                if( GRFMGR_AUTOSWAPSTREAM_LINK == pStream )
                {
                    if( HasLink() )
                    {
                        OUString aURLStr;

                        if( osl::FileBase::getFileURLFromSystemPath( GetLink(), aURLStr ) == osl::FileBase::E_None )
                        {
                            std::unique_ptr<SvStream> pIStm(::utl::UcbStreamHelper::CreateStream( aURLStr, StreamMode::READ ));

                            if( pIStm )
                            {
                                ReadGraphic( *pIStm, maGraphic );
                                mbAutoSwapped = ( maGraphic.GetType() != GraphicType::NONE );
                            }
                        }
                    }
                }
                else if( GRFMGR_AUTOSWAPSTREAM_TEMP == pStream )
                    mbAutoSwapped = !maGraphic.SwapIn();
                else if( GRFMGR_AUTOSWAPSTREAM_LOADED == pStream )
                    mbAutoSwapped = maGraphic.IsSwapOut();
                else
                {
                    mbAutoSwapped = !maGraphic.SwapIn( pStream );
                    delete pStream;
                }
            }
            else
            {
                DBG_ASSERT( ( GraphicType::NONE == meType ) || ( GraphicType::Default == meType ),
                            "GraphicObject::ImplAutoSwapIn: could not get stream to swap in graphic! (=>KA)" );
            }
        }

        mbIsInSwapIn = false;

        if (!mbAutoSwapped)
            mpGlobalMgr->ImplGraphicObjectWasSwappedIn( *this );
    }
    ImplAssignGraphicData();
}

bool GraphicObject::ImplGetCropParams( OutputDevice const * pOut, Point& rPt, Size& rSz, const GraphicAttr* pAttr,
                                       tools::PolyPolygon& rClipPolyPoly, bool& bRectClipRegion ) const
{
    bool bRet = false;

    if( GetType() != GraphicType::NONE )
    {
        tools::Polygon aClipPoly( tools::Rectangle( rPt, rSz ) );
        const sal_uInt16 nRot10 = pAttr->GetRotation() % 3600;
        const Point     aOldOrigin( rPt );
        const MapMode   aMap100( MapUnit::Map100thMM );
        Size            aSize100;
        long            nTotalWidth, nTotalHeight;

        if( nRot10 )
        {
            aClipPoly.Rotate( rPt, nRot10 );
            bRectClipRegion = false;
        }
        else
            bRectClipRegion = true;

        rClipPolyPoly = aClipPoly;

        if (maGraphic.GetPrefMapMode().GetMapUnit() == MapUnit::MapPixel)
            aSize100 = Application::GetDefaultDevice()->PixelToLogic( maGraphic.GetPrefSize(), aMap100 );
        else
        {
            MapMode m(maGraphic.GetPrefMapMode());
            aSize100 = pOut->LogicToLogic( maGraphic.GetPrefSize(), &m, &aMap100 );
        }

        nTotalWidth = aSize100.Width() - pAttr->GetLeftCrop() - pAttr->GetRightCrop();
        nTotalHeight = aSize100.Height() - pAttr->GetTopCrop() - pAttr->GetBottomCrop();

        if( aSize100.Width() > 0 && aSize100.Height() > 0 && nTotalWidth > 0 && nTotalHeight > 0 )
        {
            double fScale = (double) aSize100.Width() / nTotalWidth;
            const long nNewLeft = -FRound( ( ( pAttr->GetMirrorFlags() & BmpMirrorFlags::Horizontal ) ? pAttr->GetRightCrop() : pAttr->GetLeftCrop() ) * fScale );
            const long nNewRight = nNewLeft + FRound( aSize100.Width() * fScale ) - 1;

            fScale = (double) rSz.Width() / aSize100.Width();
            rPt.X() += FRound( nNewLeft * fScale );
            rSz.Width() = FRound( ( nNewRight - nNewLeft + 1 ) * fScale );

            fScale = (double) aSize100.Height() / nTotalHeight;
            const long nNewTop = -FRound( ( ( pAttr->GetMirrorFlags() & BmpMirrorFlags::Vertical ) ? pAttr->GetBottomCrop() : pAttr->GetTopCrop() ) * fScale );
            const long nNewBottom = nNewTop + FRound( aSize100.Height() * fScale ) - 1;

            fScale = (double) rSz.Height() / aSize100.Height();
            rPt.Y() += FRound( nNewTop * fScale );
            rSz.Height() = FRound( ( nNewBottom - nNewTop + 1 ) * fScale );

            if( nRot10 )
            {
                tools::Polygon aOriginPoly( 1 );

                aOriginPoly[ 0 ] = rPt;
                aOriginPoly.Rotate( aOldOrigin, nRot10 );
                rPt = aOriginPoly[ 0 ];
            }

            bRet = true;
        }
    }

    return bRet;
}

GraphicObject& GraphicObject::operator=( const GraphicObject& rGraphicObj )
{
    if( &rGraphicObj != this )
    {
        mpGlobalMgr->ImplUnregisterObj( *this );

        maSwapStreamHdl = Link<const GraphicObject*, SvStream*>();
        mxSimpleCache.reset();

        maGraphic = rGraphicObj.GetGraphic();
        maAttr = rGraphicObj.maAttr;
        maLink = rGraphicObj.maLink;
        maUserData = rGraphicObj.maUserData;
        ImplAssignGraphicData();
        mbAutoSwapped = false;
        mpGlobalMgr->ImplRegisterObj( *this, maGraphic, nullptr, &rGraphicObj );
        if( rGraphicObj.HasUserData() && rGraphicObj.IsSwappedOut() )
            SetSwapState();
    }

    return *this;
}

bool GraphicObject::operator==( const GraphicObject& rGraphicObj ) const
{
    return( ( rGraphicObj.maGraphic == maGraphic ) &&
            ( rGraphicObj.maAttr == maAttr ) &&
            ( rGraphicObj.GetLink() == GetLink() ) );
}

OString GraphicObject::GetUniqueID() const
{
    if ( !IsInSwapIn() && IsEPS() )
        const_cast<GraphicObject*>(this)->FireSwapInRequest();

    return mpGlobalMgr->ImplGetUniqueID(*this);
}

SvStream* GraphicObject::GetSwapStream() const
{
    if( HasSwapStreamHdl() )
        return maSwapStreamHdl.Call( this );
    else
        return GRFMGR_AUTOSWAPSTREAM_NONE;
}

void GraphicObject::SetAttr( const GraphicAttr& rAttr )
{
    maAttr = rAttr;

    if (mxSimpleCache && (mxSimpleCache->maAttr != rAttr))
        mxSimpleCache.reset();
}

void GraphicObject::SetLink()
{
    maLink.clear();
}

void GraphicObject::SetLink( const OUString& rLink )
{
    maLink = rLink;
}

void GraphicObject::SetUserData()
{
    maUserData.clear();
}

void GraphicObject::SetUserData( const OUString& rUserData )
{
    maUserData = rUserData;
    if( !rUserData.isEmpty() )
        SetSwapState();
}

static sal_uInt32 GetCacheTimeInMs()
{
    if (utl::ConfigManager::IsFuzzing())
        return 20000;

    const sal_uInt32 nSeconds =
        officecfg::Office::Common::Cache::GraphicManager::ObjectReleaseTime::get(
            comphelper::getProcessComponentContext());

    return nSeconds * 1000;
}

void GraphicObject::SetSwapStreamHdl(const Link<const GraphicObject*, SvStream*>& rHdl)
{
    maSwapStreamHdl = rHdl;

    sal_uInt32 const nSwapOutTimeout(GetCacheTimeInMs());
    if (nSwapOutTimeout)
    {
        if (!mxSwapOutTimer)
        {
            mxSwapOutTimer.reset(new Timer("svtools::GraphicObject mpSwapOutTimer"));
            mxSwapOutTimer->SetInvokeHandler( LINK( this, GraphicObject, ImplAutoSwapOutHdl ) );
        }

        mxSwapOutTimer->SetTimeout( nSwapOutTimeout );
        mxSwapOutTimer->Start();
    }
    else
    {
        mxSwapOutTimer.reset();
    }
}

void GraphicObject::FireSwapInRequest()
{
    ImplAutoSwapIn();
}

void GraphicObject::FireSwapOutRequest()
{
    ImplAutoSwapOutHdl( nullptr );
}

bool GraphicObject::IsCached( OutputDevice const * pOut, const Size& rSz,
                              const GraphicAttr* pAttr, GraphicManagerDrawFlags nFlags ) const
{
    bool bRet;

    if( nFlags & GraphicManagerDrawFlags::CACHED )
    {
        Point aPt;
        Size aSz( rSz );
        if ( pAttr && pAttr->IsCropped() )
        {
            tools::PolyPolygon aClipPolyPoly;
            bool bRectClip;
            ImplGetCropParams( pOut, aPt, aSz, pAttr, aClipPolyPoly, bRectClip );
        }
        bRet = mpGlobalMgr->IsInCache( pOut, aPt, aSz, *this, ( pAttr ? *pAttr : GetAttr() ) );
    }
    else
        bRet = false;

    return bRet;
}

bool GraphicObject::Draw( OutputDevice* pOut, const Point& rPt, const Size& rSz,
                          const GraphicAttr* pAttr, GraphicManagerDrawFlags nFlags )
{
    GraphicAttr         aAttr( pAttr ? *pAttr : GetAttr() );
    Point               aPt( rPt );
    Size                aSz( rSz );
    const DrawModeFlags nOldDrawMode = pOut->GetDrawMode();
    bool                bCropped = aAttr.IsCropped();
    bool                bCached = false;
    bool bRet;

    // #i29534# Provide output rects for PDF writer
    tools::Rectangle           aCropRect;

    if( !( GraphicManagerDrawFlags::USE_DRAWMODE_SETTINGS & nFlags ) )
        pOut->SetDrawMode( nOldDrawMode & ~DrawModeFlags( DrawModeFlags::SettingsLine | DrawModeFlags::SettingsFill | DrawModeFlags::SettingsText | DrawModeFlags::SettingsGradient ) );

    // mirrored horizontically
    if( aSz.Width() < 0 )
    {
        aPt.X() += aSz.Width() + 1;
        aSz.Width() = -aSz.Width();
        aAttr.SetMirrorFlags( aAttr.GetMirrorFlags() ^ BmpMirrorFlags::Horizontal );
    }

    // mirrored vertically
    if( aSz.Height() < 0 )
    {
        aPt.Y() += aSz.Height() + 1;
        aSz.Height() = -aSz.Height();
        aAttr.SetMirrorFlags( aAttr.GetMirrorFlags() ^ BmpMirrorFlags::Vertical );
    }

    if( bCropped )
    {
        tools::PolyPolygon aClipPolyPoly;
        bool        bRectClip;
        const bool  bCrop = ImplGetCropParams( pOut, aPt, aSz, &aAttr, aClipPolyPoly, bRectClip );

        pOut->Push( PushFlags::CLIPREGION );

        if( bCrop )
        {
            if( bRectClip )
            {
                // #i29534# Store crop rect for later forwarding to
                // PDF writer
                aCropRect = aClipPolyPoly.GetBoundRect();
                pOut->IntersectClipRegion( aCropRect );
            }
            else
            {
                pOut->IntersectClipRegion(vcl::Region(aClipPolyPoly));
            }
        }
    }

    bRet = mpGlobalMgr->DrawObj( pOut, aPt, aSz, *this, aAttr, nFlags, bCached );

    if( bCropped )
        pOut->Pop();

    pOut->SetDrawMode( nOldDrawMode );

    // #i29534# Moved below OutDev restoration, to avoid multiple swap-ins
    // (code above needs to call GetGraphic twice)
    if( bCached )
    {
        if (mxSwapOutTimer)
            mxSwapOutTimer->Start();
        else
            FireSwapOutRequest();
    }

    return bRet;
}

void GraphicObject::DrawTiled( OutputDevice* pOut, const tools::Rectangle& rArea, const Size& rSize,
                               const Size& rOffset, GraphicManagerDrawFlags nFlags, int nTileCacheSize1D )
{
    if( pOut == nullptr || rSize.Width() == 0 || rSize.Height() == 0 )
        return;

    const MapMode   aOutMapMode( pOut->GetMapMode() );
    const MapMode   aMapMode( aOutMapMode.GetMapUnit(), Point(), aOutMapMode.GetScaleX(), aOutMapMode.GetScaleY() );
    // #106258# Clamp size to 1 for zero values. This is okay, since
    // logical size of zero is handled above already
    const Size      aOutTileSize( ::std::max( 1L, pOut->LogicToPixel( rSize, aOutMapMode ).Width() ),
                                  ::std::max( 1L, pOut->LogicToPixel( rSize, aOutMapMode ).Height() ) );

    //#i69780 clip final tile size to a sane max size
    while (((sal_Int64)rSize.Width() * nTileCacheSize1D) > SAL_MAX_UINT16)
        nTileCacheSize1D /= 2;
    while (((sal_Int64)rSize.Height() * nTileCacheSize1D) > SAL_MAX_UINT16)
        nTileCacheSize1D /= 2;

    ImplDrawTiled( pOut, rArea, aOutTileSize, rOffset, nullptr, nFlags, nTileCacheSize1D );
}

bool GraphicObject::StartAnimation( OutputDevice* pOut, const Point& rPt, const Size& rSz,
                                    long nExtraData,
                                    OutputDevice* pFirstFrameOutDev )
{
    bool bRet = false;

    GetGraphic();

    if( !IsSwappedOut() )
    {
        const GraphicAttr aAttr( GetAttr() );

        if( mbAnimated )
        {
            Point   aPt( rPt );
            Size    aSz( rSz );
            bool    bCropped = aAttr.IsCropped();

            if( bCropped )
            {
                tools::PolyPolygon aClipPolyPoly;
                bool        bRectClip;
                const bool  bCrop = ImplGetCropParams( pOut, aPt, aSz, &aAttr, aClipPolyPoly, bRectClip );

                pOut->Push( PushFlags::CLIPREGION );

                if( bCrop )
                {
                    if( bRectClip )
                        pOut->IntersectClipRegion( aClipPolyPoly.GetBoundRect() );
                    else
                        pOut->IntersectClipRegion(vcl::Region(aClipPolyPoly));
                }
            }

            if (!mxSimpleCache || (mxSimpleCache->maAttr != aAttr) || pFirstFrameOutDev)
            {
                mxSimpleCache.reset(new GrfSimpleCacheObj(GetTransformedGraphic(&aAttr), aAttr));
                mxSimpleCache->maGraphic.SetAnimationNotifyHdl(GetGraphic().GetAnimationNotifyHdl());
            }

            mxSimpleCache->maGraphic.StartAnimation(pOut, aPt, aSz, nExtraData, pFirstFrameOutDev);

            if( bCropped )
                pOut->Pop();

            bRet = true;
        }
        else
            bRet = Draw( pOut, rPt, rSz, &aAttr );
    }

    return bRet;
}

void GraphicObject::StopAnimation( OutputDevice* pOut, long nExtraData )
{
    if (mxSimpleCache)
        mxSimpleCache->maGraphic.StopAnimation(pOut, nExtraData);
}

const Graphic& GraphicObject::GetGraphic() const
{
    GraphicObject *pThis = const_cast<GraphicObject*>(this);
    (void)pThis->SwapIn();

    //fdo#50697 If we've been asked to provide the graphic, then reset
    //the cache timeout to start from now and not remain at the
    //time of creation
    // restart SwapOut timer; this is like touching in a cache to reset to the full timeout value
    if (pThis->mxSwapOutTimer && pThis->mxSwapOutTimer->IsActive())
    {
        pThis->mxSwapOutTimer->Stop();
        pThis->mxSwapOutTimer->Start();
    }

    return maGraphic;
}

void GraphicObject::SetGraphic( const Graphic& rGraphic, const GraphicObject* pCopyObj )
{
    mpGlobalMgr->ImplUnregisterObj( *this );

    if (mxSwapOutTimer)
        mxSwapOutTimer->Stop();

    maGraphic = rGraphic;
    mbAutoSwapped = false;
    ImplAssignGraphicData();
    maLink.clear();
    mxSimpleCache.reset();

    mpGlobalMgr->ImplRegisterObj( *this, maGraphic, nullptr, pCopyObj);

    if (mxSwapOutTimer)
        mxSwapOutTimer->Start();


}

void GraphicObject::SetGraphic( const Graphic& rGraphic, const OUString& rLink )
{
    // in case we are called from a situation where rLink and maLink are the same thing,
    // we need a copy because SetGraphic clears maLink
    OUString sLinkCopy = rLink;
    SetGraphic( rGraphic );
    maLink = sLinkCopy;
}

Graphic GraphicObject::GetTransformedGraphic( const Size& rDestSize, const MapMode& rDestMap, const GraphicAttr& rAttr ) const
{
    // #104550# Extracted from svx/source/svdraw/svdograf.cxx
    Graphic             aTransGraphic( GetGraphic() );
    const GraphicType   eType = GetType();
    const Size          aSrcSize( aTransGraphic.GetPrefSize() );

    // #104115# Convert the crop margins to graphic object mapmode
    const MapMode aMapGraph( aTransGraphic.GetPrefMapMode() );
    const MapMode aMap100( MapUnit::Map100thMM );

    Size aCropLeftTop;
    Size aCropRightBottom;

    if( GraphicType::GdiMetafile == eType )
    {
        GDIMetaFile aMtf( aTransGraphic.GetGDIMetaFile() );

        if (aMapGraph.GetMapUnit() == MapUnit::MapPixel)
        {
            // crops are in 1/100th mm -> to aMapGraph -> to MapUnit::MapPixel
            aCropLeftTop = Application::GetDefaultDevice()->LogicToPixel(
                Size(rAttr.GetLeftCrop(), rAttr.GetTopCrop()),
                aMap100);
            aCropRightBottom = Application::GetDefaultDevice()->LogicToPixel(
                Size(rAttr.GetRightCrop(), rAttr.GetBottomCrop()),
                aMap100);
        }
        else
        {
            // crops are in GraphicObject units -> to aMapGraph
            aCropLeftTop = OutputDevice::LogicToLogic(
                Size(rAttr.GetLeftCrop(), rAttr.GetTopCrop()),
                aMap100,
                aMapGraph);
            aCropRightBottom = OutputDevice::LogicToLogic(
                Size(rAttr.GetRightCrop(), rAttr.GetBottomCrop()),
                aMap100,
                aMapGraph);
        }

        // #104115# If the metafile is cropped, give it a special
        // treatment: clip against the remaining area, scale up such
        // that this area later fills the desired size, and move the
        // origin to the upper left edge of that area.
        if( rAttr.IsCropped() )
        {
            const MapMode aMtfMapMode( aMtf.GetPrefMapMode() );

            tools::Rectangle aClipRect( aMtfMapMode.GetOrigin().X() + aCropLeftTop.Width(),
                                 aMtfMapMode.GetOrigin().Y() + aCropLeftTop.Height(),
                                 aMtfMapMode.GetOrigin().X() + aSrcSize.Width() - aCropRightBottom.Width(),
                                 aMtfMapMode.GetOrigin().Y() + aSrcSize.Height() - aCropRightBottom.Height() );

            // #104115# To correctly crop rotated metafiles, clip by view rectangle
            aMtf.AddAction( new MetaISectRectClipRegionAction( aClipRect ), 0 );

            // #104115# To crop the metafile, scale larger than the output rectangle
            aMtf.Scale( (double)rDestSize.Width() / (aSrcSize.Width() - aCropLeftTop.Width() - aCropRightBottom.Width()),
                        (double)rDestSize.Height() / (aSrcSize.Height() - aCropLeftTop.Height() - aCropRightBottom.Height()) );

            // #104115# Adapt the pref size by hand (scale changes it
            // proportionally, but we want it to be smaller than the
            // former size, to crop the excess out)
            aMtf.SetPrefSize( Size( (long)((double)rDestSize.Width() *  (1.0 + (aCropLeftTop.Width() + aCropRightBottom.Width()) / aSrcSize.Width())  + .5),
                                    (long)((double)rDestSize.Height() * (1.0 + (aCropLeftTop.Height() + aCropRightBottom.Height()) / aSrcSize.Height()) + .5) ) );

            // #104115# Adapt the origin of the new mapmode, such that it
            // is shifted to the place where the cropped output starts
            Point aNewOrigin( (long)((double)aMtfMapMode.GetOrigin().X() + rDestSize.Width() * aCropLeftTop.Width() / (aSrcSize.Width() - aCropLeftTop.Width() - aCropRightBottom.Width()) + .5),
                              (long)((double)aMtfMapMode.GetOrigin().Y() + rDestSize.Height() * aCropLeftTop.Height() / (aSrcSize.Height() - aCropLeftTop.Height() - aCropRightBottom.Height()) + .5) );
            MapMode aNewMap( rDestMap );
            aNewMap.SetOrigin( OutputDevice::LogicToLogic(aNewOrigin, aMtfMapMode, rDestMap) );
            aMtf.SetPrefMapMode( aNewMap );
        }
        else
        {
            aMtf.Scale( Fraction( rDestSize.Width(), aSrcSize.Width() ), Fraction( rDestSize.Height(), aSrcSize.Height() ) );
            aMtf.SetPrefMapMode( rDestMap );
        }

        aTransGraphic = aMtf;
    }
    else if( GraphicType::Bitmap == eType )
    {
        BitmapEx aBitmapEx( aTransGraphic.GetBitmapEx() );
        tools::Rectangle aCropRect;

        // convert crops to pixel
        if(rAttr.IsCropped())
        {
            if (aMapGraph.GetMapUnit() == MapUnit::MapPixel)
            {
                // crops are in 1/100th mm -> to MapUnit::MapPixel
                aCropLeftTop = Application::GetDefaultDevice()->LogicToPixel(
                    Size(rAttr.GetLeftCrop(), rAttr.GetTopCrop()),
                    aMap100);
                aCropRightBottom = Application::GetDefaultDevice()->LogicToPixel(
                    Size(rAttr.GetRightCrop(), rAttr.GetBottomCrop()),
                    aMap100);
            }
            else
            {
                // crops are in GraphicObject units -> to MapUnit::MapPixel
                aCropLeftTop = Application::GetDefaultDevice()->LogicToPixel(
                    Size(rAttr.GetLeftCrop(), rAttr.GetTopCrop()),
                    aMapGraph);
                aCropRightBottom = Application::GetDefaultDevice()->LogicToPixel(
                    Size(rAttr.GetRightCrop(), rAttr.GetBottomCrop()),
                    aMapGraph);
            }

            // convert from prefmapmode to pixel
            Size aSrcSizePixel(
                Application::GetDefaultDevice()->LogicToPixel(
                    aSrcSize,
                    aMapGraph));

            if(rAttr.IsCropped()
                && (aSrcSizePixel.Width() != aBitmapEx.GetSizePixel().Width() || aSrcSizePixel.Height() != aBitmapEx.GetSizePixel().Height())
                && aSrcSizePixel.Width())
            {
                // the size in pixels calculated from Graphic's internal MapMode (aTransGraphic.GetPrefMapMode())
                // and it's internal size (aTransGraphic.GetPrefSize()) is different from its real pixel size.
                // This can be interpreted as this values to be set wrong, but needs to be corrected since e.g.
                // existing cropping is calculated based on this logic values already.
                // aBitmapEx.Scale(aSrcSizePixel);

                // another possibility is to adapt the values created so far with a factor; this
                // will keep the original Bitmap untouched and thus quality will not change
                // caution: convert to double first, else pretty big errors may occur
                const double fFactorX((double)aBitmapEx.GetSizePixel().Width() / aSrcSizePixel.Width());
                const double fFactorY((double)aBitmapEx.GetSizePixel().Height() / aSrcSizePixel.Height());

                aCropLeftTop.Width() = basegfx::fround(aCropLeftTop.Width() * fFactorX);
                aCropLeftTop.Height() = basegfx::fround(aCropLeftTop.Height() * fFactorY);
                aCropRightBottom.Width() = basegfx::fround(aCropRightBottom.Width() * fFactorX);
                aCropRightBottom.Height() = basegfx::fround(aCropRightBottom.Height() * fFactorY);

                aSrcSizePixel = aBitmapEx.GetSizePixel();
            }

            // setup crop rectangle in pixel
            aCropRect = tools::Rectangle( aCropLeftTop.Width(), aCropLeftTop.Height(),
                                 aSrcSizePixel.Width() - aCropRightBottom.Width(),
                                 aSrcSizePixel.Height() - aCropRightBottom.Height() );
        }

        // #105641# Also crop animations
        if( aTransGraphic.IsAnimated() )
        {
            Animation aAnim( aTransGraphic.GetAnimation() );

            for( size_t nFrame=0; nFrame<aAnim.Count(); ++nFrame )
            {
                AnimationBitmap aAnimBmp( aAnim.Get( nFrame ) );

                if( !aCropRect.IsInside( tools::Rectangle(aAnimBmp.aPosPix, aAnimBmp.aSizePix) ) )
                {
                    // setup actual cropping (relative to frame position)
                    tools::Rectangle aCropRectRel( aCropRect );
                    aCropRectRel.Move( -aAnimBmp.aPosPix.X(),
                                       -aAnimBmp.aPosPix.Y() );

                    // cropping affects this frame, apply it then
                    // do _not_ apply enlargement, this is done below
                    ImplTransformBitmap( aAnimBmp.aBmpEx, rAttr, Size(), Size(),
                                         aCropRectRel, rDestSize, false );

                    aAnim.Replace( aAnimBmp, nFrame );
                }
                // else: bitmap completely within crop area,
                // i.e. nothing is cropped away
            }

            // now, apply enlargement (if any) through global animation size
            if( aCropLeftTop.Width() < 0 ||
                aCropLeftTop.Height() < 0 ||
                aCropRightBottom.Width() < 0 ||
                aCropRightBottom.Height() < 0 )
            {
                Size aNewSize( aAnim.GetDisplaySizePixel() );
                aNewSize.Width() += aCropRightBottom.Width() < 0 ? -aCropRightBottom.Width() : 0;
                aNewSize.Width() += aCropLeftTop.Width() < 0 ? -aCropLeftTop.Width() : 0;
                aNewSize.Height() += aCropRightBottom.Height() < 0 ? -aCropRightBottom.Height() : 0;
                aNewSize.Height() += aCropLeftTop.Height() < 0 ? -aCropLeftTop.Height() : 0;
                aAnim.SetDisplaySizePixel( aNewSize );
            }

            // if topleft has changed, we must move all frames to the
            // right and bottom, resp.
            if( aCropLeftTop.Width() < 0 ||
                aCropLeftTop.Height() < 0 )
            {
                Point aPosOffset( aCropLeftTop.Width() < 0 ? -aCropLeftTop.Width() : 0,
                                  aCropLeftTop.Height() < 0 ? -aCropLeftTop.Height() : 0 );

                for( size_t nFrame=0; nFrame<aAnim.Count(); ++nFrame )
                {
                    AnimationBitmap aAnimBmp( aAnim.Get( nFrame ) );

                    aAnimBmp.aPosPix += aPosOffset;

                    aAnim.Replace( aAnimBmp, nFrame );
                }
            }

            aTransGraphic = aAnim;
        }
        else
        {
            ImplTransformBitmap( aBitmapEx, rAttr, aCropLeftTop, aCropRightBottom,
                                 aCropRect, rDestSize, true );

            aTransGraphic = aBitmapEx;
        }

        aTransGraphic.SetPrefSize( rDestSize );
        aTransGraphic.SetPrefMapMode( rDestMap );
    }

    GraphicObject aGrfObj( aTransGraphic );
    aTransGraphic = aGrfObj.GetTransformedGraphic( &rAttr );

    return aTransGraphic;
}

Graphic GraphicObject::GetTransformedGraphic( const GraphicAttr* pAttr ) const // TODO: Change to Impl
{
    GetGraphic();

    Graphic     aGraphic;
    GraphicAttr aAttr( pAttr ? *pAttr : GetAttr() );

    if( maGraphic.IsSupportedGraphic() && !maGraphic.IsSwapOut() )
    {
        if( aAttr.IsSpecialDrawMode() || aAttr.IsAdjusted() || aAttr.IsMirrored() || aAttr.IsRotated() || aAttr.IsTransparent() )
        {
            if( GetType() == GraphicType::Bitmap )
            {
                if( IsAnimated() )
                {
                    Animation aAnimation( maGraphic.GetAnimation() );
                    GraphicManager::ImplAdjust( aAnimation, aAttr, GraphicAdjustmentFlags::ALL );
                    aAnimation.SetLoopCount( mnAnimationLoopCount );
                    aGraphic = aAnimation;
                }
                else
                {
                    BitmapEx aBmpEx( maGraphic.GetBitmapEx() );
                    GraphicManager::ImplAdjust( aBmpEx, aAttr, GraphicAdjustmentFlags::ALL );
                    aGraphic = aBmpEx;
                }
            }
            else
            {
                GDIMetaFile aMtf( maGraphic.GetGDIMetaFile() );
                GraphicManager::ImplAdjust( aMtf, aAttr, GraphicAdjustmentFlags::ALL );
                aGraphic = aMtf;
            }
        }
        else
        {
            if( ( GetType() == GraphicType::Bitmap ) && IsAnimated() )
            {
                Animation aAnimation( maGraphic.GetAnimation() );
                aAnimation.SetLoopCount( mnAnimationLoopCount );
                aGraphic = aAnimation;
            }
            else
                aGraphic = maGraphic;
        }
    }

    return aGraphic;
}

bool GraphicObject::SwapOut()
{
    const bool bRet = !mbAutoSwapped && maGraphic.SwapOut();

    if (bRet)
        mpGlobalMgr->ImplGraphicObjectWasSwappedOut( *this );

    return bRet;
}

bool GraphicObject::SwapOut( SvStream* pOStm )
{
    bool bRet = false;
    try
    {
        bRet = !mbAutoSwapped;
        // swap out as a link
        if( pOStm == GRFMGR_AUTOSWAPSTREAM_LINK )
        {
            maGraphic.SwapOutAsLink();
        }
        else
        {
            bRet = bRet && maGraphic.SwapOut( pOStm );
        }

        if (bRet)
            mpGlobalMgr->ImplGraphicObjectWasSwappedOut(*this);
    }
    catch(...)
    {
        SAL_WARN( "svtools", "GraphicObject::SwapIn exception");
    }
    return bRet;
}

bool GraphicObject::SwapIn()
{
    bool bRet = false;
    try
    {
        if( mbAutoSwapped )
        {
            ImplAutoSwapIn();
            bRet = true;
        }
        else
        {
            bRet = maGraphic.SwapIn();

            if (bRet)
                mpGlobalMgr->ImplGraphicObjectWasSwappedIn(*this);
        }

        if( bRet )
        {
            ImplAssignGraphicData();
        }
    }
    catch (...)
    {
        SAL_WARN( "svtools", "GraphicObject::SwapIn exception");
    }

    return bRet;
}

void GraphicObject::SetSwapState()
{
    if( !IsSwappedOut() )
    {
        mbAutoSwapped = true;

        mpGlobalMgr->ImplGraphicObjectWasSwappedOut(*this);
    }
}

IMPL_LINK_NOARG(GraphicObject, ImplAutoSwapOutHdl, Timer *, void)
{
    if( !IsSwappedOut() )
    {
        mbIsInSwapOut = true;

        SvStream* pStream = GetSwapStream();

        if( GRFMGR_AUTOSWAPSTREAM_NONE != pStream )
        {
            if( GRFMGR_AUTOSWAPSTREAM_LINK == pStream )
                mbAutoSwapped = SwapOut( GRFMGR_AUTOSWAPSTREAM_LINK );
            else
            {
                if( GRFMGR_AUTOSWAPSTREAM_TEMP == pStream )
                    mbAutoSwapped = SwapOut();
                else
                {
                    mbAutoSwapped = SwapOut( pStream );
                    delete pStream;
                }
            }
        }

        mbIsInSwapOut = false;
    }

    if (mxSwapOutTimer)
        mxSwapOutTimer->Start();
}

#define UNO_NAME_GRAPHOBJ_URLPREFIX "vnd.sun.star.GraphicObject:"

GraphicObject GraphicObject::CreateGraphicObjectFromURL( const OUString &rURL )
{
    const OUString aPrefix( UNO_NAME_GRAPHOBJ_URLPREFIX );
    if( rURL.startsWith( aPrefix ) )
    {
        // graphic manager url
        OString aUniqueID(OUStringToOString(rURL.copy(sizeof(UNO_NAME_GRAPHOBJ_URLPREFIX) - 1), RTL_TEXTENCODING_UTF8));
        return GraphicObject( aUniqueID );
    }
    else
    {
        Graphic     aGraphic;
        if ( !rURL.isEmpty() )
        {
            std::unique_ptr<SvStream> pStream(utl::UcbStreamHelper::CreateStream( rURL, StreamMode::READ ));
            if( pStream )
                GraphicConverter::Import( *pStream, aGraphic );
        }

        return GraphicObject( aGraphic );
    }
}

void
GraphicObject::InspectForGraphicObjectImageURL( const Reference< XInterface >& xIf,  std::vector< OUString >& rvEmbedImgUrls )
{
    static const char sImageURL[] = "ImageURL";
    Reference< XPropertySet > xProps( xIf, UNO_QUERY );
    if ( xProps.is() )
    {

        if ( xProps->getPropertySetInfo()->hasPropertyByName( sImageURL ) )
        {
            OUString sURL;
            xProps->getPropertyValue( sImageURL ) >>= sURL;
            if ( !sURL.isEmpty() && sURL.startsWith( UNO_NAME_GRAPHOBJ_URLPREFIX ) )
                rvEmbedImgUrls.push_back( sURL );
        }
    }
    Reference< XNameContainer > xContainer( xIf, UNO_QUERY );
    if ( xContainer.is() )
    {
        Sequence< OUString > sNames = xContainer->getElementNames();
        sal_Int32 nContainees = sNames.getLength();
        for ( sal_Int32 index = 0; index < nContainees; ++index )
        {
            Reference< XInterface > xCtrl;
            xContainer->getByName( sNames[ index ] ) >>= xCtrl;
            InspectForGraphicObjectImageURL( xCtrl, rvEmbedImgUrls );
        }
    }
}

// calculate scalings between real image size and logic object size. This
// is necessary since the crop values are relative to original bitmap size
basegfx::B2DVector GraphicObject::calculateCropScaling(
    double fWidth,
    double fHeight,
    double fLeftCrop,
    double fTopCrop,
    double fRightCrop,
    double fBottomCrop) const
{
    const MapMode aMapMode100thmm(MapUnit::Map100thMM);
    Size aBitmapSize(GetPrefSize());
    double fFactorX(1.0);
    double fFactorY(1.0);

    if(MapUnit::MapPixel == GetPrefMapMode().GetMapUnit())
    {
        aBitmapSize = Application::GetDefaultDevice()->PixelToLogic(aBitmapSize, aMapMode100thmm);
    }
    else
    {
        aBitmapSize = OutputDevice::LogicToLogic(aBitmapSize, GetPrefMapMode(), aMapMode100thmm);
    }

    const double fDivX(aBitmapSize.Width() - fLeftCrop - fRightCrop);
    const double fDivY(aBitmapSize.Height() - fTopCrop - fBottomCrop);

    if(!basegfx::fTools::equalZero(fDivX))
    {
        fFactorX = fabs(fWidth) / fDivX;
    }

    if(!basegfx::fTools::equalZero(fDivY))
    {
        fFactorY = fabs(fHeight) / fDivY;
    }

    return basegfx::B2DVector(fFactorX,fFactorY);
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */