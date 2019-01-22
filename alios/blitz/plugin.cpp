#include <blitz_adapter.h>
#include "document.h"
#include "xpdf.h"

extern "C" {

#if defined(HAVE_CODE_COVERAGE)
void __gcov_flush();
#endif

void __attribute__((visibility("default"))) plugin_deinit() {
    logi("");
    #if defined(HAVE_CODE_COVERAGE)
    __gcov_flush();
    #endif
    blitz::adapter::document::XPdfDocument::DeInit();
}

void __attribute__((visibility("default"))) plugin_init(void) {
    logi("plugin_init for image!");
    blitz_image_source_add(new blitz::adapter::document::ImageSource());
}

}
