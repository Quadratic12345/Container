
#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<sched.h>
#include<signal.h>
#include<string.h>
#include<sys/wait.h>
#include<unistd.h>

#include"container.h"
#include"namespace.h"
#include"filesystem.h"
#include"cgroup.h"
#include"process.h"

#define STACK_SIZE (1024*1024)

static char child_stack[STACK_SIZE];
static int container_main(void *arg){
    struct container_config *config=arg;
    printf("Inside the container \n");

    //setting up the hostname here
    if(setup_hostname(config->hostname)==-1){
        return 1;
    }
    //mounting the namespace now
    if(setup_mount_namespace()==-1){
        return 1;
    }
    //the root filesystem
    if(setup_rootfs(config->rootfs)==-1){
        return 1;
    }
    //the procs
    if(setup_proc()==-1){
        return 1;
    }
    //the dev will be here
    if(setup_dev()==-1){
        return 1;
    }

    //the sys will be here
    if(setup_sys()==-1){
        return 1;
    }

    printf("The container is executing %s\n",config->argv[0]);

    execvp(config->argv[0],config->argv);
    return 1;

}

// now for running the container
int run_container(struct container_config *config){
    int flags=CLONE_NEWPID | CLONE_NEWUTS | CLONE_NEWNS |
        CLONE_NEWNET | SIGCHLD;

    char cgruop_name[128];
    snprintf(cgruop_name,sizeof(cgroup_name),"container-%d",getpid());

    struct cgroup_config cg_config={
        .memory_limit=config -> memory_limit,
        .cpu_percent=config->cpu_percent,
        .pids_limit=config->pids_limit
    };

    if(cgruop_create(cgruop_name,&cg_config)==-1){
        return 1;
    }

    //now we can be creating an isolating child

    pid_t pid=clone(container_main, child_stack+STACK_SIZE, flags, config);

    if(pid==-1){
        perror("clone");
        cgroup_destroy(cgroup_name);
        return 1;
    }

    printf("The conatiner running host PID = %d\n",pid);

    //adding another container process to the cgruop


    if(cgroup_add_process(cgroup_name, pid)==-1){
        kill(pid,SIGKILL);
        waitpid(pid,NULL,0);
        cgroup_destroy(cgruop_name);
        return 1;
    }

    //now wait for the container
    it status =wait_for_process(pid);
    //clean up the cgruop now
    if(cgroup_destroy(cgroup_name)==-1){
        fprintf(stderr, "was not able to remove the cgruop\n");
    }
    return status;
}
