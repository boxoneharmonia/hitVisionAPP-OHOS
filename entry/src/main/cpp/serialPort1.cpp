//
// Created on 2025/6/24.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "serialPortClass.h"
#include "serialPort1.h"
#include "log.h"
#include "var.h"
#include <cstdint>
#include <cstring>

#define BAUDRATE 115200
#define PORT "/dev/ttyS0"
#define REPLY_LENGTH 15
#define BIT(x) (1U << (x))

using namespace std;

SerialPortHandler sp1(PORT, BAUDRATE);

static uint8_t recvCnt = 0;
static uint8_t sendCnt = 0;
static uint8_t replyReq = 0x00;
const uint8_t replyHead[] = {0x1A, 0xCF, 0x01};

static void onReceive(SerialPortHandler &handler, const uint8_t *data, size_t length) {
    if (length > 0) {
        LOGI("Received %{public}d bytes", length);
        if (data[0] != 0xEB || data[1] != 0x90) return;
        if (data[2] == 0x01) {
            if (length != 3) return;
            recvCnt += 1;
            replyReq = 0x01;
        }
        else if (data[2] == 0x02) {
            if (length != 4) return;
            recvCnt += 1;
            ledRunning = data[3] & BIT(0);
            cameraRunning = data[3] & BIT(1);
            modelRunning = data[3] & BIT(2);
        }
    } else {
//         LOGI("Nothing reveived");
        if (replyReq == 0x01) {
            int offset = 0;
            replyReq = 0x00;
            sendCnt += 1;
            uint8_t reply[REPLY_LENGTH];
            uint8_t tmp[REPLY_LENGTH - 3];
            
            tmp[0] = cpuUsageNow;
            tmp[1] = ledRunning == true;
            tmp[2] = cameraRunning == true;
            tmp[3] = modelRunning == true;
            tmp[4] = ddrCheckCnt;
            tmp[5] = ddrFaultCnt;
            tmp[6] = ddrResult;
            tmp[7] = fileCheckCnt;
            tmp[8] = fileFaultCnt;
            tmp[9] = fileResult;
            tmp[10] = recvCnt;
            tmp[11] = sendCnt;
            
            memcpy(reply + offset, replyHead, sizeof(replyHead));
            offset += sizeof(replyHead);
            memcpy(reply + offset, tmp, sizeof(tmp)); 
            offset += sizeof(tmp);
            handler.writeData(reply, sizeof(reply));
        }
    }
}

void startSp1() {
    sp1.setReceiveCallback(onReceive);
    sp1.start();
}

void stopSp1() {
    sp1.stop();
}
