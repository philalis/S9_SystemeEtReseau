#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int Data =1;
int BSS =0;

int main(){
  char * Str = "This is a string";
  int * Heap;
  int Stack = 1;
  
  
  printf("Data location : %p\n",&Data);
  printf("BSS location : %p\n",&BSS);
  printf("Str location : %p\n",&Str);
  printf("Heap location : %p\n",&Heap);
  printf("Stack location : %p\n",&Stack);
  printf("Main location : %p\n",&main);
  printf("Libc location : %p\n",&printf);
  //printf("PID = %d\n",getpid());
  char * s_pid;
  sprintf(s_pid,"%d",getpid());
  
  execlp("/usr/bin/pmap", "-X",s_pid,NULL);

}
