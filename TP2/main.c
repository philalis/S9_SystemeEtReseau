#include <stdio.h>
#include <unistd.h>

#define MAGIC_NUMBER 0x0123456789ABCDEFL

typedef struct HEADER_TAG {
    struct HEADER_TAG * ptr_next;
    size_t bloc_size;
    long magic_number;
} HEADER;

static struct HEADER_TAG free_list;



void* malloc_3is(size_t size) {
    HEADER *prev = NULL;
    HEADER *curr = &free_list;
    while (curr != NULL) {
        if (curr->bloc_size >= size) {
            prev->ptr_next = curr->ptr_next;
            curr->ptr_next = NULL;
            curr->magic_number = MAGIC_NUMBER;

            return (void*)(curr + sizeof(struct HEADER_TAG));
        }
        prev = curr;
        curr = curr->ptr_next;
    }

    void *request = sbrk(size + sizeof(HEADER));
    if (request == NULL) {
        perror("sbrk");
    }
    curr = (HEADER*)request;
    curr->bloc_size = size;
    curr->ptr_next = NULL;
    curr->magic_number = MAGIC_NUMBER;
    long * magic_location = sbrk(size);
    *magic_location = MAGIC_NUMBER;

    return (void*)(curr + sizeof(struct HEADER_TAG));
}

void free_3is(void *ptr) {
    if (ptr == NULL)return;
    HEADER *block = (HEADER*)ptr-sizeof(struct HEADER_TAG);

    if (block->magic_number != MAGIC_NUMBER) {
        printf("[ERROR] MEMORY CORRUPTED DETECTED\n");
        return;
    }

    struct HEADER_TAG * current_free_in_list = &free_list;
    struct HEADER_TAG * previous_free_in_list = NULL;
    while (current_free_in_list->ptr_next != NULL) {
        if (current_free_in_list->bloc_size > block->bloc_size) {
            previous_free_in_list->ptr_next = block;
            block->ptr_next = current_free_in_list;
            return;
        }
        previous_free_in_list = current_free_in_list;
        current_free_in_list = current_free_in_list->ptr_next;
    }
    //rajouter à la fin
    current_free_in_list->ptr_next = block;
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