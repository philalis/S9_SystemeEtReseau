# Infos

We inspired the functions main_getattr, main_readdir, ... from their equivalent in hello_II.c

## Errors we get
1. Before mounting  
The current code does not run properly.  
If you run it (with a god part of the code commented, the current state), it can mount a folder.
However, if you uncomment the functions used in `main_oper`, then a segfault appears right after mounting.  
The code can be runned without a segfault at the beginning with main_readdir and/or main_read.  
We couldn't find what was causing this problem.

2. Trying to use commands  
When the directory is mounted, most of the commands do not work and give `... :  Transport endpoint is not connected.`  
Somehow echo can still writes in the shell.