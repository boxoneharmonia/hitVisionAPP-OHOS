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

#define INTERVAL_FILE 5000
#define MB 1024

using namespace std;

IntegrityChecker filechecker(1);

static thread checkThread;

static void checkLoop() {
    while (fileCheckRunning) {
        filechecker.integrityCheck(fileCheckCnt, fileFaultCnt, fileResult);
        std::this_thread::sleep_for(std::chrono::milliseconds(INTERVAL_FILE));
    }
}

void startFileCheck() {
//     LOGI("start File Check");
    if (!fileCheckRunning) {
        filechecker.allocateIC(500 * MB, "/data/storage/el1/base/checkfile.bin");
        fileCheckRunning = true;
        checkThread = thread(checkLoop);
    } else {
        LOGW("File Check had started");
    }
}

void stopFileCheck() {
    if (fileCheckRunning) {
        fileCheckRunning = false;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (checkThread.joinable())
            checkThread.join();
        filechecker.releaseIC();
        LOGI("File Check stopped");
    } else {
        LOGW("File Check had stopped");
    }
}