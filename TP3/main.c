//
// Created by justine on 01/10/2025.
//

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "tosfs.h"

struct stat file_stat;
struct tosfs_superblock *sb;
void *addr;

int main(int argc, char *argv[]) {
    const char *filename = "/home/justine/CLionProjects/S9_SystemeEtReseau/TP3/Ressources FS-20251001/test_tosfs_files";
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
    printf("Root inode num : %u\n", sb->root_inode);

    munmap(addr, file_stat.st_size);
    close(fd);
    return 0;
}
