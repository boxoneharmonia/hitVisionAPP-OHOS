//
// Created on 2025/7/7.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#define PORT_1 3864
#define MAX_SIZE_TCP 2048
#define IP_1 "192.168.1.212"

#include "TCPServerClass.h"
#include "log.h"
#include <unistd.h>

TCPServer server1(IP_1, PORT_1);

static void OnAccept(int cliFd) {
    uint8_t buf[MAX_SIZE_TCP];
    while (true) {
        ssize_t n = read(cliFd, buf, MAX_SIZE_TCP);
        if (n <= 0) {
            LOGW("Client closed or read error.");
            break;
        }
        
        write(cliFd, buf, n);
        if (n == 2 && buf[0] == 0x55 && buf[1] == 0xAA) {
            LOGI("Closed by client.");
            return;
        }
    }

    close(cliFd);
}

void startServer1() {
    server1.setAcceptCallback(OnAccept);
    server1.start();
}

void stopServer1() { server1.stop(); }

bool server1Running() { return server1.isRunning();}
