
//
// Created by sebastian-f on 3/1/24.
#include "config.h"
#include "mlpt.h"
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#ifdef DEBUG
    #include "util.h"
#endif
#ifndef DEBUG
#define DEBUG 0
#endif
#if DEBUG == 0
void printBits(char const* label, size_t const * const ptr);
#endif
size_t ptbr = 0;
//Modified from https://stackoverflow.com/questions/12416639/how-to-create-a-mask-with-the-least-significant-bits-set-to-1-in-c
size_t gen_mask(const uint_fast8_t size) {
    const size_t src = (size_t) 1  << (size - 1);
    return (src - 1) ^ src;
}


typedef struct allocated_pages_struct{
    void** stack;
    int stack_top;
    int stack_size;
}
allocated_pages_struct;

#define NULLPTR 0

allocated_pages_struct allocated_pages = {NULLPTR, -1, 128};

void init_allocated_pages_struct(){
    allocated_pages.stack_size = 128;
    allocated_pages.stack_top = -1;
    allocated_pages.stack = NULLPTR;
}

typedef struct separated_va{
    size_t vpn_segments[LEVELS];
    size_t offset;
}
separated_va;

separated_va separate(size_t va){
    if (DEBUG) {
        printf("\n\tBegin separate\n");
    }
    separated_va sep_va;
    va &= gen_mask(RELEVANT_BITS);
    if (DEBUG) {
        printBits("Full Address", &va);
    }
    size_t vpn = va >> POBITS;
    sep_va.offset = va & gen_mask(POBITS);
    if (DEBUG) {
        printBits("Offset", &(sep_va.offset)); // printBits is not included in this file
    }
    size_t segment_mask = gen_mask(SEGMENT_SIZE);
    for (int i = 0; i < LEVELS; i += 1){
        int shift =  (LEVELS - i - 1) * SEGMENT_SIZE;
        sep_va.vpn_segments[i] = (vpn >> shift) & segment_mask;
        if (DEBUG) {
            char n[25];
            snprintf(n, 25, "Segment %d", i);
            printBits(n, sep_va.vpn_segments + i);
        }
    }
    return sep_va;
}

size_t translate(size_t va) {
    separated_va sep_va = separate(va);
    if (DEBUG) {
        printf("\n\tBegin translate\n");
    }
    size_t* page_address = (size_t*) ptbr; //interpret ptbr as a pointer
    for (int i  = 0; i < LEVELS; i += 1) {
        if (!page_address){ //if page address is invalid
            return ~0;
        }
        if (DEBUG) {
            char n[25];
            snprintf(n, 25, "Segment Level %d", i);
            printBits(n, &(sep_va.vpn_segments[i]));
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

void allocate(void** mem_ptr){
    int memalign_error = posix_memalign(&(*mem_ptr), PAGE_BYTES, PAGE_BYTES);
    if (memalign_error != 0) {
        printf("Memalign Error during ptr allocate. Quitting");
        _exit(1);
    }
    memset(*mem_ptr, 0, PAGE_BYTES);
    if (allocated_pages.stack == 0){
        init_allocated_pages_struct();
        allocated_pages.stack = malloc(allocated_pages.stack_size * sizeof(size_t)); //setup stack memory
    }
    if (allocated_pages.stack_top + 1 >= allocated_pages.stack_size){ //double the size of the stack
        allocated_pages.stack_size *= 2;
        void** tmp = realloc(allocated_pages.stack, allocated_pages.stack_size * sizeof(size_t));
        if (tmp == 0){
            printf("Reallocation Error during ptr reallocation. Quitting");
            _exit(1);
        }
        allocated_pages.stack = tmp;
    }
    allocated_pages.stack_top += 1;
    allocated_pages.stack[allocated_pages.stack_top] = *mem_ptr;
}

void page_allocate(size_t va){
    separated_va sep_va = separate(va);
    if (DEBUG) {
        printf("\n\tBegin page allocate\n");
    }
    void* mem_ptr = 0;
    if (ptbr == 0) {
        allocate(&mem_ptr);
        if (DEBUG) {
            printf("\tAllocated ptbr page\n");
        }
        ptbr = (size_t) mem_ptr;
    }
    size_t* page_address = (size_t*) ptbr; //interpret ptbr as a pointer
    size_t* pte_ptr; //stores the pointer to the last pte
    for (int i = 0; i < LEVELS; i += 1){
        if (DEBUG) {
            char n[25];
            snprintf(n, 25, "Segment Level %d", i);
            printBits(n, &(sep_va.vpn_segments[i]));
        }
        pte_ptr = page_address + sep_va.vpn_segments[i];
        if (DEBUG) {
            printBits("PTE value before write", pte_ptr);
            size_t* pte_ptr_address = (size_t*) &pte_ptr;
            printBits("PTE addr", pte_ptr_address);
        }
        if ((*pte_ptr & 1) == 0) { //is invalid pte
            mem_ptr = 0;
            allocate(&mem_ptr);
            if (DEBUG) {
                char n[30];
                snprintf(n, 30, "Allocated memory ptr for %d", i);
                printBits(n, (size_t*) &mem_ptr);
            }
            *pte_ptr = (size_t) mem_ptr + 0b1;
            page_address = (size_t*) mem_ptr;
            if (DEBUG) {
                printBits("PTE updated with", pte_ptr);
            }
        }
        else{
            page_address = (size_t*) (*pte_ptr & ~gen_mask(POBITS));
            if (DEBUG) {
                printBits("existing PTE accessed at", pte_ptr);
            }
        }
    }

}

void deallocate_mlpt_allocated_pages(){
    if (allocated_pages.stack == 0){
        if (DEBUG){
            printf("Warning: deallocated nothing (no allocated pages detected");
        }
        return;
    }
    for (int i = 0; i <= allocated_pages.stack_top; i += 1){
        free(allocated_pages.stack[i]);
    }
    free(allocated_pages.stack);
    init_allocated_pages_struct();
}