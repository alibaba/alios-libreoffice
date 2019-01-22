/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "sofficemain.h"


#include <unistd.h>
#include <osl/conditn.hxx>
#include <osl/mutex.hxx>
#include <sal/log.hxx>
#include <sal/main.h>
#include <osl/thread.hxx>
#include <rtl/bootstrap.hxx>
#include <cppuhelper/source/paths.hxx>


#include "officekit.hxx"
#include "officekit_int.hxx"
#include "officekit_document.hxx"

namespace officekit {

bool OfficeKit::mInitOk = false;

namespace {

static char *argv[] = {
    (char *)"officekit.bin",
    (char *)"--headless",
#if defined MACOSX
    (char *)"-env:INIFILENAME=file:///Users/lijin.lj/workspace/git/office/ref/lode/dev/libreoffice/instdir/LibreOfficeDev.app/Contents/Resources/sofficerc"
#elif defined ALIOS
    //(char *)"-env:INIFILENAME=file:///data/instdir/program/sofficerc"
#else
    (char *)"-env:INIFILENAME=file:///home/lijin.lj/workspace/libreoffice/host/instdir/program/sofficerc"
#endif
};

static const int argc = sizeof(argv)/sizeof(argv[0]);

static bool extendEnvironment(std::string &name, std::string value) {
    const char *orig = getenv(name.c_str());
    char buf[512];
    if (orig) {
        snprintf(buf, sizeof(buf), "%s:%s", orig, value.c_str());
        value = buf;
    }

    setenv(name.c_str(), value.c_str(), 1);
    SAL_INFO("desktop.app", "extend env name=" << name << " value=" << value);
    return true;
}

static bool OfficeArgs_Init() {
    SAL_INFO("desktop.app", "OfficeArgs_Init");
    sal_detail_initialize(argc, argv);
    return true;
}

class DocumentListener : public IDocumentListener {
public:
    virtual ~DocumentListener() {
    }

    virtual bool onPageImage(IImage *image, int iPage) {
        SAL_INFO("desktop.app", "onPageImage iPage=" << iPage);
        SAL_INFO("desktop.app", "\tdata=" << (void *)image->GetData());
        SAL_INFO("desktop.app", "\tdataSize=" << image->GetDataSize());
        SAL_INFO("desktop.app", "\tformat=" << image->GetFormat());
        SAL_INFO("desktop.app", "\twidth=" << image->GetWidth());
        SAL_INFO("desktop.app", "\theight=" << image->GetHeight());
        return true;
    }
};


class OfficeKitMainThread : public osl::Thread,
                            public IController {
public:
    OfficeKitMainThread() : mResultGot(false), mResult(false) {
        SAL_INFO("desktop.app", "OfficeKitMainThread");
    }

    virtual ~OfficeKitMainThread() {
    }

    bool waitInit() {
        bool got = false;
        bool r = false;
        {
            osl::Guard<osl::Mutex> guard(&mMutex);
            got = mResultGot;
            r = mResult;
        }

        while (!got) {
            mCondition.wait();
            {
                osl::Guard<osl::Mutex> guard(&mMutex);
                got = mResultGot;
                r = mResult;
            }
        }

        SAL_INFO("desktop.app", "OfficeKitMainThread::waitInit over");

        return r;
    }

#ifdef MACOSX
    bool StartOnMac() {
        run();
        return true;
    }
#endif

protected:

    // Thread functions
    virtual void SAL_CALL run() {
        SAL_INFO("desktop.app", "OfficeKitMainThread::run");
        Controller::setInterface(this);
        soffice_main();
        SAL_INFO("desktop.app", "OfficeKitMainThread::run over");
    }

    virtual void SAL_CALL onTerminated() {
        SAL_WARN("desktop.app", "OfficeKitMainThread::onTerminated");
        osl::Guard<osl::Mutex> guard(&mMutex);
        mResultGot = true;
        mResult = false;
    }

    // IController
    virtual bool onLoopEntering() {
        SAL_INFO("desktop.app", "OfficeKitMainThread::onLoopEntering");
        osl::Guard<osl::Mutex> guard(&mMutex);

        mResultGot = true;
        mResult = true;

        mCondition.set();
#ifdef MACOSX
        IDocument *doc = OfficeKit::OpenDocument("/Users/lijin.lj/workspace/daily/office/1.doc");
        IImage *img = doc->GetPageImage(2);
        img->Release();
        //doc->Release();
        doc = nullptr;
#endif

        return true;
    }

    virtual bool onGraphic(css::uno::Reference< css::lang::XComponent > doc,
                          Graphic &aGraphic,
                          sal_Int32 iPage) {
        return Document::onGraphic(doc, aGraphic, iPage);
    }

    virtual bool onGraphic(Graphic &aGraphic,
                          sal_Int32 iPage) {
        return Document::onGraphic(aGraphic, iPage);
    }

private:
    osl::Condition mCondition;
    osl::Mutex mMutex;
    bool mResultGot;
    bool mResult;
};

//static osl::Thread *gMainThread = nullptr;
static OfficeKitMainThread *gMainThread = nullptr;

static bool OfficeMain_Start() {
    OfficeKitMainThread *thread = nullptr;
    bool ok = true;

    if (gMainThread) {
        return true;
    }

    thread = new OfficeKitMainThread();

    if (!thread) {
        return false;
    }

#ifdef MACOSX
    thread->StartOnMac();
#else
    thread->create();
#endif

    gMainThread = thread;

    return ok;
}

IDocument *OfficeDoc_get(const char *filename) {
    return new Document(filename);
}

void OfficeDoc_release(IDocument *doc) {
    if (doc) {
        delete doc;
    }
}

}


bool OfficeKit::Init(const char *rootPath) {
#ifdef MACOSX
    return true;
#else
    return InitInternal(rootPath);
#endif
}

bool OfficeKit::InitInternal(const char *rootPath) {
    mInitOk = OfficeArgs_Init();
    if (!mInitOk) {
        return false;
    }

    if (rootPath) {
        std::string envName("LD_LIBRARY_PATH");
        std::string libPath("/opt/app/");
        libPath += rootPath;
        libPath += "/lib";
        extendEnvironment(envName, libPath);

        std::string sofficerc("file:///opt/app/");
        sofficerc += rootPath;

        std::string bsIni = sofficerc + "/res/program/sofficerc";
        std::string unoIni = sofficerc + "/res/program";
        rtl::Bootstrap::setIniFilename(rtl::OUString::createFromAscii(bsIni.c_str()));
        cppu::setUnoIniUri(rtl::OUString::createFromAscii(unoIni.c_str()));
    }

    mInitOk = OfficeMain_Start();

    return mInitOk;
}

bool OfficeKit::Main() {
#ifdef MACOSX
    return InitInternal();
#else
    //IDocument *doc = OpenDocument("/home/lijin.lj/workspace/libreoffice/1.doc");
    //IDocument *doc = OpenDocument("/storage/A8E2-DA3C/office/sample.docx");
    IDocument *doc = OpenDocument("/storage/7C00-0274/面向物流配送位置服务系统的设计与开发.ppt");
    doc->SetListener(new DocumentListener());
    int nPages = doc ? doc->GetTotalPage() : 0;
    SAL_INFO("desktop.app", "Document npages=" << nPages);
    int i = 0;
    while(1) {
        if (nPages) {
            doc->GetPageImage(i + 1);
            i = (i+1) % nPages;
        }
        sleep(1);
    }

    if (doc) {
        doc->Release();
        doc = nullptr;
    }

    return true;
#endif
}

IDocument *OfficeKit::OpenDocument(const char *filename) {
    SAL_INFO("desktop.app", "OpenDocument.." << filename);

    if (!mInitOk) {
        SAL_WARN("desktop.app", "OpenDocument failed due to no init");
        return nullptr;
    }

    // Ensure mainthread inited
    if (!gMainThread->waitInit()) {
        SAL_WARN("desktop.app", "OpenDocument failed due to init thread error");
        return nullptr;
    }

    IDocument *doc = OfficeDoc_get(filename);

    if (!doc->Init()) {
        OfficeDoc_release(doc);
        SAL_WARN("desktop.app", "OpenDocument failed due to init error");
        return nullptr;
    }

    return doc;
}

bool OfficeKit::Destroy() {
    if (gMainThread) {
        delete gMainThread;
        gMainThread = nullptr;
    }

    return true;
}

}


/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
