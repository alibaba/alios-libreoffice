#include "office.h"

namespace blitz {
namespace adapter {
namespace document {
using namespace officekit;

class OfficeImage : public PageImage {
public:
    OfficeImage(int iPage, void *ud) :
    PageImage(iPage, ud),
    mImage(nullptr) {
    }

    virtual ~OfficeImage() {
        logi("destructing iPage=%d mImage=%p", getPageIndex(), mImage);
        if (mImage) {
            delete mImage;
            mImage = nullptr;
        }
    }

    virtual char *getData() {
        if (mImage) {
            return mImage->GetData();
        }

        return nullptr;
    }

    virtual int getDataSize() {
        if (mImage) {
            return mImage->GetDataSize();
        }

        return 0;
    }

    virtual int getWidth() {
        if (mImage) {
            return mImage->GetWidth();
        }

        return 0;
    }

    virtual int getHeight() {
        if (mImage) {
            return mImage->GetHeight();
        }

        return 0;
    }

    void setIImage(IImage *image) {
        if (mImage) {
            loge("Error: internal-error overriding old");
            delete mImage;
            mImage = nullptr;
        }

        mImage = image;
    }

private:
    IImage *mImage;
};

OfficeDocument::OfficeDocument(std::string &path, yunos::LooperThread &looper) :
CachedDocument(path, looper),
mDoc(nullptr)
{
    logi("create OfficeDocument for %s", path.c_str());
}

OfficeDocument::~OfficeDocument() {
    logi("destructing doc:%s", mPath.c_str());

    if (mDoc) {
        delete mDoc;
    }
}

bool OfficeDocument::open() {
    logi("");
    yunos::Task task = yunos::makeCallback<void, OfficeDocument>(
                                    &OfficeDocument::openImpl,
                                    yunos::maybeUnsafe(this));
    sendTask(task);

    return true;
}

void OfficeDocument::openImpl() {
    logi("");
    IDocument *doc = OfficeKit::OpenDocument(mPath.c_str());

    if (!doc) {
        loge("read doc %s failed!", mPath.c_str());
        return;
    }

    int nPages = doc ? doc->GetTotalPage() : 0;

    if (nPages <= 0) {
        delete doc;
        loge("error doc: %s incomplete!", mPath.c_str());
        return;
    }

    logi("office Document npages=%d", nPages);
    mDoc = doc;
    mDoc->SetListener(this);

    setTotalPages(nPages);
}

bool OfficeDocument::getPageImageLockedImpl(int iPage, void *ud) {
    yunos::Task task = yunos::makeCallback<void, OfficeDocument>(
                                &OfficeDocument::doGetPageImageLockedImpl,
                                yunos::maybeUnsafe(this), iPage);
    sendTask(task);

    return true;
}

void OfficeDocument::doGetPageImageLockedImpl(int iPage) {
    if (!mDoc) {
        loge("Error: no document when doGetPageImageLockedImpl");
        onPageImage(nullptr, iPage);
        return;
    }

    if (!mDoc->GetPageImage(iPage)) {
        loge("Error: GetPageImage failed when doGetPageImageLockedImpl");
        onPageImage(nullptr, iPage);
        return;
    }
}

bool OfficeDocument::onPageImage(IImage *image, int iPage) {
    logi("onPageImage iPage=%d image=%p", iPage, image);

    if (image) {
        logi("\tdata=%p", (void *)image->GetData());
        logi("\tdataSize=%d", image->GetDataSize());
        logi("\tformat=%d", image->GetFormat());
        logi("\twidth=%d", image->GetWidth());
        logi("\theight=%d", image->GetHeight());
    }

    PageImage *pageImage = getPageImageCache(iPage);

    if (!pageImage) {
        loge("Error: Got unexpected iPage=%d", iPage);
        if (image) {
            delete image;
        }

        return false;
    }

    OfficeImage *officeImage = dynamic_cast<OfficeImage *>(pageImage);

    if (!officeImage) {
        loge("Error: Got unexpected iPage=%d with non-office record", iPage);
        if (image) {
            delete image;
        }

        return false;
    }

    officeImage->setIImage(image);

    return callback(officeImage, iPage);
}

PageImage *OfficeDocument::createPageImageImpl(int iPage, void *ud) {
    return new OfficeImage(iPage, ud);
}

}
}
}
