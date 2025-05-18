#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <bits/alltypes.h>
#include <fcntl.h>
#include <string.h>

#include "serial.h"
#include "log.h"
#include "napi/native_api.h"

const uint32_t MAX_SIZE = 1024;

static napi_value OpenSerial(napi_env env, napi_callback_info info)
// OpenSerial(env, [port: string, baudrate: number]) => fd: number
{
//     LOGI("connected to function OpenSerial in serial.cpp");
    napi_status status;
    int fd = -1;
    size_t argc = 2;
    napi_value args[2] = {nullptr};

    napi_get_cb_info(env, info, &argc, args , nullptr, nullptr);

    napi_valuetype valuetype;
    status = napi_typeof(env, args[0], &valuetype);
    if (valuetype != napi_string) 
    {
        LOGE("Wrong arguments");
        napi_throw_type_error(env, NULL, "Wrong arguments");
        return nullptr;
    }

    status = napi_typeof(env, args[1], &valuetype);
    if (valuetype != napi_number) 
    {
        LOGE("Wrong arguments");
        napi_throw_type_error(env, NULL, "Wrong arguments");
        return nullptr;
    }

    char port[MAX_SIZE] = {0};
    size_t portLen = 0;
    napi_get_value_string_utf8(env, args[0], port, MAX_SIZE, &portLen);

    double baudrateD;
    napi_get_value_double(env, args[1], &baudrateD);
    int baudrate = static_cast<int>(baudrateD);

    if (fd > 0)
    {
        LOGW("Serial port %s is already open", port);
        close(fd);
        fd = -1;
    }
    fd = open(port, O_RDWR | O_NOCTTY);
    if (fd < 0) 
    {
        napi_throw_error(env, nullptr, "Failed to open serial port");
        return nullptr;
    }
    struct termios cfg;
    tcgetattr(fd, &cfg);
    cfmakeraw(&cfg);
    cfsetispeed(&cfg, getBaudrate(baudrate));
    cfsetospeed(&cfg, getBaudrate(baudrate));
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
    cfg.c_cc[VTIME] = 10; // 1000ms 超时
    if ((tcsetattr(fd, TCSANOW, &cfg)) != 0)
        LOGE("Set serial error!");
    else
        LOGI("Open serial port %{public}s at %{public}d baud", port, baudrate);
    napi_value result;
    napi_create_int32(env, fd, &result);
    return result;
}

static napi_value WriteSerial(napi_env env, napi_callback_info info)
// WriteSerial(env, [fd: number, message: string or array]) => bytesWritten: number
{
//     LOGI("connected to function WriteSerial in serial.cpp");
    napi_status status; 
    int fd = -1;
    int ret = -1;
    size_t argc = 2;
    size_t bytesWritten = 0;
    napi_value args[2] = {nullptr};
    
    status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    
    napi_valuetype valuetype;
    status = napi_typeof(env, args[0], &valuetype);
    if (valuetype != napi_number) 
    {
        LOGE("Wrong arguments");
        napi_throw_type_error(env, NULL, "Wrong arguments");
        return nullptr;
    }
    napi_get_value_int32(env, args[0], &fd);
    if (fd <= 0)
    {
        LOGE("Invalid file descriptor");
        napi_throw_error(env, nullptr, "Invalid file descriptor");
        return nullptr;
    }

    status = napi_typeof(env, args[1], &valuetype);
    if (valuetype == napi_string) 
    {
        char message[MAX_SIZE] = {0};
        size_t len;

        status = napi_get_value_string_utf8(env, args[1], message, MAX_SIZE, &len);

        size_t totalBytesToWrite = len;
        bytesWritten = 0;

        while (bytesWritten < totalBytesToWrite) {
            ret = write(fd, message + bytesWritten, totalBytesToWrite - bytesWritten);
            if (ret == -1) {
                napi_throw_error(env, nullptr, "Write error");
                return nullptr;
            }
            bytesWritten += ret;
        }
    } 
    else if (valuetype == napi_object) 
    {
        napi_typedarray_type arrayType;
        napi_value input_buffer;
        size_t byte_offset; // 数据偏移
        size_t i, length;   // 数据字节大小
        napi_get_typedarray_info(env, args[1], &arrayType, &length, NULL, &input_buffer, &byte_offset);
      if (status != napi_ok || arrayType != napi_uint8_array) {
            napi_throw_type_error(env, nullptr, "Expected Uint8Array");
            LOGE("Expected Uint8Array");
            return nullptr;
        }

        void *data;
        size_t byte_length;
        napi_get_arraybuffer_info(env, input_buffer, &data, &byte_length);

//         LOGI("Ready Uint8Array %{public}d byte(s)", byte_length);
            
        unsigned char *message = static_cast<unsigned char *>(data);
        size_t totalBytesToWrite = byte_length;
        bytesWritten = 0;

        while (bytesWritten < totalBytesToWrite) {
            ret = write(fd, message + bytesWritten, totalBytesToWrite - bytesWritten);
            if (ret == -1) {
                napi_throw_error(env, nullptr, "Write error");
                return nullptr;
            }
            bytesWritten += ret;
        }
    } 
    else 
    {
        LOGE("Wrong arguments. Expected Uint8Array or string");
        napi_throw_type_error(env, nullptr, "Wrong arguments. Expected Uint8Array or string");
        return nullptr;
    }

    // 返回写入的字节数
    napi_value result;
    napi_create_uint32(env, bytesWritten, &result);
    return result;
}

static napi_value ReadSerial(napi_env env, napi_callback_info info) 
//ReadSerial(env, [fd: number]) => readbytes: array
{
//     LOGI("connected to function ReadSerial in serial.cpp");
    napi_status status;
    int fd = -1;
    int ret = -1;
    size_t argc = 1;
    napi_value args[1] = {nullptr};

    status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    napi_valuetype valuetype;
    status = napi_typeof(env, args[0], &valuetype);
    if (valuetype != napi_number) {
        LOGE("Wrong arguments");
        napi_throw_type_error(env, NULL, "Wrong arguments");
        return nullptr;
    }
    napi_get_value_int32(env, args[0], &fd);
    if (fd <= 0) {
        LOGE("Invalid file descriptor");
        napi_throw_error(env, nullptr, "Invalid file descriptor");
        return nullptr;
    }
    
    unsigned char readbuffer[MAX_SIZE] = {0};
    size_t len = MAX_SIZE;
    napi_value result;
    tcflush(fd, TCIFLUSH);
    ret = read(fd, readbuffer, len);
    LOGI("Read %{public}d byte(s)", ret);
    if (ret > 0) 
    {
        napi_value arrayBuffer;
        void* bufferPtr = nullptr;
        napi_create_arraybuffer(env, ret, &bufferPtr, &arrayBuffer);
        napi_create_typedarray(env, napi_uint8_array, ret, arrayBuffer, 0, &result);
        unsigned char *readData = (unsigned char *) bufferPtr;
        memcpy(readData, readbuffer, ret);
    } 
    else 
    {
        ret = 1;
        napi_value arrayBuffer;
        void *bufferPtr = nullptr;
        napi_create_arraybuffer(env, ret, &bufferPtr, &arrayBuffer);
        napi_create_typedarray(env, napi_uint8_array, ret, arrayBuffer, 0, &result);
        unsigned char *readData = (unsigned char *)bufferPtr;
        memcpy(readData, readbuffer, ret);
    }
    return result;
}

static napi_value CloseSerial(napi_env env, napi_callback_info info)
// CloseSerial(env, [fd: number]) => closed: number
{
//     LOGI("connected to function CloseSerial in serial.cpp");
    napi_status status;
    int fd = -1;
    int ret = -1;
    size_t argc = 1;
    napi_value args[1] = {nullptr};

    // 获取传递给write函数的参数
    status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    napi_valuetype valuetype;
    status = napi_typeof(env, args[0], &valuetype);
    if (valuetype != napi_number) {
        LOGE("Wrong arguments");
        napi_throw_type_error(env, NULL, "Wrong arguments");
        return nullptr;
    }
    napi_get_value_int32(env, args[0], &fd);
    if (fd <= 0) {
        LOGE("Invalid file descriptor");
        napi_throw_error(env, nullptr, "Invalid file descriptor");
        return nullptr;
    }
    
    ret = close(fd);
    
    napi_value closed;
    napi_create_int32(env, ret, &closed);
    
    return closed;
}

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        {"OpenSerial", nullptr, OpenSerial, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"WriteSerial", nullptr, WriteSerial, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"ReadSerial", nullptr, ReadSerial, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"CloseSerial", nullptr, CloseSerial, nullptr, nullptr, nullptr, napi_default, nullptr},
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}
EXTERN_C_END

static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "serialport",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};

extern "C" __attribute__((constructor)) void RegisterEntryModule(void)
{
    napi_module_register(&demoModule);
}
