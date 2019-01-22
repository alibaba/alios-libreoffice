#ifndef __BLITZ_IMAGE_EXTENSION_XPDF_H__
#define __BLITZ_IMAGE_EXTENSION_XPDF_H__
#include <poppler-document.h>
#include <poppler-page.h>
#include <poppler-page-renderer.h>
#include <poppler-image.h>
#include <mutex>
#include "document.h"
#include "xdocserver/Common.h"

namespace blitz {
namespace adapter {
namespace document {

class XPdfDocument : virtual public CachedDocument {
public:
    XPdfDocument(std::string &path, yunos::LooperThread &looper);
    virtual ~XPdfDocument();

    static bool Init(const char *rootPath);
    static bool DeInit();
    static pid_t GetServerPid();

    virtual bool open();

    virtual bool getPageImageLockedImpl(int iPage, void *ud);
    virtual PageImage *createPageImageImpl(int iPage, void *ud);

private:
    void openImpl();
    void doGetPageImageLockedImpl(int iPage);

private:
    yunos::SharedPtr<yunos::DProxy> mProxy;
    int32_t mRemoteId;
    static pid_t sServerPid;
};


}
}
}
#endif
