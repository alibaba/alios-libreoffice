#include "Common.h"

using namespace yunos;

int main(int argc, char *argv[])
{
    Looper looper;
    SharedPtr<DServiceManager> manager = DServiceManager::getInstance();
    if (!manager) {
        LOG_E("fatal-error: unable to get servicemanager");
        return -1;
    }

    SharedPtr<DService> service = manager->getService(String(PDF_SERVICE_NAME));
    if (!service) {
        LOG_E("fatal-error: unable to register %s", PDF_SERVICE_NAME);
        return -1;
    }

    SharedPtr<DProxy> proxy = new DProxy(service, String(PDF_SERVICE_OBJECT_PATH),
                                        String(PDF_SERVICE_INTERFACE_PATH));

    SharedPtr<DMessage> msg = proxy->obtainMethodCallMessageWithName(TRANSACTION_openDocument);
    msg->writeString(argv[1]);

    SharedPtr<DMessage> result = proxy->sendMessageWithReplyBlocking(msg, 10000);
    int32_t id = result->readInt32();
    int32_t nPages = result->readInt32();
    LOG_I("openDocument id=%d nPages=%d", id, nPages);

    msg = proxy->obtainMethodCallMessageWithName(TRANSACTION_getPageImage);
    msg->writeInt32(id);
    msg->writeInt32(1);

    result = proxy->sendMessageWithReplyBlocking(msg, 10000);
    bool ok = result->readBool();
    if (ok) {
        int32_t width = result->readInt32();
        int32_t height = result->readInt32();
        int32_t bufsize = 0;
        int8_t *buf = result->readByteBuffer(bufsize);
        LOG_I("w=%d h=%d buf=%p size=%d", width, height, buf, bufsize);
    } else {
        LOG_I("getPageImage failed");
    }

    looper.run();

    return 0;
}
