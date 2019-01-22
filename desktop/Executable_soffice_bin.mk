# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_Executable_Executable,soffice_bin))

$(eval $(call gb_Executable_set_targettype_gui,soffice_bin,YES))

$(eval $(call gb_Executable_set_include,soffice_bin,\
    $$(INCLUDE) \
    -I$(SRCDIR)/desktop/source/inc \
))

$(eval $(call gb_Executable_use_libraries,soffice_bin,\
    sal \
    sofficeapp \
    $(if $(DISABLE_DYNLOADING), \
        officekitint \
        cppu \
        $(MERGE_LIBRARY_LIST) \
        xmlreader \
        salhelper \
        cppuhelper \
        i18nlangtag \
        acc \
        affine_uno_uno \
        graphicfilter \
        cui \
        unoidl \
        reg \
        gie \
        store \
        clew \
        tk \
        frm \
        sw \
        swd \
        swui \
        hyphen \
        i18nsearch \
        invocadapt \
        chartcore \
        mtfrenderer \
        lnth \
        odfflatxml \
        wpftwriter \
        oox \
        sd \
        sdd \
        sdui \
        sdfilt \
        sm \
        spell \
        srtrs1 \
        embobj \
        storagefd \
        svgfilter \
        icg \
        svgio \
        ucphier1 \
        ucptdoc1 \
        unordf \
        vclcanvas \
        xmlsecurity \
        xmlfa \
        xmlfd \
        expwrap \
        xof \
        xsec_xmlsec \
        xsec_fw \
        canvasfactory \
        writerfilter \
        writerperfect \
        reflection \
        introspection \
        stocservices \
        chartcontroller \
        textfd \
        animcore \
        msword \
        dba \
        emboleobj \
        emfio \
        bootstrap \
        evtatt \
        i18npool \
        comphelper \
        collator_data \
        textconv_dict \
        index_data \
        localedata_en \
        localedata_es \
        localedata_euro \
        localedata_others \
        $(gb_CPPU_ENV)_uno \
        ) \
))

$(eval $(call gb_Executable_use_externals,soffice_bin,\
    $(if $(DISABLE_DYNLOADING), \
        harfbuzz \
        boost_lib \
        boost_filesystem \
        boost_locale \
        lcms2 \
        gpgmepp \
        graphite \
        liblangtag \
        expat \
        librdf \
        clucene \
        orcus \
        orcus-parser \
        hyphen \
        mythes \
        hunspell \
        revenge \
        odfgen \
        wps \
        mwaw \
        ) \
))

$(eval $(call gb_Executable_use_static_libraries,soffice_bin,\
    $(if $(DISABLE_DYNLOADING), \
    findsofficepath \
    ulingu \
    ) \
))

$(eval $(call gb_Executable_add_cobjects,soffice_bin,\
    desktop/source/app/main \
))

ifneq ($(BUILD_FOR_ALIOS),)
$(eval $(call gb_Executable_add_ldflags,soffice_bin,\
    $(FREETYPE_LIBS) \
    $(CURL_LIBS) \
    $(FONTCONFIG_LIBS) \
    $(ICU_LIBS) \
    $(NSS_LIBS) \
    $(LIBJPEG_LIBS) \
    $(LIBPNG_LIBS) \
    $(ZLIB_LIBS) \
    $(LIBASSUAN_LIBS) \
    $(GPG_ERROR_LIBS) \
    $(LIBXML_LIBS) \
    $(XMLSEC_LIBS) \
    $(LIBXSLT_LIBS) \
    $(PIXMAN_LIBS) \
    $(CAIRO_LIBS) \
    $(ALIOS_LOG_LDFLAGS) \
    $(LDFLAGS) \
    $(if $(DISABLE_DYNLOADING), -lstdc++ -lm) \
))
endif

ifeq ($(OS),MACOSX)

$(eval $(call gb_Executable_set_ldflags,\
    $(filter-out -bind_at_load,$$(LDFLAGS)) \
))

endif

ifeq ($(OS),WNT)

$(eval $(call gb_Executable_set_targettype_gui,soffice_bin,YES))

$(eval $(call gb_Executable_add_nativeres,soffice_bin,sofficebin/officeloader))

ifeq ($(COM),MSC)

$(eval $(call gb_Executable_add_ldflags,soffice_bin,\
    /STACK:10000000 \
))

endif

endif

# vim: set ts=4 sw=4 et:
