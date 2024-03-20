#define _XOPEN_SOURCE 700
//
// Created by sebastian-f on 3/1/24.
#include "config.h"
#include "mlpt.h"


#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#ifdef DEBUG
    #include "util.h"
#endif
#ifndef DEBUG
#define DEBUG 0
void printBits(char const* label, size_t const * const ptr);
#endif
size_t ptbr = 0;
//Modified from https://stackoverflow.com/questions/12416639/how-to-create-a-mask-with-the-least-significant-bits-set-to-1-in-c
size_t gen_mask(const uint_fast8_t size) {
    const size_t src = (size_t) 1  << (size - 1);
    return (src - 1) ^ src;
}

#define PTE_SIZE 8
#define PAGE_SIZE (int) pow(2, POBITS)
#define ENTRY_AMT PAGE_SIZE / PTE_SIZE
#define LOG2_PTE_SIZE 3
// the size of each vpn segment is determined by taking log2 of 2^POBITS over the PTE_SIZE (2^3),
// thus, log2(2^(POBITS-LOG2_PTE_SIZE))
#define SEGMENT_SIZE (POBITS - LOG2_PTE_SIZE)
#define RELEVANT_BITS (SEGMENT_SIZE * LEVELS + POBITS)

typedef struct separated_va{
    size_t vpn_segments[LEVELS];
    size_t offset;
} separated_va;

separated_va separate(size_t va){
    va &= gen_mask(RELEVANT_BITS);
    if (DEBUG) {
        printBits("Full Address", &va);
    }
    size_t vpn = va >> POBITS;
    size_t offset = va & gen_mask(POBITS);
    if (DEBUG) {
        printBits("Offset", &offset);
    }
    size_t vpn_segments[LEVELS];
    size_t segment_mask = gen_mask(SEGMENT_SIZE);
    for (int i = 0; i < LEVELS; i += 1){
        int shift =  (LEVELS - i - 1) * SEGMENT_SIZE;
        vpn_segments[i] = (vpn >> shift) & segment_mask;
        if (DEBUG) {
            char n[10];
            snprintf(n, 10, "Segment %d", i);
            printBits(n, vpn_segments + i);
        }
    }
}

size_t translate(size_t va) {
    separated_va sep_va = separate(va);
    size_t* page_address = (size_t*) ptbr; //interpret ptbr as a pointer
    for (int i  = 0; i < LEVELS; i += 1) {
        if (!page_address){ //if page address is invalid
            return ~0;
        }
        if (DEBUG) {
            printBits("segment", &(sep_va.vpn_segments[i]));
        }
        size_t pte = page_address[sep_va.vpn_segments[i]];
        if (DEBUG) {
            printBits("PTE", &pte);
        }
        if (pte & 1) { //if the entry is valid, wipe away lower bits
            page_address = (size_t*) (pte >> POBITS << POBITS);
            if (DEBUG) {
                size_t addr = (size_t) page_address;
                printBits("Page Address", &addr);
            }
        }
        else {
            return ~0;
        }
    }
    return (size_t) page_address + sep_va.offset;
}

void page_allocate(size_t va){
    separated_va sep_va = separate(va);
    size_t* page_address = (size_t*) ptbr; //interpret ptbr as a pointer
    
}