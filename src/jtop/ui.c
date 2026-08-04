#include "ui.h"
#include <ncurses.h>

void ui_init(void) {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, TRUE);
    start_color();
    
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_CYAN, COLOR_BLACK);
}

void ui_cleanup(void) {
    endwin();
}

static void draw_bar(int y, int x, int width, float percent) {
    int filled = (int)((width * percent) / 100.0f);
    int color = percent < 50.0f ? 1 : (percent < 80.0f ? 2 : 3);

    attron(COLOR_PAIR(color));
    mvprintw(y, x, "[");
    for (int i = 0; i < width; i++) {
        if (i < filled) addch('|');
        else addch(' ');
    }
    printw("] %5.1f%%", percent);
    attroff(COLOR_PAIR(color));
}

void ui_render(float cpu_usage, const MemStats *mem) {
    erase();

    attron(COLOR_PAIR(4) | A_BOLD);
    mvprintw(1, 2, "=== jtop - System Monitor ===");
    attroff(COLOR_PAIR(4) | A_BOLD);

    mvprintw(3, 2, "CPU Usage:");
    draw_bar(3, 13, 30, cpu_usage);

    mvprintw(5, 2, "RAM Usage:");
    draw_bar(5, 13, 30, mem->usage_percent);
    mvprintw(6, 13, "(%.2f GB / %.2f GB)",
             (float)mem->used_kb / 1024.0f / 1024.0f,
             (float)mem->total_kb / 1024.0f / 1024.0f);

    mvprintw(9, 2, "Press 'q' to exit.");
    refresh();
}
