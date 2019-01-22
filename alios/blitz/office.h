#ifndef __BLITZ_IMAGE_EXTENSION_OFFICE_H__
#define __BLITZ_IMAGE_EXTENSION_OFFICE_H__
#include <officekit.hxx>
#include <mutex>
#include "document.h"

namespace blitz {
namespace adapter {
namespace document {
using namespace officekit;


class OfficeDocument : virtual public IDocumentListener,
                       virtual public CachedDocument {
public:
    OfficeDocument(std::string &path, yunos::LooperThread &looper);
    virtual ~OfficeDocument();

    virtual bool open();

    bool getPageImageLockedImpl(int iPage, void *ud);

    virtual bool onPageImage(IImage *image, int iPage);
    virtual PageImage *createPageImageImpl(int iPage, void *ud);

private:
    void openImpl();
    void doGetPageImageLockedImpl(int iPage);

private:
    IDocument *mDoc;
};

}
}
}
#endif
