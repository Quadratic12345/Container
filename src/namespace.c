#define _GNU_SOURCE
#include<sched.h>
#include<stdio.h>
#include<string.h>
#include<sys/mount.h>// as it is also mount namespace
#include<unistd.h>
#include "namespace.h" // the namespace header file is here

//now to setup the hostname
int setup_hostname(const char *hostname){
    if(sethostname(hostname,strlen(hostname))==-1){
        perror("sethostname");
        return -1;
    }
    return 0;
}
//now to make the mounts private for namespaces of the container and hostname
int setup_mount_namespace(void){
    if(mount(NULL,"/",NULL,MS_REC | MS_PRIVATE,NULL)==-1){
        perror("mounting");
        return -1;
    }
    return 0;
}
