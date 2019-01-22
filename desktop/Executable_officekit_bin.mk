# -*- Mode: makefile-gmake; tab-width: 4; indent-tabs-mode: t -*-
#
# This file is part of the LibreOffice project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

$(eval $(call gb_Executable_Executable,officekit_bin))

$(eval $(call gb_Executable_set_targettype_gui,officekit_bin,YES))

$(eval $(call gb_Executable_set_include,officekit_bin,\
    $$(INCLUDE) \
    -I$(SRCDIR)/desktop/source/inc \
    -I$(SRCDIR)/desktop/source/officekit \
))

$(eval $(call gb_Executable_use_libraries,officekit_bin,\
    sal \
    sofficeapp \
    officekit\
    officekitint\
    $(if $(DISABLE_DYNLOADING), \
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

$(eval $(call gb_Executable_use_externals,officekit_bin,\
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

$(eval $(call gb_Executable_use_static_libraries,officekit_bin,\
    $(if $(DISABLE_DYNLOADING), \
    findsofficepath \
    ulingu \
    ) \
))

$(eval $(call gb_Executable_add_exception_objects,officekit_bin,\
    desktop/source/officekit/main \
))

ifneq ($(BUILD_FOR_ALIOS),)
$(eval $(call gb_Executable_add_ldflags,officekit_bin,\
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

# vim: set ts=4 sw=4 et:
