//
// Created on 2025/5/29.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include <cstdint>
#include <vector>
#include <array>
#include <atomic>
#include <termios.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include "log.h"
#include "serialPortClass.h"

#define INTERVAL 50
#define MAX_READ_SIZE 256

using namespace std;

static speed_t getBaudrate(int baudrate) {
    switch (baudrate) {
    case 0:
        return B0;
    case 50:
        return B50;
    case 75:
        return B75;
    case 110:
        return B110;
    case 134:
        return B134;
    case 150:
        return B150;
    case 200:
        return B200;
    case 300:
        return B300;
    case 600:
        return B600;
    case 1200:
        return B1200;
    case 1800:
        return B1800;
    case 2400:
        return B2400;
    case 4800:
        return B4800;
    case 9600:
        return B9600;
    case 19200:
        return B19200;
    case 38400:
        return B38400;
    case 57600:
        return B57600;
    case 115200:
        return B115200;
    case 230400:
        return B230400;
    case 460800:
        return B460800;
    case 500000:
        return B500000;
    case 576000:
        return B576000;
    case 921600:
        return B921600;
    case 1000000:
        return B1000000;
    case 1152000:
        return B1152000;
    case 1500000:
        return B1500000;
    case 2000000:
        return B2000000;
    case 2500000:
        return B2500000;
    case 3000000:
        return B3000000;
    case 3500000:
        return B3500000;
    case 4000000:
        return B4000000;
    default:
        return -1;
    }
}

SerialPortHandler::SerialPortHandler(const string &portName, int baudrate)
    : fd_(-1), portName_(portName), baudrate_(baudrate) {}

SerialPortHandler::~SerialPortHandler() { stop(); }

// class:public

void SerialPortHandler::start() {
    if (running_) {
        LOGW("Thread has started, port %{public}s.", portName_.c_str());
        return;
    }
    if (!configure()) {
        LOGW("Configure failed, port %{public}s.", portName_.c_str());
        return;
    }
    running_ = true;
    worker_ = thread(&SerialPortHandler::loop, this);
    LOGI("sp1Thread started, port %{public}s.", portName_.c_str());
    return;
}

void SerialPortHandler::stop() {
    if (!running_) {
        LOGW("Thread is not running, port %{public}s.", portName_.c_str());
        return;
    }
    running_ = false;
    if (worker_.joinable()) {
        worker_.join();
    }
    closePort();
    LOGI("Thread stopped, port %{public}s.", portName_.c_str());
}

bool SerialPortHandler::isRunning() const { return running_; }

void SerialPortHandler::setReceiveCallback(ReceiveCb callback) { callback_ = callback; }

int SerialPortHandler::writeData(const uint8_t *data, size_t length) {
    if (fd_ < 0) {
        return -1;
    }
    return write(fd_, data, length);
}

//class:private

void SerialPortHandler::loop() {
    static uint8_t buffer[MAX_READ_SIZE];
    static size_t n;
    while (running_) {
        n = readData(buffer, sizeof(buffer));
        callback_(*this, buffer, n);
        this_thread::sleep_for(chrono::milliseconds(INTERVAL));
    }
}

bool SerialPortHandler::configure() {
    const char *portName = portName_.c_str();
    if (fd_ > 0) {
        LOGW("Serial port %{public}s is already open", portName);
        close(fd_);
        fd_ = -1;
    }
    fd_ = open(portName, O_RDWR | O_NOCTTY);
    if (fd_ < 0) {
        LOGE("Failed to open serial port");
        return false;
    }
    struct termios cfg;
    tcgetattr(fd_, &cfg);
    cfmakeraw(&cfg);
    cfsetispeed(&cfg, getBaudrate(baudrate_));
    cfsetospeed(&cfg, getBaudrate(baudrate_));
    cfg.c_cflag &= ~PARENB; // 无奇偶校验
    cfg.c_cflag &= ~CSTOPB; // 1个停止位
    cfg.c_cflag &= ~CSIZE;
    cfg.c_cflag |= CS8; // 8个数据位

    cfg.c_cflag &= ~CRTSCTS;       // 无硬件流控
    cfg.c_cflag |= CREAD | CLOCAL; // 允许读，不控制DTR/RTS

    cfg.c_iflag &= ~(IXON | IXOFF | IXANY);         // 关闭软件流控
    cfg.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG); // 原始输入模式
    cfg.c_oflag &= ~OPOST;                          // 关闭输出处理

    cfg.c_cc[VMIN] = 0;
    cfg.c_cc[VTIME] = 0; // 0ms 超时
    if ((tcsetattr(fd_, TCSANOW, &cfg)) != 0) {
        LOGE("Set serial error, port %{public}s.", portName);
        return false;
    } else {
        LOGI("Open serial port %{public}s at %{public}d baud", portName, baudrate_);
    }
    return true;
}

void SerialPortHandler::closePort() {
    if (fd_ >= 0) {
        close(fd_);
        fd_ = -1;
    }
}

int SerialPortHandler::readData(uint8_t *buffer, size_t maxLength) {
    if (fd_ < 0) {
        return -1;
    }
    return read(fd_, buffer, maxLength);
}


