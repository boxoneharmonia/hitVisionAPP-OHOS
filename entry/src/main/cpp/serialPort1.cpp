//
// Created on 2025/6/24.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "integrityCheck1.h"
#include "integrityCheck2.h"
#include "serialPortClass.h"
#include "serialPort1.h"
#include "log.h"
#include "TCPServer1.cpp"
#include "var.h"
#include <cstdint>
#include <cstring>

using namespace std;

SerialPortHandler sp1(SERIAL_PORT_1, BAUDRATE_1, INTERVAL_1);

static uint8_t recvCnt = 0;
static uint8_t sendCnt = 0;
static uint8_t replyReq = 0x00;
const uint8_t replyHead[] = {0x1A, 0xCF, 0x01};

static void onReceive(SerialPortHandler &handler, const uint8_t *data, size_t length) {
    if (length > 0) {
//         LOGI("Received %{public}d bytes", length);
        if (data[0] != 0xEB || data[1] != 0x90) return;
        if (data[2] == 0x01) {
            if (length != 3) return;
            recvCnt += 1;
            replyReq = 0x01;
        }
        else if (data[2] == 0x02) {
            if (length != 8) return;
            recvCnt += 1;
                
            if (data[3] == 0xAA) 
                ledRunning = true;
            else if (data[3] == 0x55) 
                ledRunning = false;
                
            if (data[4] == 0xAA)
                cameraRunning = true;
            else if (data[4] == 0x55)
                cameraRunning = false;
                
            if (data[5] == 0xAA)
                modelRunning = true;
            else if (data[5] == 0x55)
                modelRunning = false;

            if (data[6] == 0xAA)
                startDDRCheck();
            else if (data[6] == 0x55)
                stopDDRCheck();

            if (data[7] == 0xAA)
                startFileCheck();
            else if (data[7] == 0x55)
                stopFileCheck();
        } 
        else if (data[2] == 0x03) {
            if (length != 5)
                return;
            if (data[3] == 0xAA) {
                startServer1();
                fileNum = data[4];
            }
            else if (data[3] == 0x55) {
                stopServer1();
                fileNum = 0;
            }
        }
    } else {
//         LOGI("Nothing reveived");
        if (replyReq == 0x01) {
            int offset = 0;
            replyReq = 0x00;
            sendCnt += 1;
            uint8_t reply[REPLY_LENGTH];
            uint8_t tmp[REPLY_LENGTH - 3] = {0};
            
            tmp[0] = cpuUsageNow;
            
            tmp[1] = ledRunning == true;
            tmp[2] = cameraRunning == true;
            tmp[3] = modelRunning == true;
            
            tmp[4] = ddrCheckRunning == true;
            tmp[5] = ddrCheckCnt;
            tmp[6] = ddrFaultCnt;
            tmp[7] = ddrResult;
            
            tmp[8] = fileCheckRunning == true;
            tmp[9] = fileCheckCnt;
            tmp[10] = fileFaultCnt;
            tmp[11] = fileResult;
            
            tmp[12] = server1Running();
            tmp[13] = fileNum;

            tmp[14] = recvCnt;
            tmp[15] = sendCnt;

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
