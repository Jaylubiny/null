#include "mem.h"
#include <stdio.h>
#include <string.h>

int get_mem_stats(MemStats *stats) {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) return -1;

    char line[256];
    stats->total_kb = 0;
    stats->available_kb = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "MemTotal: %lu kB", &stats->total_kb) == 1) continue;
        if (sscanf(line, "MemAvailable: %lu kB", &stats->available_kb) == 1) continue;
    }
    fclose(fp);

    if (stats->total_kb == 0) return -1;

    stats->used_kb = stats->total_kb - stats->available_kb;
    stats->usage_percent = ((float)stats->used_kb / (float)stats->total_kb) * 100.0f;
    return 0;
}
