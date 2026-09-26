#ifndef FILESYSTEM_H
#define FILESYSTEM_H

int setup_rootfs(const char *rootfs);
int setup_proc(void);
int setup_dev(void);
int setup_sys(void);

#endif
