#ifndef __OFFICE_KIT_INT__
#define __OFFICE_KIT_INT__

#include <sal/types.h>
#include <vcl/graph.hxx>

#include <com/sun/star/lang/XComponent.hpp>


namespace officekit {

class SAL_DLLPUBLIC_EXPORT IController {
public:
    virtual ~IController() {}
    virtual bool onLoopEntering() = 0;
    virtual bool onGraphic(css::uno::Reference< css::lang::XComponent > doc,
                           Graphic &aGraphic,
                           sal_Int32 iPage) = 0;
    virtual bool onGraphic(Graphic &aGraphic,
                           sal_Int32 iPage) = 0;
};

class SAL_DLLPUBLIC_EXPORT Controller {
public:
    static bool onLoopEntering();
    static bool onGraphic(css::uno::Reference< css::lang::XComponent > doc,
                          Graphic &aGraphic,
                          sal_Int32 iPage);

    static bool onGraphic(Graphic &aGraphic,
                          sal_Int32 iPage);

    static bool setInterface(IController *intf);

private:
    static IController *mIntf;
};

}

#endif
