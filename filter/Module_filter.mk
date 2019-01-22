#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# This file incorporates work covered by the following license notice:
#
#   Licensed to the Apache Software Foundation (ASF) under one or more
#   contributor license agreements. See the NOTICE file distributed
#   with this work for additional information regarding copyright
#   ownership. The ASF licenses this file to you under the Apache
#   License, Version 2.0 (the "License"); you may not use this file
#   except in compliance with the License. You may obtain a copy of
#   the License at http://www.apache.org/licenses/LICENSE-2.0 .
#

$(eval $(call gb_Module_Module,filter))

$(eval $(call gb_Module_add_targets,filter,\
	Configuration_filter \
	CustomTarget_svg \
    Library_gie \
	Library_filterconfig \
	$(if $(BUILD_FOR_ALIOS),,Library_flash) \
    Library_icg \
	Library_msfilter \
	Library_odfflatxml \
	Library_pdffilter \
	Library_storagefd \
	Library_svgfilter \
	Library_graphicfilter \
	$(if $(BUILD_FOR_ALIOS),,Library_t602filter) \
	Library_textfd \
	Library_xmlfa \
	Library_xmlfd \
	$(if $(BUILD_FOR_ALIOS),,Library_xsltdlg) \
	$(if $(BUILD_FOR_ALIOS),,Library_xsltfilter) \
	$(if $(BUILD_FOR_ALIOS),,Package_docbook) \
	$(if $(BUILD_FOR_ALIOS),,Package_xhtml) \
	$(if $(BUILD_FOR_ALIOS),,Package_xslt) \
	UIConfig_filter \
))

$(eval $(call gb_Module_add_l10n_targets,filter,\
	AllLangMoTarget_flt \
))

ifeq ($(BUILD_FOR_ALIOS),)
ifneq (,$(filter DESKTOP,$(BUILD_TYPE)))
$(eval $(call gb_Module_add_targets,filter,\
	Executable_svg2odf \
))
endif
endif

$(eval $(call gb_Module_add_check_targets,filter,\
    CppunitTest_filter_xslt \
    CppunitTest_filter_priority \
    CppunitTest_filter_msfilter \
    CppunitTest_filter_textfilterdetect \
))

ifneq ($(DISABLE_CVE_TESTS),TRUE)
$(eval $(call gb_Module_add_check_targets,filter,\
    CppunitTest_filter_dxf_test \
    CppunitTest_filter_eps_test \
    CppunitTest_filter_met_test \
    CppunitTest_filter_pcd_test \
    CppunitTest_filter_pcx_test \
    CppunitTest_filter_pict_test \
    CppunitTest_filter_ppm_test \
    CppunitTest_filter_psd_test \
    CppunitTest_filter_ras_test \
    CppunitTest_filter_tiff_test \
    CppunitTest_filter_tga_test \
))
endif

# TODO
#$(eval $(call gb_Module_add_subsequentcheck_targets,filter,\
	JunitTest_filter_complex \
))

# screenshots
$(eval $(call gb_Module_add_screenshot_targets,filter,\
    CppunitTest_filter_dialogs_test \
))

# vim: set noet sw=4 ts=4:
