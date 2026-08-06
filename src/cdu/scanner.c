#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

static DirNode *create_node(const char *name, const char *path, bool is_dir, DirNode *parent) {
    DirNode *node = calloc(1, sizeof(DirNode));
    node->name = strdup(name);
    node->path = strdup(path);
    node->is_dir = is_dir;
    node->parent = parent;
    if (is_dir) {
        node->child_capacity = 8;
        node->children = malloc(sizeof(DirNode*) * node->child_capacity);
    }
    return node;
}

static void add_child(DirNode *parent, DirNode *child) {
    if (parent->child_count >= parent->child_capacity) {
        parent->child_capacity *= 2;
        parent->children = realloc(parent->children, sizeof(DirNode*) * parent->child_capacity);
    }
    parent->children[parent->child_count++] = child;
}

static int compare_nodes(const void *a, const void *b) {
    DirNode *nodeA = *(DirNode**)a;
    DirNode *nodeB = *(DirNode**)b;
    if (nodeB->size > nodeA->size) return 1;
    if (nodeB->size < nodeA->size) return -1;
    return 0;
}

DirNode *scan_directory_tree(const char *path, DirNode *parent) {
    struct stat st;
    if (lstat(path, &st) != 0) return NULL;

    const char *basename = strrchr(path, '/');
    basename = (basename && *(basename + 1) != '\0') ? basename + 1 : path;

    DirNode *node = create_node(basename, path, S_ISDIR(st.st_mode), parent);

    if (!S_ISDIR(st.st_mode)) {
        node->size = st.st_size;
        node->items = 1;
        return node;
    }

    DIR *dir = opendir(path);
    if (!dir) {
        node->size = st.st_size;
        return node;
    }

    struct dirent *entry;
    node->size = st.st_size;
    node->items = 1;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        char full_path[4096];
        snprintf(full_path, sizeof(full_path), "%s/%s", strcmp(path, "/") == 0 ? "" : path, entry->d_name);

        DirNode *child = scan_directory_tree(full_path, node);
        if (child) {
            add_child(node, child);
            node->size += child->size;
            node->items += child->items;
        }
    }

    closedir(dir);
    if (node->child_count > 0) {
        qsort(node->children, node->child_count, sizeof(DirNode*), compare_nodes);
    }
    return node;
}

void free_dir_node(DirNode *node) {
    if (!node) return;
    for (size_t i = 0; i < node->child_count; ++i) {
        free_dir_node(node->children[i]);
    }
    if (node->children) free(node->children);
    free(node->name);
    free(node->path);
    free(node);
}
