#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include "fs.h"
#include "scanner.h"
#include "ui.h"

static void print_usage(const char *prog) {
    printf("Usage: %s [OPTIONS] [PATH]\n\n", prog);
    printf("Zero-dependency dual duf/ncdu disk analyzer.\n\n");
    printf("Options:\n");
    printf("  -a, --all         Show all filesystems (including pseudo/virtual)\n");
    printf("  -i, --interactive Open interactive directory analyzer\n");
    printf("  -h, --help        Show help\n");
}

int main(int argc, char *argv[]) {
    bool show_all = false;
    bool interactive = false;
    const char *target = ".";

    static struct option long_opts[] = {
        {"all",         no_argument, 0, 'a'},
        {"interactive", no_argument, 0, 'i'},
        {"help",        no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "aih", long_opts, NULL)) != -1) {
        switch (opt) {
            case 'a': show_all = true; break;
            case 'i': interactive = true; break;
            case 'h': print_usage(argv[0]); return 0;
            default: print_usage(argv[0]); return 1;
        }
    }

    if (optind < argc) {
        target = argv[optind];
        interactive = true;
    }

    if (interactive) {
        printf("Scanning %s...\n", target);
        DirNode *root = scan_directory_tree(target, NULL);
        if (!root) {
            fprintf(stderr, "Error scanning path: %s\n", target);
            return 1;
        }
        run_interactive_tui(root);
        free_dir_node(root);
    } else {
        MountList *mounts = get_mount_info(show_all);
        if (!mounts) return 1;
        render_fs_table_cli(mounts);
        free_mount_list(mounts);
    }

    return 0;
}
