#ifndef __PDF_COMMON_H__
#define __PDF_COMMON_H__
#include <looper/Looper.h>
#include <thread/LooperThread.h>
#include <pointer/BasePtr.h>
#include <log/Log.h>

#include <string/String.h>
#include <pointer/SharedPtr.h>

#include <dbus/DAdaptor.h>
#include <dbus/DConnection.h>
#include <dbus/DMessage.h>
#include <dbus/DService.h>
#include <dbus/DServiceManager.h>
#include <dbus/DSignalCallback.h>
#include <dbus/DSignalRule.h>
#include <dbus/DError.h>

#define PDF_SERVICE_NAME "com.yunos.tv.doc.convert"
#define PDF_SERVICE_OBJECT_PATH "/com/yunos/tv/doc/converter"
#define PDF_SERVICE_INTERFACE_PATH "com.yunos.tv.doc.interface.convert"

const static yunos::String TRANSACTION_openDocument("openDocument");
const static yunos::String TRANSACTION_getPageImage("getPageImage");

#ifndef __YFILENAME__
#define __YFILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define logd(fmt,args...) LOG_D("[DEBUG]%s %d %s " fmt, __YFILENAME__, __LINE__, __func__,  ##args)
#define logi(fmt,args...) LOG_I("[INFO]%s %d %s " fmt, __YFILENAME__, __LINE__, __func__,  ##args)
#define loge(fmt,args...) LOG_E("[ERROR]%s %d %s " fmt, __YFILENAME__, __LINE__, __func__,  ##args)
#endif

#endif
