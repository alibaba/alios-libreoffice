
#include <vcl/svapp.hxx>
#include <app.hxx>
#include <tools/urlobj.hxx>
#include <osl/file.hxx>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/uno/Exception.hpp>
#include <unotools/mediadescriptor.hxx>

#include "officekit_image.hxx"
#include "officekit_document.hxx"

#include "dispatchwatcher.hxx"

namespace officekit {

Image::Image(Graphic &aGraphic) :
mBitmap(aGraphic.GetBitmapEx().GetBitmap()),
mInfoAccess(mBitmap),
mReadAccess(mBitmap)
{
}

Image::~Image() {
    SAL_INFO("desktop.app", "Image destructor");
}

bool Image::Init() {
    return true;
}

void Image::Release() {
}

char *Image::GetData() {
    return (char *)mReadAccess->GetBuffer();
}

int Image::GetDataSize() {
    BitmapBuffer *buffer =  mReadAccess->ImplGetBitmapBuffer();
    long scanlineSize = buffer->mnScanlineSize;
    long height = buffer->mnHeight;
    return scanlineSize * height;
}

int Image::GetFormat() {
    return (int)mReadAccess->ImplGetBitmapBuffer()->mnFormat;
}

int Image::GetWidth() {
    return (int)mInfoAccess->Width();
}

int Image::GetHeight() {
    return (int)mInfoAccess->Height();
}

}
