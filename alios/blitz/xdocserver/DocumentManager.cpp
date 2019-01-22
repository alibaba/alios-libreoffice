#include <math.h>
#include "DocumentManager.h"

namespace doc {
namespace pdf {

inline bool Equal(float a, float b, float epsilon = 0.01) {
    return ::fabs(a - b) < epsilon;
}

static int32_t sId = 1;

Document::Document(String &path) :
mPath(path),
mDoc(nullptr),
mDPI(160),
mTotalPage(0),
mId(0)
{
    mId = sId++;
}

Document::~Document(){
    logi("destructing doc:%s", mPath.c_str());

    if (mDoc) {
        delete mDoc;
        mDoc = nullptr;
    }
}

int32_t Document::getId() {
    return mId;
}

int32_t Document::getTotalPages() {
    return mTotalPage;
}

bool Document::open(SharedPtr<DMessage> &result) {
    std::string path(mPath.c_str());
    poppler::document* doc = poppler::document::load_from_file(path);

    if (!doc) {
        loge("read doc %s failed!", mPath.c_str());
        return false;
    }

    int nPages = doc->pages();

    if (nPages <= 0) {
        delete doc;
        loge("error doc: %s incomplete!", mPath.c_str());
        return false;
    }

    logi("pdf Document npages=%d", nPages);
    mDoc = doc;
    mTotalPage = nPages;

    return true;
}

bool Document::getPageImage(int iPage, SharedPtr<DMessage> &result) {
    logi("");
    poppler::image img;

    if (!mDoc) {
        loge("Error: no document when doGetPageImageLockedImpl");
        return false;
    }

    poppler::page* page = mDoc->create_page(iPage - 1);

    if (!page) {
        loge("Error: create page failed");
        return false;
    }

    poppler::page_renderer renderer;
    renderer.set_render_hint(poppler::page_renderer::text_antialiasing);

    int maxw = 1920;
    //int maxh = 1080;
    int maxh = 1920; // in case of w < h
    int tw = 300;
    int th = 300;
    int max = 2048;
    float ndpi = mDPI;

    do {
        mDPI = ndpi;
        img = renderer.render_page(page, mDPI, mDPI);

        // as image.data() will dup data, it's better to use orig image
        poppler::image &img2 = img;
        int w = img2.width();
        int h = img2.height();
        float rw = 0, rh = 0;
        if (w > max) {
            rw = (float)maxw / w;
        }

        if (h > max) {
            rh = (float)maxh / h;
        }

        if (Equal(rw, 0.0) && Equal(rh, 0.0)) {
            break;
        }

        float r = rw < rh ? rw : rh;
        ndpi = ceilf(ndpi * r);
        logi("\twidth=%d height=%d => DPI=%f", w, h, ndpi);

    } while (ndpi > 70);

    logi("\tdata=%p", img.data());
    logi("\tformat=%d", img.format());
    logi("\twidth=%d", img.width());
    logi("\theight=%d", img.height());
    int size = img.width() * img.height() * 4;
    int8_t *data = (int8_t *)img.data();
    int format = img.format();

    if(format != poppler::image::format_argb32) {
        loge("unknown format = %d, size to 0", format);
        size = 0;
    }
    result->writeBool(true);
    result->writeInt32(img.width());
    result->writeInt32(img.height());
    result->writeByteBuffer(size, data);

    delete page;

    return true;
}

DocumentManager::DocumentManager() {
}

DocumentManager::~DocumentManager() {
}

SharedPtr<Document> DocumentManager::createDocument(String &client,
                                                         String &path) {
    SharedPtr<Document> doc = new Document(path);

    if (doc) {
        int32_t id = doc->getId();
        mDocuments[id] = doc;
    }

    return doc;
}

SharedPtr<Document> DocumentManager::getDocument(int32_t id) {
    if (mDocuments.count(id)) {
        return mDocuments[id];
    }

    return nullptr;
}

}
}
