//
// Created on 2025/7/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "var.h"

using namespace std;

atomic<bool> ledRunning(false);
atomic<bool> cameraRunning(false);
atomic<bool> modelRunning(false);
uint8_t cpuUsageNow = 0;
uint8_t ddrCheckCnt = 0;
uint8_t ddrFaultCnt = 0;
uint8_t ddrResult = 0;