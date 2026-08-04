#ifndef JTOP_UI_H
#define JTOP_UI_H

#include "mem.h"

void ui_init(void);
void ui_cleanup(void);
void ui_render(float cpu_usage, const MemStats *mem);

#endif
