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

/*
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
*/
static void main_readdir(fuse_req_t req, fuse_ino_t ino, size_t size,
                        off_t off, struct fuse_file_info *fi){
    if (inodes[ino].mode != S_IFDIR) {
        fuse_reply_err(req, ENOTDIR);
        return;
    }

    struct tosfs_dentry *dentries = addr + inodes[ino].block_no * TOSFS_BLOCK_SIZE;

    char buffer[size];
    size_t total_size = 0;

    for (int i = 0; i < TOSFS_BLOCK_SIZE / sizeof(struct tosfs_dentry); i++) {
        struct tosfs_dentry *dentry = &dentries[i];

        if (dentry->inode == 0) {
            continue;
        }

        struct stat stbuf = {0};
        stbuf.st_ino = dentry->inode;

        size_t entry_size = fuse_add_direntry(req, buffer + total_size, size - total_size, dentry->name, &stbuf, total_size + 1);
        if (entry_size > size - total_size) {
            break;
        }
        total_size += entry_size;
    }

    fuse_reply_buf(req, buffer, total_size);
}
/*
static void main_lookup(fuse_req_t req, fuse_ino_t parent, const char *name) {
    struct fuse_entry_param e;

    if (inodes[parent].mode != S_IFDIR) {
        fuse_reply_err(req, ENOTDIR);
        return;
    }

    struct tosfs_dentry *dentries = (struct tosfs_dentry *) ((char*)addr + inodes[parent].block_no * TOSFS_BLOCK_SIZE);

    // Cross the dentries and find the file
    for (int i = 0; i < TOSFS_BLOCK_SIZE / sizeof(struct tosfs_dentry); i++) {
        if (strncmp(dentries[i].name, name, TOSFS_MAX_NAME_LENGTH) == 0) {
            // File found, send the entry back
            memset(&e, 0, sizeof(e));
            e.ino = dentries[i].inode;
            e.attr.st_ino = e.ino;
            e.attr.st_uid = inodes[e.ino].uid;
            e.attr.st_gid = inodes[e.ino].gid;
            e.attr.st_mode = inodes[e.ino].mode;
            e.attr.st_nlink = inodes[e.ino].nlink;
            e.attr.st_size = inodes[e.ino].size;
            e.attr.st_blksize = sb->block_size;
            e.attr.st_blocks = inodes[e.ino].size / sb->block_size;

            fuse_reply_entry(req, &e);
            return;
        }
    }

    fuse_reply_err(req, ENOENT);
}
*/
static void main_read(fuse_req_t req, fuse_ino_t ino, size_t size, off_t off, struct fuse_file_info *fi) {
    if (ino == 0 || ino > sb->inodes) {
        fuse_reply_err(req, ENOENT);
        return;
    }

    if ((inodes[ino].mode & S_IFMT) == S_IFDIR) {
        fuse_reply_err(req, EISDIR);
        return;
    }

    if (!(inodes[ino].perm & S_IRUSR)) {
        fuse_reply_err(req, EACCES);
        return;
    }

    // Compute the size to read
    if (off >= inodes[ino].size) {
        fuse_reply_buf(req, NULL, 0);
        return;
    }

    if (off + size > inodes[ino].size) {
        size = inodes[ino].size - off;
    }

    void *file_data_block = (void *)((char *)addr + TOSFS_BLOCK_SIZE * inodes[ino].block_no);

    fuse_reply_buf(req, (char *)file_data_block + off, size);
}

static struct fuse_lowlevel_ops main_oper = {
    // .getattr = main_getattr,
    .readdir = main_readdir,
    //.lookup = main_lookup,
    .read = main_read,
};

int main(int argc, char *argv[]) {
    const char *filename = "Ressources FS-20251001/test_tosfs_files";
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
    /*
    printf("---- Superblock ----\n");
    printf("Magic number   : 0x%08x\n", sb->magic);
    printf("Block size     : %u\n", sb->block_size);
    printf("Block count    : %u\n", sb->blocks);
    printf("Inode count    : %u\n", sb->inodes);
    printf("Root inode num : %u\n", sb->root_inode);*/

    //----------------------------------------------------------------------------------

    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
    struct fuse_chan *ch;
    char *mountpoint = NULL;
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
    close(fd);

    return err ? 1 : 0;
}
