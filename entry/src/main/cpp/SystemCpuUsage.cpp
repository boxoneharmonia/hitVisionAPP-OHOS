//
// Created on 2025/5/14.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "SystemCpuUsage.h"
#include "log.h"
#include "napi/native_api.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>

using namespace std;

// proc/stat文件结构
// cpu  633666 46912 249878 176813696 782884 2859 19625 0
// cpu0 633666 46912 249878 176813696 782884 2859 19625 0
// intr 5812844
// ctxt 265816063
// btime 1455203832
// processes 596625
// procs_running 1
// procs_blocked 0

typedef struct CPUPACKED // 定义一个cpu occupy的结构体
{
    char name[20];       // 定义一个char类型的数组名name有20个元素
    unsigned int user;   // 定义一个无符号的int类型的user
    unsigned int nice;   // 定义一个无符号的int类型的nice
    unsigned int system; // 定义一个无符号的int类型的system
    unsigned int idle;   // 定义一个无符号的int类型的idle
    unsigned int lowait;
    unsigned int irq;
    unsigned int softirq;
} CPU_OCCUPY;



int get_cpuoccupy(CPU_OCCUPY *cpust) // 对无类型get函数含有一个形参结构体类弄的指针O
{
    FILE *fd;
    char buff[256];
    CPU_OCCUPY *cpu_occupy;
    cpu_occupy = cpust;

    fd = fopen("/proc/stat", "r");
    fgets(buff, sizeof(buff), fd);

    sscanf(buff, "%s %u %u %u %u %u %u %u", cpu_occupy->name, &cpu_occupy->user, &cpu_occupy->nice, &cpu_occupy->system,
           &cpu_occupy->idle, &cpu_occupy->lowait, &cpu_occupy->irq, &cpu_occupy->softirq);


    fclose(fd);

    return 0;
}


double cal_cpuoccupy(CPU_OCCUPY *o, CPU_OCCUPY *n) {
    unsigned long od, nd;
    double cpu_use = 0;

    od = (unsigned long)(o->user + o->nice + o->system + o->idle + o->lowait + o->irq +
                         o->softirq); // 第一次(用户+优先级+系统+空闲)的时间再赋给od
    nd = (unsigned long)(n->user + n->nice + n->system + n->idle + n->lowait + n->irq +
                         n->softirq); // 第二次(用户+优先级+系统+空闲)的时间再赋给od
    double sum = nd - od;
    double idle = n->idle - o->idle;
    cpu_use = idle / sum;
    idle = n->user + n->system + n->nice - o->user - o->system - o->nice;
    cpu_use = idle / sum;
    return cpu_use;
}

static napi_value SysCpuUsage(napi_env env, napi_callback_info info)  //没有输入，所以没有napi_info
{
//     LOGI("connected to function 'SysCpuUsage' in SystemCpuUsage.cpp");
    CPU_OCCUPY cpu_stat1;
    CPU_OCCUPY cpu_stat2;
    double cpu_use;
    
    get_cpuoccupy((CPU_OCCUPY *)&cpu_stat1);
    
    usleep(100000);

    // 第二次获取cpu使用情况
    get_cpuoccupy((CPU_OCCUPY *)&cpu_stat2);
    // 计算cpu使用率
    cpu_use = cal_cpuoccupy((CPU_OCCUPY *)&cpu_stat1, (CPU_OCCUPY *)&cpu_stat2);
    
    napi_value result;
    napi_create_double(env, cpu_use, &result);
    return result;
}


EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports) {
    napi_property_descriptor desc[] = {
        {"SysCpuUsage", nullptr, SysCpuUsage, nullptr, nullptr, nullptr, napi_default, nullptr},
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
    .nm_modname = "SystemCpuUsage",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

extern "C" __attribute__((constructor)) void RegisterSystemCpuUsageModule(void) 
{ 
    napi_module_register(&demoModule);
    };
