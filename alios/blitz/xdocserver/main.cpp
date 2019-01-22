#include "PdfService.h"

using namespace yunos;

int main(int argc, char *argv[])
{
    Looper looper;
    logi("");
    SharedPtr<DServiceManager> manager = DServiceManager::getInstance();
    if (!manager) {
        loge("fatal-error: unable to get servicemanager");
        return -1;
    }

    SharedPtr<DService> service = manager->registerService(String(PDF_SERVICE_NAME));
    if (!service) {
        loge("fatal-error: unable to register %s", PDF_SERVICE_NAME);
        return -1;
    }

    SharedPtr<DAdaptor> adapter = new doc::PdfAdaptor(service);

    if (!adapter) {
        loge("fatal-error: oom ");
        return -1;
    }

    int ret = adapter->publish();

    logi("publish %s ret=%d", PDF_SERVICE_NAME, ret);

    looper.run();

    return 0;
}
