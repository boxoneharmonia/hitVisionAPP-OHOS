//
// Created on 2025/6/24.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "serialPortClass.h"
#include "log.h"
#include "napi/native_api.h"

#define baudrate 115200
#define port "/dev/ttyS0"

SerialPortHandler sp1(port, baudrate);

static void onReceive(SerialPortHandler &handler, const uint8_t *data, size_t length) {
    if (length > 0) {
        LOGI("Received %{public}d bytes", length);
        uint8_t reply[] = {0xDE, 0xAD};
        handler.writeData(reply, sizeof(reply));
    }

}

void startSp1() {
    sp1.setReceiveCallback(onReceive);
    sp1.start();
}

void stopSp1() {
    sp1.stop();
}

static napi_value startSp1Napi(napi_env env, napi_callback_info info) {
    startSp1();
    return nullptr;
}

static napi_value stopSp1Napi(napi_env env, napi_callback_info info) { 
    stopSp1();
    return nullptr; 
}

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports) {
    napi_property_descriptor desc[] = {
        {"startSp1", nullptr, startSp1Napi, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"stopSp1", nullptr, stopSp1Napi, nullptr, nullptr, nullptr, napi_default, nullptr}};
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

extern "C" __attribute__((constructor)) void RegisterBurnerModule(void) { napi_module_register(&demoModule); }