#include<stdio.h>
#include<sys/wait.h>
#include<process.h>

//it works for the process to finish and gets to know how was the process
int wait_for_process(int pid){
    int status;
    if(waitpid(pid,&status,0)==-1){
        perror("waitpid");
        return -1;
    }
    if(WIFEXITED(status)){
        return WEXITSTATUS(status);
    }
    if(WIFSIGNALED(status)){
        fprintf(stderr,"container is stopped by the signal %d",WTERMSIG(status));
    }

return 1;
}
