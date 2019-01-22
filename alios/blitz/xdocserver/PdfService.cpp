#include "PdfService.h"

namespace doc {

PdfAdaptor::PdfAdaptor(SharedPtr<DService> service) :
DAdaptor(service,
        String(PDF_SERVICE_OBJECT_PATH),
        String(PDF_SERVICE_INTERFACE_PATH))
{
    mDocManager = new pdf::DocumentManager();
    if (!mDocManager) {
        loge("OOM");
    }
}

PdfAdaptor::~PdfAdaptor() {
}

void PdfAdaptor::onDeath(const DLifecycleListener::DeathInfo& deathInfo) {
    logi("name=%s seq=%d exiting", deathInfo.mName.c_str(), deathInfo.mSeq);
    exit(0);
}

bool PdfAdaptor::handleMethodCall(const SharedPtr<DMessage>& msg) {
    String method = msg->methodName();
    String client= msg->name();
    logi("handleMethodCall %s %s", method.c_str(), client.c_str());

    if (method == TRANSACTION_openDocument) {
        String filename = msg->readString();
        logi("opening %s", filename.c_str());
        SharedPtr<DMessage> reply = msg->makeMethodReturn();
        SharedPtr<pdf::Document> doc = mDocManager->createDocument(
                                                        client, filename);

        if (doc && doc->open(reply)) {
            reply->writeInt32(doc->getId());
            reply->writeInt32(doc->getTotalPages());
        } else {
            reply->writeInt32(0);
            reply->writeInt32(0);
        }

        sendMessage(reply);
        logi("result sent");
    } else if (method == TRANSACTION_getPageImage) {
        //writeByteBuffer
        int32_t id = msg->readInt32();
        int32_t iPage = msg->readInt32();
        logi("converting %d %d", id, iPage);

        SharedPtr<DMessage> reply = msg->makeMethodReturn();
        SharedPtr<pdf::Document> doc = mDocManager->getDocument(id);

        if (doc && doc->getPageImage(iPage, reply)) {
        } else {
            reply->writeBool(false);
        }

        sendMessage(reply);
        logi("result sent");
    }

    return true;
}

}
