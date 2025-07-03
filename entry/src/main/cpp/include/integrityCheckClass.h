//
// Created on 2025/7/3.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include <cstdint>
#include <cstdio>
#include <string>

class IntegrityChecker {
public:
    IntegrityChecker(const size_t kiloBytes);
    IntegrityChecker(const size_t kiloBytes, const std::string &path);
    ~IntegrityChecker();
    void allocateIC(const size_t kiloBytes, const std::string &path);
    void integrityCheck(uint8_t &checkCnt, uint8_t &faultCnt, uint8_t &result);
    void releaseIC();
    
private:
    enum checkType {None, DDR, File} mode_;
    uint8_t checkCnt_ = 0;
    uint8_t faultCnt_ = 0;
    uint8_t checksum_ = 0;
    uint8_t checksumEx_ = 0;
    bool firstCheck_ = true;

    bool isAllocated_ = false;
    uint8_t *ddrBuffer_ = nullptr;
    FILE *fileHandle_ = nullptr;
    size_t kiloBytes_ = 0;
    size_t length_ = 0;

    void allocateDDR(const size_t length);
    void allocateFile(const size_t length, const std::string &path);
    void checkDDR(uint8_t &checkCnt, uint8_t &faultCnt, uint8_t &result);
    void checkFile(uint8_t &checkCnt, uint8_t &faultCnt, uint8_t &result);
    void releaseDDR();
    void releaseFile();
    uint8_t crc8Check(const uint8_t *data, size_t length); // general IC function
};
