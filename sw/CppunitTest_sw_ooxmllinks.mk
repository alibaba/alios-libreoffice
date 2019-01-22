# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#*************************************************************************
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
#*************************************************************************

$(eval $(call gb_CppunitTest_CppunitTest,sw_ooxmllinks))

$(eval $(call gb_CppunitTest_add_exception_objects,sw_ooxmllinks, \
    sw/qa/extras/ooxmlexport/ooxmllinks \
))

$(eval $(call gb_CppunitTest_use_libraries,sw_ooxmllinks, \
    $(sw_ooxmlexport_libraries) \
))

$(eval $(call gb_CppunitTest_use_externals,sw_ooxmllinks,\
    boost_headers \
    libxml2 \
))

$(eval $(call gb_CppunitTest_set_include,sw_ooxmllinks,\
    -I$(SRCDIR)/sw/inc \
    -I$(SRCDIR)/sw/source/core/inc \
    -I$(SRCDIR)/sw/qa/extras/inc \
    $$(INCLUDE) \
))

$(eval $(call gb_CppunitTest_use_sdk_api,sw_ooxmllinks))

$(eval $(call gb_CppunitTest_use_ure,sw_ooxmllinks))
$(eval $(call gb_CppunitTest_use_vcl,sw_ooxmllinks))

$(eval $(call gb_CppunitTest_use_components,sw_ooxmllinks,\
    $(sw_ooxmlexport_components) \
))

$(eval $(call gb_CppunitTest_use_configuration,sw_ooxmllinks))

# vim: set noet sw=4 ts=4:
