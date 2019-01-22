#ifndef __OFFICE_KIT_DOCUMENT_H__
#define __OFFICE_KIT_DOCUMENT_H__

#include <rtl/ustring.hxx>
#include <tools/link.hxx>
#include <osl/conditn.hxx>
#include <osl/mutex.hxx>

#include <com/sun/star/io/IOException.hpp>
#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/util/XCloseable.hpp>
#include <com/sun/star/util/CloseVetoException.hpp>
#include <com/sun/star/task/InteractionHandler.hpp>
#include <com/sun/star/util/URL.hpp>
#include <com/sun/star/frame/Desktop.hpp>
#include <com/sun/star/container/XContainerQuery.hpp>
#include <com/sun/star/container/XEnumeration.hpp>
#include <com/sun/star/frame/XDispatch.hpp>
#include <com/sun/star/frame/XNotifyingDispatch.hpp>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/view/XPrintable.hpp>
#include <com/sun/star/util/URLTransformer.hpp>
#include <com/sun/star/util/XURLTransformer.hpp>
#include <com/sun/star/document/MacroExecMode.hpp>
#include <com/sun/star/document/XTypeDetection.hpp>
#include <com/sun/star/document/UpdateDocMode.hpp>
#include <com/sun/star/frame/XStorable.hpp>
#include <com/sun/star/script/ModuleInfo.hpp>
#include <com/sun/star/script/ModuleType.hpp>
#include <com/sun/star/script/XLibraryContainer2.hpp>
#include <com/sun/star/document/XEmbeddedScripts.hpp>
#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/document/XDocumentProperties.hpp>
#include <com/sun/star/document/XDocumentPropertiesSupplier.hpp>
#include <com/sun/star/drawing/XDrawPagesSupplier.hpp>

#include "officekit.hxx"

namespace officekit {

class Document : public IDocument {
public:
    Document(const char *filename);
    virtual ~Document();

    virtual bool Init();
    virtual bool Release();
    virtual int GetTotalPage();

    virtual bool GetPageImage(int iPage);
    virtual bool SetListener(IDocumentListener *l);

public:
    DECL_LINK( OpenDocument_Impl, void*, void );
    DECL_LINK( GetPageImage_Impl, void*, void );
    css::uno::Reference < css::view::XPrintable > &GetUnoDocument() { return mDoc; }
    rtl::OUString &GetDocumentPath() { return mFilename; }

    bool setCurrentPageNumber(sal_Int32 iPage);
    sal_Int32 getCurrentPageNumber();

private:
    bool onPageGraphic(Graphic &aGraphic, sal_Int32 iPage);

public:
    static bool onGraphic(css::uno::Reference< css::lang::XComponent > doc,
                           Graphic &aGraphic,
                           sal_Int32 iPage);

    static bool onGraphic(Graphic &aGraphic,
                           sal_Int32 iPage);

private:
    virtual void onPageImageReleased(int iPage);
private:
    void onDocumentReady();

private:
    rtl::OUString mFilename;
    css::uno::Reference < css::view::XPrintable > mDoc;
    int mTotalPages;

    osl::Condition mCondition;
    osl::Mutex mMutex;
    IDocumentListener *mListener;
    bool mInited;
    sal_Int32 mCurrentPageNumber;
private:
    static void AddOpenedDocument(Document *doc);
    static void RemoveOpenedDocument(Document *doc);
    static Document *FindOpenedDocument(css::uno::Reference< css::lang::XComponent > doc);
    static Document *GetCurrentDocument();
    static bool SetCurrentDocument(Document *doc);
private:
    static std::vector<Document *> mOpenedDocuments;
    static Document * mCurrentDocument;
};

}

#endif
