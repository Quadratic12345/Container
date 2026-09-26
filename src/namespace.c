#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <sys/mount.h>
#include <unistd.h>

#include "namespace.h"

int setup_hostname(const char *hostname)
{
    if (sethostname(hostname, strlen(hostname)) == -1) {
        perror("sethostname");
        return -1;
    }

    return 0;
}

int setup_mount_namespace(void)
{
    if (mount(
            NULL,
            "/",
            NULL,
            MS_REC | MS_PRIVATE,
            NULL
        ) == -1)
    {
        perror("mount MS_PRIVATE");
        return -1;
    }

    return 0;
}
