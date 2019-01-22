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

#include <com/sun/star/awt/XBitmap.hpp>
#include <com/sun/star/graphic/XGraphic.hpp>
#include <tools/stream.hxx>
#include <vcl/window.hxx>
#include <vcl/virdev.hxx>
#include <vcl/bitmapex.hxx>
#include <toolkit/helper/vclunohelper.hxx>
#include <svl/style.hxx>
#include <editeng/memberids.h>
#include <svx/strings.hrc>
#include <svx/xattr.hxx>
#include <svx/xtable.hxx>
#include <svx/xdef.hxx>
#include <svx/unomid.hxx>
#include <editeng/unoprnms.hxx>
#include <svx/unoapi.hxx>
#include <svx/svdmodel.hxx>
#include <svx/xbitmap.hxx>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <vcl/salbtype.hxx>
#include <vcl/bitmapaccess.hxx>
#include <vcl/dibtools.hxx>

#include <libxml/xmlwriter.h>

using namespace ::com::sun::star;

XOBitmap::XOBitmap( const Bitmap& rBmp ) :
    xGraphicObject  (new GraphicObject(rBmp)),
    bGraphicDirty   ( false )
{
}

XOBitmap::~XOBitmap()
{
}

Bitmap XOBitmap::GetBitmap() const
{
    return GetGraphicObject().GetGraphic().GetBitmap();
}

const GraphicObject& XOBitmap::GetGraphicObject() const
{
    if( bGraphicDirty )
        const_cast<XOBitmap*>(this)->Array2Bitmap();

    return *xGraphicObject;
}

void XOBitmap::Bitmap2Array()
{
    ScopedVclPtrInstance< VirtualDevice > pVDev;
    bool            bPixelColor = false;
    const Bitmap    aBitmap( GetBitmap() );
    const sal_Int32 nLines = 8; // type dependent

    if( !pPixelArray )
        pPixelArray.reset( new sal_uInt16[ nLines * nLines ] );

    pVDev->SetOutputSizePixel( aBitmap.GetSizePixel() );
    pVDev->DrawBitmap( Point(), aBitmap );
    aPixelColor = aBckgrColor = pVDev->GetPixel( Point() );

    // create array and determine foreground and background color
    for (sal_Int32 i = 0; i < nLines; ++i)
    {
        for (sal_Int32 j = 0; j < nLines; ++j)
        {
            if ( pVDev->GetPixel( Point( j, i ) ) == aBckgrColor )
                pPixelArray[ j + i * nLines ] = 0;
            else
            {
                pPixelArray[ j + i * nLines ] = 1;
                if( !bPixelColor )
                {
                    aPixelColor = pVDev->GetPixel( Point( j, i ) );
                    bPixelColor = true;
                }
            }
        }
    }
}

/// convert array, fore- and background color into a bitmap
void XOBitmap::Array2Bitmap()
{
    if (!pPixelArray)
        return;

    ScopedVclPtrInstance< VirtualDevice > pVDev;
    const sal_Int32 nLines = 8; // type dependent

    pVDev->SetOutputSizePixel( Size( nLines, nLines ) );

    // create bitmap
    for (sal_Int32 i = 0; i < nLines; ++i)
    {
        for (sal_Int32 j = 0; j < nLines; ++j)
        {
            if( pPixelArray[ j + i * nLines ] == 0 )
                pVDev->DrawPixel( Point( j, i ), aBckgrColor );
            else
                pVDev->DrawPixel( Point( j, i ), aPixelColor );
        }
    }

    xGraphicObject.reset(new GraphicObject(pVDev->GetBitmap(Point(), Size(nLines, nLines))));
    bGraphicDirty = false;
}


SfxPoolItem* XFillBitmapItem::CreateDefault() { return new XFillBitmapItem; }

XFillBitmapItem::XFillBitmapItem(const OUString& rName, const GraphicObject& rGraphicObject)
:   NameOrIndex(XATTR_FILLBITMAP, rName),
    maGraphicObject(rGraphicObject)
{
}

XFillBitmapItem::XFillBitmapItem(const XFillBitmapItem& rItem)
:   NameOrIndex(rItem),
    maGraphicObject(rItem.maGraphicObject)
{
}

Bitmap createHistorical8x8FromArray(const sal_uInt16* pArray, Color aColorPix, Color aColorBack)
{
    BitmapPalette aPalette(2);

    aPalette[0] = BitmapColor(aColorBack);
    aPalette[1] = BitmapColor(aColorPix);

    Bitmap aBitmap(Size(8, 8), 1, &aPalette);
    Bitmap::ScopedWriteAccess pContent(aBitmap);

    if(pContent)
    {
        for(sal_uInt16 a(0); a < 8; a++)
        {
            for(sal_uInt16 b(0); b < 8; b++)
            {
                if(pArray[(a * 8) + b])
                {
                    pContent->SetPixelIndex(a, b, 1);
                }
                else
                {
                    pContent->SetPixelIndex(a, b, 0);
                }
            }
        }

        pContent.reset();
    }

    return aBitmap;
}

bool isHistorical8x8(const BitmapEx& rBitmapEx, BitmapColor& o_rBack, BitmapColor& o_rFront)
{
    bool bRet(false);

    if(!rBitmapEx.IsTransparent())
    {
        Bitmap aBitmap(rBitmapEx.GetBitmap());

        if(8 == aBitmap.GetSizePixel().Width() && 8 == aBitmap.GetSizePixel().Height())
        {
            if(2 == aBitmap.GetColorCount())
            {
                BitmapReadAccess* pRead = aBitmap.AcquireReadAccess();

                if(pRead)
                {
                    if(pRead->HasPalette() && 2 == pRead->GetPaletteEntryCount())
                    {
                        const BitmapPalette& rPalette = pRead->GetPalette();

                        // #i123564# background and foreground were exchanged; of course
                        // rPalette[0] is the background color
                        o_rFront = rPalette[1];
                        o_rBack = rPalette[0];

                        bRet = true;
                    }

                    Bitmap::ReleaseAccess(pRead);
                }
            }
        }
    }

    return bRet;
}

XFillBitmapItem::XFillBitmapItem(const GraphicObject& rGraphicObject)
    : NameOrIndex(XATTR_FILLBITMAP, -1)
    , maGraphicObject(rGraphicObject)
{
}

SfxPoolItem* XFillBitmapItem::Clone(SfxItemPool* /*pPool*/) const
{
    return new XFillBitmapItem(*this);
}

bool XFillBitmapItem::operator==(const SfxPoolItem& rItem) const
{
    return (NameOrIndex::operator==(rItem)
        && maGraphicObject == static_cast<const XFillBitmapItem&>(rItem).maGraphicObject);
}


bool XFillBitmapItem::isPattern() const
{
    BitmapColor aBack, aFront;
    return isHistorical8x8(GetGraphicObject().GetGraphic().GetBitmap(), aBack, aFront);
}

sal_uInt16 XFillBitmapItem::GetVersion(sal_uInt16 /*nFileFormatVersion*/) const
{
    return 2;
}

bool XFillBitmapItem::GetPresentation(
    SfxItemPresentation /*ePres*/,
    MapUnit /*eCoreUnit*/,
    MapUnit /*ePresUnit*/,
    OUString& rText,
    const IntlWrapper&) const
{
    rText += GetName();
    return true;
}

bool XFillBitmapItem::QueryValue(css::uno::Any& rVal, sal_uInt8 nMemberId) const
{
    nMemberId &= ~CONVERT_TWIPS;

    // needed for MID_NAME
    OUString aApiName;
    // needed for complete item (MID 0)
    OUString aInternalName;

    OUString aURL;
    css::uno::Reference< css::awt::XBitmap > xBmp;

    if( nMemberId == MID_NAME )
    {
         aApiName = SvxUnogetApiNameForItem(Which(), GetName());
    }
    else if( nMemberId == 0  )
    {
        aInternalName = GetName();
    }

    if( nMemberId == MID_GRAFURL ||
        nMemberId == 0 )
    {
        aURL = UNO_NAME_GRAPHOBJ_URLPREFIX;
        aURL += OStringToOUString(
            GetGraphicObject().GetUniqueID(),
            RTL_TEXTENCODING_ASCII_US);
    }
    if( nMemberId == MID_BITMAP ||
        nMemberId == 0  )
    {
        xBmp.set(VCLUnoHelper::CreateBitmap(GetGraphicObject().GetGraphic().GetBitmapEx()));
    }

    if( nMemberId == MID_NAME )
        rVal <<= aApiName;
    else if( nMemberId == MID_GRAFURL )
        rVal <<= aURL;
    else if( nMemberId == MID_BITMAP )
        rVal <<= xBmp;
    else
    {
        // member-id 0 => complete item (e.g. for toolbars)
        DBG_ASSERT( nMemberId == 0, "invalid member-id" );
        uno::Sequence< beans::PropertyValue > aPropSeq( 3 );

        aPropSeq[0].Name  = "Name";
        aPropSeq[0].Value <<= aInternalName;
        aPropSeq[1].Name  = "FillBitmapURL";
        aPropSeq[1].Value <<= aURL;
        aPropSeq[2].Name  = "Bitmap";
        aPropSeq[2].Value <<= xBmp;

        rVal <<= aPropSeq;
    }

    return true;
}

bool XFillBitmapItem::PutValue( const css::uno::Any& rVal, sal_uInt8 nMemberId )
{
    nMemberId &= ~CONVERT_TWIPS;

    OUString aName;
    OUString aURL;
    css::uno::Reference< css::awt::XBitmap > xBmp;
    css::uno::Reference< css::graphic::XGraphic > xGraphic;

    bool bSetName   = false;
    bool bSetURL    = false;
    bool bSetBitmap = false;

    if( nMemberId == MID_NAME )
        bSetName = (rVal >>= aName);
    else if( nMemberId == MID_GRAFURL )
        bSetURL = (rVal >>= aURL);
    else if( nMemberId == MID_BITMAP )
    {
        bSetBitmap = (rVal >>= xBmp);
        if ( !bSetBitmap )
            bSetBitmap = (rVal >>= xGraphic );
    }
    else
    {
        DBG_ASSERT( nMemberId == 0, "invalid member-id" );
        uno::Sequence< beans::PropertyValue >   aPropSeq;
        if( rVal >>= aPropSeq )
        {
            for ( sal_Int32 n = 0; n < aPropSeq.getLength(); n++ )
            {
                if ( aPropSeq[n].Name == "Name" )
                    bSetName = (aPropSeq[n].Value >>= aName);
                else if ( aPropSeq[n].Name == "FillBitmapURL" )
                    bSetURL = (aPropSeq[n].Value >>= aURL);
                else if ( aPropSeq[n].Name == "Bitmap" )
                    bSetBitmap = (aPropSeq[n].Value >>= xBmp);
            }
        }
    }

    if( bSetName )
    {
        SetName( aName );
    }
    if( bSetURL )
    {
        GraphicObject aGraphicObject  = GraphicObject::CreateGraphicObjectFromURL(aURL);
        if( aGraphicObject.GetType() != GraphicType::NONE )
            maGraphicObject = aGraphicObject;

        // #121194# Prefer GraphicObject over bitmap object if both are provided
        if(bSetBitmap && GraphicType::NONE != maGraphicObject.GetType())
        {
            bSetBitmap = false;
        }
    }
    if( bSetBitmap )
    {
        if(xBmp.is())
        {
            maGraphicObject.SetGraphic(VCLUnoHelper::GetBitmap(xBmp));
        }
        else if(xGraphic.is())
        {
            maGraphicObject.SetGraphic(xGraphic);
        }
    }

    return (bSetName || bSetURL || bSetBitmap);
}

bool XFillBitmapItem::CompareValueFunc( const NameOrIndex* p1, const NameOrIndex* p2 )
{
    const GraphicObject& aGraphicObjectA(static_cast<const XFillBitmapItem*>(p1)->GetGraphicObject());
    const GraphicObject& aGraphicObjectB(static_cast<const XFillBitmapItem*>(p2)->GetGraphicObject());

    return aGraphicObjectA == aGraphicObjectB;
}

XFillBitmapItem* XFillBitmapItem::checkForUniqueItem( SdrModel* pModel ) const
{
    if( pModel )
    {
        XPropertyListType aListType = XPropertyListType::Bitmap;
        if(isPattern())
            aListType = XPropertyListType::Pattern;
        const OUString aUniqueName = NameOrIndex::CheckNamedItem(
                this, XATTR_FILLBITMAP, &pModel->GetItemPool(),
                XFillBitmapItem::CompareValueFunc, RID_SVXSTR_BMP21,
                pModel->GetPropertyList( aListType ) );

        // if the given name is not valid, replace it!
        if( aUniqueName != GetName() )
        {
            return new XFillBitmapItem(aUniqueName, maGraphicObject);
        }
    }

    return nullptr;
}

void XFillBitmapItem::dumpAsXml(xmlTextWriterPtr pWriter) const
{
    xmlTextWriterStartElement(pWriter, BAD_CAST("XFillBitmapItem"));
    xmlTextWriterWriteAttribute(pWriter, BAD_CAST("whichId"), BAD_CAST(OString::number(Which()).getStr()));

    NameOrIndex::dumpAsXml(pWriter);

    xmlTextWriterEndElement(pWriter);
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
