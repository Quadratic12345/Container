#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<sys/mount.h>
#include<sys/stat.h>
#include<unistd.h>

#include "filesystem.h"

//now for changing the root directory of the process
int setup_rootfs(const char *rootfs){
    if (chroot(rootfs)==-1){
        perror("chroot");
        return -1;
    }
    //now the directory can move to the new root
    if(chdir("/")==-1){
        perror("chdir");
        return -1;
    }
}

//now setting up the proc in here
int setup_proc(void){
    struct stat st;
    if(stat("/proc",&st)==-1){
        if(mkdir("/proc",0555)==-1){
            perror("mkdir /proc");
            return -1;
        }
    }
    if (mount("proc","/proc","proc",MS_NOSUID | MS_NOEXEC | MS_NODEV,NULL)==-1){
        perror("mount /proc");
        return -1;
    }
    return 0;
}
//dev now

int setup_dev(void){
    struct stat st;
    if(stat("/dev",&st)==-1){
        if(mkdir("/dev",0755)==-1){
            perror("mkdir /dev");
            return -1;
        }
    }

    if (mount("tmpfs","/dev","tmpfs",MS_NOSUID|MS_STRICTATIME,"mode=755")==-1){
        perror("mount /dev");
        return -1;
    }
    return 0;
}
//sys files
int setup_sys(void){
    struct stat st;
    if(stat("/sys",&st)==-1){
        if(mkdir("/sys",0555)==-1){
            perror("mkdir /sys");
            return -1;
        }
    }
    return 0;
}
