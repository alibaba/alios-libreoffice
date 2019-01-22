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

#include <algorithm>
#include <string.h>
#include <osl/thread.h>
#include <tools/debug.hxx>
#include <tools/fract.hxx>
#include <tools/stream.hxx>
#include <tools/helpers.hxx>
#include <vcl/dibtools.hxx>
#include <vcl/virdev.hxx>
#include <vcl/graph.hxx>
#include <vcl/lineinfo.hxx>
#include <rtl/strbuf.hxx>

#include <svmconverter.hxx>

#include <memory>
#include <o3tl/make_unique.hxx>

// Inlines
void ImplReadRect( SvStream& rIStm, tools::Rectangle& rRect )
{
    Point aTL;
    Point aBR;

    ReadPair( rIStm, aTL );
    ReadPair( rIStm, aBR );

    rRect = tools::Rectangle( aTL, aBR );
}

void ImplWriteRect( SvStream& rOStm, const tools::Rectangle& rRect )
{
    WritePair( rOStm, rRect.TopLeft() );
    WritePair( rOStm, rRect.BottomRight() );
}

bool ImplReadPoly(SvStream& rIStm, tools::Polygon& rPoly)
{
    sal_Int32 nSize32(0);
    rIStm.ReadInt32(nSize32);
    sal_uInt16 nSize = nSize32;

    const size_t nMaxPossiblePoints = rIStm.remainingSize() / 2 * sizeof(sal_Int32);
    if (nSize > nMaxPossiblePoints)
    {
        SAL_WARN("vcl.gdi", "svm record claims to have: " << nSize << " points, but only " << nMaxPossiblePoints << " possible");
        return false;
    }

    rPoly = tools::Polygon(nSize);

    for (sal_uInt16 i = 0; i < nSize && rIStm.good(); ++i)
        ReadPair(rIStm, rPoly[i]);

    return rIStm.good();
}

bool ImplReadPolyPoly(SvStream& rIStm, tools::PolyPolygon& rPolyPoly)
{
    bool bSuccess = true;

    tools::Polygon aPoly;
    sal_Int32 nPolyCount32(0);
    rIStm.ReadInt32(nPolyCount32);
    sal_uInt16 nPolyCount = (sal_uInt16)nPolyCount32;

    for (sal_uInt16 i = 0; i < nPolyCount && rIStm.good(); ++i)
    {
        if (!ImplReadPoly(rIStm, aPoly))
        {
            bSuccess = false;
            break;
        }
        rPolyPoly.Insert(aPoly);
    }

    return bSuccess && rIStm.good();
}

void ImplWritePolyPolyAction( SvStream& rOStm, const tools::PolyPolygon& rPolyPoly )
{
    const sal_uInt16    nPoly = rPolyPoly.Count();
    sal_uInt16          nPoints = 0;
    sal_uInt16          n;

    for( n = 0; n < nPoly; n++ )
        nPoints = sal::static_int_cast<sal_uInt16>(nPoints + rPolyPoly[ n ].GetSize());

    rOStm.WriteInt16( GDI_POLYPOLYGON_ACTION );
    rOStm.WriteInt32( 8 + ( nPoly << 2 ) + ( nPoints << 3 ) );
    rOStm.WriteInt32( nPoly );

    for( n = 0; n < nPoly; n++ )
    {
        // #i102224# Here the possible curved nature of Polygon was
        // ignored (for all those years). Adapted to at least write
        // a polygon representing the curve as good as possible
        tools::Polygon aSimplePoly;
         rPolyPoly[n].AdaptiveSubdivide(aSimplePoly);
         const sal_uInt16 nSize(aSimplePoly.GetSize());

        rOStm.WriteInt32( nSize );

        for( sal_uInt16 j = 0; j < nSize; j++ )
            WritePair( rOStm, aSimplePoly[ j ] );
    }
}

void ImplReadColor( SvStream& rIStm, Color& rColor )
{
    sal_Int16 nVal(0);

    rIStm.ReadInt16( nVal ); rColor.SetRed( sal::static_int_cast<sal_uInt8>((sal_uInt16)nVal >> 8) );
    rIStm.ReadInt16( nVal ); rColor.SetGreen( sal::static_int_cast<sal_uInt8>((sal_uInt16)nVal >> 8) );
    rIStm.ReadInt16( nVal ); rColor.SetBlue( sal::static_int_cast<sal_uInt8>((sal_uInt16)nVal >> 8) );
}

void ImplWriteColor( SvStream& rOStm, const Color& rColor )
{
    sal_Int16 nVal;

    nVal = ( (sal_Int16) rColor.GetRed() << 8 ) | rColor.GetRed();
    rOStm.WriteInt16( nVal );

    nVal = ( (sal_Int16) rColor.GetGreen() << 8 ) | rColor.GetGreen();
    rOStm.WriteInt16( nVal );

    nVal = ( (sal_Int16) rColor.GetBlue() << 8 ) | rColor.GetBlue();
    rOStm.WriteInt16( nVal );
}

bool ImplReadMapMode(SvStream& rIStm, MapMode& rMapMode)
{
    sal_Int16 nUnit(0);
    rIStm.ReadInt16(nUnit);

    Point aOrg;
    ReadPair(rIStm, aOrg);

    sal_Int32 nXNum(0), nXDenom(0), nYNum(0), nYDenom(0);
    rIStm.ReadInt32(nXNum).ReadInt32(nXDenom).ReadInt32(nYNum).ReadInt32(nYDenom);

    if (!rIStm.good() || nXDenom <= 0 || nYDenom <= 0 || nXNum <= 0 || nYNum <= 0)
    {
        SAL_WARN("vcl.gdi", "Parsing error: invalid mapmode fraction");
        return false;
    }

    if (nUnit < sal_Int16(MapUnit::Map100thMM) || nUnit > sal_Int16(MapUnit::LAST))
    {
        SAL_WARN("vcl.gdi", "Parsing error: invalid mapmode");
        return false;
    }

    rMapMode = MapMode((MapUnit) nUnit, aOrg, Fraction(nXNum, nXDenom), Fraction(nYNum, nYDenom));

    return true;
}

void ImplWriteMapMode( SvStream& rOStm, const MapMode& rMapMode )
{
    rOStm.WriteInt16( (sal_uInt16)rMapMode.GetMapUnit() );
    WritePair( rOStm, rMapMode.GetOrigin() );
    rOStm.WriteInt32( rMapMode.GetScaleX().GetNumerator() );
    rOStm.WriteInt32( rMapMode.GetScaleX().GetDenominator() );
    rOStm.WriteInt32( rMapMode.GetScaleY().GetNumerator() );
    rOStm.WriteInt32( rMapMode.GetScaleY().GetDenominator() );
}

void ImplWritePushAction( SvStream& rOStm )
{
    rOStm.WriteInt16( GDI_PUSH_ACTION );
    rOStm.WriteInt32( 4 );
}

void ImplWritePopAction( SvStream& rOStm )
{
    rOStm.WriteInt16( GDI_POP_ACTION );
    rOStm.WriteInt32( 4 );
}

void ImplWriteLineColor( SvStream& rOStm, const Color& rColor, sal_Int16 nStyle, sal_Int32 nWidth = 0 )
{
    if( rColor.GetTransparency() > 127 )
        nStyle = 0;

    rOStm.WriteInt16( GDI_PEN_ACTION );
    rOStm.WriteInt32( 16 );
    ImplWriteColor( rOStm, rColor );
    rOStm.WriteInt32( nWidth );
    rOStm.WriteInt16( nStyle );
}

void ImplWriteFillColor( SvStream& rOStm, const Color& rColor, sal_Int16 nStyle )
{
    rOStm.WriteInt16( GDI_FILLBRUSH_ACTION );
    rOStm.WriteInt32( 20 );
    ImplWriteColor( rOStm, rColor );

    if( rColor.GetTransparency() > 127 )
        nStyle = 0;

    if( nStyle > 1 )
    {
        ImplWriteColor( rOStm, COL_WHITE );
        rOStm.WriteInt16( nStyle );
        rOStm.WriteInt16( 1 );
    }
    else
    {
        ImplWriteColor( rOStm, COL_BLACK );
        rOStm.WriteInt16( nStyle );
        rOStm.WriteInt16( 0 );
    }
}

void ImplWriteFont( SvStream& rOStm, const vcl::Font& rFont,
                    rtl_TextEncoding& rActualCharSet )
{
    char    aName[33];
    short   nWeight;

    OString aByteName(OUStringToOString(rFont.GetFamilyName(),
        rOStm.GetStreamCharSet()));
    strncpy( aName, aByteName.getStr(), 32 );
    aName[32] = 0;

    switch ( rFont.GetWeight() )
    {
        case WEIGHT_THIN:
        case WEIGHT_ULTRALIGHT:
        case WEIGHT_LIGHT:
            nWeight = 1;
        break;

        case WEIGHT_NORMAL:
        case WEIGHT_MEDIUM:
            nWeight = 2;
        break;

        case WEIGHT_BOLD:
        case WEIGHT_ULTRABOLD:
        case WEIGHT_BLACK:
            nWeight = 3;
        break;

        default:
            nWeight = 0;
        break;
    }

    rOStm.WriteInt16( GDI_FONT_ACTION );
    rOStm.WriteInt32( 78 );

    rActualCharSet = GetStoreCharSet( rFont.GetCharSet() );
    ImplWriteColor( rOStm, rFont.GetColor() );
    ImplWriteColor( rOStm, rFont.GetFillColor() );
    rOStm.WriteBytes( aName, 32 );
    WritePair( rOStm, rFont.GetFontSize() );
    rOStm.WriteInt16( 0 ); // no character orientation anymore
    rOStm.WriteInt16( rFont.GetOrientation() );
    rOStm.WriteInt16( rActualCharSet );
    rOStm.WriteInt16( rFont.GetFamilyType() );
    rOStm.WriteInt16( rFont.GetPitch() );
    rOStm.WriteInt16( rFont.GetAlignment() );
    rOStm.WriteInt16( nWeight );
    rOStm.WriteInt16( rFont.GetUnderline() );
    rOStm.WriteInt16( rFont.GetStrikeout() );
    rOStm.WriteBool( rFont.GetItalic() != ITALIC_NONE );
    rOStm.WriteBool( rFont.IsOutline() );
    rOStm.WriteBool( rFont.IsShadow() );
    rOStm.WriteBool( rFont.IsTransparent() );
    if ( rActualCharSet == RTL_TEXTENCODING_DONTKNOW )
        rActualCharSet = osl_getThreadTextEncoding();
}

void ImplWriteRasterOpAction( SvStream& rOStm, sal_Int16 nRasterOp )
{
    rOStm.WriteInt16( GDI_RASTEROP_ACTION ).WriteInt32( 6 ).WriteInt16( nRasterOp );
}

bool ImplWriteUnicodeComment( SvStream& rOStm, const OUString& rString )
{
    sal_Int32 nStringLen = rString.getLength();
    if ( nStringLen )
    {
        sal_uInt32  nSize = ( nStringLen << 1 ) + 4;
        rOStm.WriteUInt16( GDI_UNICODE_COMMENT ).WriteUInt32( nSize );
        write_uInt16s_FromOUString(rOStm, rString);
    }
    return nStringLen != 0;
}

void ImplReadUnicodeComment( sal_uInt32 nStrmPos, SvStream& rIStm, OUString& rString )
{
    sal_uInt32 nOld = rIStm.Tell();
    if ( nStrmPos )
    {
        sal_uInt16  nType;
        sal_uInt32  nActionSize;
        std::size_t nStringLen;

        rIStm.Seek( nStrmPos );
        rIStm  .ReadUInt16( nType )
               .ReadUInt32( nActionSize );

        nStringLen = (nActionSize - 4) >> 1;

        if ( nStringLen && ( nType == GDI_UNICODE_COMMENT ) )
            rString = read_uInt16s_ToOUString(rIStm, nStringLen);
    }
    rIStm.Seek( nOld );
}

void ImplSkipActions(SvStream& rIStm, sal_uLong nSkipCount)
{
    sal_Int32 nActionSize;
    sal_Int16 nType;
    for (sal_uLong i = 0; i < nSkipCount; ++i)
    {
        rIStm.ReadInt16(nType).ReadInt32(nActionSize);
        if (!rIStm.good() || nActionSize < 4)
            break;
        rIStm.SeekRel(nActionSize - 4);
    }
}

bool ImplWriteExtendedPolyPolygonAction(SvStream& rOStm, const tools::PolyPolygon& rPolyPolygon)
{
    const sal_uInt16 nPolygonCount(rPolyPolygon.Count());

    if(nPolygonCount)
    {
        sal_uInt32 nAllPolygonCount(0);
        sal_uInt32 nAllPointCount(0);
        sal_uInt32 nAllFlagCount(0);
        sal_uInt16 a(0);

        for(a = 0; a < nPolygonCount; a++)
        {
            const tools::Polygon& rCandidate = rPolyPolygon.GetObject(a);
            const sal_uInt16 nPointCount(rCandidate.GetSize());

            if(nPointCount)
            {
                nAllPolygonCount++;
                nAllPointCount += nPointCount;

                if(rCandidate.HasFlags())
                {
                    nAllFlagCount += nPointCount;
                }
            }
        }

        if(nAllFlagCount)
        {
            rOStm.WriteInt16( GDI_EXTENDEDPOLYGON_ACTION );

            const sal_Int32 nActionSize(
                4 +                         // Action size
                2 +                         // PolygonCount
                (nAllPolygonCount * 2) +    // Points per polygon
                (nAllPointCount << 3) +     // Points themselves
                nAllPolygonCount +          // Bool if (when poly has points) it has flags, too
                nAllFlagCount);             // Flags themselves

            rOStm.WriteInt32( nActionSize );
            rOStm.WriteUInt16( nAllPolygonCount );

            for(a = 0; a < nPolygonCount; a++)
            {
                const tools::Polygon& rCandidate = rPolyPolygon.GetObject(a);
                const sal_uInt16 nPointCount(rCandidate.GetSize());

                if(nPointCount)
                {
                    rOStm.WriteUInt16( nPointCount );

                    for(sal_uInt16 b(0); b < nPointCount; b++)
                    {
                        WritePair( rOStm, rCandidate[b] );
                    }

                    if(rCandidate.HasFlags())
                    {
                        rOStm.WriteBool( true );

                        for(sal_uInt16 c(0); c < nPointCount; c++)
                        {
                            rOStm.WriteUChar( (sal_uInt8)rCandidate.GetFlags(c) );
                        }
                    }
                    else
                    {
                        rOStm.WriteBool( false );
                    }
                }
            }

            return true;
        }
    }

    return false;
}

void ImplReadExtendedPolyPolygonAction(SvStream& rIStm, tools::PolyPolygon& rPolyPoly)
{
    rPolyPoly.Clear();
    sal_uInt16 nPolygonCount(0);
    rIStm.ReadUInt16( nPolygonCount );

    if (!nPolygonCount)
        return;

    const size_t nMinRecordSize = sizeof(sal_uInt16);
    const size_t nMaxRecords = rIStm.remainingSize() / nMinRecordSize;
    if (nPolygonCount > nMaxRecords)
    {
        SAL_WARN("vcl.gdi", "Parsing error: " << nMaxRecords <<
                 " max possible entries, but " << nPolygonCount << " claimed, truncating");
        nPolygonCount = nMaxRecords;
    }

    for(sal_uInt16 a(0); a < nPolygonCount; a++)
    {
        sal_uInt16 nPointCount(0);
        rIStm.ReadUInt16(nPointCount);

        const size_t nMinPolygonSize = sizeof(sal_Int32) * 2;
        const size_t nMaxPolygons = rIStm.remainingSize() / nMinPolygonSize;
        if (nPointCount > nMaxPolygons)
        {
            SAL_WARN("vcl.gdi", "Parsing error: " << nMaxPolygons <<
                     " max possible entries, but " << nPointCount << " claimed, truncating");
            nPointCount = nMaxPolygons;
        }

        tools::Polygon aCandidate(nPointCount);

        if (nPointCount)
        {
            for(sal_uInt16 b(0); b < nPointCount; b++)
            {
                ReadPair( rIStm , aCandidate[b] );
            }

            sal_uInt8 bHasFlags(int(false));
            rIStm.ReadUChar( bHasFlags );

            if(bHasFlags)
            {
                sal_uInt8 aPolyFlags(0);

                for(sal_uInt16 c(0); c < nPointCount; c++)
                {
                    rIStm.ReadUChar( aPolyFlags );
                    aCandidate.SetFlags(c, (PolyFlags)aPolyFlags);
                }
            }
        }

        rPolyPoly.Insert(aCandidate);
    }
}

SVMConverter::SVMConverter( SvStream& rStm, GDIMetaFile& rMtf, sal_uLong nConvertMode )
{
    if( !rStm.GetError() )
    {
        if( CONVERT_FROM_SVM1 == nConvertMode )
            ImplConvertFromSVM1( rStm, rMtf );
        else if( CONVERT_TO_SVM1 == nConvertMode )
            ImplConvertToSVM1( rStm, rMtf );
    }
}

namespace
{
    sal_Int32 SkipActions(sal_Int32 i, sal_Int32 nFollowingActionCount, sal_Int32 nActions)
    {
        sal_Int32 remainingActions = nActions - i;
        if (nFollowingActionCount < 0)
            nFollowingActionCount = remainingActions;
        return std::min(remainingActions, nFollowingActionCount);
    }
}

#define LF_FACESIZE 32

void SVMConverter::ImplConvertFromSVM1( SvStream& rIStm, GDIMetaFile& rMtf )
{
    const sal_uLong         nPos = rIStm.Tell();
    const SvStreamEndian    nOldFormat = rIStm.GetEndian();

    rIStm.SetEndian( SvStreamEndian::LITTLE );

    char    aCode[ 5 ];
    Size    aPrefSz;

    // read header
    rIStm.ReadBytes(aCode, sizeof(aCode));  // Identifier
    sal_Int16 nSize(0);
    rIStm.ReadInt16( nSize );                                 // Size
    sal_Int16 nVersion(0);
    rIStm.ReadInt16( nVersion );                              // Version
    sal_Int32 nTmp32(0);
    rIStm.ReadInt32( nTmp32 );
    aPrefSz.Width() = nTmp32;                       // PrefSize.Width()
    rIStm.ReadInt32( nTmp32 );
    aPrefSz.Height() = nTmp32;                      // PrefSize.Height()

    // check header-magic and version
    if( rIStm.GetError()
        || ( memcmp( aCode, "SVGDI", sizeof( aCode ) ) != 0 )
        || ( nVersion != 200 ) )
    {
        rIStm.SetError( SVSTREAM_FILEFORMAT_ERROR );
        rIStm.SetEndian( nOldFormat );
        rIStm.Seek( nPos );
        return;
    }

    LineInfo            aLineInfo( LineStyle::NONE, 0 );
    std::stack<std::unique_ptr<LineInfo>> aLIStack;
    ScopedVclPtrInstance< VirtualDevice > aFontVDev;
    rtl_TextEncoding    eActualCharSet = osl_getThreadTextEncoding();
    bool                bFatLine = false;

    tools::Polygon     aActionPoly;
    tools::Rectangle   aRect;
    Point       aPt, aPt1;
    Size        aSz;
    Color       aActionColor;

    sal_uInt32  nUnicodeCommentStreamPos = 0;
    sal_Int32       nUnicodeCommentActionNumber = 0;

    rMtf.SetPrefSize(aPrefSz);

    MapMode aMapMode;
    if (ImplReadMapMode(rIStm, aMapMode))           // MapMode
        rMtf.SetPrefMapMode(aMapMode);

    sal_Int32 nActions(0);
    rIStm.ReadInt32(nActions);                      // Action count
    if (nActions < 0)
    {
        SAL_WARN("vcl.gdi", "svm claims negative action count (" << nActions << ")");
        nActions = 0;
    }

    const size_t nMinActionSize = sizeof(sal_uInt16) + sizeof(sal_Int32);
    const size_t nMaxPossibleActions = rIStm.remainingSize() / nMinActionSize;
    if (static_cast<sal_uInt32>(nActions) > nMaxPossibleActions)
    {
        SAL_WARN("vcl.gdi", "svm claims more actions (" << nActions << ") than stream could provide, truncating");
        nActions = nMaxPossibleActions;
    }

    size_t nLastPolygonAction(0);

    for (sal_Int32 i = 0; i < nActions && rIStm.good(); ++i)
    {
        sal_Int16 nType(0);
        rIStm.ReadInt16(nType);
        sal_Int32 nActBegin = rIStm.Tell();
        sal_Int32 nActionSize(0);
        rIStm.ReadInt32(nActionSize);

        SAL_WARN_IF( ( nType > 33 ) && ( nType < 1024 ), "vcl.gdi", "Unknown GDIMetaAction while converting!" );

        switch( nType )
        {
            case GDI_PIXEL_ACTION:
            {
                ReadPair( rIStm, aPt );
                ImplReadColor( rIStm, aActionColor );
                rMtf.AddAction( new MetaPixelAction( aPt, aActionColor ) );
            }
            break;

            case GDI_POINT_ACTION:
            {
                ReadPair( rIStm, aPt );
                rMtf.AddAction( new MetaPointAction( aPt ) );
            }
            break;

            case GDI_LINE_ACTION:
            {
                ReadPair( rIStm, aPt );
                ReadPair( rIStm, aPt1 );
                rMtf.AddAction( new MetaLineAction( aPt, aPt1, aLineInfo ) );
            }
            break;

            case GDI_LINEJOIN_ACTION :
            {
                sal_Int16 nLineJoin(0);
                rIStm.ReadInt16( nLineJoin );
                aLineInfo.SetLineJoin((basegfx::B2DLineJoin)nLineJoin);
            }
            break;

            case GDI_LINECAP_ACTION :
            {
                sal_Int16 nLineCap(0);
                rIStm.ReadInt16( nLineCap );
                aLineInfo.SetLineCap((css::drawing::LineCap)nLineCap);
            }
            break;

            case GDI_LINEDASHDOT_ACTION :
            {
                sal_Int16 a(0);
                sal_Int32 b(0);

                rIStm.ReadInt16( a ); aLineInfo.SetDashCount(a);
                rIStm.ReadInt32( b ); aLineInfo.SetDashLen(b);
                rIStm.ReadInt16( a ); aLineInfo.SetDotCount(a);
                rIStm.ReadInt32( b ); aLineInfo.SetDotLen(b);
                rIStm.ReadInt32( b ); aLineInfo.SetDistance(b);

                if(((aLineInfo.GetDashCount() && aLineInfo.GetDashLen())
                    || (aLineInfo.GetDotCount() && aLineInfo.GetDotLen()))
                    && aLineInfo.GetDistance())
                {
                    aLineInfo.SetStyle(LineStyle::Dash);
                }
            }
            break;

            case GDI_EXTENDEDPOLYGON_ACTION :
            {
                // read the tools::PolyPolygon in every case
                tools::PolyPolygon aInputPolyPolygon;
                ImplReadExtendedPolyPolygonAction(rIStm, aInputPolyPolygon);

                // now check if it can be set somewhere
                if(nLastPolygonAction < rMtf.GetActionSize())
                {
                    MetaPolyLineAction* pPolyLineAction = dynamic_cast< MetaPolyLineAction* >(rMtf.GetAction(nLastPolygonAction));

                    if(pPolyLineAction)
                    {
                        // replace MetaPolyLineAction when we have a single polygon. Do not rely on the
                        // same point count; the originally written GDI_POLYLINE_ACTION may have been
                        // Subdivided for better quality for older usages
                        if(1 == aInputPolyPolygon.Count())
                        {
                            MetaAction* pAction = rMtf.ReplaceAction(
                                new MetaPolyLineAction(
                                    aInputPolyPolygon.GetObject(0),
                                    pPolyLineAction->GetLineInfo()),
                                nLastPolygonAction);
                            if(pAction)
                                pAction->Delete();
                        }
                    }
                    else
                    {
                        MetaPolyPolygonAction* pPolyPolygonAction = dynamic_cast< MetaPolyPolygonAction* >(rMtf.GetAction(nLastPolygonAction));

                        if(pPolyPolygonAction)
                        {
                            // replace MetaPolyPolygonAction when we have a curved polygon. Do rely on the
                            // same sub-polygon count
                            if(pPolyPolygonAction->GetPolyPolygon().Count() == aInputPolyPolygon.Count())
                            {
                                MetaAction* pAction = rMtf.ReplaceAction(
                                    new MetaPolyPolygonAction(
                                        aInputPolyPolygon),
                                    nLastPolygonAction);
                                if(pAction)
                                    pAction->Delete();
                            }
                        }
                        else
                        {
                            MetaPolygonAction* pPolygonAction = dynamic_cast< MetaPolygonAction* >(rMtf.GetAction(nLastPolygonAction));

                            if(pPolygonAction)
                            {
                                // replace MetaPolygonAction
                                if(1 == aInputPolyPolygon.Count())
                                {
                                    MetaAction* pAction = rMtf.ReplaceAction(
                                        new MetaPolygonAction(
                                            aInputPolyPolygon.GetObject(0)),
                                        nLastPolygonAction);
                                    if(pAction)
                                        pAction->Delete();
                                }
                            }
                        }
                    }
                }
            }
            break;

            case GDI_RECT_ACTION:
            {
                ImplReadRect( rIStm, aRect );
                sal_Int32 nTmp(0), nTmp1(0);
                rIStm.ReadInt32( nTmp ).ReadInt32( nTmp1 );

                if( nTmp || nTmp1 )
                    rMtf.AddAction( new MetaRoundRectAction( aRect, nTmp, nTmp1 ) );
                else
                {
                    rMtf.AddAction( new MetaRectAction( aRect ) );

                    if( bFatLine )
                        rMtf.AddAction( new MetaPolyLineAction( aRect, aLineInfo ) );
                }
            }
            break;

            case GDI_ELLIPSE_ACTION:
            {
                ImplReadRect( rIStm, aRect );

                if( bFatLine )
                {
                    const tools::Polygon aPoly( aRect.Center(), aRect.GetWidth() >> 1, aRect.GetHeight() >> 1 );

                    rMtf.AddAction( new MetaPushAction( PushFlags::LINECOLOR ) );
                    rMtf.AddAction( new MetaLineColorAction( COL_TRANSPARENT, false ) );
                    rMtf.AddAction( new MetaPolygonAction( aPoly ) );
                    rMtf.AddAction( new MetaPopAction() );
                    rMtf.AddAction( new MetaPolyLineAction( aPoly, aLineInfo ) );
                }
                else
                    rMtf.AddAction( new MetaEllipseAction( aRect ) );
            }
            break;

            case GDI_ARC_ACTION:
            {
                ImplReadRect( rIStm, aRect );
                ReadPair( rIStm, aPt );
                ReadPair( rIStm, aPt1 );

                if( bFatLine )
                {
                    const tools::Polygon aPoly( aRect, aPt, aPt1, PolyStyle::Arc );

                    rMtf.AddAction( new MetaPushAction( PushFlags::LINECOLOR ) );
                    rMtf.AddAction( new MetaLineColorAction( COL_TRANSPARENT, false ) );
                    rMtf.AddAction( new MetaPolygonAction( aPoly ) );
                    rMtf.AddAction( new MetaPopAction() );
                    rMtf.AddAction( new MetaPolyLineAction( aPoly, aLineInfo ) );
                }
                else
                    rMtf.AddAction( new MetaArcAction( aRect, aPt, aPt1 ) );
            }
            break;

            case GDI_PIE_ACTION:
            {
                ImplReadRect( rIStm, aRect );
                ReadPair( rIStm, aPt );
                ReadPair( rIStm, aPt1 );

                if( bFatLine )
                {
                    const tools::Polygon aPoly( aRect, aPt, aPt1, PolyStyle::Pie );

                    rMtf.AddAction( new MetaPushAction( PushFlags::LINECOLOR ) );
                    rMtf.AddAction( new MetaLineColorAction( COL_TRANSPARENT, false ) );
                    rMtf.AddAction( new MetaPolygonAction( aPoly ) );
                    rMtf.AddAction( new MetaPopAction() );
                    rMtf.AddAction( new MetaPolyLineAction( aPoly, aLineInfo ) );
                }
                else
                    rMtf.AddAction( new MetaPieAction( aRect, aPt, aPt1 ) );
            }
            break;

            case GDI_INVERTRECT_ACTION:
            case GDI_HIGHLIGHTRECT_ACTION:
            {
                ImplReadRect( rIStm, aRect );
                rMtf.AddAction( new MetaPushAction( PushFlags::RASTEROP ) );
                rMtf.AddAction( new MetaRasterOpAction( RasterOp::Invert ) );
                rMtf.AddAction( new MetaRectAction( aRect ) );
                rMtf.AddAction( new MetaPopAction() );
            }
            break;

            case GDI_POLYLINE_ACTION:
            {
                if (ImplReadPoly(rIStm, aActionPoly))
                {
                    nLastPolygonAction = rMtf.GetActionSize();

                    if( bFatLine )
                        rMtf.AddAction( new MetaPolyLineAction( aActionPoly, aLineInfo ) );
                    else
                        rMtf.AddAction( new MetaPolyLineAction( aActionPoly ) );
                }
            }
            break;

            case GDI_POLYGON_ACTION:
            {
                if (ImplReadPoly(rIStm, aActionPoly))
                {
                    if( bFatLine )
                    {
                        rMtf.AddAction( new MetaPushAction( PushFlags::LINECOLOR ) );
                        rMtf.AddAction( new MetaLineColorAction( COL_TRANSPARENT, false ) );
                        rMtf.AddAction( new MetaPolygonAction( aActionPoly ) );
                        rMtf.AddAction( new MetaPopAction() );
                        rMtf.AddAction( new MetaPolyLineAction( aActionPoly, aLineInfo ) );
                    }
                    else
                    {
                        nLastPolygonAction = rMtf.GetActionSize();
                        rMtf.AddAction( new MetaPolygonAction( aActionPoly ) );
                    }
                }
            }
            break;

            case GDI_POLYPOLYGON_ACTION:
            {
                tools::PolyPolygon aPolyPoly;

                if (ImplReadPolyPoly(rIStm, aPolyPoly))
                {
                    if( bFatLine )
                    {
                        rMtf.AddAction( new MetaPushAction( PushFlags::LINECOLOR ) );
                        rMtf.AddAction( new MetaLineColorAction( COL_TRANSPARENT, false ) );
                        rMtf.AddAction( new MetaPolyPolygonAction( aPolyPoly ) );
                        rMtf.AddAction( new MetaPopAction() );

                        for( sal_uInt16 nPoly = 0, nCount = aPolyPoly.Count(); nPoly < nCount; nPoly++ )
                            rMtf.AddAction( new MetaPolyLineAction( aPolyPoly[ nPoly ], aLineInfo ) );
                    }
                    else
                    {
                        nLastPolygonAction = rMtf.GetActionSize();
                        rMtf.AddAction( new MetaPolyPolygonAction( aPolyPoly ) );
                    }
                }
            }
            break;

            case GDI_FONT_ACTION:
            {
                vcl::Font   aFont;
                char        aName[LF_FACESIZE+1];

                ImplReadColor( rIStm, aActionColor ); aFont.SetColor( aActionColor );
                ImplReadColor( rIStm, aActionColor ); aFont.SetFillColor( aActionColor );
                size_t nRet = rIStm.ReadBytes(aName, LF_FACESIZE);
                aName[nRet] = 0;
                aFont.SetFamilyName( OUString( aName, strlen(aName), rIStm.GetStreamCharSet() ) );

                sal_Int32 nWidth(0), nHeight(0);
                rIStm.ReadInt32(nWidth).ReadInt32(nHeight);
                sal_Int16 nCharOrient(0), nLineOrient(0);
                rIStm.ReadInt16(nCharOrient).ReadInt16(nLineOrient);
                sal_Int16 nCharSet(0), nFamily(0), nPitch(0), nAlign(0), nWeight(0), nUnderline(0), nStrikeout(0);
                rIStm.ReadInt16(nCharSet).ReadInt16(nFamily).ReadInt16(nPitch).ReadInt16(nAlign).ReadInt16(nWeight).ReadInt16(nUnderline).ReadInt16(nStrikeout);
                bool bItalic(false), bOutline(false), bShadow(false), bTransparent(false);
                rIStm.ReadCharAsBool(bItalic).ReadCharAsBool(bOutline).ReadCharAsBool(bShadow).ReadCharAsBool(bTransparent);

                aFont.SetFontSize( Size( nWidth, nHeight ) );
                aFont.SetCharSet( (rtl_TextEncoding) nCharSet );
                aFont.SetFamily( (FontFamily) nFamily );
                aFont.SetPitch( (FontPitch) nPitch );
                aFont.SetAlignment( (FontAlign) nAlign );
                aFont.SetWeight( ( nWeight == 1 ) ? WEIGHT_LIGHT : ( nWeight == 2 ) ? WEIGHT_NORMAL :
                                 ( nWeight == 3 ) ? WEIGHT_BOLD : WEIGHT_DONTKNOW );
                aFont.SetUnderline( (FontLineStyle) nUnderline );
                aFont.SetStrikeout( (FontStrikeout) nStrikeout );
                aFont.SetItalic( bItalic ? ITALIC_NORMAL : ITALIC_NONE );
                aFont.SetOutline( bOutline );
                aFont.SetShadow( bShadow );
                aFont.SetOrientation( nLineOrient );
                aFont.SetTransparent( bTransparent );

                eActualCharSet = aFont.GetCharSet();
                if ( eActualCharSet == RTL_TEXTENCODING_DONTKNOW )
                    eActualCharSet = osl_getThreadTextEncoding();

                rMtf.AddAction( new MetaFontAction( aFont ) );
                rMtf.AddAction( new MetaTextAlignAction( aFont.GetAlignment() ) );
                rMtf.AddAction( new MetaTextColorAction( aFont.GetColor() ) );
                rMtf.AddAction( new MetaTextFillColorAction( aFont.GetFillColor(), !aFont.IsTransparent() ) );

                // #106172# Track font relevant data in shadow VDev
                aFontVDev->SetFont( aFont );
            }
            break;

            case GDI_TEXT_ACTION:
            {
                sal_Int32 nIndex(0), nLen(0), nTmp(0);

                ReadPair( rIStm, aPt ).ReadInt32( nIndex ).ReadInt32( nLen ).ReadInt32( nTmp );
                if (nTmp > 0)
                {
                    OString aByteStr = read_uInt8s_ToOString(rIStm, nTmp);
                    sal_uInt8 nTerminator = 0;
                    rIStm.ReadUChar( nTerminator );
                    SAL_WARN_IF( nTerminator != 0, "vcl.gdi", "expected string to be NULL terminated" );

                    OUString aStr(OStringToOUString(aByteStr, eActualCharSet));
                    if ( nUnicodeCommentActionNumber == i )
                        ImplReadUnicodeComment( nUnicodeCommentStreamPos, rIStm, aStr );
                    rMtf.AddAction( new MetaTextAction( aPt, aStr, nIndex, nLen ) );
                }

                if (nActionSize < 24)
                    rIStm.SetError(SVSTREAM_FILEFORMAT_ERROR);
                else
                    rIStm.Seek(nActBegin + nActionSize);
            }
            break;

            case GDI_TEXTARRAY_ACTION:
            {
                sal_Int32 nIndex(0), nLen(0), nAryLen(0), nTmp(0);

                ReadPair( rIStm, aPt ).ReadInt32( nIndex ).ReadInt32( nLen ).ReadInt32( nTmp ).ReadInt32( nAryLen );
                if (nTmp > 0)
                {
                    OString aByteStr = read_uInt8s_ToOString(rIStm, nTmp);
                    sal_uInt8 nTerminator = 0;
                    rIStm.ReadUChar( nTerminator );
                    SAL_WARN_IF( nTerminator != 0, "vcl.gdi", "expected string to be NULL terminated" );

                    OUString aStr(OStringToOUString(aByteStr, eActualCharSet));

                    std::unique_ptr<long[]> pDXAry;
                    sal_Int32 nDXAryLen = 0;
                    if (nAryLen > 0)
                    {
                        const size_t nMinRecordSize = sizeof(sal_Int32);
                        const size_t nMaxRecords = rIStm.remainingSize() / nMinRecordSize;
                        if (static_cast<sal_uInt32>(nAryLen) > nMaxRecords)
                        {
                            SAL_WARN("vcl.gdi", "Parsing error: " << nMaxRecords <<
                                     " max possible entries, but " << nAryLen << " claimed, truncating");
                            nAryLen = nMaxRecords;
                        }

                        sal_Int32 nStrLen( aStr.getLength() );

                        nDXAryLen = std::max(nAryLen, nStrLen);

                        if (nDXAryLen < nLen)
                        {
                            //MetaTextArrayAction ctor expects pDXAry to be >= nLen if set, so if this can't
                            //be achieved, don't read it, it's utterly broken.
                            SAL_WARN("vcl.gdi", "dxary too short, discarding completely");
                            rIStm.SeekRel(sizeof(sal_Int32) * nDXAryLen);
                            nLen = 0;
                            nIndex = 0;
                        }
                        else
                        {
                            pDXAry.reset(new long[nDXAryLen]);

                            for (sal_Int32 j = 0; j < nAryLen; ++j)
                            {
                                rIStm.ReadInt32( nTmp );
                                pDXAry[ j ] = nTmp;
                            }

                            // #106172# Add last DX array elem, if missing
                            if( nAryLen != nStrLen )
                            {
                                if (nAryLen+1 == nStrLen && nIndex >= 0)
                                {
                                    std::unique_ptr<long[]> pTmpAry(new long[nStrLen]);

                                    aFontVDev->GetTextArray( aStr, pTmpAry.get(), nIndex, nLen );

                                    // now, the difference between the
                                    // last and the second last DX array
                                    // is the advancement for the last
                                    // glyph. Thus, to complete our meta
                                    // action's DX array, just add that
                                    // difference to last elem and store
                                    // in very last.
                                    if( nStrLen > 1 )
                                        pDXAry[ nStrLen-1 ] = pDXAry[ nStrLen-2 ] + pTmpAry[ nStrLen-1 ] - pTmpAry[ nStrLen-2 ];
                                    else
                                        pDXAry[ nStrLen-1 ] = pTmpAry[ nStrLen-1 ]; // len=1: 0th position taken to be 0
                                }
#ifdef DBG_UTIL
                                else
                                    OSL_FAIL("More than one DX array element missing on SVM import");
#endif
                            }
                        }
                    }
                    if ( nUnicodeCommentActionNumber == i )
                        ImplReadUnicodeComment( nUnicodeCommentStreamPos, rIStm, aStr );
                    rMtf.AddAction( new MetaTextArrayAction( aPt, aStr, pDXAry.get(), nIndex, nLen ) );
                }

                if (nActionSize < 24)
                    rIStm.SetError(SVSTREAM_FILEFORMAT_ERROR);
                else
                    rIStm.Seek(nActBegin + nActionSize);
            }
            break;

            case GDI_STRETCHTEXT_ACTION:
            {
                sal_Int32 nIndex(0), nLen(0), nWidth(0), nTmp(0);

                ReadPair( rIStm, aPt ).ReadInt32( nIndex ).ReadInt32( nLen ).ReadInt32( nTmp ).ReadInt32( nWidth );
                if (nTmp > 0)
                {
                    OString aByteStr = read_uInt8s_ToOString(rIStm, nTmp);
                    sal_uInt8 nTerminator = 0;
                    rIStm.ReadUChar( nTerminator );
                    SAL_WARN_IF( nTerminator != 0, "vcl.gdi", "expected string to be NULL terminated" );

                    OUString aStr(OStringToOUString(aByteStr, eActualCharSet));
                    if ( nUnicodeCommentActionNumber == i )
                        ImplReadUnicodeComment( nUnicodeCommentStreamPos, rIStm, aStr );
                    rMtf.AddAction( new MetaStretchTextAction( aPt, nWidth, aStr, nIndex, nLen ) );
                }

                if (nActionSize < 28)
                    rIStm.SetError(SVSTREAM_FILEFORMAT_ERROR);
                else
                    rIStm.Seek(nActBegin + nActionSize);
            }
            break;

            case GDI_BITMAP_ACTION:
            {
                Bitmap aBmp;

                ReadPair( rIStm, aPt );
                ReadDIB(aBmp, rIStm, true);
                rMtf.AddAction( new MetaBmpAction( aPt, aBmp ) );
            }
            break;

            case GDI_BITMAPSCALE_ACTION:
            {
                Bitmap aBmp;

                ReadPair( rIStm, aPt );
                ReadPair( rIStm, aSz );
                ReadDIB(aBmp, rIStm, true);
                rMtf.AddAction( new MetaBmpScaleAction( aPt, aSz, aBmp ) );
            }
            break;

            case GDI_BITMAPSCALEPART_ACTION:
            {
                Bitmap  aBmp;
                Size    aSz2;

                ReadPair( rIStm, aPt );
                ReadPair( rIStm, aSz );
                ReadPair( rIStm, aPt1 );
                ReadPair( rIStm, aSz2 );
                ReadDIB(aBmp, rIStm, true);
                rMtf.AddAction( new MetaBmpScalePartAction( aPt, aSz, aPt1, aSz2, aBmp ) );
            }
            break;

            case GDI_PEN_ACTION:
            {
                sal_Int32 nPenWidth;
                sal_Int16 nPenStyle;

                ImplReadColor( rIStm, aActionColor );
                rIStm.ReadInt32( nPenWidth ).ReadInt16( nPenStyle );

                aLineInfo.SetStyle( nPenStyle ? LineStyle::Solid : LineStyle::NONE );
                aLineInfo.SetWidth( nPenWidth );
                bFatLine = nPenStyle && !aLineInfo.IsDefault();

                rMtf.AddAction( new MetaLineColorAction( aActionColor, nPenStyle != 0 ) );
            }
            break;

            case GDI_FILLBRUSH_ACTION:
            {
                sal_Int16 nBrushStyle;

                ImplReadColor( rIStm, aActionColor );
                rIStm.SeekRel( 6 );
                rIStm.ReadInt16( nBrushStyle );
                rMtf.AddAction( new MetaFillColorAction( aActionColor, nBrushStyle != 0 ) );
                rIStm.SeekRel( 2 );
            }
            break;

            case GDI_MAPMODE_ACTION:
            {
                if (ImplReadMapMode(rIStm, aMapMode))
                {
                    rMtf.AddAction(new MetaMapModeAction(aMapMode));

                    // #106172# Track font relevant data in shadow VDev
                    aFontVDev->SetMapMode(aMapMode);
                };
            }
            break;

            case GDI_CLIPREGION_ACTION:
            {
                vcl::Region  aRegion;
                sal_Int16   nRegType;
                sal_Int16   bIntersect;
                bool    bClip = false;

                rIStm.ReadInt16( nRegType ).ReadInt16( bIntersect );
                ImplReadRect( rIStm, aRect );

                switch( nRegType )
                {
                    case 0:
                    break;

                    case 1:
                    {
                        tools::Rectangle aRegRect;

                        ImplReadRect( rIStm, aRegRect );
                        aRegion = vcl::Region( aRegRect );
                        bClip = true;
                    }
                    break;

                    case 2:
                    {
                        if (ImplReadPoly(rIStm, aActionPoly))
                        {
                            aRegion = vcl::Region( aActionPoly );
                            bClip = true;
                        }
                    }
                    break;

                    case 3:
                    {
                        bool bSuccess = true;
                        tools::PolyPolygon aPolyPoly;
                        sal_Int32 nPolyCount32(0);
                        rIStm.ReadInt32(nPolyCount32);
                        sal_uInt16 nPolyCount(nPolyCount32);

                        for (sal_uInt16 j = 0; j < nPolyCount && rIStm.good(); ++j)
                        {
                            if (!ImplReadPoly(rIStm, aActionPoly))
                            {
                                bSuccess = false;
                                break;
                            }
                            aPolyPoly.Insert(aActionPoly);
                        }

                        if (bSuccess)
                        {
                            aRegion = vcl::Region( aPolyPoly );
                            bClip = true;
                        }
                    }
                    break;
                }

                if( bIntersect )
                    aRegion.Intersect( aRect );

                rMtf.AddAction( new MetaClipRegionAction( aRegion, bClip ) );
            }
            break;

            case GDI_MOVECLIPREGION_ACTION:
            {
                sal_Int32 nTmp(0), nTmp1(0);
                rIStm.ReadInt32( nTmp ).ReadInt32( nTmp1 );
                rMtf.AddAction( new MetaMoveClipRegionAction( nTmp, nTmp1 ) );
            }
            break;

            case GDI_ISECTCLIPREGION_ACTION:
            {
                ImplReadRect( rIStm, aRect );
                rMtf.AddAction( new MetaISectRectClipRegionAction( aRect ) );
            }
            break;

            case GDI_RASTEROP_ACTION:
            {
                RasterOp    eRasterOp;
                sal_Int16       nRasterOp;

                rIStm.ReadInt16( nRasterOp );

                switch( nRasterOp )
                {
                    case 1:
                        eRasterOp = RasterOp::Invert;
                    break;

                    case 4:
                    case 5:
                        eRasterOp = RasterOp::Xor;
                    break;

                    default:
                        eRasterOp = RasterOp::OverPaint;
                    break;
                }

                rMtf.AddAction( new MetaRasterOpAction( eRasterOp ) );
            }
            break;

            case GDI_PUSH_ACTION:
            {
                aLIStack.push(o3tl::make_unique<LineInfo>(aLineInfo));
                rMtf.AddAction( new MetaPushAction( PushFlags::ALL ) );

                // #106172# Track font relevant data in shadow VDev
                aFontVDev->Push();
            }
            break;

            case GDI_POP_ACTION:
            {

                std::unique_ptr<LineInfo> xLineInfo;
                if (!aLIStack.empty())
                {
                    xLineInfo = std::move(aLIStack.top());
                    aLIStack.pop();
                }

                // restore line info
                if (xLineInfo)
                {
                    aLineInfo = *xLineInfo;
                    xLineInfo.reset();
                    bFatLine = ( LineStyle::NONE != aLineInfo.GetStyle() ) && !aLineInfo.IsDefault();
                }

                rMtf.AddAction( new MetaPopAction() );

                // #106172# Track font relevant data in shadow VDev
                aFontVDev->Pop();
            }
            break;

            case GDI_GRADIENT_ACTION:
            {
                Color   aStartCol;
                Color   aEndCol;
                sal_Int16   nStyle;
                sal_Int16   nAngle;
                sal_Int16   nBorder;
                sal_Int16   nOfsX;
                sal_Int16   nOfsY;
                sal_Int16   nIntensityStart;
                sal_Int16   nIntensityEnd;

                ImplReadRect( rIStm, aRect );
                rIStm.ReadInt16( nStyle );
                ImplReadColor( rIStm, aStartCol );
                ImplReadColor( rIStm, aEndCol );
                rIStm.ReadInt16( nAngle ).ReadInt16( nBorder ).ReadInt16( nOfsX ).ReadInt16( nOfsY ).ReadInt16( nIntensityStart ).ReadInt16( nIntensityEnd );

                Gradient aGrad( (GradientStyle) nStyle, aStartCol, aEndCol );

                aGrad.SetAngle( nAngle );
                aGrad.SetBorder( nBorder );
                aGrad.SetOfsX( nOfsX );
                aGrad.SetOfsY( nOfsY );
                aGrad.SetStartIntensity( nIntensityStart );
                aGrad.SetEndIntensity( nIntensityEnd );
                rMtf.AddAction( new MetaGradientAction( aRect, aGrad ) );
            }
            break;

            case GDI_TRANSPARENT_COMMENT:
            {
                tools::PolyPolygon aPolyPoly;
                sal_Int32 nFollowingActionCount(0);
                sal_Int16 nTrans(0);

                ReadPolyPolygon( rIStm, aPolyPoly );
                rIStm.ReadInt16( nTrans ).ReadInt32( nFollowingActionCount );
                ImplSkipActions( rIStm, nFollowingActionCount );
                rMtf.AddAction( new MetaTransparentAction( aPolyPoly, nTrans ) );

                i = SkipActions(i, nFollowingActionCount, nActions);
            }
            break;

            case GDI_FLOATTRANSPARENT_COMMENT:
            {
                GDIMetaFile aMtf;
                Point       aPos;
                Size        aSize;
                Gradient    aGradient;
                sal_Int32   nFollowingActionCount(0);

                ReadGDIMetaFile( rIStm, aMtf );
                ReadPair( rIStm, aPos );
                ReadPair( rIStm, aSize );
                ReadGradient( rIStm, aGradient );
                rIStm.ReadInt32( nFollowingActionCount );
                ImplSkipActions( rIStm, nFollowingActionCount );
                rMtf.AddAction( new MetaFloatTransparentAction( aMtf, aPos, aSize, aGradient ) );

                i = SkipActions(i, nFollowingActionCount, nActions);
            }
            break;

            case GDI_HATCH_COMMENT:
            {
                tools::PolyPolygon aPolyPoly;
                Hatch       aHatch;
                sal_Int32   nFollowingActionCount(0);

                ReadPolyPolygon( rIStm, aPolyPoly );
                ReadHatch( rIStm, aHatch );
                rIStm.ReadInt32( nFollowingActionCount );
                ImplSkipActions( rIStm, nFollowingActionCount );
                rMtf.AddAction( new MetaHatchAction( aPolyPoly, aHatch ) );

                i = SkipActions(i, nFollowingActionCount, nActions);
            }
            break;

            case GDI_REFPOINT_COMMENT:
            {
                Point   aRefPoint;
                bool    bSet;
                sal_Int32 nFollowingActionCount(0);

                ReadPair( rIStm, aRefPoint );
                rIStm.ReadCharAsBool( bSet ).ReadInt32( nFollowingActionCount );
                ImplSkipActions( rIStm, nFollowingActionCount );
                rMtf.AddAction( new MetaRefPointAction( aRefPoint, bSet ) );

                i = SkipActions(i, nFollowingActionCount, nActions);

                // #106172# Track font relevant data in shadow VDev
                if( bSet )
                    aFontVDev->SetRefPoint( aRefPoint );
                else
                    aFontVDev->SetRefPoint();
            }
            break;

            case GDI_TEXTLINECOLOR_COMMENT:
            {
                Color   aColor;
                bool    bSet;
                sal_Int32 nFollowingActionCount(0);

                ReadColor( rIStm, aColor );
                rIStm.ReadCharAsBool( bSet ).ReadInt32( nFollowingActionCount );
                ImplSkipActions( rIStm, nFollowingActionCount );
                rMtf.AddAction( new MetaTextLineColorAction( aColor, bSet ) );

                i = SkipActions(i, nFollowingActionCount, nActions);
            }
            break;

            case GDI_TEXTLINE_COMMENT:
            {
                Point   aStartPt;
                sal_Int32  nWidth(0);
                sal_uInt32 nStrikeout(0);
                sal_uInt32 nUnderline(0);
                sal_Int32  nFollowingActionCount(0);

                ReadPair( rIStm, aStartPt );
                rIStm.ReadInt32(nWidth ).ReadUInt32(nStrikeout).ReadUInt32(nUnderline).ReadInt32(nFollowingActionCount);
                ImplSkipActions(rIStm, nFollowingActionCount);
                rMtf.AddAction( new MetaTextLineAction( aStartPt, nWidth,
                                                        (FontStrikeout) nStrikeout,
                                                        (FontLineStyle) nUnderline,
                                                        LINESTYLE_NONE ) );

                i = SkipActions(i, nFollowingActionCount, nActions);
            }
            break;

            case GDI_GRADIENTEX_COMMENT:
            {
                tools::PolyPolygon aPolyPoly;
                Gradient    aGradient;
                sal_Int32 nFollowingActionCount(0);

                ReadPolyPolygon( rIStm, aPolyPoly );
                ReadGradient( rIStm, aGradient );
                rIStm.ReadInt32( nFollowingActionCount );
                ImplSkipActions( rIStm, nFollowingActionCount );
                rMtf.AddAction( new MetaGradientExAction( aPolyPoly, aGradient ) );

                i = SkipActions(i, nFollowingActionCount, nActions);
            }
            break;

            case GDI_COMMENT_COMMENT:
            {
                std::vector<sal_uInt8> aData;

                OString aComment = read_uInt16_lenPrefixed_uInt8s_ToOString(rIStm);
                sal_Int32 nValue(0);
                sal_uInt32 nDataSize(0);
                rIStm.ReadInt32(nValue).ReadUInt32(nDataSize);

                if (nDataSize)
                {
                    const size_t nMaxPossibleData = rIStm.remainingSize();
                    if (nDataSize > nMaxPossibleActions)
                    {
                        SAL_WARN("vcl.gdi", "svm record claims to have: " << nDataSize << " data, but only " << nMaxPossibleData << " possible");
                        nDataSize = nMaxPossibleActions;
                    }
                    aData.resize(nDataSize);
                    nDataSize = rIStm.ReadBytes(aData.data(), nDataSize);
                }

                sal_Int32 nFollowingActionCount(0);
                rIStm.ReadInt32(nFollowingActionCount);
                ImplSkipActions( rIStm, nFollowingActionCount );
                rMtf.AddAction(new MetaCommentAction(aComment, nValue, aData.data(), nDataSize));

                i = SkipActions(i, nFollowingActionCount, nActions);
            }
            break;

            case GDI_UNICODE_COMMENT:
            {
                nUnicodeCommentActionNumber = i + 1;
                nUnicodeCommentStreamPos = rIStm.Tell() - 6;
                if (nActionSize < 4)
                    rIStm.SetError(SVSTREAM_FILEFORMAT_ERROR);
                else
                    rIStm.SeekRel(nActionSize - 4);
            }
            break;

            default:
                if (nActionSize < 4)
                    rIStm.SetError(SVSTREAM_FILEFORMAT_ERROR);
                else
                    rIStm.SeekRel(nActionSize - 4);
            break;
        }
    }

    rIStm.SetEndian( nOldFormat );
}

void SVMConverter::ImplConvertToSVM1( SvStream& rOStm, GDIMetaFile const & rMtf )
{
    sal_uLong           nCountPos;
    vcl::Font           aSaveFont;
    const SvStreamEndian nOldFormat = rOStm.GetEndian();
    rtl_TextEncoding    eActualCharSet = osl_getThreadTextEncoding();
    const Size          aPrefSize( rMtf.GetPrefSize() );
    bool                bRop_0_1 = false;
    ScopedVclPtrInstance< VirtualDevice > aSaveVDev;
    Color               aLineCol( COL_BLACK );
    ::std::stack< Color* >  aLineColStack;

    rOStm.SetEndian( SvStreamEndian::LITTLE );

    // Write MagicCode
    rOStm.WriteCharPtr( "SVGDI" );                                   // Identifier
    rOStm.WriteInt16( 42 );                            // HeaderSize
    rOStm.WriteInt16( 200 );                           // VERSION
    rOStm.WriteInt32( aPrefSize.Width() );
    rOStm.WriteInt32( aPrefSize.Height() );
    ImplWriteMapMode( rOStm, rMtf.GetPrefMapMode() );

    // ActionCount will be written later
    nCountPos = rOStm.Tell();
    rOStm.SeekRel( 4 );

    const sal_Int32 nActCount = ImplWriteActions( rOStm, rMtf, *aSaveVDev.get(), bRop_0_1, aLineCol, aLineColStack, eActualCharSet );
    const sal_uLong nActPos = rOStm.Tell();

    rOStm.Seek( nCountPos );
    rOStm.WriteInt32( nActCount );
    rOStm.Seek( nActPos );
    rOStm.SetEndian( nOldFormat );

    // cleanup push-pop stack if necessary
    while ( !aLineColStack.empty() )
    {
        delete aLineColStack.top();
        aLineColStack.pop();
    }
}

sal_uLong SVMConverter::ImplWriteActions( SvStream& rOStm, GDIMetaFile const & rMtf,
                                      VirtualDevice& rSaveVDev, bool& rRop_0_1,
                                      Color& rLineCol, ::std::stack< Color* >& rLineColStack,
                                      rtl_TextEncoding& rActualCharSet )
{
    sal_uLong nCount = 0;
    for( size_t i = 0, nActionCount = rMtf.GetActionSize(); i < nActionCount; i++ )
    {
        const MetaAction* pAction = rMtf.GetAction( i );

        switch( pAction->GetType() )
        {
            case MetaActionType::PIXEL:
            {
                const MetaPixelAction* pAct = static_cast<const MetaPixelAction*>(pAction);

                rOStm.WriteInt16( GDI_PIXEL_ACTION );
                rOStm.WriteInt32( 18 );
                WritePair( rOStm, pAct->GetPoint() );
                ImplWriteColor( rOStm, pAct->GetColor() );
                nCount++;
            }
            break;

            case MetaActionType::POINT:
            {
                const MetaPointAction* pAct = static_cast<const MetaPointAction*>(pAction);

                rOStm.WriteInt16( GDI_POINT_ACTION );
                rOStm.WriteInt32( 12 );
                WritePair( rOStm, pAct->GetPoint() );
                nCount++;
            }
            break;

            case MetaActionType::LINE:
            {
                const MetaLineAction* pAct = static_cast<const MetaLineAction*>(pAction);
                const LineInfo& rInfo = pAct->GetLineInfo();
                const bool bFatLine(!rInfo.IsDefault() && (LineStyle::NONE != rInfo.GetStyle()));
                const bool bLineJoin(bFatLine && basegfx::B2DLineJoin::Round != rInfo.GetLineJoin());
                const bool bLineCap(bFatLine && css::drawing::LineCap_BUTT != rInfo.GetLineCap());
                const bool bLineDashDot(LineStyle::Dash == rInfo.GetStyle());

                if( bFatLine )
                {
                    ImplWritePushAction( rOStm );
                    ImplWriteLineColor( rOStm, rLineCol, 1, rInfo.GetWidth() );

                    if(bLineJoin)
                    {
                        rOStm.WriteInt16( GDI_LINEJOIN_ACTION );
                        rOStm.WriteInt32( 6 );
                        rOStm.WriteInt16( static_cast<sal_Int16>(rInfo.GetLineJoin()) );
                    }

                    if(bLineCap)
                    {
                        rOStm.WriteInt16( GDI_LINECAP_ACTION );
                        rOStm.WriteInt32( 6 );
                        rOStm.WriteInt16( (sal_Int16)rInfo.GetLineCap() );
                    }
                }

                if(bLineDashDot)
                {
                    rOStm.WriteInt16( GDI_LINEDASHDOT_ACTION );
                    rOStm.WriteInt32( 4 + 16 );
                    rOStm.WriteInt16( rInfo.GetDashCount() );
                    rOStm.WriteInt32( rInfo.GetDashLen() );
                    rOStm.WriteInt16( rInfo.GetDotCount() );
                    rOStm.WriteInt32( rInfo.GetDotLen() );
                    rOStm.WriteInt32( rInfo.GetDistance() );
                }

                rOStm.WriteInt16( GDI_LINE_ACTION );
                rOStm.WriteInt32( 20 );
                WritePair( rOStm, pAct->GetStartPoint() );
                WritePair( rOStm, pAct->GetEndPoint() );
                nCount++;

                if( bFatLine )
                {
                    ImplWritePopAction( rOStm );
                    nCount += 3;

                    if(bLineJoin)
                    {
                        nCount += 1;
                    }

                    if(bLineCap)
                    {
                        nCount += 1;
                    }
                }

                if(bLineDashDot)
                {
                    nCount += 1;
                }
            }
            break;

            case MetaActionType::RECT:
            {
                const MetaRectAction* pAct = static_cast<const MetaRectAction*>(pAction);

                rOStm.WriteInt16( GDI_RECT_ACTION );
                rOStm.WriteInt32( 28 );
                ImplWriteRect( rOStm, pAct->GetRect() );
                rOStm.WriteInt32( 0 );
                rOStm.WriteInt32( 0 );
                nCount++;
            }
            break;

            case MetaActionType::ROUNDRECT:
            {
                const MetaRoundRectAction* pAct = static_cast<const MetaRoundRectAction*>(pAction);

                rOStm.WriteInt16( GDI_RECT_ACTION );
                rOStm.WriteInt32( 28 );
                ImplWriteRect( rOStm, pAct->GetRect() );
                rOStm.WriteInt32( pAct->GetHorzRound() );
                rOStm.WriteInt32( pAct->GetVertRound() );
                nCount++;
            }
            break;

            case MetaActionType::ELLIPSE:
            {
                const MetaEllipseAction* pAct = static_cast<const MetaEllipseAction*>(pAction);

                rOStm.WriteInt16( GDI_ELLIPSE_ACTION );
                rOStm.WriteInt32( 20 );
                ImplWriteRect( rOStm, pAct->GetRect() );
                nCount++;
            }
            break;

            case MetaActionType::ARC:
            {
                const MetaArcAction* pAct = static_cast<const MetaArcAction*>(pAction);

                rOStm.WriteInt16( GDI_ARC_ACTION );
                rOStm.WriteInt32( 36 );
                ImplWriteRect( rOStm, pAct->GetRect() );
                WritePair( rOStm, pAct->GetStartPoint() );
                WritePair( rOStm, pAct->GetEndPoint() );
                nCount++;
            }
            break;

            case MetaActionType::PIE:
            {
                const MetaPieAction* pAct = static_cast<const MetaPieAction*>(pAction);

                rOStm.WriteInt16( GDI_PIE_ACTION );
                rOStm.WriteInt32( 36 );
                ImplWriteRect( rOStm, pAct->GetRect() );
                WritePair( rOStm, pAct->GetStartPoint() );
                WritePair( rOStm, pAct->GetEndPoint() );
                nCount++;
            }
            break;

            case MetaActionType::CHORD:
            {
                const MetaChordAction* pAct = static_cast<const MetaChordAction*>(pAction);
                tools::Polygon aChordPoly( pAct->GetRect(), pAct->GetStartPoint(),
                                           pAct->GetEndPoint(), PolyStyle::Chord );
                const sal_uInt16       nPoints = aChordPoly.GetSize();

                rOStm.WriteInt16( GDI_POLYGON_ACTION );
                rOStm.WriteInt32( 8 + ( nPoints << 3 ) );
                rOStm.WriteInt32( nPoints );

                for( sal_uInt16 n = 0; n < nPoints; n++ )
                    WritePair( rOStm, aChordPoly[ n ] );
                nCount++;
            }
            break;

            case MetaActionType::POLYLINE:
            {
                // #i102224#
                const MetaPolyLineAction* pAct = static_cast<const MetaPolyLineAction*>(pAction);
                // #i102224# Here the possible curved nature of Polygon was
                // ignored (for all those years). Adapted to at least write
                // a polygon representing the curve as good as possible
                tools::Polygon aSimplePoly;
                pAct->GetPolygon().AdaptiveSubdivide(aSimplePoly);
                const LineInfo& rInfo = pAct->GetLineInfo();
                const sal_uInt16 nPoints(aSimplePoly.GetSize());
                const bool bFatLine(!rInfo.IsDefault() && (LineStyle::NONE != rInfo.GetStyle()));
                const bool bLineJoin(bFatLine && basegfx::B2DLineJoin::Round != rInfo.GetLineJoin());
                const bool bLineCap(bFatLine && css::drawing::LineCap_BUTT != rInfo.GetLineCap());
                const bool bLineDashDot(LineStyle::Dash == rInfo.GetStyle());

                if( bFatLine )
                {
                    ImplWritePushAction( rOStm );
                    ImplWriteLineColor( rOStm, rLineCol, 1, rInfo.GetWidth() );

                    if(bLineJoin)
                    {
                        rOStm.WriteInt16( GDI_LINEJOIN_ACTION );
                        rOStm.WriteInt32( 6 );
                        rOStm.WriteInt16( static_cast<sal_Int16>(rInfo.GetLineJoin()) );
                    }

                    if(bLineCap)
                    {
                        rOStm.WriteInt16( GDI_LINECAP_ACTION );
                        rOStm.WriteInt32( 6 );
                        rOStm.WriteInt16( (sal_Int16)rInfo.GetLineCap() );
                    }
                }

                if(bLineDashDot)
                {
                    rOStm.WriteInt16( GDI_LINEDASHDOT_ACTION );
                    rOStm.WriteInt32( 4 + 16 );
                    rOStm.WriteInt16( rInfo.GetDashCount() );
                    rOStm.WriteInt32( rInfo.GetDashLen() );
                    rOStm.WriteInt16( rInfo.GetDotCount() );
                    rOStm.WriteInt32( rInfo.GetDotLen() );
                    rOStm.WriteInt32( rInfo.GetDistance() );
                }

                rOStm.WriteInt16( GDI_POLYLINE_ACTION );
                rOStm.WriteInt32( 8 + ( nPoints << 3 ) );
                rOStm.WriteInt32( nPoints );

                for( sal_uInt16 n = 0; n < nPoints; n++ )
                {
                    WritePair( rOStm, aSimplePoly[ n ] );
                }

                nCount++;

                const tools::PolyPolygon aPolyPolygon(pAct->GetPolygon());
                if(ImplWriteExtendedPolyPolygonAction(rOStm, aPolyPolygon))
                {
                    nCount++;
                }

                if( bFatLine )
                {
                    ImplWritePopAction( rOStm );
                    nCount += 3;

                    if(bLineJoin)
                    {
                        nCount += 1;
                    }

                    if(bLineCap)
                    {
                        nCount += 1;
                    }
                }

                if(bLineDashDot)
                {
                    nCount += 1;
                }
            }
            break;

            case MetaActionType::POLYGON:
            {
                const MetaPolygonAction* pAct = static_cast<const MetaPolygonAction*>(pAction);
                // #i102224# Here the possible curved nature of Polygon was
                // ignored (for all those years). Adapted to at least write
                // a polygon representing the curve as good as possible
                tools::Polygon aSimplePoly;
                pAct->GetPolygon().AdaptiveSubdivide(aSimplePoly);
                const sal_uInt16 nPoints(aSimplePoly.GetSize());

                rOStm.WriteInt16( GDI_POLYGON_ACTION );
                rOStm.WriteInt32( 8 + ( nPoints << 3 ) );
                rOStm.WriteInt32( nPoints );

                for( sal_uInt16 n = 0; n < nPoints; n++ )
                    WritePair( rOStm, aSimplePoly[ n ] );

                nCount++;

                const tools::PolyPolygon aPolyPolygon(pAct->GetPolygon());
                if(ImplWriteExtendedPolyPolygonAction(rOStm, aPolyPolygon))
                {
                    nCount++;
                }
            }
            break;

            case MetaActionType::POLYPOLYGON:
            {
                const MetaPolyPolygonAction* pAct = static_cast<const MetaPolyPolygonAction*>(pAction);
                ImplWritePolyPolyAction( rOStm, pAct->GetPolyPolygon() );
                nCount++;

                if(ImplWriteExtendedPolyPolygonAction(rOStm, pAct->GetPolyPolygon()))
                {
                    nCount++;
                }
            }
            break;

            case MetaActionType::TEXT:
            {
                const MetaTextAction* pAct = static_cast<const MetaTextAction*>(pAction);
                OUString aUniText( pAct->GetText() );
                OString  aText(OUStringToOString(aUniText, rActualCharSet));
                const sal_Int32 nStrLen = aText.getLength();

                if ( ImplWriteUnicodeComment( rOStm, aUniText ) )
                    nCount++;

                rOStm.WriteInt16( GDI_TEXT_ACTION );
                rOStm.WriteInt32( 24 + ( nStrLen + 1 ) );
                WritePair( rOStm, pAct->GetPoint() );
                rOStm.WriteInt32( pAct->GetIndex() );
                rOStm.WriteInt32( pAct->GetLen() );
                rOStm.WriteInt32( nStrLen );
                rOStm.WriteBytes( aText.getStr(), nStrLen + 1 );
                nCount++;
            }
            break;

            case MetaActionType::TEXTARRAY:
            {
                const MetaTextArrayAction* pAct = static_cast<const MetaTextArrayAction*>(pAction);
                OString aText(OUStringToOString(pAct->GetText(), rActualCharSet));
                OUString aUniText = pAct->GetText().copy(
                        pAct->GetIndex(),
                        std::min<sal_Int32>(pAct->GetText().getLength() - pAct->GetIndex(), pAct->GetLen()) );
                sal_Int32 nAryLen;
                sal_Int32 nLen = pAct->GetLen();
                const sal_Int32 nTextLen = aText.getLength();
                long* pDXArray = pAct->GetDXArray();

                if ( ImplWriteUnicodeComment( rOStm, aUniText ) )
                    nCount++;

                if( ( nLen + pAct->GetIndex() ) > nTextLen )
                {
                    if( pAct->GetIndex() <= nTextLen )
                        nLen = nTextLen - pAct->GetIndex();
                    else
                        nLen = 0;
                }

                if( !pDXArray || !nLen )
                    nAryLen = 0;
                else
                    nAryLen = nLen; // #105987# Write out all of DX array

                rOStm.WriteInt16( GDI_TEXTARRAY_ACTION );
                rOStm.WriteInt32( 28 + ( nLen + 1 ) + ( nAryLen * 4 ) );
                WritePair( rOStm, pAct->GetPoint() );
                rOStm.WriteInt32( 0 );
                rOStm.WriteInt32( nLen );
                rOStm.WriteInt32( nLen );
                rOStm.WriteInt32( nAryLen );
                rOStm.WriteBytes( aText.getStr()+pAct->GetIndex(), nLen + 1 );

                for (sal_Int32 n = 0; n < nAryLen; ++n)
                    rOStm.WriteInt32( pDXArray[ n ] );

                nCount++;
            }
            break;

            case MetaActionType::STRETCHTEXT:
            {
                const MetaStretchTextAction* pAct = static_cast<const MetaStretchTextAction*>(pAction);
                OUString aUniText( pAct->GetText() );
                OString  aText(OUStringToOString(aUniText, rActualCharSet));
                const sal_Int32 nStrLen = aText.getLength();

                if ( ImplWriteUnicodeComment( rOStm, aUniText ) )
                    nCount++;

                rOStm.WriteInt16( GDI_STRETCHTEXT_ACTION );
                rOStm.WriteInt32( 28 + ( nStrLen + 1 ) );
                WritePair( rOStm, pAct->GetPoint() );
                rOStm.WriteInt32( pAct->GetIndex() );
                rOStm.WriteInt32( pAct->GetLen() );
                rOStm.WriteInt32( nStrLen );
                rOStm.WriteInt32( pAct->GetWidth() );
                rOStm.WriteBytes( aText.getStr(), nStrLen + 1 );
                nCount++;
            }
            break;

            case MetaActionType::BMP:
            {
                const MetaBmpAction* pAct = static_cast<const MetaBmpAction*>(pAction);

                rOStm.WriteInt16( GDI_BITMAP_ACTION );
                rOStm.WriteInt32( 12 );
                WritePair( rOStm, pAct->GetPoint() );
                WriteDIB(pAct->GetBitmap(), rOStm, false, true);
                nCount++;
            }
            break;

            case MetaActionType::BMPSCALE:
            {
                const MetaBmpScaleAction* pAct = static_cast<const MetaBmpScaleAction*>(pAction);

                rOStm.WriteInt16( GDI_BITMAPSCALE_ACTION );
                rOStm.WriteInt32( 20 );
                WritePair( rOStm, pAct->GetPoint() );
                WritePair( rOStm, pAct->GetSize() );
                WriteDIB(pAct->GetBitmap(), rOStm, false, true);
                nCount++;
            }
            break;

            case MetaActionType::BMPSCALEPART:
            {
                const MetaBmpScalePartAction* pAct = static_cast<const MetaBmpScalePartAction*>(pAction);

                rOStm.WriteInt16( GDI_BITMAPSCALEPART_ACTION );
                rOStm.WriteInt32( 36 );
                WritePair( rOStm, pAct->GetDestPoint() );
                WritePair( rOStm, pAct->GetDestSize() );
                WritePair( rOStm, pAct->GetSrcPoint() );
                WritePair( rOStm, pAct->GetSrcSize() );
                WriteDIB(pAct->GetBitmap(), rOStm, false, true);
                nCount++;
            }
            break;

            case MetaActionType::BMPEX:
            {
                const MetaBmpExAction* pAct = static_cast<const MetaBmpExAction*>(pAction);
                const Bitmap           aBmp( Graphic( pAct->GetBitmapEx() ).GetBitmap() );

                rOStm.WriteInt16( GDI_BITMAP_ACTION );
                rOStm.WriteInt32( 12 );
                WritePair( rOStm, pAct->GetPoint() );
                WriteDIB(aBmp, rOStm, false, true);
                nCount++;
            }
            break;

            case MetaActionType::BMPEXSCALE:
            {
                const MetaBmpExScaleAction* pAct = static_cast<const MetaBmpExScaleAction*>(pAction);
                const Bitmap                aBmp( Graphic( pAct->GetBitmapEx() ).GetBitmap() );

                rOStm.WriteInt16( GDI_BITMAPSCALE_ACTION );
                rOStm.WriteInt32( 20 );
                WritePair( rOStm, pAct->GetPoint() );
                WritePair( rOStm, pAct->GetSize() );
                WriteDIB(aBmp, rOStm, false, true);
                nCount++;
            }
            break;

            case MetaActionType::BMPEXSCALEPART:
            {
                const MetaBmpExScalePartAction* pAct = static_cast<const MetaBmpExScalePartAction*>(pAction);
                const Bitmap                    aBmp( Graphic( pAct->GetBitmapEx() ).GetBitmap() );

                rOStm.WriteInt16( GDI_BITMAPSCALEPART_ACTION );
                rOStm.WriteInt32( 36 );
                WritePair( rOStm, pAct->GetDestPoint() );
                WritePair( rOStm, pAct->GetDestSize() );
                WritePair( rOStm, pAct->GetSrcPoint() );
                WritePair( rOStm, pAct->GetSrcSize() );
                WriteDIB(aBmp, rOStm, false, true);
                nCount++;
            }
            break;

            case MetaActionType::GRADIENT:
            {
                const MetaGradientAction* pAct = static_cast<const MetaGradientAction*>(pAction);
                const Gradient&           rGrad = pAct->GetGradient();

                rOStm.WriteInt16( GDI_GRADIENT_ACTION );
                rOStm.WriteInt32( 46 );
                ImplWriteRect( rOStm, pAct->GetRect() );
                rOStm.WriteInt16( (sal_Int16)rGrad.GetStyle() );
                ImplWriteColor( rOStm, rGrad.GetStartColor() );
                ImplWriteColor( rOStm, rGrad.GetEndColor() );
                rOStm.WriteInt16( rGrad.GetAngle() );
                rOStm.WriteInt16( rGrad.GetBorder() );
                rOStm.WriteInt16( rGrad.GetOfsX() );
                rOStm.WriteInt16( rGrad.GetOfsY() );
                rOStm.WriteInt16( rGrad.GetStartIntensity() );
                rOStm.WriteInt16( rGrad.GetEndIntensity() );
                nCount++;
            }
            break;

            case MetaActionType::GRADIENTEX:
            {
                const MetaGradientExAction* pA = static_cast<const MetaGradientExAction*>(pAction);
                sal_uLong                   nOldPos, nNewPos;

                // write RefPoint comment
                rOStm.WriteInt16( GDI_GRADIENTEX_COMMENT );

                // we'll write the ActionSize later
                nOldPos = rOStm.Tell();
                rOStm.SeekRel( 4 );

                // write data
                WritePolyPolygon( rOStm, pA->GetPolyPolygon() );
                WriteGradient( rOStm, pA->GetGradient() );
                rOStm.WriteInt32( 0 ); // number of actions that follow this comment

                // calculate and write ActionSize of comment
                nNewPos = rOStm.Tell();
                rOStm.Seek( nOldPos );
                rOStm.WriteInt32( nNewPos - nOldPos );
                rOStm.Seek( nNewPos );

                nCount++;
            }
            break;

            case MetaActionType::WALLPAPER:
            {
                const MetaWallpaperAction* pAct = static_cast<const MetaWallpaperAction*>(pAction);
                const Color&               rColor = pAct->GetWallpaper().GetColor();

                ImplWritePushAction( rOStm );
                ImplWriteLineColor( rOStm, rColor, 1 );
                ImplWriteFillColor( rOStm, rColor, 1 );

                rOStm.WriteInt16( GDI_RECT_ACTION );
                rOStm.WriteInt32( 28 );
                ImplWriteRect( rOStm, pAct->GetRect() );
                rOStm.WriteInt32( 0 );
                rOStm.WriteInt32( 0 );

                ImplWritePopAction( rOStm );
                nCount += 5;
            }
            break;

            case MetaActionType::CLIPREGION:
            {
                const MetaClipRegionAction* pAct = static_cast<const MetaClipRegionAction*>(pAction);
                const vcl::Region&          rRegion = pAct->GetRegion();
                tools::Rectangle                   aClipRect;

                rOStm.WriteInt16( GDI_CLIPREGION_ACTION );
                rOStm.WriteInt32( 24 );

                if( pAct->IsClipping() )
                {
                    aClipRect = rRegion.GetBoundRect();
                    rOStm.WriteInt16( 1 );
                }
                else
                    rOStm.WriteInt16( 0 );

                rOStm.WriteInt16( 0 );
                ImplWriteRect( rOStm, aClipRect );

                if( pAct->IsClipping() )
                    ImplWriteRect( rOStm, aClipRect );

                nCount++;
            }
            break;

            case MetaActionType::ISECTRECTCLIPREGION:
            {
                const MetaISectRectClipRegionAction* pAct = static_cast<const MetaISectRectClipRegionAction*>(pAction);

                rOStm.WriteInt16( GDI_ISECTCLIPREGION_ACTION );
                rOStm.WriteInt32( 20 );
                WriteRectangle( rOStm, pAct->GetRect() );
                nCount++;
            }
            break;

            case MetaActionType::MOVECLIPREGION:
            {
                const MetaMoveClipRegionAction* pAct = static_cast<const MetaMoveClipRegionAction*>(pAction);

                rOStm.WriteInt16( GDI_MOVECLIPREGION_ACTION );
                rOStm.WriteInt32( 12 );
                rOStm.WriteInt32( pAct->GetHorzMove() );
                rOStm.WriteInt32( pAct->GetVertMove() );
                nCount++;
            }
            break;

            case MetaActionType::LINECOLOR:
            {
                const MetaLineColorAction* pAct = static_cast<const MetaLineColorAction*>(pAction);
                ImplWriteLineColor( rOStm, rLineCol = pAct->GetColor(), pAct->IsSetting() ? 1 : 0 );
                nCount++;
            }
            break;

            case MetaActionType::FILLCOLOR:
            {
                const MetaFillColorAction* pAct = static_cast<const MetaFillColorAction*>(pAction);
                ImplWriteFillColor( rOStm, pAct->GetColor(), pAct->IsSetting() ? 1 : 0 );
                nCount++;
            }
            break;

            case MetaActionType::FONT:
            {
                rSaveVDev.SetFont( static_cast<const MetaFontAction*>(pAction)->GetFont() );
                ImplWriteFont( rOStm, rSaveVDev.GetFont(), rActualCharSet );
                nCount++;
            }
            break;

            case MetaActionType::TEXTCOLOR:
            {
                vcl::Font aSaveFont( rSaveVDev.GetFont() );

                aSaveFont.SetColor( static_cast<const MetaTextColorAction*>(pAction)->GetColor() );
                rSaveVDev.SetFont( aSaveFont );
                ImplWriteFont( rOStm, rSaveVDev.GetFont(), rActualCharSet );
                nCount++;
            }
            break;

            case MetaActionType::TEXTFILLCOLOR:
            {
                const MetaTextFillColorAction* pAct = static_cast<const MetaTextFillColorAction*>(pAction);
                vcl::Font                      aSaveFont( rSaveVDev.GetFont() );

                if( pAct->IsSetting() )
                    aSaveFont.SetFillColor( pAct->GetColor() );
                else
                    aSaveFont.SetFillColor( Color( COL_TRANSPARENT ) );

                rSaveVDev.SetFont( aSaveFont );
                ImplWriteFont( rOStm, rSaveVDev.GetFont(), rActualCharSet );
                nCount++;
            }
            break;

            case MetaActionType::TEXTALIGN:
            {
                vcl::Font aSaveFont( rSaveVDev.GetFont() );

                aSaveFont.SetAlignment( static_cast<const MetaTextAlignAction*>(pAction)->GetTextAlign() );
                rSaveVDev.SetFont( aSaveFont );
                ImplWriteFont( rOStm, rSaveVDev.GetFont(), rActualCharSet );
                nCount++;
            }
            break;

            case MetaActionType::MAPMODE:
            {
                const MetaMapModeAction* pAct = static_cast<const MetaMapModeAction*>(pAction);

                rOStm.WriteInt16( GDI_MAPMODE_ACTION );
                rOStm.WriteInt32( 30 );
                ImplWriteMapMode( rOStm, pAct->GetMapMode() );
                nCount++;
            }
            break;

            case MetaActionType::PUSH:
            {
                ImplWritePushAction( rOStm );
                rLineColStack.push( new Color( rLineCol ) );
                rSaveVDev.Push();
                nCount++;
            }
            break;

            case MetaActionType::POP:
            {
                Color* pCol;
                if (rLineColStack.empty())
                    pCol = nullptr;
                else
                {
                    pCol = rLineColStack.top();
                    rLineColStack.pop();
                }

                if( pCol )
                {
                    rLineCol = *pCol;
                    delete pCol;
                }

                ImplWritePopAction( rOStm );
                rSaveVDev.Pop();
                nCount++;
            }
            break;

            case MetaActionType::RASTEROP:
            {
                const MetaRasterOpAction* pAct = static_cast<const MetaRasterOpAction*>(pAction);

                if( ( pAct->GetRasterOp() != RasterOp::N0 ) && ( pAct->GetRasterOp() != RasterOp::N1 ) )
                {
                    sal_Int16 nRasterOp;

                    // If RasterOp::N0/1 was set earlier, restore old state
                    // via a Pop first
                    if( rRop_0_1 )
                    {
                        ImplWritePopAction( rOStm );
                        rSaveVDev.Pop();
                        rRop_0_1 = false;
                        nCount++;
                    }

                    switch( pAct->GetRasterOp() )
                    {
                        case RasterOp::OverPaint : nRasterOp = 0; break;
                        case RasterOp::Xor :       nRasterOp = 4; break;
                        case RasterOp::Invert:     nRasterOp = 1; break;
                        default:                   nRasterOp = 0; break;
                    }

                    ImplWriteRasterOpAction( rOStm, nRasterOp );
                    nCount++;
                }
                else
                {
                    ImplWritePushAction( rOStm );
                    rSaveVDev.Push();

                    if( pAct->GetRasterOp() == RasterOp::N0 )
                    {
                        ImplWriteLineColor( rOStm, COL_BLACK, 1 );
                        ImplWriteFillColor( rOStm, COL_BLACK, 1 );
                    }
                    else
                    {
                        ImplWriteLineColor( rOStm, COL_WHITE, 1 );
                        ImplWriteFillColor( rOStm, COL_WHITE, 1 );
                    }

                    ImplWriteRasterOpAction( rOStm, 0 );
                    rRop_0_1 = true;
                    nCount += 4;
                }
            }
            break;

            case MetaActionType::Transparent:
            {
                const tools::PolyPolygon& rPolyPoly = static_cast<const MetaTransparentAction*>(pAction)->GetPolyPolygon();
                const sal_Int16           nTrans = static_cast<const MetaTransparentAction*>(pAction)->GetTransparence();
                const sal_Int16           nBrushStyle = ( nTrans < 38 ) ? 8 : ( nTrans < 63 ) ? 9 : 10;
                sal_uLong                 nOldPos, nNewPos;

                // write transparence comment
                rOStm.WriteInt16( GDI_TRANSPARENT_COMMENT );

                // we'll write the ActionSize later
                nOldPos = rOStm.Tell();
                rOStm.SeekRel( 4 );

                // write comment data
                WritePolyPolygon( rOStm, rPolyPoly );
                rOStm.WriteInt16( nTrans );
                rOStm.WriteInt32( 15 ); // number of actions that follow this comment

                // calculate and write ActionSize of comment
                nNewPos = rOStm.Tell();
                rOStm.Seek( nOldPos );
                rOStm.WriteInt32( nNewPos - nOldPos );
                rOStm.Seek( nNewPos );

                {
                    // write actions for transparence
                    ImplWritePushAction( rOStm );
                    {
                        ImplWriteRasterOpAction( rOStm, 4 );
                        ImplWritePolyPolyAction( rOStm, rPolyPoly );

                        ImplWritePushAction( rOStm );
                        {
                            ImplWriteRasterOpAction( rOStm, 2 );
                            ImplWriteFillColor( rOStm, COL_BLACK, nBrushStyle );
                            ImplWritePolyPolyAction( rOStm, rPolyPoly );
                        }
                        ImplWritePopAction( rOStm );

                        ImplWriteRasterOpAction( rOStm, 4 );
                        ImplWritePolyPolyAction( rOStm, rPolyPoly );
                    }
                    ImplWritePopAction( rOStm );

                    ImplWritePushAction( rOStm );
                    {
                        ImplWriteFillColor( rOStm, Color(), 0 );
                        ImplWritePolyPolyAction( rOStm, rPolyPoly );
                    }
                    ImplWritePopAction( rOStm );

                    nCount += 15;
                }

                nCount++;
            }
            break;

            case MetaActionType::FLOATTRANSPARENT:
            {
                const MetaFloatTransparentAction*   pA = static_cast<const MetaFloatTransparentAction*>(pAction);
                const GDIMetaFile&                  rTransMtf = pA->GetGDIMetaFile();
                const Point&                        rPos = pA->GetPoint();
                const Size&                         rSize = pA->GetSize();
                const Gradient&                     rGradient = pA->GetGradient();
                sal_uLong                               nOldPos, nNewPos;

                // write RefPoint comment
                rOStm.WriteInt16( GDI_FLOATTRANSPARENT_COMMENT );

                // we'll write the ActionSize later
                nOldPos = rOStm.Tell();
                rOStm.SeekRel( 4 );

                // write comment data
                WriteGDIMetaFile( rOStm, rTransMtf );
                WritePair( rOStm, rPos );
                WritePair( rOStm, rSize );
                WriteGradient( rOStm, rGradient );

                // calculate and write ActionSize of comment
                nNewPos = rOStm.Tell();
                rOStm.Seek( nOldPos );
                rOStm.WriteInt32( nNewPos - nOldPos + 4 );
                rOStm.Seek( ( nOldPos = nNewPos ) + 4 );

                {
                    // write actions for float transparence
                    sal_uLong       nAddCount;
                    GDIMetaFile     aMtf( rTransMtf );
                    const Size      aSrcSize( rTransMtf.GetPrefSize() );
                    Point           aSrcPt( rTransMtf.GetPrefMapMode().GetOrigin() );
                    const double    fScaleX = aSrcSize.Width() ? (double) rSize.Width() / aSrcSize.Width() : 1.0;
                    const double    fScaleY = aSrcSize.Height() ? (double) rSize.Height() / aSrcSize.Height() : 1.0;
                    long            nMoveX, nMoveY;

                    if( fScaleX != 1.0 || fScaleY != 1.0 )
                    {
                        aMtf.Scale( fScaleX, fScaleY );
                        aSrcPt.X() = FRound( aSrcPt.X() * fScaleX );
                        aSrcPt.Y() = FRound( aSrcPt.Y() * fScaleY );
                    }

                    nMoveX = rPos.X() - aSrcPt.X();
                    nMoveY = rPos.Y() - aSrcPt.Y();

                    if( nMoveX || nMoveY )
                        aMtf.Move( nMoveX, nMoveY );

                    nAddCount = ImplWriteActions( rOStm, aMtf, rSaveVDev, rRop_0_1, rLineCol, rLineColStack, rActualCharSet );
                    nNewPos = rOStm.Tell();
                    rOStm.Seek( nOldPos );
                    rOStm.WriteInt32( nAddCount );
                    rOStm.Seek( nNewPos );

                    nCount += nAddCount;
                }

                nCount++;
            }
            break;

            case MetaActionType::HATCH:
            {
                const MetaHatchAction*    pA = static_cast<const MetaHatchAction*>(pAction);
                const tools::PolyPolygon& rPolyPoly = pA->GetPolyPolygon();
                const Hatch&            rHatch = pA->GetHatch();
                sal_uLong               nOldPos, nNewPos, nAddCount;

                // write hatch comment
                rOStm.WriteInt16( GDI_HATCH_COMMENT );

                // we'll write the ActionSize later
                nOldPos = rOStm.Tell();
                rOStm.SeekRel( 4 );

                // write comment data
                WritePolyPolygon( rOStm, rPolyPoly );
                WriteHatch( rOStm, rHatch );

                // calculate and write ActionSize of comment
                nNewPos = rOStm.Tell();
                rOStm.Seek( nOldPos );
                rOStm.WriteInt32( nNewPos - nOldPos + 4 );
                rOStm.Seek( ( nOldPos = nNewPos ) + 4 );

                {
                    // write actions for hatch
                    ScopedVclPtrInstance< VirtualDevice > aVDev;
                    GDIMetaFile     aTmpMtf;

                    aVDev->AddHatchActions( rPolyPoly, rHatch, aTmpMtf );
                    nAddCount = ImplWriteActions( rOStm, aTmpMtf, rSaveVDev, rRop_0_1, rLineCol, rLineColStack, rActualCharSet );
                    nNewPos = rOStm.Tell();
                    rOStm.Seek( nOldPos );
                    rOStm.WriteInt32( nAddCount );
                    rOStm.Seek( nNewPos );

                    nCount += nAddCount;
                }

                nCount++;
            }
            break;

            case MetaActionType::REFPOINT:
            {
                const MetaRefPointAction*   pA = static_cast<const MetaRefPointAction*>(pAction);
                const Point&                rRefPoint = pA->GetRefPoint();
                const bool                  bSet = pA->IsSetting();
                sal_uLong                   nOldPos, nNewPos;

                // write RefPoint comment
                rOStm.WriteInt16( GDI_REFPOINT_COMMENT );

                // we'll write the ActionSize later
                nOldPos = rOStm.Tell();
                rOStm.SeekRel( 4 );

                // write data
                WritePair( rOStm, rRefPoint );
                rOStm.WriteBool( bSet );
                rOStm.WriteInt32( 0 ); // number of actions that follow this comment

                // calculate and write ActionSize of comment
                nNewPos = rOStm.Tell();
                rOStm.Seek( nOldPos );
                rOStm.WriteInt32( nNewPos - nOldPos );
                rOStm.Seek( nNewPos );

                nCount++;
            }
            break;

            case MetaActionType::TEXTLINECOLOR:
            {
                const MetaTextLineColorAction*  pA = static_cast<const MetaTextLineColorAction*>(pAction);
                const Color&                    rColor = pA->GetColor();
                const bool                      bSet = pA->IsSetting();
                sal_uLong                       nOldPos, nNewPos;

                // write RefPoint comment
                rOStm.WriteInt16( GDI_TEXTLINECOLOR_COMMENT );

                // we'll write the ActionSize later
                nOldPos = rOStm.Tell();
                rOStm.SeekRel( 4 );

                // write data
                WriteColor( rOStm, rColor );
                rOStm.WriteBool( bSet );
                rOStm.WriteInt32( 0 ); // number of actions that follow this comment

                // calculate and write ActionSize of comment
                nNewPos = rOStm.Tell();
                rOStm.Seek( nOldPos );
                rOStm.WriteInt32( nNewPos - nOldPos );
                rOStm.Seek( nNewPos );

                nCount++;
            }
            break;

            case MetaActionType::TEXTLINE:
            {
                const MetaTextLineAction*   pA = static_cast<const MetaTextLineAction*>(pAction);
                const Point&                rStartPt = pA->GetStartPoint();
                const sal_Int32             nWidth = (sal_Int32) pA->GetWidth();
                const FontStrikeout         eStrikeout = pA->GetStrikeout();
                const FontLineStyle         eUnderline = pA->GetUnderline();
                sal_uLong                   nOldPos, nNewPos;

                // write RefPoint comment
                rOStm.WriteInt16( GDI_TEXTLINE_COMMENT );

                // we'll write the ActionSize later
                nOldPos = rOStm.Tell();
                rOStm.SeekRel( 4 );

                // write data
                WritePair( rOStm, rStartPt );
                rOStm.WriteInt32( nWidth ).WriteUInt32( eStrikeout ).WriteUInt32( eUnderline );
                rOStm.WriteInt32( 0 ); // number of actions that follow this comment

                // calculate and write ActionSize of comment
                nNewPos = rOStm.Tell();
                rOStm.Seek( nOldPos );
                rOStm.WriteInt32( nNewPos - nOldPos );
                rOStm.Seek( nNewPos );

                nCount++;
            }
            break;

            case MetaActionType::EPS:
            break;

            case MetaActionType::COMMENT:
            {
                const MetaCommentAction*    pA = static_cast<const MetaCommentAction*>(pAction);
                const sal_uInt32            nDataSize = pA->GetDataSize();
                sal_uLong                   nOldPos, nNewPos;

                // write RefPoint comment
                rOStm.WriteInt16( GDI_COMMENT_COMMENT );

                // we'll write the ActionSize later
                nOldPos = rOStm.Tell();
                rOStm.SeekRel( 4 );

                // write data
                write_uInt16_lenPrefixed_uInt8s_FromOString(rOStm, pA->GetComment());
                rOStm.WriteInt32( pA->GetValue() ).WriteUInt32( nDataSize );

                if( nDataSize )
                    rOStm.WriteBytes( pA->GetData(), nDataSize );

                rOStm.WriteInt32( 0 ); // number of actions that follow this comment

                // calculate and write ActionSize of comment
                nNewPos = rOStm.Tell();
                rOStm.Seek( nOldPos );
                rOStm.WriteInt32( nNewPos - nOldPos );
                rOStm.Seek( nNewPos );

                nCount++;
            }
            break;

            default:
                SAL_WARN( "vcl", "Missing implementation for Action#: " << static_cast<sal_Int32>(pAction->GetType()) );
            break;
        }
    }

    return nCount;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
