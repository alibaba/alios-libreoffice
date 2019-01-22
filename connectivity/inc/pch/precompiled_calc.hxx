/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/*
 This file has been autogenerated by update_pch.sh. It is possible to edit it
 manually (such as when an include file has been moved/renamed/removed). All such
 manual changes will be rewritten by the next run of update_pch.sh (which presumably
 also fixes all possible problems, so it's usually better to use it).

 Generated on 2017-09-20 22:51:50 using:
 ./bin/update_pch connectivity calc --cutoff=2 --exclude:system --exclude:module --exclude:local

 If after updating build fails, use the following command to locate conflicting headers:
 ./bin/update_pch_bisect ./connectivity/inc/pch/precompiled_calc.hxx "make connectivity.build" --find-conflicts
*/

#include <cassert>
#include <cstddef>
#include <cstring>
#include <stdlib.h>
#include <osl/diagnose.h>
#include <osl/thread.h>
#include <rtl/math.hxx>
#include <rtl/string.h>
#include <rtl/stringutils.hxx>
#include <rtl/textenc.h>
#include <rtl/unload.h>
#include <rtl/ustrbuf.h>
#include <rtl/ustrbuf.hxx>
#include <rtl/ustring.hxx>
#include <sal/config.h>
#include <sal/types.h>
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#include <com/sun/star/sdbc/ColumnValue.hpp>
#include <com/sun/star/sdbc/DataType.hpp>
#include <com/sun/star/sdbc/XRow.hpp>
#include <com/sun/star/sheet/XCellRangeAddressable.hpp>
#include <com/sun/star/sheet/XCellRangesQuery.hpp>
#include <com/sun/star/sheet/XDatabaseRange.hpp>
#include <com/sun/star/sheet/XDatabaseRanges.hpp>
#include <com/sun/star/sheet/XSpreadsheet.hpp>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/uno/Type.hxx>
#include <com/sun/star/util/NumberFormat.hpp>
#include <comphelper/processfactory.hxx>
#include <comphelper/types.hxx>
#include <cppuhelper/cppuhelperdllapi.h>
#include <svl/svldllapi.h>
#include <connectivity/dbexception.hxx>
#include <connectivity/dbtoolsdllapi.hxx>

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
