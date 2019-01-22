#ifndef __BLITZ_IMAGE_EXTENSION_DOCUMENT_H__
#define __BLITZ_IMAGE_EXTENSION_DOCUMENT_H__
#include <blitz_adapter.h>
#include <thread/LooperThread.h>
#include <mutex>

/*#define IE_LOGI(fmt, ...) logi("IE:"fmt, __VA_ARGS__)*/
#undef logi
#undef logd
#undef loge
#undef LOG_TAG

#define LOG_TAG "imageext"

#define logi(fmt,args...) LOG_I("[DEBUG]%s %d %s " fmt, __YFILENAME__, __LINE__, __func__,  ##args)
#define logd(fmt,args...) LOG_D("[DEBUG]%s %d %s " fmt, __YFILENAME__, __LINE__, __func__,  ##args)
#define loge(fmt,args...) LOG_E("[DEBUG]%s %d %s " fmt, __YFILENAME__, __LINE__, __func__,  ##args)


namespace blitz {
namespace adapter {
namespace document {

class PageImage {
public:
    PageImage(int iPage, void *ud) :
    mPageIndex(iPage),
    mUData(ud) {
    }
    virtual ~PageImage() { }

    int getPageIndex() {
        return mPageIndex;
    }

    void *getUserData() {
        return mUData;
    }

    virtual char *getData() = 0;
    virtual int getDataSize() = 0;
    virtual int getWidth() = 0;
    virtual int getHeight() = 0;

private:
    int mPageIndex;
    void *mUData;
};

class CachedDocument {
typedef std::list<PageImage *>::iterator MapValueType;
public:
    enum Flag {
        NONE     = 0,
        CREATE   = 1,
    };

    CachedDocument(std::string &path, yunos::LooperThread &looper);
    virtual ~CachedDocument();

    virtual bool open() = 0;
    virtual int getTotalPage();
    virtual bool getPageImageLockedImpl(int iPage, void *ud) = 0;
    virtual PageImage *createPageImageImpl(int iPage, void *ud) = 0;

    bool getPage(int iPage, void *ud);
    bool putPage(int iPage);

    static bool SetImageListener(BlitzImageSourceListener *listener);

protected:
    bool sendTask(const yunos::Task& task);
    PageImage *getPageImageCache(int iPage);

    bool setTotalPages(int nPages);
    bool callback(PageImage *pageImage, int iPage, void *ud = nullptr, bool fromCache = false);
    PageImage *getPageImageCacheLocked(int iPage);

private:
    bool addPageImageCacheLocked(int iPage, void *ud);

    void releasePageImagesLocked();

    bool callbackLocked(PageImage *pageImage, int iPage, void *ud = nullptr, bool fromCache = false);
    bool onPageImageCallbackLocked(PageImage *pageImage, int iPage, bool fromCache);

protected:
    std::string mPath;
    static BlitzImageSourceListener *mImageListener;

private:

    yunos::LooperThread &mLooper;

    int mTotalPages;
    int mRefs;

    std::mutex mImageCacheMutex;
    int mCachedSize;
    std::list<PageImage *> mCacheValues;
    std::map<int, MapValueType> mCacheKeyMap;
};

class ImageSource : public BlitzImageSource {
public:
    ImageSource();

    virtual ~ImageSource();

    virtual bool getImage(const char *url, int iPage, void *cbData);

    virtual bool putImage(const char *url, int iPage);

    virtual bool setListener(BlitzImageSourceListener *listener);

private:
    CachedDocument *getCache_(const char *url, int flag);
    CachedDocument *allocDocument_(std::string path);

private:
    std::map<std::string, CachedDocument *> mCaches;
    yunos::LooperThread mThread;
};

}
}
}
#endif
