#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "container.h"

static unsigned long parse_memory(const char *value)
{
    char *end;

    unsigned long number =
        strtoul(value, &end, 10);

    if (*end == '\0') {
        return number;
    }

    if (strcmp(end, "M") == 0 ||
        strcmp(end, "MB") == 0)
    {
        return number * 1024UL * 1024UL;
    }

    if (strcmp(end, "G") == 0 ||
        strcmp(end, "GB") == 0)
    {
        return number *
               1024UL *
               1024UL *
               1024UL;
    }

    fprintf(
        stderr,
        "Invalid memory value: %s\n",
        value
    );

    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc < 4) {

        fprintf(
            stderr,
            "Usage:\n"
            "  %s run [options] ROOTFS COMMAND [ARGS...]\n\n"
            "Options:\n"
            "  --memory SIZE\n"
            "  --cpu PERCENT\n"
            "  --pids NUMBER\n"
            "  --hostname NAME\n",
            argv[0]
        );

        return 1;
    }

    if (strcmp(argv[1], "run") != 0) {

        fprintf(
            stderr,
            "Unknown command: %s\n",
            argv[1]
        );

        return 1;
    }

    struct container_config config = {
        .rootfs = NULL,
        .hostname = "cbox",
        .argv = NULL,
        .memory_limit = 0,
        .cpu_percent = 0,
        .pids_limit = 0,
        .enable_network = 1
    };

    int i = 2;

    while (i < argc) {

        if (strcmp(argv[i], "--memory") == 0) {

            if (i + 1 >= argc) {
                return 1;
            }

            config.memory_limit =
                parse_memory(argv[i + 1]);

            i += 2;
        }

        else if (strcmp(argv[i], "--cpu") == 0) {

            if (i + 1 >= argc) {
                return 1;
            }

            config.cpu_percent =
                strtoul(
                    argv[i + 1],
                    NULL,
                    10
                );

            i += 2;
        }

        else if (strcmp(argv[i], "--pids") == 0) {

            if (i + 1 >= argc) {
                return 1;
            }

            config.pids_limit =
                strtoul(
                    argv[i + 1],
                    NULL,
                    10
                );

            i += 2;
        }

        else if (
            strcmp(argv[i], "--hostname") == 0
        ) {

            if (i + 1 >= argc) {
                return 1;
            }

            config.hostname = argv[i + 1];

            i += 2;
        }

        else {
            break;
        }
    }

    if (i >= argc) {
        fprintf(stderr, "Missing rootfs\n");
        return 1;
    }

    config.rootfs = argv[i++];

    if (i >= argc) {
        fprintf(stderr, "Missing command\n");
        return 1;
    }

    config.argv = &argv[i];

    return run_container(&config);
}
