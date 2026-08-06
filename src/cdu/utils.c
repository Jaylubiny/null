#include "utils.h"
#include <stdio.h>
#include <string.h>

void format_human_size(uint64_t bytes, char *buffer, size_t buf_size) {
    const char *units[] = {"B", "KiB", "MiB", "GiB", "TiB", "PiB"};
    double size = (double)bytes;
    int unit_idx = 0;

    while (size >= 1024.0 && unit_idx < 5) {
        size /= 1024.0;
        unit_idx++;
    }

    if (unit_idx == 0) {
        snprintf(buffer, buf_size, "%llu B", (unsigned long long)bytes);
    } else {
        snprintf(buffer, buf_size, "%.2f %s", size, units[unit_idx]);
    }
}

void create_progress_bar(double percentage, int width, char *buffer, size_t buf_size) {
    if (percentage < 0.0) percentage = 0.0;
    if (percentage > 100.0) percentage = 100.0;

    int filled = (int)((percentage / 100.0) * width);
    if (filled > width) filled = width;

    size_t pos = 0;
    pos += snprintf(buffer + pos, buf_size - pos, "[");

    for (int i = 0; i < width; ++i) {
        if (i < filled) {
            pos += snprintf(buffer + pos, buf_size - pos, "#");
        } else {
            pos += snprintf(buffer + pos, buf_size - pos, ".");
        }
    }

    snprintf(buffer + pos, buf_size - pos, "]");
}
