//
// Created on 2025/7/4.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "integrityCheckClass.h"
#include "integrityCheck2.h"
#include "log.h"
#include "var.h"
#include <thread>
#include <atomic>
#include <chrono>

#define INTERVAL 5000
#define MB 1024

using namespace std;

IntegrityChecker filechecker(1);
static atomic<bool> threadRunning(false);

static thread checkThread;

static void checkLoop() {
    while (threadRunning) {
        filechecker.integrityCheck(fileCheckCnt, fileFaultCnt, fileResult);
        std::this_thread::sleep_for(std::chrono::milliseconds(INTERVAL));
    }
}

void startFileCheck() {
//     LOGI("start File Check");
    if (!threadRunning) {
        filechecker.allocateIC(250 * MB, "/data/storage/el1/base/checkfile.bin");
        threadRunning = true;
        checkThread = thread(checkLoop);
    } else {
        LOGW("File Check has started");
    }
}

void stopFileCheck() {
    if (threadRunning) {
        threadRunning = false;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (checkThread.joinable())
            checkThread.join();
        filechecker.releaseIC();
        LOGI("File Check stopped");
    } else {
        LOGW("File Check has stopped");
    }
}