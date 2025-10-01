//
// Created by justine on 01/10/2025.
//

#ifndef S9_SYSTEMEETRESEAU_MAIN_H
#define S9_SYSTEMEETRESEAU_MAIN_H


#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fuse/fuse_lowlevel.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

void getattr(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi);

#endif //S9_SYSTEMEETRESEAU_MAIN_H