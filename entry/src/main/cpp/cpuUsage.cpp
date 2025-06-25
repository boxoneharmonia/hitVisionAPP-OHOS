//
// Created on 2025/5/14.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#include "log.h"
#include "cpuUsage.h"
#include <opencl-c-base.h>
#include <stdio.h>
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

typedef struct CPUPACKED {
    char name[20];       // 定义一个char类型的数组名name有20个元素
    unsigned int user;   // 定义一个无符号的int类型的user
    unsigned int nice;   // 定义一个无符号的int类型的nice
    unsigned int system; // 定义一个无符号的int类型的system
    unsigned int idle;   // 定义一个无符号的int类型的idle
    unsigned int lowait;
    unsigned int irq;
    unsigned int softirq;
} CPU_OCCUPY;

void get_cpuoccupy(CPU_OCCUPY *cpust) {
    FILE *fd;
    char buff[1024];
    CPU_OCCUPY *cpu_occupy;
    cpu_occupy = cpust;
    fd = fopen("/proc/stat", "r");
    fgets(buff, sizeof(buff), fd);
    sscanf(buff, "%s %u %u %u %u %u %u %u", cpu_occupy->name, &cpu_occupy->user, &cpu_occupy->nice, &cpu_occupy->system,
           &cpu_occupy->idle, &cpu_occupy->lowait, &cpu_occupy->irq, &cpu_occupy->softirq);
    fclose(fd);
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

void SysCpuUsage(uint8_t &usage) {
    CPU_OCCUPY cpu_stat1;
    CPU_OCCUPY cpu_stat2;
    double cpu_use;
    get_cpuoccupy((CPU_OCCUPY *)&cpu_stat1);
    usleep(10000);
    get_cpuoccupy((CPU_OCCUPY *)&cpu_stat2);
    cpu_use = cal_cpuoccupy((CPU_OCCUPY *)&cpu_stat1, (CPU_OCCUPY *)&cpu_stat2);
    usage = static_cast<uint8_t>(cpu_use);
}

