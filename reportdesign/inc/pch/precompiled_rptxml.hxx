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

 Generated on 2017-09-20 22:52:55 using:
 ./bin/update_pch reportdesign rptxml --cutoff=2 --exclude:system --exclude:module --include:local

 If after updating build fails, use the following command to locate conflicting headers:
 ./bin/update_pch_bisect ./reportdesign/inc/pch/precompiled_rptxml.hxx "make reportdesign.build" --find-conflicts
*/

#include <cassert>
#include <cstddef>
#include <cstring>
#include <memory>
#include <unordered_set>
#include <vector>
#include <osl/diagnose.h>
#include <rtl/math.hxx>
#include <rtl/ref.hxx>
#include <rtl/strbuf.h>
#include <rtl/strbuf.hxx>
#include <rtl/string.h>
#include <rtl/string.hxx>
#include <rtl/stringutils.hxx>
#include <rtl/textenc.h>
#include <rtl/unload.h>
#include <rtl/ustrbuf.h>
#include <rtl/ustrbuf.hxx>
#include <rtl/ustring.hxx>
#include <sal/config.h>
#include <sal/detail/log.h>
#include <sal/macros.h>
#include <sal/saldllapi.h>
#include <sal/types.h>
#include <salhelper/simplereferenceobject.hxx>
#include <vcl/dllapi.h>
#include <vcl/errcode.hxx>
#include <vcl/svapp.hxx>
#include <RptDef.hxx>
#include <com/sun/star/awt/FontDescriptor.hpp>
#include <com/sun/star/awt/ImagePosition.hpp>
#include <com/sun/star/awt/ImageScaleMode.hpp>
#include <com/sun/star/awt/TextAlign.hpp>
#include <com/sun/star/beans/NamedValue.hpp>
#include <com/sun/star/beans/PropertyAttribute.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/beans/XMultiPropertySet.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/beans/XTolerantMultiPropertySet.hpp>
#include <com/sun/star/chart/XComplexDescriptionAccess.hpp>
#include <com/sun/star/io/XInputStream.hpp>
#include <com/sun/star/lang/IllegalArgumentException.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <com/sun/star/lang/XSingleComponentFactory.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <com/sun/star/lang/XTypeProvider.hpp>
#include <com/sun/star/lang/XUnoTunnel.hpp>
#include <com/sun/star/reflection/ProxyFactory.hpp>
#include <com/sun/star/registry/XRegistryKey.hpp>
#include <com/sun/star/report/ForceNewPage.hpp>
#include <com/sun/star/report/GroupOn.hpp>
#include <com/sun/star/report/KeepTogether.hpp>
#include <com/sun/star/report/ReportPrintOption.hpp>
#include <com/sun/star/report/XFixedLine.hpp>
#include <com/sun/star/report/XFixedText.hpp>
#include <com/sun/star/report/XShape.hpp>
#include <com/sun/star/sdb/CommandType.hpp>
#include <com/sun/star/sdb/XOfficeDatabaseDocument.hpp>
#include <com/sun/star/style/ParagraphAdjust.hpp>
#include <com/sun/star/style/XStyle.hpp>
#include <com/sun/star/table/BorderLine2.hpp>
#include <com/sun/star/uno/Any.hxx>
#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/uno/Type.hxx>
#include <com/sun/star/uno/TypeClass.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <com/sun/star/util/DateTime.hpp>
#include <com/sun/star/util/MeasureUnit.hpp>
#include <com/sun/star/util/XNumberFormatsSupplier.hpp>
#include <com/sun/star/xml/sax/XAttributeList.hpp>
#include <comphelper/comphelperdllapi.h>
#include <comphelper/documentconstants.hxx>
#include <comphelper/extract.hxx>
#include <comphelper/genericpropertyset.hxx>
#include <comphelper/namecontainer.hxx>
#include <comphelper/sequenceashashmap.hxx>
#include <connectivity/dbtools.hxx>
#include <cppu/unotype.hxx>
#include <cppuhelper/cppuhelperdllapi.h>
#include <cppuhelper/factory.hxx>
#include <cppuhelper/supportsservice.hxx>
#include <o3tl/typed_flags_set.hxx>
#include <sax/tools/converter.hxx>
#include <strings.hxx>
#include <tools/toolsdllapi.h>
#include <ucbhelper/content.hxx>
#include <unotools/options.hxx>
#include <unotools/saveopt.hxx>
#include <unotools/unotoolsdllapi.h>
#include <xmloff/ProgressBarHelper.hxx>
#include <xmloff/XMLConstantsPropertyHandler.hxx>
#include <xmloff/attrlist.hxx>
#include <xmloff/dllapi.h>
#include <xmloff/families.hxx>
#include <xmloff/nmspmap.hxx>
#include <xmloff/prstylei.hxx>
#include <xmloff/txtimp.hxx>
#include <xmloff/txtprmap.hxx>
#include <xmloff/xmlement.hxx>
#include <xmloff/xmlictxt.hxx>
#include <xmloff/xmlimppr.hxx>
#include <xmloff/xmlnmspe.hxx>
#include <xmloff/xmlprhdl.hxx>
#include <xmloff/xmlstyle.hxx>
#include <xmloff/xmltoken.hxx>
#include <xmloff/xmluconv.hxx>

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
