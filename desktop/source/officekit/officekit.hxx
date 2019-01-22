#ifndef __OFFICE_KIT_H__
#define __OFFICE_KIT_H__


#if defined CPPU_ENV
#include <sal/types.h>
#define OFK_DLLPUBLIC SAL_DLLPUBLIC_EXPORT
#else
#define OFK_DLLPUBLIC
#endif

namespace officekit {

class OFK_DLLPUBLIC IImage {
public:
    virtual ~IImage() {}

    virtual char *GetData() = 0;
    virtual int GetDataSize() = 0;
    virtual int GetFormat() = 0;
    virtual int GetWidth() = 0;
    virtual int GetHeight() = 0;
};

class OFK_DLLPUBLIC IDocumentListener {
public:
    virtual ~IDocumentListener() {}
    virtual bool onPageImage(IImage *image, int iPage) = 0;
};

class OFK_DLLPUBLIC IDocument {
public:
    virtual ~IDocument() {}

    virtual bool Init() = 0;
    virtual bool Release() = 0;
    virtual int GetTotalPage() = 0;

    virtual bool GetPageImage(int iPage) = 0;
    virtual bool SetListener(IDocumentListener *l) = 0;
};

class OFK_DLLPUBLIC OfficeKit {
public:
    static bool Init(const char *rootPath = nullptr);
    static IDocument *OpenDocument(const char *filename);
    static bool Main();
    static bool Destroy();
private:
    static bool InitInternal(const char *rootPath);
private:
    static bool mInitOk;
};


}


#endif
