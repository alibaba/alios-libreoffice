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

#ifndef INCLUDED_SD_SOURCE_UI_INC_DIACTRL_HXX
#define INCLUDED_SD_SOURCE_UI_INC_DIACTRL_HXX

#include <svl/intitem.hxx>
#include <sfx2/bindings.hxx>
#include <svx/itemwin.hxx>
#include <vcl/fixed.hxx>
#include <vcl/field.hxx>
#include <vcl/toolbox.hxx>
#include <sfx2/tbxctrl.hxx>
#include <com/sun/star/frame/XFrame.hpp>

// SdPagesField:

class SdPagesField : public SvxMetricField
{
private:
    css::uno::Reference< css::frame::XFrame > m_xFrame;
protected:
    virtual void    Modify() override;

public:
                    SdPagesField( vcl::Window* pParent,
                                  const css::uno::Reference< css::frame::XFrame >& rFrame );
                    virtual ~SdPagesField() override;

    void            UpdatePagesField( const SfxUInt16Item* pItem );
};

// SdTbxCtlDiaPages:

class SdTbxCtlDiaPages : public SfxToolBoxControl
{
public:
    virtual void        StateChanged( sal_uInt16 nSID, SfxItemState eState,
                                      const SfxPoolItem* pState ) override;
    virtual VclPtr<vcl::Window> CreateItemWindow( vcl::Window *pParent ) override;

    SFX_DECL_TOOLBOX_CONTROL();

    SdTbxCtlDiaPages( sal_uInt16 nSlotId, sal_uInt16 nId, ToolBox& rTbx );
    virtual ~SdTbxCtlDiaPages() override;
};

#endif // INCLUDED_SD_SOURCE_UI_INC_DIACTRL_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
