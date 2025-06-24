//
// Created on 2025/6/24.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "serialPortClass.h"
#include "log.h"

#define baudrate 115200
#define port "/dev/ttyS0"

SerialPortHandler sp1(port, baudrate);

static void onReceive(SerialPortHandler &handler, const uint8_t *data, size_t length) {
    LOGI("Received %d bytes", length);
    uint8_t reply[] = {0xDE, 0xAD};
    handler.writeData(reply, sizeof(reply));
}

void startSp1() {
    sp1.setReceiveCallback(onReceive);
    sp1.start();
}

void stopSp1() {
    sp1.stop();
}
