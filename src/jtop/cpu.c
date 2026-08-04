#include "cpu.h"
#include <stdio.h>

static CpuStats prev_stats = {0};

static int read_cpu_stats(CpuStats *s) {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) return -1;

    int ret = fscanf(fp, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
                     &s->user, &s->nice, &s->system, &s->idle,
                     &s->iowait, &s->irq, &s->softirq, &s->steal);
    fclose(fp);
    return ret == 8 ? 0 : -1;
}

float get_cpu_usage(void) {
    CpuStats curr;
    if (read_cpu_stats(&curr) != 0) return 0.0f;

    unsigned long long prev_idle = prev_stats.idle + prev_stats.iowait;
    unsigned long long curr_idle = curr.idle + curr.iowait;

    unsigned long long prev_non_idle = prev_stats.user + prev_stats.nice + prev_stats.system +
                                       prev_stats.irq + prev_stats.softirq + prev_stats.steal;
    unsigned long long curr_non_idle = curr.user + curr.nice + curr.system +
                                       curr.irq + curr.softirq + curr.steal;

    unsigned long long prev_total = prev_idle + prev_non_idle;
    unsigned long long curr_total = curr_idle + curr_non_idle;

    unsigned long long total_diff = curr_total - prev_total;
    unsigned long long idle_diff = curr_idle - prev_idle;

    prev_stats = curr;

    if (total_diff == 0) return 0.0f;
    return (float)(total_diff - idle_diff) / (float)total_diff * 100.0f;
}
