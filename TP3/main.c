//
// Created by justine on 01/10/2025.
//
#define FUSE_USE_VERSION 26

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "tosfs.h"
#include <fuse/fuse_lowlevel.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include "main.h"

struct stat file_stat;
struct tosfs_superblock *sb;
struct tosfs_inode *inodes;
void *addr;

static void main_getattr(fuse_req_t req, fuse_ino_t ino, struct fuse_file_info *fi __attribute__((unused))){
    struct stat stbuf = {0};
    if (ino == 0 || ino > sb->inodes) {
        fuse_reply_err(req, ENOENT);
        return;
    }
    stbuf.st_ino = ino;
    stbuf.st_uid = inodes[ino].uid;
    stbuf.st_gid = inodes[ino].gid;
    stbuf.st_mode = inodes[ino].mode;
    stbuf.st_nlink = inodes[ino].nlink;
    stbuf.st_size = inodes[ino].size;
    stbuf.st_blksize = sb->block_size;
    stbuf.st_blocks = inodes[ino].size / sb->block_size;
    fuse_reply_attr(req, &stbuf, 1.0);
}

static struct fuse_lowlevel_ops main_oper = {
    .getattr	= main_getattr,
};

int main(int argc, char *argv[]) {
    /*const char *filename = "/home/justine/CLionProjects/S9_SystemeEtReseau/TP3/Ressources FS-20251001/test_tosfs_files";
    int fd = open(filename, O_RDWR);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    if (fstat(fd, &file_stat) == -1) {
        perror("fstat");
        return 1;
    }
    size_t file_size = file_stat.st_size;

    addr = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("Error in memory mapping");
        close(fd);
        exit(EXIT_FAILURE);
    }

    sb = addr;

    printf("---- Superblock ----\n");
    printf("Magic number   : 0x%08x\n", sb->magic);
    printf("Block size     : %u\n", sb->block_size);
    printf("Block count    : %u\n", sb->blocks);
    printf("Inode count    : %u\n", sb->inodes);
    printf("Root inode num : %u\n", sb->root_inode);*/

    //----------------------------------------------------------------------------------

    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
    struct fuse_chan *ch;
    char *mountpoint;
    int err = -1;

    if (fuse_parse_cmdline(&args, &mountpoint, NULL, NULL) != -1 &&
        (ch = fuse_mount(mountpoint, &args)) != NULL) {
        struct fuse_session *se;

        se = fuse_lowlevel_new(&args, &main_oper,
                       sizeof(main_oper), NULL);
        if (se != NULL) {
            if (fuse_set_signal_handlers(se) != -1) {
                fuse_session_add_chan(se, ch);
                err = fuse_session_loop(se);
                fuse_remove_signal_handlers(se);
                fuse_session_remove_chan(ch);
            }
            fuse_session_destroy(se);
        }
        fuse_unmount(mountpoint, ch);
        }
    fuse_opt_free_args(&args);

    munmap(addr, file_stat.st_size);
    //close(fd);

    return err ? 1 : 0;
}
