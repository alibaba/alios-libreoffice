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

#define vertcurs_mask_width 16
#define vertcurs_mask_height 16
#define vertcurs_mask_x_hot 8
#define vertcurs_mask_y_hot 8
static unsigned char vertcurs_mask_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xe0, 0x07, 0xe0, 0x0f, 0xf0,
   0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0x0f, 0xf0, 0x07, 0xe0, 0x07, 0xe0,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
