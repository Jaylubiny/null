#include "ui.h"
#include "utils.h"
#include "tui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void render_fs_table_cli(MountList *list) {
    if (!list) return;

    printf("\n%s %-18s %-8s %-10s %-10s %-10s %-22s %-20s%s\n",
           COLOR_BOLD, "Device", "Type", "Total", "Used", "Avail", "Usage", "Mount Point", COLOR_RESET);
    printf("────────────────────────────────────────────────────────────────────────────────────────────────────\n");

    for (size_t i = 0; i < list->count; ++i) {
        MountInfo *m = &list->mounts[i];
        char total_str[16], used_str[16], free_str[16], bar_str[64];

        format_human_size(m->total_bytes, total_str, sizeof(total_str));
        format_human_size(m->used_bytes, used_str, sizeof(used_str));
        format_human_size(m->free_bytes, free_str, sizeof(free_str));
        create_progress_bar(m->use_percent, 12, bar_str, sizeof(bar_str));

        const char *color = COLOR_GREEN;
        if (m->use_percent > 85.0) color = COLOR_RED;
        else if (m->use_percent > 70.0) color = COLOR_YELLOW;

        printf(" %-18.18s %-8.8s %-10s %-10s %-10s %s%-22s%s %-20s\n",
               m->device, m->fstype, total_str, used_str, free_str,
               color, bar_str, COLOR_RESET, m->mount_point);
    }
    printf("\n");
}

void run_interactive_tui(DirNode *root) {
    tui_enable_raw_mode();

    DirNode *current = root;
    int selected = 0;
    int scroll_offset = 0;

    while (1) {
        int rows, cols;
        tui_get_terminal_size(&rows, &cols);

        int visible_lines = rows - 4; // Top 2 header lines + bottom 2 footer/status lines
        if (visible_lines < 1) visible_lines = 1;

        // --- Viewport / Scroll Bounds Logic ---
        if (selected < 0) selected = 0;
        if (current->child_count > 0 && selected >= (int)current->child_count) {
            selected = (int)current->child_count - 1;
        }

        if (selected < scroll_offset) {
            scroll_offset = selected;
        } else if (selected >= scroll_offset + visible_lines) {
            scroll_offset = selected - visible_lines + 1;
        }

        tui_clear_screen();

        // --- Render Top Header Bar ---
        tui_move_cursor(1, 1);
        char total_sz_buf[32];
        format_human_size(current->size, total_sz_buf, sizeof(total_sz_buf));
        
        char header_buf[512];
        snprintf(header_buf, sizeof(header_buf), " cdu Explorer  |  Path: %s", current->path);
        printf("\x1b[44m\x1b[37m\x1b[1m%-*.*s\x1b[0m\x1b[K", cols, cols, header_buf);

        tui_move_cursor(2, 1);
        char subheader_buf[512];
        snprintf(subheader_buf, sizeof(subheader_buf), " Total: %s  |  Items: %llu  |  Position: %d/%zu",
                 total_sz_buf, (unsigned long long)current->items, 
                 current->child_count > 0 ? selected + 1 : 0, current->child_count);
        printf("\x1b[100m\x1b[37m%-*.*s\x1b[0m\x1b[K", cols, cols, subheader_buf);

        // --- Render File List Window ---
        for (int i = 0; i < visible_lines; ++i) {
            int item_idx = scroll_offset + i;
            tui_move_cursor(3 + i, 1);

            if (item_idx < (int)current->child_count) {
                DirNode *child = current->children[item_idx];
                
                char child_sz[16];
                format_human_size(child->size, child_sz, sizeof(child_sz));

                double pct = (current->size > 0) ? ((double)child->size / (double)current->size) * 100.0 : 0.0;
                char graph[64];
                create_progress_bar(pct, 10, graph, sizeof(graph));

                bool is_selected = (item_idx == selected);
                const char *type_tag = child->is_dir ? "[DIR] " : "      ";
                
                if (is_selected) {
                    printf("%s> %6.1f%% %-18s %10s  %s%-35.35s%s\x1b[K",
                           COLOR_REVERSE, pct, graph, child_sz, type_tag, child->name, COLOR_RESET);
                } else {
                    printf("  %6.1f%% %-18s %10s  %s%s%-35.35s%s\x1b[K",
                           pct, graph, child_sz,
                           child->is_dir ? COLOR_CYAN : COLOR_RESET,
                           type_tag, child->name, COLOR_RESET);
                }
            } else {
                // Clear unused screen rows to avoid gaps/leftovers
                printf("\x1b[K");
            }
        }

        // --- Render Bottom Navigation Footer ---
        tui_move_cursor(rows - 1, 1);
        printf("\x1b[90m────────────────────────────────────────────────────────────────────────────────────────────\x1b[0m\x1b[K");

        tui_move_cursor(rows, 1);
        const char *nav_help = " [▲/▼] Navigate   [ENTER] Open Dir   [BACKSPACE] Parent Dir   [q] Quit";
        printf("%s%-*.*s%s\x1b[K", COLOR_BOLD, cols, cols, nav_help, COLOR_RESET);
        
        fflush(stdout);

        // --- Input Handling ---
        KeyCode key = tui_read_key();
        if (key == KEY_QUIT) {
            break;
        } else if (key == KEY_DOWN) {
            if (selected < (int)current->child_count - 1) {
                selected++;
            }
        } else if (key == KEY_UP) {
            if (selected > 0) {
                selected--;
            }
        } else if (key == KEY_ENTER) {
            if (current->child_count > 0 && current->children[selected]->is_dir) {
                current = current->children[selected];
                selected = 0;
                scroll_offset = 0;
            }
        } else if (key == KEY_BACKSPACE) {
            if (current->parent) {
                current = current->parent;
                selected = 0;
                scroll_offset = 0;
            }
        }
    }

    tui_disable_raw_mode();
}
