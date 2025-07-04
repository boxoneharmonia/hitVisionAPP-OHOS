//
// Created on 2025/7/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "serialPort1.h"
#include "integrityCheck1.h"
#include "integrityCheck2.h"
#include "cpuUsage.h"
#include "napi/native_api.h"

static napi_value startSp1Napi(napi_env env, napi_callback_info info) {
    startSp1();
    return nullptr;
}

static napi_value stopSp1Napi(napi_env env, napi_callback_info info) {
    stopSp1();
    return nullptr;
}

static napi_value startGetUsageNapi(napi_env env, napi_callback_info info) {
    startGetUsage();
    return nullptr;
}

static napi_value stopGetUsageNapi(napi_env env, napi_callback_info info) {
    stopGetUsage();
    return nullptr;
}

static napi_value startDDRCheckNapi(napi_env env, napi_callback_info info) {
    startDDRCheck();
    return nullptr;
}

static napi_value stopDDRCheckNapi(napi_env env, napi_callback_info info) {
    stopDDRCheck();
    return nullptr;
}

static napi_value startFileCheckNapi(napi_env env, napi_callback_info info) {
    startFileCheck();
    return nullptr;
}

static napi_value stopFileCheckNapi(napi_env env, napi_callback_info info) {
    stopFileCheck();
    return nullptr;
}

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports) {
    napi_property_descriptor desc[] = {
        {"startSp1", nullptr, startSp1Napi, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"stopSp1", nullptr, stopSp1Napi, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"startGetUsage", nullptr, startGetUsageNapi, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"stopGetUsage", nullptr, stopGetUsageNapi, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"startDDRCheck", nullptr, startDDRCheckNapi, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"stopDDRCheck", nullptr, stopDDRCheckNapi, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"startFileCheck", nullptr, startFileCheckNapi, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"stopFileCheck", nullptr, stopFileCheckNapi, nullptr, nullptr, nullptr, napi_default, nullptr},
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
