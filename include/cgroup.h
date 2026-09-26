#ifndef CGROUP_H
#define CGROUP_H

struct cgroup_config {
    unsigned long memory_limit;
    unsigned long cpu_percent;
    unsigned long pids_limit;
};

int cgroup_create(
    const char *name,
    struct cgroup_config *config
);

int cgroup_add_process(
    const char *name,
    int pid
);

int cgroup_destroy(
    const char *name
);

#endif
