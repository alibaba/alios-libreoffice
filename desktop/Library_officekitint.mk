# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_Library_Library,officekitint))

$(eval $(call gb_Library_set_include,officekitint,\
    $$(INCLUDE) \
    -I$(SRCDIR)/desktop/inc \
    -I$(SRCDIR)/desktop/source/inc \
))

$(eval $(call gb_Library_use_sdk_api,officekitint))

$(eval $(call gb_Library_add_libs,officekitint,\
    $(if $(filter LINUX %BSD SOLARIS, $(OS)), \
        $(DLOPEN_LIBS) \
        -lpthread \
    ) \
))

$(eval $(call gb_Library_use_externals,officekitint, \
))



$(eval $(call gb_Library_add_exception_objects,officekitint,\
    desktop/source/officekit/officekit_int \
))

# vim: set ts=4 sw=4 et:
