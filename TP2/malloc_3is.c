//
// Created by vboxuser on 9/24/25.
//

#include "malloc_3is.h"

#define MAGIC_NUMBER 0x0123456789ABCDEFL


void * malloc_3is(long required_size) {

//Plan :
    // récupération de la liste chainée             : X
    // vérification des blocs disponibles
        // création d'un bloc si nécessaire
    // allocation d'un bloc
    void * To_return;

    struct HEADER_TAG * current_header = header_start;      // header start : static from .h
    struct HEADER_TAG * previous_header = NULL;
    while (current_header != NULL) {
        if (current_header->bloc_size > required_size+sizeof(long)) {   // TODO : test if allowed to compare NULL and int (on 1st call)
            To_return = current_header+sizeof(struct HEADER_TAG);
            previous_header->ptr_next = current_header->ptr_next;
            break;
        }
        previous_header = current_header;
        current_header = current_header->ptr_next;
    }

    if (current_header == NULL) {
        // create new block
        struct HEADER_TAG header;
        header.magic_number = MAGIC_NUMBER;
        header.bloc_size = required_size;
        header.ptr_next= NULL;

        To_return = sbrk(0);        // value to return is after the Header, before the magic number

        long * new_break = sbrk(sizeof(HEADER)+required_size+sizeof(long));     //points to long
        *new_break = MAGIC_NUMBER;      // is set as the following value in memory
    }


    // Here, a block of enough length exists.

    return To_return;
}
