//
// Created by Ludw on 4/19/2024.
//

#include "util.h"

char check_pointer(void *ptr) {
    char *char_ptr = (char *) ptr;

    printf("pointer address: %p\n", char_ptr);
    printf("value at address: %d\n", *char_ptr);
    return 1;
}