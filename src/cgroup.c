
#include<errno.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<unistd.h>
#include<cgroup.h>

#define CGROUP_ROOT "/sys/fs/cgroup"

//write file will give the value to the control files of the cgroup
static int write_file(const char *path,const char  *value){
    FILE *file=fopen(path,"w");
    if(!file){
        perror(path);
        return -1;
    }
    if (fprintf(file,"%s",value)<0){
        perror("fprintf");
        fclose(file);
        return -1;
    }
    fclose(file);
    return 0;
}

//now for creating the cgruop
int cgroup_create(const char *name, struct cgroup_config *config){
    char path[512];
    //this is for the path
    snprintf(path,sizeof(path),"%s%s",CGROUP_ROOT,name);

    if(mkdir(path,0755)==-1){
        if(errno!=EEXIST){
            perror("mkdir cgroup error");
            return -1;
        }
    }

    //now for the memory part
    if((config->memory_limit) >0){
        char file[512];
        char value[64];

        snprintf(file,sizeof(file),"%s/memory.max",path);

    }
    if(write_file(file,value)==-1){
        return -1;
    }

//now comes the PID parts
if((config->pids_limit)>0){
    char file[512];
    char value[64];
    snprintf(file,sizeof(file),"%s/pids.max",path);
    snprintf(value,sizeof(value),"%lu",config->pids_limit);

    if(write_file(file,value)==-1){
        return -1;
    }

}
//now for the CPU
if((config->cpu_percent)>0 && (config->cpu_percent)<=100){
    char file[512];
    char value[64];
    unsigned long quota=config->cpu_percent*1000;
    snprintf(value,sizeof(value),"%lu 100000",quota);

    if(write_file(file,value)!=-1){
        return -1;
    }

}
return 0;
}

//now adding the process to cgroup
int cgroup_add_process(
    const char *name, int pid
)
{
    char path[512];
    char value[64];
    snprintf(path,sizeof(path),"%s/%s/cgroup.procs",CGROUP_ROOT,name);

    snprintf(value,sizeof(value),"%d",pid);
    return write_file(path,value);

}

//now for destroying cgroup
int cgroup_destroy(const char *name){
    char path[512];
    snprintf(path,sizeof(path),"%s/%s",CGROUP_ROOT,name);
    if(rmdir(path)==-1){
        perror("rmdir cgroup");
        return -1;
    }
    return 0;
}
