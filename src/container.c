#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "container.h"
#include "namespace.h"
#include "filesystem.h"
#include "cgroup.h"
#include "process.h"

#define STACK_SIZE (1024 * 1024)

static char child_stack[STACK_SIZE];


static int container_main(void *arg)
{
    struct container_config *config = arg;

    printf("CBox: inside container\n");

    if (setup_hostname(config->hostname) == -1) {
        return 1;
    }

    if (setup_mount_namespace() == -1) {
        return 1;
    }

    if (setup_rootfs(config->rootfs) == -1) {
        return 1;
    }

    if (setup_proc() == -1) {
        return 1;
    }

    if (setup_dev() == -1) {
        return 1;
    }

    if (setup_sys() == -1) {
        return 1;
    }

    printf("the container is executing %s\n", config->argv[0]);

    execvp(config->argv[0], config->argv);

    perror("execvp");

    return 1;
}


int run_container(struct container_config *config)
{
    int flags =
        CLONE_NEWPID |
        CLONE_NEWUTS |
        CLONE_NEWNS |
        CLONE_NEWNET |
        SIGCHLD;

    /*
     * Create a unique cgroup name.
     */
    char cgroup_name[128];

    snprintf(
        cgroup_name,
        sizeof(cgroup_name),
        "container-%d",
        getpid()
    );


    /*
     * Configure resource limits.
     */
    struct cgroup_config cg_config = {
        .memory_limit = config->memory_limit,
        .cpu_percent = config->cpu_percent,
        .pids_limit = config->pids_limit
    };


    /*
     * Create the cgroup.
     */
    if (cgroup_create(cgroup_name, &cg_config) == -1) {
        return 1;
    }


    /*
     * Create the isolated container process.
     */
    pid_t pid = clone(
        container_main,
        child_stack + STACK_SIZE,
        flags,
        config
    );

    if (pid == -1) {
        perror("clone");

        cgroup_destroy(cgroup_name);

        return 1;
    }


    printf(
        "the container host PID = %d\n",
        pid
    );


    /*
     * Add container process to cgroup.
     */
    if (cgroup_add_process(cgroup_name, pid) == -1) {

        kill(pid, SIGKILL);

        waitpid(pid, NULL, 0);

        cgroup_destroy(cgroup_name);

        return 1;
    }


    /*
     * Wait for the container to finish.
     */
    int status = wait_for_process(pid);


    /*
     * Remove the cgroup.
     */
    if (cgroup_destroy(cgroup_name) == -1) {

        fprintf(
            stderr,
            "Warning: could not remove cgroup\n"
        );
    }


    return status;
}
