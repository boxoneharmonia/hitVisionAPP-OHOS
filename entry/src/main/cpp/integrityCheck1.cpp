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

#define INTERVAL 5000
#define MB 1024

using namespace std;

IntegrityChecker ddrchecker(1);
static atomic<bool>threadRunning(false);

static thread checkThread;

static void checkLoop() {
    while (threadRunning) {
//         LOGI("DDR Check Loop");
        ddrchecker.integrityCheck(ddrCheckCnt, ddrFaultCnt, ddrResult);
        std::this_thread::sleep_for(std::chrono::milliseconds(INTERVAL));
    }
}

void startDDRCheck() {
//     LOGI("start DDR Check");
    if (!threadRunning) {
        ddrchecker.allocateIC(250 * MB, "");
        threadRunning = true;
        checkThread = thread(checkLoop);
    } else {
        LOGW("DDR Check has started");
    }
}

void stopDDRCheck() {
    if (threadRunning) {
        threadRunning = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (checkThread.joinable()) checkThread.join();
        ddrchecker.releaseIC();
    } else {
        LOGW("DDR Check has stopped");
    }
}


