#ifndef CONTAINER_H
#define CONTAINER_H

struct container_config {
    char *rootfs;
    char *hostname;
    char **argv;

    unsigned long memory_limit;
    unsigned long cpu_percent;
    unsigned long pids_limit;

    int enable_network;
};

int run_container(struct container_config *config);

#endif
