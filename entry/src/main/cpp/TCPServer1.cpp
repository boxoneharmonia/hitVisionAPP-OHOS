//
// Created on 2025/7/7.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "TCPServerClass.h"
#include "log.h"
#include "var.h"
#include <fstream>
#include <linux/tcp.h>
#include <sys/time.h>
#include <unistd.h>

using namespace std;

TCPServer server1(IP_1, PORT_1);

static bool sendSucceed = false;

static void OnAccept(int cliFd) {
    uint8_t buf[MAX_SIZE_TCP];
    int sndbuf = 2048 * MAX_SIZE_TCP; //
    setsockopt(cliFd, SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf));
    if (fileNum == 0xff) {
        ifstream file(CHECKFILE, ios::binary);
        if (!file.is_open()) {
            LOGE("Failed to open file: %s", CHECKFILE);
            close(cliFd);
            return;
        }
        while (true) {
            file.read(reinterpret_cast<char *>(buf), MAX_SIZE_TCP);
            std::streamsize bytesRead = file.gcount();

            if (bytesRead <= 0) break;
            write(cliFd, buf, bytesRead);
            if (bytesRead < MAX_SIZE_TCP) break;
            usleep(10);
        }
    }  
    LOGI("Send Succeed.");
    sendSucceed = true;
    close(cliFd);
}

void startServer1() {
    sendSucceed = false;
    server1.setAcceptCallback(OnAccept);
    server1.start();
}

void stopServer1() { server1.stop(); }

int server1Running() { 
    if (server1.isRunning()) {
        if (sendSucceed) return 0x02;
        else return 0x01;
    }
    else return 0x00;
}
