#ifndef UI_H
#define UI_H

#include "fs.h"
#include "scanner.h"

void render_fs_table_cli(MountList *list);
void run_interactive_tui(DirNode *root);

#endif /* UI_H */
