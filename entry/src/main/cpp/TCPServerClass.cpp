//
// Created on 2025/7/7.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "TCPServerClass.h"
#include "log.h"
#include <arpa/inet.h>
#include <chrono>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

// public

TCPServer::TCPServer(const std::string &bindIp, const unsigned int port) : bindIp_(bindIp), port_(port){ }

TCPServer::~TCPServer() { stop(); }

void TCPServer::start() {
    if (isRunning_) {
        LOGW("Server has started, port %{public}u.", port_);
        return;
    }

    if (!configure()) {
        LOGE("Configure Failed, port %{public}u", port_);
        return;
    }
    
    isRunning_ = true;
    worker_ = thread(&TCPServer::loop, this);
    LOGI("Server started, port %{public}u.", port_);
}

void TCPServer::stop() {
    if (!isRunning_) {
        LOGW("Server is not running, port %{public}u.", port_);
        return;
    }
    isRunning_ = false;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    if (worker_.joinable()) {
        worker_.join();
    }
    closeFd();
    LOGI("Server stopped, port %{public}u.", port_);
}

void TCPServer::setAcceptCallback(AcceptCb callback) { callback_ = callback; }

bool TCPServer::isRunning() const { return isRunning_; }

// private

bool TCPServer::configure() {
    socklen_t serLen = sizeof(serAddr_);

    serFd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serFd_ < 0) {
        LOGE("Socket creation failed: %{public}s", strerror(errno));
        return false;
    }

    int opt = 1;
    setsockopt(serFd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&serAddr_, 0, serLen);
    serAddr_.sin_family = AF_INET;
    serAddr_.sin_port = htons(port_);

    int ret = inet_pton(AF_INET, bindIp_.c_str(), &serAddr_.sin_addr);
    if (ret <= 0) {
        LOGE("Invalid IP: %{public}s", bindIp_.c_str());
        return false;
    }

    if (::bind(serFd_, (struct sockaddr *)&serAddr_, serLen) == -1) {
        LOGE("Bind Error: %{public}s", strerror(errno));
        return false;
    }

    if (listen(serFd_, 8) == -1) {
        LOGE("Listen Error: %{public}s", strerror(errno));
        return false;
    }

    LOGI("Listening on %{public}s:%{public}u", bindIp_.c_str(), port_);
    return true;
}


void TCPServer::closeFd() {
    if (serFd_ > 0) {
        close(serFd_);
        serFd_ = -1;
    }
}

void TCPServer::loop() {
    while (isRunning_) {
        int cliFd;
        struct sockaddr_in cliAddr;
        socklen_t cliLen = sizeof(cliAddr);
        cliFd = accept(serFd_, (struct sockaddr *)&cliAddr, &cliLen);
        if (cliFd < 0) {
            LOGE("Accept Error: %{public}s", strerror(errno));
            continue;
        }
        LOGI("Client Accepted, IP: %{public}s : %{public}u", inet_ntoa(cliAddr.sin_addr), htons(cliAddr.sin_port));
        if (callback_) {
            callback_(cliFd);
        } else {
            isRunning_ = false;
            close(cliFd);
        }
    }
}
