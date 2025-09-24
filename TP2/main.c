#include <stdio.h>
#include <unistd.h>

#define MAGIC_NUMBER 0x0123456789ABCDEFL

typedef struct HEADER_TAG {
    struct HEADER_TAG * ptr_next;
    size_t bloc_size;
    long magic_number;
} HEADER;

static HEADER *free_list = NULL;

void* malloc_3is(size_t size) {
    HEADER *prev = NULL;
    HEADER *curr = free_list;
    while (curr != NULL) {
        if (curr->bloc_size >= size) {
            if (prev) prev->ptr_next = curr->ptr_next;
            else free_list = curr->ptr_next;

            curr->ptr_next = NULL;
            curr->magic_number = MAGIC_NUMBER;

            return (void*)(curr + 1);
        }
    }

    void *request = sbrk(size + sizeof(HEADER));
    if (request == NULL) {
        perror("sbrk");
    }
    curr = (HEADER*)request;
    curr->bloc_size = size;
    curr->ptr_next = NULL;
    curr->magic_number = MAGIC_NUMBER;

    return (void*)(curr + 1);
}

void free_3is(void *ptr) {
    if (ptr == NULL)return;
    HEADER *block = (HEADER*)ptr-1;

    if (block->magic_number != MAGIC_NUMBER) {
        printf("[ERROR] MEMORY CORRUPTED DETECTED\n");
        return;
    }

    block->ptr_next = free_list;
    free_list = block;
}
// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {

    printf("Allocation of 50 octets\n");
    void * var = sbrk(0);
    char *p = (char*)malloc_3is(10);
    void * var2 = sbrk(0);
    char *p2 = (char*)malloc_3is(100);



    sprintf(p, "Hello Allocator");
    printf("%s\n",p);
    printf("memory before : %p", var);
    printf("memory inter  : %p",var2);
    printf("memory after  : %p",sbrk(0));


    printf("Bloc Liberation");
    free_3is(p);

    printf("Re-allocation of 30 octets");
    var = sbrk(0);
    char *q = (char*)malloc_3is(30);
    printf("memory before : ",var);
    printf("memory after  : ",sbrk(0));

    sprintf(q, "Hello Allocator Again !");
    printf("%s",q);

    free_3is(q);
    return 0;
}