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

#include <unx/x11_cursors/nodrop_curs.h>
#include <unx/x11_cursors/nodrop_mask.h>
#include <unx/x11_cursors/magnify_curs.h>
#include <unx/x11_cursors/magnify_mask.h>
#include <unx/x11_cursors/rotate_curs.h>
#include <unx/x11_cursors/rotate_mask.h>
#include <unx/x11_cursors/hshear_curs.h>
#include <unx/x11_cursors/hshear_mask.h>
#include <unx/x11_cursors/vshear_curs.h>
#include <unx/x11_cursors/vshear_mask.h>
#include <unx/x11_cursors/drawline_curs.h>
#include <unx/x11_cursors/drawline_mask.h>
#include <unx/x11_cursors/drawrect_curs.h>
#include <unx/x11_cursors/drawrect_mask.h>
#include <unx/x11_cursors/drawpolygon_curs.h>
#include <unx/x11_cursors/drawpolygon_mask.h>
#include <unx/x11_cursors/drawbezier_curs.h>
#include <unx/x11_cursors/drawbezier_mask.h>
#include <unx/x11_cursors/drawarc_curs.h>
#include <unx/x11_cursors/drawarc_mask.h>
#include <unx/x11_cursors/drawpie_curs.h>
#include <unx/x11_cursors/drawpie_mask.h>
#include <unx/x11_cursors/drawcirclecut_curs.h>
#include <unx/x11_cursors/drawcirclecut_mask.h>
#include <unx/x11_cursors/drawellipse_curs.h>
#include <unx/x11_cursors/drawellipse_mask.h>
#include <unx/x11_cursors/drawconnect_curs.h>
#include <unx/x11_cursors/drawconnect_mask.h>
#include <unx/x11_cursors/drawtext_curs.h>
#include <unx/x11_cursors/drawtext_mask.h>
#include <unx/x11_cursors/mirror_curs.h>
#include <unx/x11_cursors/mirror_mask.h>
#include <unx/x11_cursors/crook_curs.h>
#include <unx/x11_cursors/crook_mask.h>
#include <unx/x11_cursors/crop_curs.h>
#include <unx/x11_cursors/crop_mask.h>
#include <unx/x11_cursors/movepoint_curs.h>
#include <unx/x11_cursors/movepoint_mask.h>
#include <unx/x11_cursors/movebezierweight_curs.h>
#include <unx/x11_cursors/movebezierweight_mask.h>
#include <unx/x11_cursors/drawfreehand_curs.h>
#include <unx/x11_cursors/drawfreehand_mask.h>
#include <unx/x11_cursors/drawcaption_curs.h>
#include <unx/x11_cursors/drawcaption_mask.h>
#include <unx/x11_cursors/movedata_curs.h>
#include <unx/x11_cursors/movedata_mask.h>
#include <unx/x11_cursors/copydata_curs.h>
#include <unx/x11_cursors/copydata_mask.h>
#include <unx/x11_cursors/linkdata_curs.h>
#include <unx/x11_cursors/linkdata_mask.h>
#include <unx/x11_cursors/movedlnk_curs.h>
#include <unx/x11_cursors/movedlnk_mask.h>
#include <unx/x11_cursors/copydlnk_curs.h>
#include <unx/x11_cursors/copydlnk_mask.h>
#include <unx/x11_cursors/movefile_curs.h>
#include <unx/x11_cursors/movefile_mask.h>
#include <unx/x11_cursors/copyfile_curs.h>
#include <unx/x11_cursors/copyfile_mask.h>
#include <unx/x11_cursors/linkfile_curs.h>
#include <unx/x11_cursors/linkfile_mask.h>
#include <unx/x11_cursors/moveflnk_curs.h>
#include <unx/x11_cursors/moveflnk_mask.h>
#include <unx/x11_cursors/copyflnk_curs.h>
#include <unx/x11_cursors/copyflnk_mask.h>
#include <unx/x11_cursors/movefiles_curs.h>
#include <unx/x11_cursors/movefiles_mask.h>
#include <unx/x11_cursors/copyfiles_curs.h>
#include <unx/x11_cursors/copyfiles_mask.h>

#include <unx/x11_cursors/chart_curs.h>
#include <unx/x11_cursors/chart_mask.h>
#include <unx/x11_cursors/detective_curs.h>
#include <unx/x11_cursors/detective_mask.h>
#include <unx/x11_cursors/pivotcol_curs.h>
#include <unx/x11_cursors/pivotcol_mask.h>
#include <unx/x11_cursors/pivotfld_curs.h>
#include <unx/x11_cursors/pivotfld_mask.h>
#include <unx/x11_cursors/pivotrow_curs.h>
#include <unx/x11_cursors/pivotrow_mask.h>
#include <unx/x11_cursors/pivotdel_curs.h>
#include <unx/x11_cursors/pivotdel_mask.h>

#include <unx/x11_cursors/chain_curs.h>
#include <unx/x11_cursors/chain_mask.h>
#include <unx/x11_cursors/chainnot_curs.h>
#include <unx/x11_cursors/chainnot_mask.h>

#include <unx/x11_cursors/ase_curs.h>
#include <unx/x11_cursors/ase_mask.h>
#include <unx/x11_cursors/asn_curs.h>
#include <unx/x11_cursors/asn_mask.h>
#include <unx/x11_cursors/asne_curs.h>
#include <unx/x11_cursors/asne_mask.h>
#include <unx/x11_cursors/asns_curs.h>
#include <unx/x11_cursors/asns_mask.h>
#include <unx/x11_cursors/asnswe_curs.h>
#include <unx/x11_cursors/asnswe_mask.h>
#include <unx/x11_cursors/asnw_curs.h>
#include <unx/x11_cursors/asnw_mask.h>
#include <unx/x11_cursors/ass_curs.h>
#include <unx/x11_cursors/ass_mask.h>
#include <unx/x11_cursors/asse_curs.h>
#include <unx/x11_cursors/asse_mask.h>
#include <unx/x11_cursors/assw_curs.h>
#include <unx/x11_cursors/assw_mask.h>
#include <unx/x11_cursors/asw_curs.h>
#include <unx/x11_cursors/asw_mask.h>
#include <unx/x11_cursors/aswe_curs.h>
#include <unx/x11_cursors/aswe_mask.h>
#include <unx/x11_cursors/null_curs.h>
#include <unx/x11_cursors/null_mask.h>

#include <unx/x11_cursors/fill_curs.h>
#include <unx/x11_cursors/fill_mask.h>
#include <unx/x11_cursors/vertcurs_curs.h>
#include <unx/x11_cursors/vertcurs_mask.h>
#include <unx/x11_cursors/tblsele_curs.h>
#include <unx/x11_cursors/tblsele_mask.h>
#include <unx/x11_cursors/tblsels_curs.h>
#include <unx/x11_cursors/tblsels_mask.h>
#include <unx/x11_cursors/tblselse_curs.h>
#include <unx/x11_cursors/tblselse_mask.h>
#include <unx/x11_cursors/tblselw_curs.h>
#include <unx/x11_cursors/tblselw_mask.h>
#include <unx/x11_cursors/tblselsw_curs.h>
#include <unx/x11_cursors/tblselsw_mask.h>
#include <unx/x11_cursors/wshide_curs.h>
#include <unx/x11_cursors/wshide_mask.h>
#include <unx/x11_cursors/wsshow_curs.h>
#include <unx/x11_cursors/wsshow_mask.h>

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
