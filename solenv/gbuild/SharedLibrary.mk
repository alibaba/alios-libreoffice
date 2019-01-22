# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
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


# Static Library class

# defined globally in gbuild.mk
# defined by platform
#  gb_SharedLibrary_get_filename
#  gb_SharedLibrary_PLAINEXT
#  gb_SharedLibrary_SharedLibrary_platform


# EVIL: gb_SharedLibrary and gb_Library need the same deliver rule because they are indistinguishable on windows
.PHONY : $(WORKDIR)/Clean/SharedLibrary/%
$(WORKDIR)/Clean/SharedLibrary/% :
	$(call gb_Helper_abbreviate_dirs,\
		rm -f $(AUXTARGETS))

define gb_SharedLibrary_SharedLibrary
$(call gb_SharedLibrary__SharedLibrary_impl,$(1),$(call gb_SharedLibrary_get_linktarget,$(1)))
$(info gb_SharedLibrary__SharedLibrary_impl,$(1),$(call gb_SharedLibrary_get_linktarget,$(1)))

endef

# call gb_SharedLibrary__SharedLibrary_impl,staticlib,linktarget
define gb_SharedLibrary__SharedLibrary_impl
$(call gb_LinkTarget_LinkTarget,$(2),SharedLibrary_$(1),NONE)
$(call gb_LinkTarget_set_targettype,$(2),SharedLibrary)
$(call gb_SharedLibrary_get_clean_target,$(1)) : $(call gb_LinkTarget_get_clean_target,$(2))
$(call gb_SharedLibrary_get_clean_target,$(1)) : AUXTARGETS :=
$(call gb_SharedLibrary_SharedLibrary_platform,$(1),$(2))
$$(eval $$(call gb_Module_register_target,$(call gb_SharedLibrary_get_target,$(1)),$(call gb_SharedLibrary_get_clean_target,$(1))))
$$(info $$(call gb_Module_register_target,$(call gb_SharedLibrary_get_target,$(1)),$(call gb_SharedLibrary_get_clean_target,$(1))))
$(call gb_Helper_make_userfriendly_targets,$(1),SharedLibrary)

endef

# this is a crude hack to "build" header files with include-what-you-use
define gb_SharedLibrary__add_iwyu_header
$(call gb_LinkTarget_get_target,$(call gb_SharedLibrary_get_linktarget,$(1))) : $(call gb_GenCxxObject_get_target,$(basename $(2)))
$(call gb_GenCxxObject_get_target,$(basename $(2))) : $(SRCDIR)/$(2)
$(call gb_GenCxxObject_get_target,$(basename $(2))) : WARNINGS_NOT_ERRORS := $(true)
$(call gb_GenCxxObject_get_target,$(basename $(2))) : GEN_CXX_SOURCE := $(SRCDIR)/$(2)

endef
define gb_SharedLibrary__add_iwyu_headers
$(foreach file,$(2),$(call gb_SharedLibrary__add_iwyu_header,$(1),$(file)))
endef

# forward the call to the gb_LinkTarget implementation
# (note: because the function name is in $(1), the other args are shifted by 1)
define gb_SharedLibrary__forward_to_Linktarget
$(call gb_LinkTarget_$(1),$(call gb_SharedLibrary_get_linktarget,$(2)),$(3),$(4),SharedLibrary_$(2))

endef

# copy pasta for forwarding: this could be (and was) done more elegantly, but
# these here can be found by both git grep and ctags
gb_SharedLibrary_add_cobject = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_add_cobjects = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_add_cxxobject = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_add_cxxobjects = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_add_objcxxobject = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_add_objcxxobjects = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_add_cxxclrobject = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_add_cxxclrobjects = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_add_asmobject = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_add_asmobjects = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_add_exception_objects = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_add_x64_generated_exception_objects = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_add_generated_cobjects = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_add_x64_generated_cobjects = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_add_generated_exception_objects = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_add_cflags = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_add_cxxflags = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_add_objcflags = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_add_objcxxflags = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_add_cxxclrflags = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_add_defs = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_set_include = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_add_ldflags = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_set_ldflags = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_set_x64 = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_add_libs = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_set_library_path_flags = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_use_api = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_use_sdk_api = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_use_udk_api = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_use_internal_api = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_use_internal_bootstrap_api = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_use_internal_comprehensive_api = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_use_external = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_use_externals = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_use_custom_headers = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_use_package = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_use_packages = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_use_unpacked = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_use_external_project = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_use_static_libraries = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_add_sdi_headers = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_set_precompiled_header = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_set_warnings_not_errors = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_set_external_code = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_set_generated_cxx_suffix = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))
gb_SharedLibrary_use_libraries = $(call gb_SharedLibrary__forward_to_Linktarget,$(subst gb_SharedLibrary_,,$(0)),$(1),$(2),$(3))

# vim: set noet sw=4:
