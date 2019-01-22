
#include "officekit_int.hxx"

namespace officekit {
IController *Controller::mIntf = nullptr;
bool Controller::onLoopEntering() {
    if (mIntf) {
        return mIntf->onLoopEntering();
    }

    return false;
}

bool Controller::onGraphic(css::uno::Reference< css::lang::XComponent > doc,
                          Graphic &aGraphic,
                          sal_Int32 iPage) {
    if (mIntf) {
        mIntf->onGraphic(doc, aGraphic, iPage);
        // return true to indicate controller exists
        return true;
    }

    return false;
}

bool Controller::onGraphic(Graphic &aGraphic,
                          sal_Int32 iPage) {
    if (mIntf) {
        mIntf->onGraphic(aGraphic, iPage);
        // return true to indicate controller exists
        return true;
    }

    return false;
}
bool Controller::setInterface(IController *intf) {
    mIntf = intf;

    return true;
}


}
