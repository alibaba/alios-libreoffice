#include "document.h"
#include "office.h"
#include "xpdf.h"

#define CACHE_SIZE_LIMIT 200 * 1024 * 1024
//#define CACHE_SIZE_LIMIT 10 * 1024 * 1024

namespace blitz {
namespace adapter {
namespace document {
bool StringEndsWith(string strings, string suffix) {
    size_t sLen = strings.length();
    size_t pLen = suffix.length();

    if (sLen < pLen) {
        return false;
    }

    size_t start = sLen - pLen;

    string strSuffix(strings, start, pLen);
    const char *s = strSuffix.c_str();
    const char *p = suffix.c_str();

    size_t i = 0;

    while (i < pLen) {
        if (s[i] != p[i]) {
            return false;
        }
        ++i;
    }

    return true;
}

CachedDocument::CachedDocument(std::string &path, yunos::LooperThread &looper) :
mPath(path),
mLooper(looper),
mTotalPages(0),
mRefs(0),
mCachedSize(0)
{
}

CachedDocument::~CachedDocument() {
    logi("destructing doc:%s", mPath.c_str());

    releasePageImagesLocked();
}

int CachedDocument::getTotalPage() {
    return mTotalPages;
}

bool CachedDocument::getPage(int iPage, void *ud) {
    std::lock_guard<std::mutex> _lock(mImageCacheMutex);

    PageImage *pageImage = getPageImageCacheLocked(iPage);
    if (pageImage) {
        ++mRefs;
        logi("Cache: found PageImage in cache!!!");
        callbackLocked(pageImage, iPage, ud, true);
        return true;
    }

    // add to cache before call getPageImageLockedImpl(),
    // to avoid race condition with ImageSourceWorkerThread
    if (!addPageImageCacheLocked(iPage, ud)) {
        return false;
    }

    if (!getPageImageLockedImpl(iPage, ud)) {
        // TODO: remove invalid cache
        return false;
    }

    ++mRefs;

    return true;
}

bool CachedDocument::putPage(int iPage) {
    std::lock_guard<std::mutex> _lock(mImageCacheMutex);
    // TODO release iPage image
    logi("iPage=%d mRefs=%d", iPage, mRefs);
    --mRefs;
    if (mRefs == 0) {
        // TODO
        //RemoveDocument(mPath);
        logi("iPage=%d mRefs=%d destroy document", iPage, mRefs);
        delete this;
    }

    return true;
}

bool CachedDocument::SetImageListener(BlitzImageSourceListener *listener) {
    mImageListener = listener;
    return true;
}

bool CachedDocument::addPageImageCacheLocked(int iPage, void *ud) {
    logi("");
    PageImage *pageImage = createPageImageImpl(iPage, ud);

    if (!pageImage) {
        loge("oom when addPageImageCacheLocked!!!\n");
        return false;
    }


    // Add to lru cache
    mCacheValues.emplace_front(pageImage);
    mCacheKeyMap[iPage] = mCacheValues.begin();

    return true;
}

PageImage *CachedDocument::getPageImageCache(int iPage) {
    std::lock_guard<std::mutex> _lock(mImageCacheMutex);
    return getPageImageCacheLocked(iPage);
}

PageImage *CachedDocument::getPageImageCacheLocked(int iPage) {
    if (mCacheKeyMap.count(iPage)) {
        return *mCacheKeyMap[iPage];
    }

    return nullptr;
}

void CachedDocument::releasePageImagesLocked() {
    auto it = mCacheValues.begin();
    auto end = mCacheValues.end();

    for (; it != end; ++it) {
        delete *it;
    }

    mCacheValues.clear();
    mCacheKeyMap.clear();
}

bool CachedDocument::callback(PageImage *pageImage, int iPage, void *ud, bool fromCache) {
    logi("");
    std::lock_guard<std::mutex> _lock(mImageCacheMutex);
    return callbackLocked(pageImage, iPage, ud, fromCache);
}

bool CachedDocument::callbackLocked(PageImage *pageImage, int iPage, void *ud, bool fromCache) {
    logi("");
    if (!mImageListener) {
        loge("callback without listener");
        return false;
    }

    if (!pageImage) {
        loge("callback without PageImage");
        return false;
    }

    if (!ud) {
        ud = pageImage->getUserData();
    }

    if (!ud) {
        loge("callback without ud");
        return false;
    }

    int type = 0;
    char *data = pageImage->getData();
    int size = pageImage->getDataSize();
    int w = pageImage->getWidth();
    int h = pageImage->getHeight();
    //logi("w=%d h=%d data=%p s=%d", w, h, data, size);

    bool ret = mImageListener->onImage(type, data, size, w, h, getTotalPage(), ud);

    onPageImageCallbackLocked(pageImage, iPage, fromCache);

    return ret;
}

bool CachedDocument::onPageImageCallbackLocked(PageImage *pageImage,
                                               int iPage, bool fromCache) {

    logi("iPage=%d fromCache=%d", iPage, fromCache);

    // move recently get page to front
    if (fromCache) {
        auto it = mCacheKeyMap[iPage];
        PageImage *pageImage = *it;
        mCacheValues.splice(mCacheValues.begin(), mCacheValues, it);
        PageImage *pageImage2 = *mCacheValues.begin();
        if (pageImage && pageImage2) {
            logi("latest iPage: %d-%d", pageImage->getPageIndex(),
                                        pageImage2->getPageIndex());
        }
        return true;
    }

    int size = pageImage->getDataSize();

    if (size <= 0) {
        logi("omit cache size %d", size);
        return true;
    }

    mCachedSize += size;

    if (mCachedSize <= CACHE_SIZE_LIMIT) {
        return true;
    }

    logi("cached size (%d vs %d), trim", mCachedSize, CACHE_SIZE_LIMIT);

    while (mCachedSize > CACHE_SIZE_LIMIT) {
        //auto it = mCacheValues.back();
        PageImage *pageImage = mCacheValues.back();
        mCacheValues.pop_back();
        if (!pageImage) {
            loge("internal-error:trim:invalid pageImage in cache");
            break;
        }

        int iPage = pageImage->getPageIndex();

        auto mapIter = mCacheKeyMap.find(iPage);
        if (mapIter == mCacheKeyMap.end()) {
            loge("internal-error:trim:invalid record in cacheMap");
        } else {
            mCacheKeyMap.erase(mapIter);
        }

        size = pageImage->getDataSize();
        mCachedSize -= size;
        logi("trimming cache: iPage=%d size=%d", iPage, size);

        delete pageImage;
    }

    return true;
}

bool CachedDocument::setTotalPages(int nPages) {
    mTotalPages = nPages;
    return true;
}

bool CachedDocument::sendTask(const yunos::Task& task) {
    return mLooper.sendTask(task);
}

BlitzImageSourceListener *CachedDocument::mImageListener = nullptr;

static void officekit_init() {
    const char *packageName = Blitz_Adapter::BlitzAdapter::getCurrPackageName();
    officekit::OfficeKit::Init(packageName);
    XPdfDocument::Init(packageName);
}

ImageSource::ImageSource() {
    mThread.start("ImageSourceWorkerThread");
    yunos::Task task = yunos::makeCallback<void>(officekit_init);
    mThread.sendTask(task);
}

ImageSource::~ImageSource() {
    logi("");
}

bool ImageSource::getImage(const char *url, int iPage, void *cbData) {
    logi("url=%s iPage=%d cbData=%p", url, iPage, cbData);
    CachedDocument *doc = getCache_(url, CachedDocument::CREATE);

    if (!doc) {
        return false;
    }

    return doc->getPage(iPage, cbData);
}

bool ImageSource::putImage(const char *url, int iPage) {
    CachedDocument *doc = getCache_(url, CachedDocument::NONE);

    if (!doc) {
        loge("internal error: putImage for non-created document:%s", url);
        return false;
    }

    return doc->putPage(iPage);
}

bool ImageSource::setListener(BlitzImageSourceListener *listener) {
    CachedDocument::SetImageListener(listener);
    return true;
}

CachedDocument *ImageSource::getCache_(const char *url, int flag) {
    std::string key(url);
    CachedDocument *doc = nullptr;

    if (mCaches.count(key)) {
        return mCaches[key];
    }

    if (CachedDocument::Flag::CREATE & flag) {
        doc = allocDocument_(std::string(url));
        if (doc) {
            mCaches[key] = doc;
        }
    }

    return doc;
}

CachedDocument *ImageSource::allocDocument_(std::string path) {
    CachedDocument *doc = nullptr;

    if (StringEndsWith(path, ".pdf")) {
        doc = new XPdfDocument(path, mThread);
    } else {
        doc = new OfficeDocument(path, mThread);
    }

    if (!doc) {
        loge("Error:oom when creating Document for %s!!!", path.c_str());
        return nullptr;
    }

    doc->open();

    return doc;
}

}
}
}
