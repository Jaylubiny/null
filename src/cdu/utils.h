#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdint.h>

#define COLOR_RESET   "\x1b[0m"
#define COLOR_BOLD    "\x1b[1m"
#define COLOR_REVERSE "\x1b[7m"
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_CYAN    "\x1b[36m"

void format_human_size(uint64_t bytes, char *buffer, size_t buf_size);
void create_progress_bar(double percentage, int width, char *buffer, size_t buf_size);

#endif /* UTILS_H */
