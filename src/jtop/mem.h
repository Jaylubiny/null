#ifndef JTOP_MEM_H
#define JTOP_MEM_H

typedef struct {
    unsigned long total_kb;
    unsigned long available_kb;
    unsigned long used_kb;
    float usage_percent;
} MemStats;

int get_mem_stats(MemStats *stats);

#endif
