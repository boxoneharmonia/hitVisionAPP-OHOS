#include "cpuUsage.h"
#include "var.h"
#include "log.h"
#include <cstdint>
#include <cstdio>
#include <unistd.h>
#include <thread>
#include <atomic>

using namespace std;

typedef struct {
    char name[20];
    uint64_t user;
    uint64_t nice;
    uint64_t system;
    uint64_t idle;
    uint64_t iowait;
    uint64_t irq;
    uint64_t softirq;
} CPU_OCCUPY;

static void get_cpuoccupy(CPU_OCCUPY *cpu) {
    FILE *fd = fopen("/proc/stat", "r");
    if (!fd)
        return;

    char buff[1024] = {0};
    fgets(buff, sizeof(buff), fd);
    sscanf(buff, "%s %llu %llu %llu %llu %llu %llu %llu", cpu->name, &cpu->user, &cpu->nice, &cpu->system, &cpu->idle,
           &cpu->iowait, &cpu->irq, &cpu->softirq);
    fclose(fd);
}

static double cal_cpuoccupy(const CPU_OCCUPY *prev, const CPU_OCCUPY *curr) {
    uint64_t prev_idle = prev->idle + prev->iowait;
    uint64_t curr_idle = curr->idle + curr->iowait;

    uint64_t prev_total =
        prev->user + prev->nice + prev->system + prev->idle + prev->iowait + prev->irq + prev->softirq;

    uint64_t curr_total =
        curr->user + curr->nice + curr->system + curr->idle + curr->iowait + curr->irq + curr->softirq;

    uint64_t delta_total = curr_total - prev_total;
    uint64_t delta_idle = curr_idle - prev_idle;

    if (delta_total == 0)
        return 0.0;
    return 100.0 * (delta_total - delta_idle) / delta_total;
}

static void SysCpuUsage(uint8_t &usage) {
    CPU_OCCUPY cpu1, cpu2;
    get_cpuoccupy(&cpu1);
    usleep(50000); // 50ms
    get_cpuoccupy(&cpu2);
    double cpu_percent = cal_cpuoccupy(&cpu1, &cpu2);
    usage = static_cast<uint8_t>(cpu_percent + 0.5); // 四舍五入
}

static thread getSysCpuUsageThread;
static atomic<bool> threadRunning(false);

static void SysCpuUsageLoop() {
    while (threadRunning) {
        SysCpuUsage(cpuUsageNow);
        sleep(2);
    }
}

void startGetUsage() {
    if (!threadRunning) {
        threadRunning = true;
        getSysCpuUsageThread = thread(SysCpuUsageLoop);
    } else {
        LOGW("GetUsage has started");
    }
}

void stopGetUsage() {
    if (threadRunning) {
        threadRunning = false;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (getSysCpuUsageThread.joinable()) getSysCpuUsageThread.join();
        LOGI("GetUsage stopped");
    } else {
        LOGW("GetUsage has stopped");
    }
}
