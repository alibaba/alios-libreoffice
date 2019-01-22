#ifndef __OFFICE_KIT_IMAGE_H__
#define __OFFICE_KIT_IMAGE_H__

#include "officekit.hxx"
#include <vcl/bitmapaccess.hxx>
#include <vcl/bitmap.hxx>

namespace officekit {

class Document;
class Image : public IImage {
public:
    Image(Graphic &aGraphic);
    virtual ~Image();

    virtual bool Init();
    virtual void Release();
    virtual char *GetData();
    virtual int GetDataSize();
    virtual int GetFormat();
    virtual int GetWidth();
    virtual int GetHeight();

private:
    Bitmap mBitmap;

    Bitmap::ScopedInfoAccess mInfoAccess;
    Bitmap::ScopedReadAccess mReadAccess;
};

}

#endif
