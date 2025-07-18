//
// Created on 2025/5/29.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef MYAPPLICATION_SERIALPORTCLASS_H
#define MYAPPLICATION_SERIALPORTCLASS_H

#include <string>
#include <atomic>
#include <thread>
#include <cstdint>
#include <cstddef>
#include <functional>

class SerialPortHandler  {
public:
    using ReceiveCb = std::function<void(SerialPortHandler &, const uint8_t *, size_t)>;
    SerialPortHandler(const std::string &portName, int baudrate, int intervalMs);
    ~SerialPortHandler ();

    void start();
    void stop();
    bool isRunning() const;
    void setReceiveCallback(ReceiveCb callback);
    int writeData(const uint8_t *data, size_t length);

private:
    void loop();
    bool configure();
    void closePort();
    int readData(uint8_t *buffer, size_t maxLength);
    
    int fd_;
    std::string portName_;
    int baudrate_;
    int interval_;
    std::atomic<bool> running_{false};
    std::thread worker_;
    ReceiveCb callback_;
};
    
#endif //MYAPPLICATION_SERIALPORTCLASS_H
