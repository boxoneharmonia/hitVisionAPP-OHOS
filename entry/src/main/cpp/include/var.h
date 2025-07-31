//
// Created on 2025/7/2.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef MYAPPLICATION_VAR_H
#define MYAPPLICATION_VAR_H

#include <atomic>
#include <cstdint>

#define BIT(x) (1U << (x))

#define BAUDRATE_1 115200
#define SERIAL_PORT_1 "/dev/ttyS0"
#define INTERVAL_1 50
#define REPLY_LENGTH 19

#define INTERVAL_DDR 5000
#define INTERVAL_FILE 5000
#define ddrMB 500 * 1024
#define fileMB 50 * 1024
#define CHECKFILE "/data/storage/el1/base/checkfile.bin"

#define PORT_1 3864
#define MAX_SIZE_TCP 1024
#define IP_1 "192.168.1.211"

extern std::atomic<bool> ledRunning;
extern std::atomic<bool> cameraRunning;
extern std::atomic<bool> modelRunning;
extern std::atomic<bool> ddrCheckRunning;
extern std::atomic<bool> fileCheckRunning;
extern std::atomic<uint8_t> fileNum;
extern uint8_t cpuUsageNow;
extern uint8_t ddrCheckCnt;
extern uint8_t ddrFaultCnt;
extern uint8_t ddrResult;
extern uint8_t fileCheckCnt;
extern uint8_t fileFaultCnt;
extern uint8_t fileResult;

#endif //MYAPPLICATION_VAR_H
