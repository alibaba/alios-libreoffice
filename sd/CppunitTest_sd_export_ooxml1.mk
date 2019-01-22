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

$(eval $(call gb_CppunitTest_CppunitTest,sd_export_ooxml1))

$(eval $(call gb_CppunitTest_use_externals,sd_export_ooxml1,\
	boost_headers \
	libxml2 \
))

$(eval $(call gb_CppunitTest_add_exception_objects,sd_export_ooxml1, \
	sd/qa/unit/export-tests-ooxml1 \
))

$(eval $(call gb_CppunitTest_use_libraries,sd_export_ooxml1, \
	$(call gb_Helper_optional,AVMEDIA,avmedia) \
	basegfx \
	comphelper \
	cppu \
	cppuhelper \
	drawinglayer \
	editeng \
	for \
	forui \
	i18nlangtag \
	msfilter \
	oox \
	sal \
	salhelper \
	sax \
	sd \
	sfx \
	sot \
	svl \
	svt \
	svx \
	svxcore \
	test \
	tl \
	tk \
	ucbhelper \
	unotest \
	utl \
	vcl \
	xo \
))

$(eval $(call gb_CppunitTest_set_include,sd_export_ooxml1,\
	-I$(SRCDIR)/sd/source/ui/inc \
	-I$(SRCDIR)/sd/inc \
	$$(INCLUDE) \
))

$(eval $(call gb_CppunitTest_use_sdk_api,sd_export_ooxml1))

$(eval $(call gb_CppunitTest_use_ure,sd_export_ooxml1))
$(eval $(call gb_CppunitTest_use_vcl,sd_export_ooxml1))

$(eval $(call gb_CppunitTest_use_rdb,sd_export_ooxml1,services))

$(eval $(call gb_CppunitTest_use_custom_headers,sd_export_ooxml1,\
	officecfg/registry \
))

$(eval $(call gb_CppunitTest_use_configuration,sd_export_ooxml1))

$(eval $(call gb_CppunitTest_add_arguments,sd_export_ooxml1,\
	-env:SVG_DISABLE_FONT_EMBEDDING= \
))

# vim: set noet sw=4 ts=4:
