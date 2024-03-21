//
// Created by sebastian-f on 3/1/24.
//
#define _XOPEN_SOURCE 700
#include "mlpt.h"
#include "config.h"
#include "util.h"
#include "test.h"

#ifndef DEBUG
#define DEBUG 0
#endif

#define LEVEL(n) ((POBITS-3) * (LEVELS - n - 1) + POBITS)

void setup_page_tables(size_t* page_tables[LEVELS]){
    for (int i = 0; i < LEVELS; i += 1){
        void* ptr;
        int memalign_error = posix_memalign( &ptr, PAGE_BYTES,  ENTRY_AMOUNT * sizeof (size_t));
        page_tables[i] = (size_t*) ptr; //an array of size ENTRY_AMOUNT
        if (memalign_error != 0){
            printf("Memalign Error with Input %d. Quitting", i);
            _exit(1);
        }
    }
}

size_t manually_allocate(size_t* page_tables[LEVELS], size_t * vpn_segments, const size_t PHYSICAL_ADDRESS, size_t offset){
    if (*page_tables == 0){
        printf("Error: Page tables must be set up before allocating!");
        _exit(1);
    }
    size_t address = 0;

    if (DEBUG) {
        for (int i = 0; i < LEVELS; i++) {
            char n[15];
            snprintf(n, 15, "Table Addr %d", i);
            printBits(n, (size_t *) &page_tables[i]);
        }
    }
    for (int i = 0; i < LEVELS; i++) {
        address += ((size_t) vpn_segments[i] << LEVEL(i));
        if (DEBUG) {
            char n[10];
            snprintf(n, 10, "Input %d", i);
            printBits(n, &vpn_segments[i]);
            printBits("Val", &address);
        }
        if (i + 1 < LEVELS) { //vpn to vpn
            page_tables[i][vpn_segments[i]] = (size_t) &page_tables[i + 1][0] + 0b1;
        }
        else{ //physical address
            page_tables[i][vpn_segments[i]] = (size_t) PHYSICAL_ADDRESS + 0b1; //we don't need to shift it pobits because it is aligned
        }
        if (DEBUG) {
            char n[10];
            snprintf(n, 10, "Table %d", i);
            printBits(n, &page_tables[i][vpn_segments[i]]);
        }
    }
    address += offset;
    if(DEBUG) {
        printBits("Offset", &offset);
    }
    return address;
}

void test(int test_version) {
    srandom(time(NULL));
    size_t vpn_segments[LEVELS];
    for (int i = 0; i < LEVELS; i += 1) {
        vpn_segments[i] = random() % ENTRY_AMOUNT;
    }
    size_t* page_tables[LEVELS];
    int successful_tests = 0;
    for (int test = 0; test < TESTS; test++) {
        if (DEBUG) {
            printf("Begin test %d\n", test);
        }
        size_t PHYSICAL_ADDRESS = (random() % ENTRY_AMOUNT) << POBITS;
        size_t OFFSET = random() % (size_t) pow(2, POBITS);
        size_t address = 0;
        if(test_version == 0) {
            setup_page_tables(page_tables);
            address = manually_allocate(page_tables, vpn_segments, PHYSICAL_ADDRESS, OFFSET);
            ptbr = (size_t) &page_tables[0][0];
        }
        else if (test_version == 1){
            ptbr = 0;
            int memalign_error = posix_memalign((void**)&ptbr, PAGE_BYTES, PAGE_BYTES);
            if (memalign_error != 0) {
                printf("Memalign Error during allocate of main. Quitting");
                _exit(1);
            }
            address = 0;
            for (int i = 0; i < LEVELS; i += 1) {
                address += ((size_t) vpn_segments[i] << LEVEL(i));
            }
            address += OFFSET;
        }


        if (DEBUG) {
            printf("\n");
            printBits("Input", &address);
            printBits("Base register", &ptbr);
        }
        if(test_version == 0){
            size_t out = translate(address);
            if (DEBUG) {
                printBits("Output", &out);
            }
            size_t actual_address = PHYSICAL_ADDRESS + OFFSET;
            if (out == actual_address) {
                successful_tests += 1;
            }
            else {
                printf("%d/%d passed\n------FAILED------\n", successful_tests, TESTS);
                printBits("received address: ", &out);
                printBits("expected address: ", &actual_address);
                _exit(1);
            }
            for (int i = 0; i < LEVELS; i += 1) {
                free(page_tables[i]);
            }
        }else if(test_version == 1){
            size_t out;
            out = translate(address);
            if(out != (size_t) ~0){
                printf("%d/%d passed\n------FAILED------\n", successful_tests, TESTS);
                printBits("Instead of ~0, received address ", &out);
                _exit(1);
            }
            page_allocate(address);
            out = translate(address);
            if(out == (size_t) ~0){
                printf("%d/%d passed\n------FAILED------\n", successful_tests, TESTS);
                printBits("Received ~0 as output for address", &address);
                _exit(1);
            }
            if (DEBUG) {
                printf("\n");
                printBits("Physical address", &out);
            }
            successful_tests += 1;
            free((size_t*) ptbr);
            deallocate_mlpt_allocated_pages();
        }
        if (DEBUG) {
            printf("Ended test %d\n", test);
        }
    }
    if (DEBUG) {
        printf("\nPage Size: %d bytes\n", PAGE_BYTES);
    }
    printf("Ran Test #%d with %d Levels and %d page offset bits:\n\t%d/%d passed\n\n", test_version, LEVELS, POBITS, successful_tests, TESTS);
}


// suggested usage:
// int main(){
//     test(0);
//     test(1);
// }