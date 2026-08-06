#ifndef FS_H
#define FS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct {
    char device[256];
    char mount_point[256];
    char fstype[64];
    uint64_t total_bytes;
    uint64_t used_bytes;
    uint64_t free_bytes;
    double use_percent;
    bool is_pseudo;
} MountInfo;

typedef struct {
    MountInfo *mounts;
    size_t count;
    size_t capacity;
} MountList;

MountList *get_mount_info(bool show_all);
void free_mount_list(MountList *list);

#endif /* FS_H */
