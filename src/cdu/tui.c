#include "tui.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

static struct termios orig_termios;
static int raw_mode_enabled = 0;

void tui_disable_raw_mode(void) {
    if (raw_mode_enabled) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
        tui_show_cursor();
        raw_mode_enabled = 0;
    }
}

void tui_enable_raw_mode(void) {
    if (raw_mode_enabled) return;
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(tui_disable_raw_mode);

    struct termios raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    raw_mode_enabled = 1;
    tui_hide_cursor();
}

void tui_get_terminal_size(int *rows, int *cols) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        *rows = 24;
        *cols = 80;
    } else {
        *rows = ws.ws_row;
        *cols = ws.ws_col;
    }
}

void tui_clear_screen(void) {
    write(STDOUT_FILENO, "\x1b[2J\x1b[H", 7);
}

void tui_move_cursor(int row, int col) {
    char buf[32];
    int len = snprintf(buf, sizeof(buf), "\x1b[%d;%dH", row, col);
    write(STDOUT_FILENO, buf, len);
}

void tui_hide_cursor(void) {
    write(STDOUT_FILENO, "\x1b[?25l", 6);
}

void tui_show_cursor(void) {
    write(STDOUT_FILENO, "\x1b[?25h", 6);
}

KeyCode tui_read_key(void) {
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1) return KEY_NONE;

    if (c == 'q' || c == 'Q') return KEY_QUIT;
    if (c == '\n' || c == '\r') return KEY_ENTER;
    if (c == 127 || c == 8 || c == 'b') return KEY_BACKSPACE;

    if (c == '\x1b') {
        char seq[3];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return KEY_OTHER;
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return KEY_OTHER;

        if (seq[0] == '[') {
            switch (seq[1]) {
                case 'A': return KEY_UP;
                case 'B': return KEY_DOWN;
            }
        }
        return KEY_OTHER;
    }

    return KEY_OTHER;
}
