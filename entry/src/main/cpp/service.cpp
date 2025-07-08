//
// Created on 2025/7/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "TCPServer1.cpp"
#include "cpuUsage.h"
#include "integrityCheck1.h"
#include "integrityCheck2.h"
#include "napi/native_api.h"
#include "serialPort1.h"
#include "TCPServer1.h"

void startService() {
    startGetUsage();
    startServer1();
    startSp1();
}

void stopService() {
    stopGetUsage();
    stopDDRCheck();
    stopFileCheck();
    stopServer1();
    stopSp1();
}

static napi_value startServiceNapi(napi_env env, napi_callback_info info) {
    startService();
    return nullptr;
}

static napi_value stopServiceNapi(napi_env env, napi_callback_info info) {
    stopService();
    return nullptr;
}

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports) {
    napi_property_descriptor desc[] = {
        {"startService", nullptr, startServiceNapi, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"stopService", nullptr, stopServiceNapi, nullptr, nullptr, nullptr, napi_default, nullptr},
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}
EXTERN_C_END

static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "serialPort1",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

extern "C" __attribute__((constructor)) void RegisterNapiModule(void) { napi_module_register(&demoModule); }
