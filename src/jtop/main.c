#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <ncurses.h>
#include "cpu.h"
#include "mem.h"
#include "ui.h"

int main(void) {
    ui_init();

    get_cpu_usage(); // Warmup snapshot

    int running = 1;
    while (running) {
        int ch = getch();
        if (ch == 'q' || ch == 'Q') {
            running = 0;
            break;
        }

        float cpu = get_cpu_usage();
        MemStats mem;
        get_mem_stats(&mem);

        ui_render(cpu, &mem);

        usleep(1000000);
    }

    ui_cleanup();
    return 0;
}
