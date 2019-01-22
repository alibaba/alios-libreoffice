#include <vcl/svapp.hxx>
#include <cmdlineargs.hxx>
#include <app.hxx>
#include <tools/urlobj.hxx>
#include <tools/urlobj.hxx>
#include <osl/file.hxx>

#include <unotools/mediadescriptor.hxx>

#include <comphelper/processfactory.hxx>
#include <comphelper/synchronousdispatch.hxx>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/uno/Exception.hpp>

#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <com/sun/star/uno/Any.hxx>

#include "dispatchwatcher.hxx"

#include "officekit_document.hxx"
#include "officekit_image.hxx"

namespace officekit {

std::vector<Document *> Document::mOpenedDocuments;
Document *Document::mCurrentDocument = nullptr;

Document::Document(const char *filename) :
mFilename(filename, strlen(filename), osl_getThreadTextEncoding()),
mTotalPages(0),
mListener(nullptr),
mInited(false),
mCurrentPageNumber(0)
{
    AddOpenedDocument(this);
}

Document::~Document() {
    RemoveOpenedDocument(this);
}

bool Document::Init() {
    SAL_INFO("desktop.app", "Document::Init");
    Application::PostUserEvent( LINK( this, Document, OpenDocument_Impl ) );

#ifndef MACOSX
    bool r = false;
    {
        osl::Guard<osl::Mutex> guard(&mMutex);
        r = mInited;
    }

    while (!r) {
        mCondition.wait();
        {
            osl::Guard<osl::Mutex> guard(&mMutex);
            r = mInited;
        }
    }
#endif

    SAL_INFO("desktop.app", "Document::Init over");

    return true;
}

bool Document::SetListener(IDocumentListener *l) {
    mListener = l;
    return true;
}

bool Document::Release() {
    return true;
}

int Document::GetTotalPage() {
    return mTotalPages;
}

bool Document::GetPageImage(int iPage) {
    SAL_INFO("desktop.app", "Document::GetPageImage iPage=" << iPage);
#ifndef MACOSX
    if (!mDoc.is() || iPage > mTotalPages) {
        return nullptr;
    }
#endif
    Application::PostUserEvent( LINK( this, Document, GetPageImage_Impl ), (void *)iPage);
    return true;

#if 0
    Image *img = new Image(this, iPage);

    if (img) {
        img->Init();
    }

    return img;
#endif
}

void Document::onPageImageReleased(int iPage) {
    SAL_INFO("desktop.app", "Document::onPageImageReleased iPage=" << iPage);
}

IMPL_LINK_NOARG(Document, OpenDocument_Impl, void*, void) {
    SAL_INFO("desktop.app", "Document::OpenDocument_Impl");

    css::uno::Reference< css::frame::XDesktop2 > xDesktop =
        css::frame::Desktop::create( ::comphelper::getProcessComponentContext() );

    sal_Int32 nCount = 5;


    css::uno::Sequence < css::beans::PropertyValue > aArgs( nCount );

    // mark request as user interaction from outside
    aArgs[0].Name = "Referer";
    aArgs[0].Value <<= rtl::OUString("private:OpenEvent");

    // documents opened for printing are opened readonly because they must be opened as a new document and this
    // document could be open already
    aArgs[1].Name = "ReadOnly";
    aArgs[1].Value <<= true;

    // always open a new document for printing, because it must be disposed afterwards
    aArgs[2].Name = "OpenNewView";
    aArgs[2].Value <<= true;

    // printing is done in a hidden view
    aArgs[3].Name = "Hidden";
    aArgs[3].Value <<= true;

    // load document for printing without user interaction
    aArgs[4].Name = "Silent";
    aArgs[4].Value <<= true;


    const desktop::CommandLineArgs& rArgs = desktop::Desktop::GetCommandLineArgs();
    rtl::OUString aName( desktop::GetURL_Impl( mFilename, rArgs.getCwdUrl() ) );
    // hidden documents should never be put into open tasks
    rtl::OUString aTarget("_blank");


    // load the document ... if they are loadable!
    // Otherwise try to dispatch it ...
    INetURLObject aObj( aName );
    if ( aObj.GetProtocol() == INetProtocol::PrivSoffice )
        aTarget = "_default";

    SAL_INFO( "desktop.app", "opening name=" << aName << " target=" << aTarget);

    // This is a synchron loading of a component so we don't have to deal with our statusChanged listener mechanism.
    try
    {
        mDoc.set( ::comphelper::SynchronousDispatch::dispatch(
                                    xDesktop, aName, aTarget, 0, aArgs ),
                                    css::uno::UNO_QUERY );
    }
    catch (const css::lang::IllegalArgumentException& iae)
    {
        SAL_WARN(
                "desktop.app",
                "Dispatchwatcher IllegalArgumentException while calling"
                " loadComponentFromURL: " << iae);
    }
    catch (const css::io::IOException& ioe)
    {
        SAL_WARN(
                "desktop.app",
                "Dispatchwatcher IOException while calling"
                " loadComponentFromURL: " << ioe);
    }

    if ( mDoc.is() ) {
        SAL_INFO( "desktop.app", "open succeeded");
        onDocumentReady();
    } else {
        SAL_WARN( "desktop.app", "open failed");
    }

    {
        osl::Guard<osl::Mutex> guard(&mMutex);
        mInited = true;
        mCondition.set();
    }
}

bool Document::onPageGraphic(Graphic &aGraphic, sal_Int32 iPage) {
    if (mListener) {
        Image *img = new Image(aGraphic);
        return mListener->onPageImage(img, iPage);
    }

    return false;
}

void Document::onDocumentReady() {
    if ( !mDoc.is() )
    {
        return;
    }
    css::uno::Reference< css::frame::XModel > xModel2( mDoc, css::uno::UNO_QUERY );

    sal_uInt32 nPages = 0;
    if (!xModel2.is()) {
        SAL_WARN("desktop.app", "onDocumentReady query model failed");
        return;
    }
    // First check PageCount from property
    css::uno::Reference< css::frame::XController > xController = xModel2->getCurrentController();
    if (xController.is()) {
        css::uno::Reference< css::beans::XPropertySet > xPropertySet( xController, css::uno::UNO_QUERY );
        if (xPropertySet.is()) {
            try {
                css::uno::Any anyN = xPropertySet->getPropertyValue(OUStringLiteral("PageCount"));
                if (anyN.hasValue()) {
                    nPages = anyN.get<sal_Int32>();
                    SAL_INFO("desktop.app", "Word nPages=" << nPages);
                }
            } catch (css::beans::UnknownPropertyException &) {
            }
        }
    }

    // Secondly, check whether it's a ppt file?
    if (nPages == 0) {
        css::uno::Reference< css::drawing::XDrawPagesSupplier > xdrawPagesSP( xModel2, css::uno::UNO_QUERY );
        if (xdrawPagesSP.is()) {
            css::uno::Reference< css::drawing::XDrawPages > xPages = xdrawPagesSP->getDrawPages();
            if (xPages.is()) {
                nPages = xPages->getCount();
                SAL_INFO("desktop.app", "PPT nPages=" << nPages);
            }
        }
    }

    // Finally, check XDocumentStatistics
    if (nPages == 0) {
        css::uno::Reference< css::document::XDocumentProperties > xDocProp( xModel2, css::uno::UNO_QUERY );
        css::uno::Reference< css::document::XDocumentPropertiesSupplier > xDPS( mDoc, css::uno::UNO_QUERY);
        css::uno::Reference<    css::document::XDocumentProperties > xDocProps;
        if (xDPS.is()) {
            xDocProp = xDPS->getDocumentProperties();
        }
        if (xDocProp.is()){
            css::uno::Sequence< css::beans::NamedValue > stats = xDocProp->getDocumentStatistics();
            sal_Int32 length = stats.getLength();
            for (int i = 0; i < length; ++i) {
                css::beans::NamedValue &stat = stats[i];
                //std::cout << "Name=" << stat.Name << std::endl;
                if (stat.Name.equalsIgnoreAsciiCaseAscii("PageCount")) {
                    if (stat.Value.hasValue()) {
                        nPages = stat.Value.get<sal_Int32>();
                        SAL_INFO("desktop.app", "Word nPages=\n" << nPages);
                    }
                }
            }
        }
    }

    if (nPages == 0) {
        SAL_WARN("desktop.app", "cannot get document pages");
    }

    mTotalPages = nPages;
}

IMPL_LINK(Document, GetPageImage_Impl, void*, arg, void) {
    long lPage = (long)arg;
    sal_Int32 iPage = lPage & 0xffffffff;
    SAL_INFO("desktop.app", "Document::GetPageImage_Impl iPage=" << iPage);

    css::uno::Reference < css::view::XPrintable > &xDoc = GetUnoDocument();
    css::uno::Reference< css::frame::XStorable > xStorable( xDoc, css::uno::UNO_QUERY );

    if ( !xStorable.is() ) {
        SAL_WARN("desktop.app", "Image::GetPage_Impl failed to query XStorble");
        return;
    }

    rtl::OUString aImgOut;
    rtl::OUString aFilter;
    rtl::OUString aFilterExt("png");
    bool bGuess = true;

    rtl::OUString srcPath;
    osl::FileBase::getFileURLFromSystemPath(GetDocumentPath(), srcPath);

    INetURLObject aObj(srcPath);
    INetURLObject aOutFilename( aObj );
    aOutFilename.SetExtension( aFilterExt );

    rtl::OUString dstPath;
    osl::FileBase::getFileURLFromSystemPath(aObj.GetPath(), dstPath);
    rtl::OUString aOutFile = dstPath + "/" + aOutFilename.getName();
    SAL_WARN("desktop.app" ,"source=" << srcPath
                            << " path=" << aObj.GetPath()
                            << " name=" << aOutFilename.getName()
                            << " aout=" << aOutFile
                            );

    if ( bGuess )
    {
        rtl::OUString aDocService;
        css::uno::Reference< css::frame::XModel > xModel( xDoc, css::uno::UNO_QUERY );
        if ( xModel.is() )
        {
            utl::MediaDescriptor aMediaDesc( xModel->getArgs() );
            aDocService = aMediaDesc.getUnpackedValueOrDefault(utl::MediaDescriptor::PROP_DOCUMENTSERVICE(), OUString() );
        }
        aFilter = desktop::impl_GuessFilter( aOutFile, aDocService );
    }

    if (aFilter.isEmpty())
    {
        SAL_WARN("desktop.app" ,"Error: no export filter");
        return;
    }

    sal_Int32 nFilterOptionsIndex = aFilter.indexOf(':');
    sal_Int32 nProps = ( 0 < nFilterOptionsIndex ) ? 4 : 3;

    if ( !aImgOut.isEmpty() )
        nProps +=1;

    css::uno::Sequence < css::beans::PropertyValue > conversionProperties( nProps );

    conversionProperties[0].Name = "Overwrite";
    conversionProperties[0].Value <<= true;

    conversionProperties[1].Name = "FilterName";
    if( 0 < nFilterOptionsIndex )
    {
        conversionProperties[1].Value <<= aFilter.copy(0, nFilterOptionsIndex);

        conversionProperties[2].Name = "FilterOptions";
        conversionProperties[2].Value <<= aFilter.copy(nFilterOptionsIndex + 1);
    }
    else
    {
        conversionProperties[1].Value <<= aFilter;
    }

    //sal_uInt32 iPage = mPageId;

    if ( !aImgOut.isEmpty() )
    {
        conversionProperties[nProps-1].Name = "ImageFilter";
        conversionProperties[nProps-1].Value <<= aImgOut;
        conversionProperties[nProps-2].Name = "PageNumber";
        conversionProperties[nProps-2].Value <<= iPage;
    } else {
        conversionProperties[nProps-1].Name = "PageNumber";
        conversionProperties[nProps-1].Value <<= iPage;
    }

    OUString aTempName;
    osl::FileBase::getSystemPathFromFileURL(srcPath, aTempName);
    rtl::OString aSource8 = OUStringToOString(aTempName, osl_getThreadTextEncoding());
    osl::FileBase::getSystemPathFromFileURL(aOutFile, aTempName);
    rtl::OString aTargetURL8 = OUStringToOString(aTempName, osl_getThreadTextEncoding());

    SAL_INFO("desktop.app", "convert " << aSource8
                            << " -> " << aTargetURL8 << "(iPage=" << iPage << ")");
    SAL_INFO("desktop.app",  " using filter : " << OUStringToOString(aFilter, osl_getThreadTextEncoding()));

    try
    {
        SetCurrentDocument(this);
        setCurrentPageNumber(iPage);
        xStorable->storeToURL(aOutFile, conversionProperties);
    }
    catch (const css::uno::Exception& rException)
    {
        //SAL_WARN("desktop.app", "Error: Please verify input parameters...");
        if (!rException.Message.isEmpty())
            SAL_WARN("desktop.app", " (" << rException.Message << ")");

    }
}


bool Document::onGraphic(css::uno::Reference< css::lang::XComponent > doc,
                         Graphic &aGraphic,
                         sal_Int32 iPage) {
    SAL_WARN("desktop.app", "Document::onGraphic page=" << iPage);
    Document *appDoc = FindOpenedDocument(doc);
    if (!appDoc) {
        SAL_WARN("desktop.app", "Document::onGraphic cannot find document");
        return false;
    }

    return appDoc->onPageGraphic(aGraphic, iPage);
}

bool Document::onGraphic(Graphic &aGraphic, sal_Int32 iPage) {
    SAL_WARN("desktop.app", "Document::onGraphic2");
    Document *appDoc = GetCurrentDocument();
    if (!appDoc) {
        SAL_WARN("desktop.app", "Document::onGraphic2 cannot find document");
        return false;
    }

    iPage = appDoc->getCurrentPageNumber();
    SAL_WARN("desktop.app", "Document::onGraphic2 iPage=" << iPage);

    return appDoc->onPageGraphic(aGraphic, iPage);
}

void Document::AddOpenedDocument(Document *doc) {
    std::vector<Document *>::iterator it = std::find(mOpenedDocuments.begin(),
                                                     mOpenedDocuments.end(),
                                                     doc);
    if (it != mOpenedDocuments.end()) {
        return;
    }

    mOpenedDocuments.push_back(doc);
}

void Document::RemoveOpenedDocument(Document *doc) {
    std::vector<Document *>::iterator it = std::find(mOpenedDocuments.begin(),
                                                     mOpenedDocuments.end(),
                                                     doc);
    if (it == mOpenedDocuments.end()) {
        return;
    }

    mOpenedDocuments.erase(it);
}

Document * Document::FindOpenedDocument(css::uno::Reference< css::lang::XComponent > doc) {
    for (Document *appDoc : mOpenedDocuments) {
        css::uno::Reference < css::view::XPrintable > &xDoc = appDoc->GetUnoDocument();
        //css::uno::Reference< css::lang::XComponent > xComp( xDoc, css::uno::UNO_QUERY );
        if (xDoc == doc) {
            return appDoc;
        }
    }

    return nullptr;
}

Document *Document::GetCurrentDocument() {
    return mCurrentDocument;
}

bool Document::SetCurrentDocument(Document *doc) {
    mCurrentDocument = doc;
    return true;
}

bool Document::setCurrentPageNumber(sal_Int32 iPage) {
    mCurrentPageNumber = iPage;
    return true;
}

sal_Int32 Document::getCurrentPageNumber() {
    return mCurrentPageNumber;
}

}
