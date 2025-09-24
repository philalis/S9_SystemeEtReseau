//
// Created by vboxuser on 9/24/25.
//

#ifndef TP2_HEADER_H
#define TP2_HEADER_H

#include <stddef.h>
static void * start = NULL;

typedef struct HEADER_TAG {
    struct HEADER_TAG * ptr_next; /* pointe sur le prochain bloc libre */
    size_t bloc_size; /* taille du memory bloc en octets*/
    long magic_number; /* 0x0123456789ABCDEFL */
} HEADER;
#endif //TP2_HEADER_H