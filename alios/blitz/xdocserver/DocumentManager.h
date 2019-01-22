#ifndef __XPDF_MANAGER_H__
#define __XPDF_MANAGER_H__

#include "Common.h"
#include <poppler-document.h>
#include <poppler-page.h>
#include <poppler-page-renderer.h>
#include <poppler-image.h>
#include <mutex>

namespace doc {
namespace pdf {
using namespace yunos;

class Document : public BasePtr {
public:
    Document(String &path);
    virtual ~Document();

    bool open(SharedPtr<DMessage> &result);
    int32_t getId();
    int32_t getTotalPages();
    bool getPageImage(int iPage, SharedPtr<DMessage> &result);

private:
    String mPath;
    poppler::document *mDoc;
    int32_t mDPI;
    int32_t mTotalPage;
    int32_t mId;
};

class DocumentManager : public BasePtr {
public:
    DocumentManager();
    ~DocumentManager();

    SharedPtr<Document> createDocument(String &client, String &path);
    SharedPtr<Document> getDocument(int32_t id);

private:
    std::map<int32_t, SharedPtr<Document> > mDocuments;
};

}
}
#endif
