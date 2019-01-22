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

#include <memory>
#include <QtGui/QColor>
#include <QtWidgets/QStyle>
#include <QtCore/QDebug>
#include <QtWidgets/QToolTip>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMenuBar>

#include <KConfig>
#include <KConfigGroup>
#include <KSharedConfig>

#undef Region

#include "KDE5SalFrame.hxx"
#include "KDE5XLib.hxx"
#include "KDE5SalGraphics.hxx"

#include <tools/color.hxx>

#include <vcl/font.hxx>
#include <vcl/settings.hxx>

#include "unx/fontmanager.hxx"

#include <svdata.hxx>

#include <boost/optional.hpp>


KDE5SalFrame::KDE5SalFrame( SalFrame* pParent, SalFrameStyleFlags nState )
{
}

void KDE5SalFrame::Show( bool bVisible, bool bNoActivate )
{
    if ( !GetParent() /*&& ! (GetStyle() & SalFrameStyleFlags::INTRO)*/ )
    {
        KDE5XLib* pXLib = new KDE5XLib();//static_cast<KDE5XLib*>(GetDisplay()->GetXLib());
        pXLib->doStartup();
    }

    //X11SalFrame::Show( bVisible, bNoActivate );
}

/** Helper function to convert colors.
*/
static Color toColor( const QColor &rColor )
{
    return Color( rColor.red(), rColor.green(), rColor.blue() );
}


/** Helper function to add information to Font from QFont.

    Mostly grabbed from the Gtk+ vclplug (salnativewidgets-gtk.cxx).
*/
static vcl::Font toFont( const QFont &rQFont, const css::lang::Locale& rLocale )
{
    psp::FastPrintFontInfo aInfo;
    QFontInfo qFontInfo( rQFont );

    // set family name
    aInfo.m_aFamilyName = OUString( static_cast<const char *>(rQFont.family().toUtf8()), strlen( static_cast<const char *>(rQFont.family().toUtf8()) ), RTL_TEXTENCODING_UTF8 );

    // set italic
    aInfo.m_eItalic = ( qFontInfo.italic()? ITALIC_NORMAL: ITALIC_NONE );

    // set weight
    int nWeight = qFontInfo.weight();
    if ( nWeight <= QFont::Light )
        aInfo.m_eWeight = WEIGHT_LIGHT;
    else if ( nWeight <= QFont::Normal )
        aInfo.m_eWeight = WEIGHT_NORMAL;
    else if ( nWeight <= QFont::DemiBold )
        aInfo.m_eWeight = WEIGHT_SEMIBOLD;
    else if ( nWeight <= QFont::Bold )
        aInfo.m_eWeight = WEIGHT_BOLD;
    else
        aInfo.m_eWeight = WEIGHT_ULTRABOLD;

    // set width
    int nStretch = rQFont.stretch();
    if ( nStretch <= QFont::UltraCondensed )
        aInfo.m_eWidth = WIDTH_ULTRA_CONDENSED;
    else if ( nStretch <= QFont::ExtraCondensed )
        aInfo.m_eWidth = WIDTH_EXTRA_CONDENSED;
    else if ( nStretch <= QFont::Condensed )
        aInfo.m_eWidth = WIDTH_CONDENSED;
    else if ( nStretch <= QFont::SemiCondensed )
        aInfo.m_eWidth = WIDTH_SEMI_CONDENSED;
    else if ( nStretch <= QFont::Unstretched )
        aInfo.m_eWidth = WIDTH_NORMAL;
    else if ( nStretch <= QFont::SemiExpanded )
        aInfo.m_eWidth = WIDTH_SEMI_EXPANDED;
    else if ( nStretch <= QFont::Expanded )
        aInfo.m_eWidth = WIDTH_EXPANDED;
    else if ( nStretch <= QFont::ExtraExpanded )
        aInfo.m_eWidth = WIDTH_EXTRA_EXPANDED;
    else
        aInfo.m_eWidth = WIDTH_ULTRA_EXPANDED;

    SAL_INFO( "vcl.kde5", "font name BEFORE system match: \"" << aInfo.m_aFamilyName << "\"" );

    // match font to e.g. resolve "Sans"
    psp::PrintFontManager::get().matchFont( aInfo, rLocale );

    SAL_INFO( "vcl.kde5", "font match " <<
              (aInfo.m_nID != 0 ? "succeeded" : "failed") <<
              ", name AFTER: \"" << aInfo.m_aFamilyName << "\"" );

    // font height
    int nPointHeight = qFontInfo.pointSize();
    if ( nPointHeight <= 0 )
        nPointHeight = rQFont.pointSize();

    // Create the font
    vcl::Font aFont( aInfo.m_aFamilyName, Size( 0, nPointHeight ) );
    if( aInfo.m_eWeight != WEIGHT_DONTKNOW )
        aFont.SetWeight( aInfo.m_eWeight );
    if( aInfo.m_eWidth != WIDTH_DONTKNOW )
        aFont.SetWidthType( aInfo.m_eWidth );
    if( aInfo.m_eItalic != ITALIC_DONTKNOW )
        aFont.SetItalic( aInfo.m_eItalic );
    if( aInfo.m_ePitch != PITCH_DONTKNOW )
        aFont.SetPitch( aInfo.m_ePitch );

    return aFont;
}

/** Implementation of KDE integration's main method.
*/
void KDE5SalFrame::UpdateSettings( AllSettings& rSettings )
{
    StyleSettings style( rSettings.GetStyleSettings() );
    bool bSetTitleFont = false;

    // General settings
    QPalette pal = QApplication::palette();

    style.SetToolbarIconSize( ToolbarIconSize::Large );

    style.SetActiveColor(toColor(pal.color(QPalette::Active, QPalette::Window)));
    style.SetDeactiveColor(toColor(pal.color(QPalette::Inactive, QPalette::Window)));

    style.SetActiveTextColor(toColor(pal.color(QPalette::Active, QPalette::WindowText)));
    style.SetDeactiveTextColor(toColor(pal.color(QPalette::Inactive, QPalette::WindowText)));

    // WM settings
    /*KConfig *pConfig = KGlobal::config().data();
    if ( pConfig )
    {
        const char *pKey;

        {
            KConfigGroup aWMGroup = pConfig->group( "WM" );

            pKey = "titleFont";
            if (aWMGroup.hasKey(pKey))
            {
                vcl::Font aFont = toFont(aWMGroup.readEntry(pKey, QFont()),
                                         rSettings.GetUILanguageTag().getLocale());
                style.SetTitleFont( aFont );
                bSetTitleFont = true;
            }
        }

        KConfigGroup aIconsGroup = pConfig->group("Icons");

        pKey = "Theme";
        if (aIconsGroup.hasKey(pKey))
            style.SetPreferredIconTheme( readEntryUntranslated(&aIconsGroup, pKey));

        //toolbar
        pKey = "toolbarFont";
        if (aIconsGroup.hasKey(pKey))
        {
            vcl::Font aFont = toFont(aIconsGroup.readEntry(pKey, QFont()),
                                     rSettings.GetUILanguageTag().getLocale());
            style.SetToolFont( aFont );
        }
    }*/

    Color aFore = toColor( pal.color( QPalette::Active, QPalette::WindowText ) );
    Color aBack = toColor( pal.color( QPalette::Active, QPalette::Window ) );
    Color aText = toColor( pal.color( QPalette::Active, QPalette::Text ) );
    Color aBase = toColor( pal.color( QPalette::Active, QPalette::Base ) );
    Color aButn = toColor( pal.color( QPalette::Active, QPalette::ButtonText ) );
    Color aMid = toColor( pal.color( QPalette::Active, QPalette::Mid ) );
    Color aHigh = toColor( pal.color( QPalette::Active, QPalette::Highlight ) );
    Color aHighText = toColor( pal.color( QPalette::Active, QPalette::HighlightedText ) );

    style.SetSkipDisabledInMenus( true );

    // Foreground
    style.SetRadioCheckTextColor( aFore );
    style.SetLabelTextColor( aFore );
    style.SetDialogTextColor( aFore );
    style.SetGroupTextColor( aFore );

    // Text
    style.SetFieldTextColor( aText );
    style.SetFieldRolloverTextColor( aText );
    style.SetWindowTextColor( aText );
    style.SetToolTextColor( aText );

    // Base
    style.SetFieldColor( aBase );
    style.SetWindowColor( aBase );
    style.SetActiveTabColor( aBase );

    // Buttons
    style.SetButtonTextColor( aButn );
    style.SetButtonRolloverTextColor( aButn );

    // Tabs
    style.SetTabTextColor( aButn );
    style.SetTabRolloverTextColor( aButn );
    style.SetTabHighlightTextColor( aButn );

    // Disable color
    style.SetDisableColor( toColor( pal.color( QPalette::Disabled, QPalette::WindowText ) ) );

    // Background
    style.BatchSetBackgrounds( aBack );
    style.SetInactiveTabColor( aBack );

    // Workspace
    style.SetWorkspaceColor( aMid );

    // Selection
    style.SetHighlightColor( aHigh );
    style.SetHighlightTextColor( aHighText );

    // Tooltip
    style.SetHelpColor( toColor( QToolTip::palette().color( QPalette::Active, QPalette::ToolTipBase )));
    style.SetHelpTextColor( toColor( QToolTip::palette().color( QPalette::Active, QPalette::ToolTipText )));

    // Font
    vcl::Font aFont = toFont( QApplication::font(), rSettings.GetUILanguageTag().getLocale() );

    style.BatchSetFonts( aFont, aFont );

    aFont.SetWeight( WEIGHT_BOLD );
    if( !bSetTitleFont )
    {
        style.SetTitleFont( aFont );
    }
    style.SetFloatTitleFont( aFont );
    style.SetHelpFont( toFont( QToolTip::font(), rSettings.GetUILanguageTag().getLocale()));

    int flash_time = QApplication::cursorFlashTime();
    style.SetCursorBlinkTime( flash_time != 0 ? flash_time/2 : STYLE_CURSOR_NOBLINKTIME );

    // Menu
    std::unique_ptr<QMenuBar> pMenuBar = std::unique_ptr<QMenuBar>( new QMenuBar() );
    QPalette qMenuCG = pMenuBar->palette();

    // Menu text and background color, theme specific
    Color aMenuFore = toColor( qMenuCG.color( QPalette::WindowText ) );
    Color aMenuBack = toColor( qMenuCG.color( QPalette::Window ) );

    style.SetMenuTextColor( aMenuFore );
    style.SetMenuBarTextColor( style.GetPersonaMenuBarTextColor().get_value_or( aMenuFore ) );
    style.SetMenuColor( aMenuBack );
    style.SetMenuBarColor( aMenuBack );
    style.SetMenuHighlightColor( toColor ( qMenuCG.color( QPalette::Highlight ) ) );
    style.SetMenuHighlightTextColor( toColor ( qMenuCG.color( QPalette::HighlightedText ) ) );

    // set special menubar highlight text color
    if ( QApplication::style()->inherits( "HighContrastStyle" ) )
        ImplGetSVData()->maNWFData.maMenuBarHighlightTextColor = toColor( qMenuCG.color( QPalette::HighlightedText ) );
    else
        ImplGetSVData()->maNWFData.maMenuBarHighlightTextColor = aMenuFore;

    // set menubar rollover color
    if ( pMenuBar->style()->styleHint( QStyle::SH_MenuBar_MouseTracking ) )
    {
        style.SetMenuBarRolloverColor( toColor ( qMenuCG.color( QPalette::Highlight ) ) );
        style.SetMenuBarRolloverTextColor( ImplGetSVData()->maNWFData.maMenuBarHighlightTextColor );
    }
    else
    {
        style.SetMenuBarRolloverColor( aMenuBack );
        style.SetMenuBarRolloverTextColor( aMenuFore );
    }
    style.SetMenuBarHighlightTextColor(style.GetMenuHighlightTextColor());

    // Font
    aFont = toFont( pMenuBar->font(), rSettings.GetUILanguageTag().getLocale() );
    style.SetMenuFont( aFont );

    // Scroll bar size
    style.SetScrollBarSize( QApplication::style()->pixelMetric( QStyle::PM_ScrollBarExtent ) );
    style.SetMinThumbSize( QApplication::style()->pixelMetric( QStyle::PM_ScrollBarSliderMin ));

    // These colors are used for the ruler text and marks
    style.SetShadowColor(toColor(pal.color(QPalette::Disabled, QPalette::WindowText)));
    style.SetDarkShadowColor(toColor(pal.color(QPalette::Inactive, QPalette::WindowText)));

    rSettings.SetStyleSettings( style );
}

void KDE5SalFrame::ReleaseGraphics( SalGraphics *pGraphics )
{
    for( int i = 0; i < nMaxGraphics; i++ )
    {
        if( m_aGraphics[i].pGraphics.get() == pGraphics )
        {
            m_aGraphics[i].bInUse = false;
            if ( m_aGraphics[i].pSurface != nullptr )
                cairo_surface_destroy( m_aGraphics[i].pSurface );
            break;
        }
    }
}

void KDE5SalFrame::updateGraphics( bool bClear )
{
    vcl::Window* pWindow = GetWindow();
    for( int i = 0; i < nMaxGraphics; i++ )
    {
        /*if( m_aGraphics[i].bInUse )
            m_aGraphics[i].pGraphics->SetDrawable( aDrawable, GetScreenNumber() );*/
    }
}

SalGraphics* KDE5SalFrame::AcquireGraphics()
{
    if( GetWindow() )
    {
        basegfx::B2IVector aSize( 142, 142 );
        for( int i = 0; i < nMaxGraphics; i++ )
        {
            if( ! m_aGraphics[i].bInUse )
            {
                m_aGraphics[i].bInUse = true;
                if( ! m_aGraphics[i].pGraphics )
                {
                    m_aGraphics[i].pGraphics.reset( new KDE5SalGraphics( this, m_pWindow ) );

                    //FIXME: those parameters are completely arbitrary, IDK what the right ones should be yet
                    m_aGraphics[i].pSurface = cairo_image_surface_create( CAIRO_FORMAT_ARGB32, aSize.getX(), aSize.getY() );
                    m_aGraphics[i].pGraphics->setSurface( m_aGraphics[i].pSurface, aSize );
                    //m_aGraphics[i].pGraphics->Init( this, GetWindow(), GetScreenNumber() );
                }
                return m_aGraphics[i].pGraphics.get();
            }
        }
    }

    return nullptr;
}

cairo_t* KDE5SalFrame::getCairoContext() const
{
    for( int i = 0; i < nMaxGraphics; i++ )
    {
        if( ! m_aGraphics[i].bInUse )
        {
            cairo_t* cr = cairo_create( m_aGraphics[i].pSurface );
            assert(cr);
            return cr;
        }
    }
}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
