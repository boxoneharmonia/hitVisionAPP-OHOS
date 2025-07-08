//
// Created on 2025/7/7.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef MYAPPLICATION_TCPSERVERCLASS_H
#define MYAPPLICATION_TCPSERVERCLASS_H

#include <functional>
#include <netinet/in.h>
#include <thread>

class TCPServer {
public:
    using AcceptCb = std::function<void(int cliFd)>;
    TCPServer(const std::string &bindIp, const unsigned int port);
    ~TCPServer();
    
    void start();
    void stop();
    void setAcceptCallback(AcceptCb callback);

private:
    bool configure();
    void closeFd();
    void loop();

    std::string bindIp_;
    int serFd_;
    unsigned int port_;
    struct sockaddr_in serAddr_;
    AcceptCb callback_;
    std::thread worker_;
    std::atomic<bool> isRunning_;
};

#endif //MYAPPLICATION_TCPSERVERCLASS_H
