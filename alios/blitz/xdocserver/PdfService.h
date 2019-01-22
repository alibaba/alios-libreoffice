#ifndef __PDF_SERVICE_H__
#define __PDF_SERVICE_H__
#include "Common.h"
#include "DocumentManager.h"

namespace doc {
using namespace yunos;

class PdfAdaptor : public DAdaptor {
public:
    PdfAdaptor(SharedPtr<DService> service);
    virtual ~PdfAdaptor();

    virtual bool handleMethodCall(const SharedPtr<DMessage>& msg);
    virtual void onDeath(const DLifecycleListener::DeathInfo& deathInfo);

private:
    SharedPtr<DService> mService;
    SharedPtr<pdf::DocumentManager> mDocManager;
};

}

#endif
