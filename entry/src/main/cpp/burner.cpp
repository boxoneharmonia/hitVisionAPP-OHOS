//
// Created on 2025/5/17.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#include "napi/native_api.h"
#include "burner.h"
#include "log.h"

static BurningCPU burner;

static napi_value StartBurning(napi_env env, napi_callback_info info) {
    napi_status status;
    size_t argc = 1;
    napi_value args[1];
    int32_t thread_count = 1;
    
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    
    napi_valuetype valuetype;
    status = napi_typeof(env, args[0], &valuetype);
    if (valuetype != napi_number) {
        LOGE("Wrong arguments");
        napi_throw_type_error(env, NULL, "Wrong arguments");
        return nullptr;
    }
    
    napi_get_value_int32(env, args[0], &thread_count);

    burner.start_burning(thread_count);
    return nullptr;
}

static napi_value StopBurning(napi_env env, napi_callback_info info) {
    burner.stop_burning();
    return nullptr;
}

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports) {
    napi_property_descriptor desc[] = {
        {"StartBurning", nullptr, StartBurning, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"StopBurning", nullptr, StopBurning, nullptr, nullptr, nullptr, napi_default, nullptr}
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
    .nm_modname = "burner",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

extern "C" __attribute__((constructor)) void RegisterBurnerModule(void) { napi_module_register(&demoModule); }