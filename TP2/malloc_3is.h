//
// Created by vboxuser on 9/24/25.
//

#ifndef TP2_MALLOC_3IS_H
#define TP2_MALLOC_3IS_H

#include <unistd.h>
#include "HEADER.h"

static struct HEADER_TAG * header_start;


void * malloc_3is(long required_size);

static struct HEADER_TAG * get_header_start() {
    return header_start;
}


#endif //TP2_MALLOC_3IS_H