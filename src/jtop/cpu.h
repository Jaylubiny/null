#ifndef JTOP_CPU_H
#define JTOP_CPU_H

typedef struct {
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
} CpuStats;

float get_cpu_usage(void);

#endif
