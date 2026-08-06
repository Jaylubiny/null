#ifndef TUI_H
#define TUI_H

typedef enum {
    KEY_NONE = 0,
    KEY_UP,
    KEY_DOWN,
    KEY_ENTER,
    KEY_BACKSPACE,
    KEY_QUIT,
    KEY_OTHER
} KeyCode;

void tui_enable_raw_mode(void);
void tui_disable_raw_mode(void);
void tui_get_terminal_size(int *rows, int *cols);
void tui_clear_screen(void);
void tui_move_cursor(int row, int col);
void tui_hide_cursor(void);
void tui_show_cursor(void);
KeyCode tui_read_key(void);

#endif /* TUI_H */
