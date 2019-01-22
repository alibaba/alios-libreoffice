#include <iostream>
#include <fstream>
#include <sstream>
#include <poppler-document.h>
#include <poppler-page.h>
#include <poppler-page-renderer.h>
#include <poppler-image.h>
#include <math.h>
#include <unistd.h>

#include <mutex>
#include "xpdf.h"

#define XPDF_DBUS_TIMEOUT -1

namespace blitz {
namespace adapter {
namespace document {
using namespace yunos;

inline bool Equal(float a, float b, float epsilon = 0.01) {
    return ::fabs(a - b) < epsilon;
}

class PdfImage : public PageImage {
public:
    PdfImage(int iPage, void *ud) :
    PageImage(iPage, ud),
    mData(nullptr),
    mDataSize(0),
    mWidth(0),
    mHeight(0)
    {
    }

    virtual ~PdfImage() {
        logi("destructing iPage=%d", getPageIndex());
        if (mData) {
            delete[] mData;
            mData = nullptr;
        }
    }

    virtual char *getData() {
        return (char *)mData;
    }

    virtual int getDataSize() {
        return mDataSize;
    }

    virtual int getWidth() {
        return mWidth;
    }

    virtual int getHeight() {
        return mHeight;
    }

    void setData(int8_t *data, int32_t datasize, int32_t width, int32_t height) {
        mData = data;
        mDataSize = datasize;
        mWidth = width;
        mHeight = height;
    }

    int8_t *mData;
    int32_t mDataSize;
    int32_t mWidth;
    int32_t mHeight;
};

pid_t XPdfDocument::sServerPid = 0;

bool XPdfDocument::Init(const char *rootPath) {
    std::string appPath("/opt/app/");

    appPath += rootPath;

    std::string binPath = appPath + "/res/program/xdocserver";

    if (chmod(binPath.c_str(), S_IRUSR | S_IXUSR) != 0) {
        loge("fatal-error: cannot chmod r|x %s:%s", binPath.c_str(),
                                                    strerror(errno));
    }

    if (access(binPath.c_str(), R_OK | X_OK) != 0) {
        loge("fatal-error: cannot r|x %s:%s", binPath.c_str(), strerror(errno));
        return false;
    }

    pid_t pid = fork();
    if (pid == 0) {
        int argc = 1;
        char *argv[] = { (char *)binPath.c_str(), NULL};
        char *env[] = { NULL };

        logi("close non-needed fd after fork");
        // close existing fds, or kdbus won't work
        for (int i = 3; i < 1024; ++i) {
            close(i);
        }

        execv(binPath.c_str(), argv);
        loge("fatal-error: cannot exec %s", binPath.c_str());
        exit(EXIT_FAILURE);
    }

    sServerPid = pid;

    return true;
}

pid_t XPdfDocument::GetServerPid() {
    return sServerPid;
}

bool XPdfDocument::DeInit() {
    logi("");
    if (sServerPid <= 0) {
        loge("no server pid found");
        return true;
    }

    return kill(sServerPid, SIGKILL);
}

XPdfDocument::XPdfDocument(std::string &path, yunos::LooperThread &looper) :
CachedDocument(path, looper),
mRemoteId(0)
{
    logi("create XPdfDocument for %s", path.c_str());

    SharedPtr<DServiceManager> manager = DServiceManager::getInstance();
    if (!manager) {
        LOG_E("fatal-error: unable to get servicemanager");
        return;
    }

    SharedPtr<DService> service = manager->getService(String(PDF_SERVICE_NAME));

    int cnt = 3;
    while (cnt > 0) {
        if (service) {
            break;
        }

        loge("server not ready, wait cnt=%d", cnt);

        sleep(1);

        service = manager->getService(String(PDF_SERVICE_NAME));
        --cnt;
    }

    if (!service) {
        loge("fatal-error: unable to getService %s", PDF_SERVICE_NAME);
        return;
    }

    mProxy = new DProxy(service, String(PDF_SERVICE_OBJECT_PATH),
                                        String(PDF_SERVICE_INTERFACE_PATH));

    if (!mProxy) {
        LOG_E("fatal-error: oom");
        return;
    }
}

XPdfDocument::~XPdfDocument() {
    logi("destructing doc:%s", mPath.c_str());
}

bool XPdfDocument::open() {
    logi("");
    yunos::Task task = yunos::makeCallback<void, XPdfDocument>(
                                        &XPdfDocument::openImpl,
                                        yunos::maybeUnsafe(this));
    sendTask(task);

    return true;
}

void XPdfDocument::openImpl() {
    logi("");

    SharedPtr<DMessage> msg = mProxy->obtainMethodCallMessageWithName(TRANSACTION_openDocument);
    msg->writeString(mPath.c_str());

    SharedPtr<DMessage> result = mProxy->sendMessageWithReplyBlocking(msg,
                                                            XPDF_DBUS_TIMEOUT);
    if (!result) {
        loge("open failed: null result");
        return;
    }

    int32_t id = result->readInt32();
    int32_t nPages = result->readInt32();

    logi("openDocument id=%d nPages=%d", id, nPages);

    if (id <= 0) {
        return;
    }

    mRemoteId = id;
    setTotalPages(nPages);
}

bool XPdfDocument::getPageImageLockedImpl(int iPage, void *ud) {
    logi("");
    yunos::Task task = yunos::makeCallback<void, XPdfDocument>(
                                    &XPdfDocument::doGetPageImageLockedImpl,
                                    yunos::maybeUnsafe(this), iPage);
    sendTask(task);

    return true;
}

void XPdfDocument::doGetPageImageLockedImpl(int iPage) {
    logi("");

    PageImage *pageImage = getPageImageCache(iPage);

    if (!pageImage) {
        loge("Error: Got unexpected iPage=%d", iPage);
        callback(pageImage, iPage);
        return;
    }

    PdfImage *pdfImage = dynamic_cast<PdfImage *>(pageImage);

    if (!pdfImage) {
        loge("Error: Got unexpected iPage=%d with non-pdf record", iPage);
        callback(pageImage, iPage);
        return;
    }

    if (mRemoteId <= 0) {
        loge("Error: %s iPage=%d remoteId invalid", mPath.c_str(), iPage);
        callback(pageImage, iPage);
        return;
    }

    SharedPtr<DMessage> msg = mProxy->obtainMethodCallMessageWithName(TRANSACTION_getPageImage);
    msg->writeInt32(mRemoteId);
    msg->writeInt32(iPage);

    SharedPtr<DMessage> result = mProxy->sendMessageWithReplyBlocking(msg,
                                                            XPDF_DBUS_TIMEOUT);

    bool ok = result->readBool();

    if (!ok) {
        loge("convert failed");
        callback(pageImage, iPage);
        return;
    }

    int32_t width = result->readInt32();
    int32_t height = result->readInt32();
    int32_t bufsize = 0;
    int8_t *buf = result->readByteBuffer(bufsize);

    logi("\tdata=%p", buf);
    logi("\twidth=%d", width);
    logi("\theight=%d", height);

    pdfImage->setData(buf, bufsize, width, height);

    callback(pdfImage, iPage);
}

PageImage *XPdfDocument::createPageImageImpl(int iPage, void *ud) {
    return new PdfImage(iPage, ud);
}

}
}
}
