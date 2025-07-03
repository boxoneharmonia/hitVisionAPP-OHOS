//
// Created on 2025/7/2.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef MYAPPLICATION_VAR_H
#define MYAPPLICATION_VAR_H

#include <atomic>
#include <cstdint>

extern std::atomic<bool> ledRunning;
extern std::atomic<bool> cameraRunning;
extern std::atomic<bool> modelRunning;
extern uint8_t cpuUsageNow;
extern uint8_t ddrCheckCnt;
extern uint8_t ddrFaultCnt;
extern uint8_t ddrResult;

#endif //MYAPPLICATION_VAR_H
