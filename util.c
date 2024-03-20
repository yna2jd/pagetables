//
// Created by sebastian-f on 3/1/24.
//
#include "util.h"
#include <stdbool.h>
void _printBits_internal(char const* label, size_t const * const ptr, bool only_value){
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    int on = 0;
    size_t size = 8 / sizeof (byte);
    for (i = size-1; i >= 0; i--) {
        if (!only_value || on) {
            printf(" ");
        }
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            if (byte && !on){
                on = 1;
            }else if(j == 0 && i == 0 && !on){ //if the last value is a 0
                printf("0");
                break;
            }
            if (on){
                printf("%u", byte);
            }
            else if (!only_value){
                printf(" ");
            }

        }
    }
    if (!only_value) {
        printf(" : %s ", label);
        printf(": 0x%04lx\n", *ptr);
    }
    fflush(0);
}

void printBits(char const* label, size_t const * const ptr) {
    //https://stackoverflow.com/questions/111928/is-there-a-printf-converter-to-print-in-binary-format
    _printBits_internal(label, ptr, false);

}

void putBits(size_t const* const ptr){
    _printBits_internal(NULL,ptr,true);
}