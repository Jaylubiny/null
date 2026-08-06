#include "fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mntent.h>
#include <sys/statvfs.h>

static bool is_pseudo_fs(const char *fstype) {
    const char *pseudo_types[] = {
        "proc", "sysfs", "devtmpfs", "devpts", "tmpfs", "securityfs",
        "cgroup", "cgroup2", "pstore", "efivarfs", "bpf", "configfs",
        "debugfs", "tracefs", "hugetlbfs", "mqueue", "autofs", NULL
    };
    for (int i = 0; pseudo_types[i] != NULL; ++i) {
        if (strcmp(fstype, pseudo_types[i]) == 0) return true;
    }
    return false;
}

MountList *get_mount_info(bool show_all) {
    FILE *fp = setmntent("/etc/mtab", "r");
    if (!fp) fp = setmntent("/proc/mounts", "r");
    if (!fp) return NULL;

    MountList *list = malloc(sizeof(MountList));
    list->capacity = 16;
    list->count = 0;
    list->mounts = malloc(sizeof(MountInfo) * list->capacity);

    struct mntent *entry;
    struct statvfs stat;

    while ((entry = getmntent(fp)) != NULL) {
        bool pseudo = is_pseudo_fs(entry->mnt_type);
        if (!show_all && pseudo) continue;

        if (statvfs(entry->mnt_dir, &stat) == 0) {
            if (stat.f_blocks == 0 && !show_all) continue;

            if (list->count >= list->capacity) {
                list->capacity *= 2;
                list->mounts = realloc(list->mounts, sizeof(MountInfo) * list->capacity);
            }

            MountInfo *info = &list->mounts[list->count];
            strncpy(info->device, entry->mnt_fsname, sizeof(info->device) - 1);
            strncpy(info->mount_point, entry->mnt_dir, sizeof(info->mount_point) - 1);
            strncpy(info->fstype, entry->mnt_type, sizeof(info->fstype) - 1);

            uint64_t block_size = stat.f_frsize ? stat.f_frsize : stat.f_bsize;
            info->total_bytes = stat.f_blocks * block_size;
            info->free_bytes = stat.f_bavail * block_size;
            info->used_bytes = info->total_bytes - (stat.f_bfree * block_size);
            info->use_percent = (info->total_bytes > 0)
                ? ((double)info->used_bytes / (double)info->total_bytes) * 100.0
                : 0.0;
            info->is_pseudo = pseudo;
            list->count++;
        }
    }

    endmntent(fp);
    return list;
}

void free_mount_list(MountList *list) {
    if (!list) return;
    free(list->mounts);
    free(list);
}
