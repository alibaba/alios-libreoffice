LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

OFFICEKIT_DIR := $(LOCAL_PATH)/../..
OFFICEKIT_INCLUDES := $(OFFICEKIT_DIR)/desktop/source/officekit/
OFFICEKIT_LDFLAGS := -L$(OFFICEKIT_DIR)/alios/deploy/program/ \
                     -Wl,--start-group \
                     $(OFFICEKIT_DIR)/alios/deploy/program/libharfbuzz.a \
                     -lofficekit \
                     -lofficekitint \
                     -luno_sal \
                     -lsofficeapp \
                     -lvcllo \
                     -luno_cppu \
                     -ltllo \
                     -lcomphelper \
                     -lutllo \
                     -lavmedialo \
                     -lbasegfxlo \
                     -lcanvastoolslo \
                     -lcomphelper \
                     -lconfigmgrlo \
                     -lcppcanvaslo \
                     -ldbtoolslo \
                     -ldeployment \
                     -ldeploymentmisclo \
                     -ldesktopbe1lo \
                     -ldrawinglayerlo \
                     -leditenglo \
                     -lfilterconfiglo \
                     -lfsstoragelo \
                     -lfwelo \
                     -lfwilo \
                     -lfwklo \
                     -lhelplinkerlo \
                     -li18npoollo \
                     -li18nutil \
                     -llnglo \
                     -llocalebe1lo \
                     -lmcnttype \
                     -lmsfilterlo \
                     -lopencllo \
                     -lpackage2 \
                     -lsaxlo \
                     -lsblo \
                     -lsfxlo \
                     -lsofficeapp \
                     -lsotlo \
                     -lsvllo \
                     -lsvtlo \
                     -lsvxlo \
                     -lsvxcorelo \
                     -ltklo \
                     -ltllo \
                     -lucb1 \
                     -lucbhelper \
                     -lucpexpand1lo \
                     -lucpfile1 \
                     -lunoxmllo \
                     -lutllo \
                     -luuilo \
                     -lvcllo \
                     -lxmlscriptlo \
                     -lxolo \
                     -lxstor \
                     -lxmlreaderlo \
                     -luno_salhelpergcc3 \
                     -luno_cppuhelpergcc3 \
                     -li18nlangtag \
                     -lacclo \
                     -laffine_uno_uno \
                     -lgraphicfilterlo \
                     -lcuilo \
                     -lunoidllo \
                     -lreglo \
                     -lgielo \
                     -lstorelo \
                     -lclewlo \
                     -ltklo \
                     -lfrmlo \
                     -lswlo \
                     -lswdlo \
                     -lswuilo \
                     -lhyphenlo \
                     -li18nsearchlo \
                     -linvocadaptlo \
                     -lchartcorelo \
                     -lmtfrendererlo \
                     -llnthlo \
                     -lodfflatxmllo \
                     -lwpftwriterlo \
                     -looxlo \
                     -lsdlo \
                     -lsddlo \
                     -lsduilo \
                     -lsdfiltlo \
                     -lsmlo \
                     -lspelllo \
                     -lsrtrs1 \
                     -lembobj \
                     -lstoragefdlo \
                     -lsvgfilterlo \
                     -licglo \
                     -lsvgiolo \
                     -lucphier1 \
                     -lucptdoc1lo \
                     -lunordflo \
                     -lvclcanvaslo \
                     -lxmlsecurity \
                     -lxmlfalo \
                     -lxmlfdlo \
                     -lexpwraplo \
                     -lxoflo \
                     -lxsec_xmlsec \
                     -lxsec_fw \
                     -lcanvasfactorylo \
                     -lwriterfilterlo \
                     -lwriterperfectlo \
                     -lreflectionlo \
                     -lintrospectionlo \
                     -lstocserviceslo \
                     -lchartcontrollerlo \
                     -ltextfdlo \
                     -lanimcorelo \
                     -lmswordlo \
                     -ldbalo \
                     -lemboleobj \
                     -lemfiolo \
                     -lbootstraplo \
                     -levtattlo \
                     -li18npoollo \
                     -lcollator_data \
                     -ltextconv_dict \
                     -lindex_data \
                     -llocaledata_en \
                     -llocaledata_es \
                     -llocaledata_euro \
                     -llocaledata_others \
                     -lgcc3_uno \
                     -lclucene \
                     -lassuan \
                     -lgpg-error \
                     -lboost_filesystem \
                     -lboost_locale \
                     -lgraphite \
                     -lfindsofficepath \
                     -lulingu \
                     -llcms2 \
                     -lgpgmepp \
                     -lgpgme \
                     -llangtag \
                     -lrdf \
                     -lraptor2 \
                     -lrasqal \
                     -lorcus-0.13 \
                     -lorcus-parser-0.13 \
                     -lhyphen \
                     -lmythes-1.2 \
                     -lhunspell-1.6 \
                     -lrevenge-0.0 \
                     -lodfgen-0.1 \
                     -lwps-0.4 \
                     -lpdfimportlo \
                     -lpdffilterlo \
                     -lmozbootstraplo \
                     -lmwaw-0.3 \
                     -Wl,--end-group \
                     -lpthread \
                     -ldl \


POPPLER_DIR := $(OFFICEKIT_DIR)/workdir/UnpackedTarball/poppler

LOCAL_MODULE := libblitzplugin_image
ALL_DEFAULT_INSTALLED_MODULES += $(LOCAL_MODULE)

LOCAL_SRC_FILES := $(shell find $(LOCAL_PATH)/ -name "*.cpp")
LOCAL_SRC_FILES := document.cpp  \
                   plugin.cpp    \
                   office.cpp    \
                   xpdf.cpp


LOCAL_C_INCLUDES := \
    $(native_page-includes) \
    $(base-includes) \
    $(jsoncpp-includes) \
    $(libhybris-includes) \
    $(page-includes) \
    $(pagewindow-includes) \
    $(properties-includes) \
    $(dynamicpagemanager-includes) \
    $(tvmedia-includes) \
    $(audioserver-includes) \
    $(liteblitz-includes) \
    $(liteblitz-includes)/core_api \
    $(POPPLER_DIR)/cpp \
    $(OFFICEKIT_INCLUDES)

LOCAL_CXXFLAGS := \
    -fvisibility=hidden \
    -Wno-unused-variable \
    -Wno-write-strings \
    -std=c++11 \
    -DNULL=0 \
    -Wno-conversion-null \
    -Wno-deprecated-declarations \
    -DBUILD_FOR_YUNOS

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libjsoncpp \
    libbase \
    libpagewindow \
    libproperties \
    libliteblitzadapter \
    libliteblitzpage \
    libxml2 \
    libfreetype \
    libcurl \
    libssl \
    libcrypto \
    libfontconfig \
    libicui18n \
    libicudata \
    libicuuc\
    libnspr4 \
    libnss3 \
    libsmime3 \
    libplc4 \
    libnssutil3 \
    libplds4 \
    libjpeg \
    libpng16 \
    libz \
    liblzma \
    libexpat \
    libcairo \
    libxmlsec1 \
    libxmlsec1-nss \
    libxmlsec1-gcrypt \
    libxslt \


LOCAL_SHARED_LIBRARIES_2 := libliteblitzcore

LOCAL_LDFLAGS := -lstdc++ \
                 -Wl,-rpath,./lib \
                 -lpthread \
                 $(POPPLER_DIR)/cpp/.libs/libpoppler-cpp.a \
                 $(POPPLER_DIR)/poppler/.libs/libpoppler.a \
                 $(OFFICEKIT_LDFLAGS)

LOCAL_MODULE_PATH := usr/lib/liteblitzplugin

ifeq ($(HAVE_CODE_COVERAGE),yes)
LOCAL_CXXFLAGS += --coverage -DHAVE_CODE_COVERAGE
LOCAL_LDLIBS += -lgcov
endif

include $(BUILD_SHARED_LIBRARY)

include $(LOCAL_PATH)/xdocserver/yunos.mk
