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

#ifndef INCLUDED_SVX_INC_SDR_OVERLAY_OVERLAYHELPLINE_HXX
#define INCLUDED_SVX_INC_SDR_OVERLAY_OVERLAYHELPLINE_HXX

#include <svx/sdr/overlay/overlayobject.hxx>
#include <svx/svdhlpln.hxx>


namespace sdr
{
    namespace overlay
    {
        class OverlayHelplineStriped final : public OverlayObjectWithBasePosition
        {
            // remember HelpLineKind
            SdrHelpLineKind                             meKind;

            // geometry creation for OverlayObject
            virtual drawinglayer::primitive2d::Primitive2DContainer createOverlayObjectPrimitive2DSequence() override;

        public:
            explicit OverlayHelplineStriped(
                const basegfx::B2DPoint& rBasePos,
                SdrHelpLineKind eNewKind);
            virtual ~OverlayHelplineStriped() override;

            // dat read access
            SdrHelpLineKind getKind() const { return meKind; }

            // react on stripe definition change
            virtual void stripeDefinitionHasChanged() override;
        };
    } // end of namespace overlay
} // end of namespace sdr


#endif // INCLUDED_SVX_INC_SDR_OVERLAY_OVERLAYHELPLINE_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
