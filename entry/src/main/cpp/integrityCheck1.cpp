//
// Created on 2025/7/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "integrityCheckClass.h"
#include "integrityCheck1.h"
#include "log.h"
#include "var.h"
#include <thread>
#include <atomic>
#include <chrono>

#define INTERVAL_DDR 5000
#define MB 1024

using namespace std;

IntegrityChecker ddrchecker(1);

static thread checkThread;

static void checkLoop() {
    while (ddrCheckRunning) {
//         LOGI("DDR Check Loop");
        ddrchecker.integrityCheck(ddrCheckCnt, ddrFaultCnt, ddrResult);
        std::this_thread::sleep_for(std::chrono::milliseconds(INTERVAL_DDR));
    }
}

void startDDRCheck() {
//     LOGI("start DDR Check");
    if (!ddrCheckRunning) {
        ddrchecker.allocateIC(500 * MB, "");
        ddrCheckRunning = true;
        checkThread = thread(checkLoop);
    } else {
        LOGW("DDR Check had started");
    }
}

void stopDDRCheck() {
    if (ddrCheckRunning) {
        ddrCheckRunning = false;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (checkThread.joinable()) checkThread.join();
        ddrchecker.releaseIC();
        LOGI("DDR Check stopped");
    } else {
        LOGW("DDR Check had stopped");
    }
}


