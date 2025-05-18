//
// Created on 2025/5/17.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef MYAPPLICATION_BURNER_H
#define MYAPPLICATION_BURNER_H

#pragma once

#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <cmath>
#include <random>
#include "log.h"

class BurningCPU {
public:
    BurningCPU() : running(false) {}

    // 启动 N 个线程进行烧机
    void start_burning(int thread_count) {
        if (running.load()) {
            LOGW("CPU burner is running, do not call again");
            return;
        }

        running.store(true);
        for (int i = 0; i < thread_count; ++i) {
            LOGI("CPU burner has been started");
            threads.emplace_back(&BurningCPU::burning_thread, this, i);
        }
    }
    
    void stop_burning() {
        if (!running.load()) {
            LOGW("CPU burner is not running, do not call again");
            return;
        }

        running.store(false);
        for (auto &t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
        LOGI("CPU burner has been stopped");
        threads.clear();
    }

    ~BurningCPU() {
        stop_burning(); // 保证析构时停止线程
    }

private:
    std::vector<std::thread> threads;
    std::atomic<bool> running;
    
    void burning_thread(int id) {
        volatile double x = 0.0001;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);
        
        while (running.load()) {
            for (int i = 0; i < 1000000; ++i) {
                double r = dis(gen);
                x += std::sin(r * 1e6) + std::cos(r);
                x += std::pow(x, r);
                x += std::sin(x) * std::cos(x);
            }
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
    }
};

#endif //MYAPPLICATION_BURNER_H
