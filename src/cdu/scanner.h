#ifndef SCANNER_H
#define SCANNER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct DirNode {
    char *name;
    char *path;
    uint64_t size;
    uint64_t items;
    bool is_dir;
    struct DirNode *parent;
    struct DirNode **children;
    size_t child_count;
    size_t child_capacity;
} DirNode;

DirNode *scan_directory_tree(const char *path, DirNode *parent);
void free_dir_node(DirNode *node);

#endif /* SCANNER_H */
