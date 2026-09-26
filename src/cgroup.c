#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "cgroup.h"

#define CGROUP_ROOT "/sys/fs/cgroup"
#define PATH_SIZE 1024


static int write_file(const char *path, const char *value)
{
    FILE *file = fopen(path, "w");

    if (file == NULL) {
        perror(path);
        return -1;
    }

    if (fprintf(file, "%s", value) < 0) {
        perror("fprintf");
        fclose(file);
        return -1;
    }

    fclose(file);

    return 0;
}


int cgroup_create(
    const char *name,
    struct cgroup_config *config
)
{
    char path[PATH_SIZE];

    int written = snprintf(
        path,
        sizeof(path),
        "%s/%s",
        CGROUP_ROOT,
        name
    );

    if (written < 0 || (size_t)written >= sizeof(path)) {
        fprintf(stderr, "Cgroup path is too long\n");
        return -1;
    }


    if (mkdir(path, 0755) == -1) {

        if (errno != EEXIST) {
            perror("mkdir cgroup");
            return -1;
        }
    }


    if (config->memory_limit > 0) {

        char file[PATH_SIZE];
        char value[64];

        written = snprintf(
            file,
            sizeof(file),
            "%s/memory.max",
            path
        );

        if (written < 0 || (size_t)written >= sizeof(file)) {
            fprintf(stderr, "Memory cgroup path is too long\n");
            return -1;
        }

        snprintf(
            value,
            sizeof(value),
            "%lu",
            config->memory_limit
        );

        if (write_file(file, value) == -1) {
            return -1;
        }
    }


    if (config->pids_limit > 0) {

        char file[PATH_SIZE];
        char value[64];

        written = snprintf(
            file,
            sizeof(file),
            "%s/pids.max",
            path
        );

        if (written < 0 || (size_t)written >= sizeof(file)) {
            fprintf(stderr, "PID cgroup path is too long\n");
            return -1;
        }

        snprintf(
            value,
            sizeof(value),
            "%lu",
            config->pids_limit
        );

        if (write_file(file, value) == -1) {
            return -1;
        }
    }


    if (config->cpu_percent > 0 &&
        config->cpu_percent <= 100) {

        char file[PATH_SIZE];
        char value[64];

        unsigned long quota =
            config->cpu_percent * 1000;

        written = snprintf(
            file,
            sizeof(file),
            "%s/cpu.max",
            path
        );

        if (written < 0 || (size_t)written >= sizeof(file)) {
            fprintf(stderr, "CPU cgroup path is too long\n");
            return -1;
        }

        snprintf(
            value,
            sizeof(value),
            "%lu 100000",
            quota
        );

        if (write_file(file, value) == -1) {
            return -1;
        }
    }

    return 0;
}


int cgroup_add_process(
    const char *name,
    int pid
)
{
    char path[PATH_SIZE];
    char value[64];

    int written = snprintf(
        path,
        sizeof(path),
        "%s/%s/cgroup.procs",
        CGROUP_ROOT,
        name
    );

    if (written < 0 || (size_t)written >= sizeof(path)) {
        fprintf(stderr, "Cgroup process path is too long\n");
        return -1;
    }

    snprintf(
        value,
        sizeof(value),
        "%d",
        pid
    );

    return write_file(path, value);
}


int cgroup_destroy(const char *name)
{
    char path[PATH_SIZE];

    int written = snprintf(
        path,
        sizeof(path),
        "%s/%s",
        CGROUP_ROOT,
        name
    );

    if (written < 0 || (size_t)written >= sizeof(path)) {
        fprintf(stderr, "Cgroup path is too long\n");
        return -1;
    }

    if (rmdir(path) == -1) {
        perror("rmdir cgroup");
        return -1;
    }

    return 0;
}
