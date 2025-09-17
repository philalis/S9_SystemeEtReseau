#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc,char ** argv){

  int fd = open(argv[1],O_RDWR);
  if (fd ==-1) {
    printf("Error reading file\n");
    return -1;
  }
  struct stat * stat_struct;
  int err = fstat(fd,stat_struct);
  if (err ==-1){
    perror("fstat");
    return -1;
  }
  printf("Size of test.txt = %ld\n",stat_struct->st_size);
  char * addr = mmap(NULL, stat_struct->st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
  close(fd);
  printf("%s",addr);
  fd =  open(argv[1],O_WRONLY);
  for (int i = stat_struct->st_size; i>=0;i--){
    write(fd,addr[i],1);
    printf("%c",addr[i]);
  }
  
  
}
