#include <stdio.h>
#include <unistd.h>

#define MAGIC_NUMBER 0x0123456789ABCDEFL

typedef struct HEADER_TAG {
    struct HEADER_TAG * ptr_next;
    size_t bloc_size;
    long magic_number;
} HEADER;

static HEADER * free_list;


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

int main() {
// 1. allocations multiples
    printf("Allocation of 50 octets\n");
    int * var = sbrk(0);
    char *p = (char*)malloc_3is(40);
    int * var2 = sbrk(0);
    char *p2 = (char*)malloc_3is(20);

    sprintf(p, "Hello Allocator\n");
    printf("%s\n",p);
    printf("memory before : %p\n", var);
    printf("memory inter  : %p\n",var2);        //memory grew by 64 bits
    printf("memory after  : %p\n",sbrk(0));// memory grew by 44 bits
    //-> the size seems right, the header + magic_number are 24 bits long.


    // 2. liberation block
    printf("Bloc Liberation\n");
    free_3is(p);

// 4. Réutilisation de block existant.
    printf("Re-allocation of 30 octets\n");
    var = sbrk(0);
    char *q = (char*)malloc_3is(30);
    printf("memory before : %p\n",var);
    printf("memory after  : %p\n",sbrk(0)); // memory stays the same, since the block is re-used

    sprintf(q, "Hello Allocator Again !\n");
    printf("%s",q);
    free_3is(q);

//3 vérification débordement

         return 0;
}