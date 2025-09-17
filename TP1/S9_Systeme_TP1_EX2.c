#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc,char ** argv){

  int fd = open(argv[1],O_RDONLY);
  if (fd ==-1) {
    printf("Error reading file\n");
    return -1;
  }
  struct stat * stat_struct;
  int err = fstat(fd,stat_struct);
  if (err ==-1){
    perror("fstat");
  }
  else {
    printf("Size of test.txt = %ld\n",stat_struct->st_size);
  }
}
