//
// Created by sebastian-f on 2/29/24.
//
#ifndef CSO2_MLPT_H
#define CSO2_MLPT_H
#include <unistd.h>
/**
 * Page table base register.
 * Declared here so tester code can look at it; because it is extern
 * you'll need to define it (without extern) in exactly one .c file.
 */
extern size_t ptbr;
/**
 * Given a virtual address, return the physical address.
 * Return a value consisting of all 1 bits
 * if this virtual address does not have a physical address.
 */
size_t translate(size_t va);

/**
 * Use posix_memalign to create page tables and other pages sufficient
 * to have a mapping between the given virtual address and some physical address.
 * If there already is such a page, does nothing.
 */
void page_allocate(size_t va);

void deallocate_mlpt_allocated_pages();

/** ################################
 *  # DO NOT EDIT THE VALUES BELOW #
 *  ################################*/

/** PTE_SIZE = a constant of 8 bytes */
#define PTE_SIZE 8 //page table entries are 8 bytes. This is constant. Defined at 6.3.4
#define LOG2_PTE_SIZE 3
/** Segment size: the size of each vpn segment is determined by taking log2 of 2^POBITS over the PTE_SIZE (2^3),
 * thus, log2(2^(POBITS-LOG2_PTE_SIZE))
*/
#define SEGMENT_SIZE (POBITS - LOG2_PTE_SIZE) // see 2.4 of deliverable
/** RELEVANT_BITS = the amount of of bits that have meaningful data based on POBITS and LEVELS */
#define RELEVANT_BITS (SEGMENT_SIZE * LEVELS + POBITS) // see 2.4 of deliverable
/** ENTRY_AMOUNT = the amount of page table entries per page table. Equivalent to 2^(POBITS - LOG2_PTE_SIZE) */
#define ENTRY_AMOUNT (0x1 << SEGMENT_SIZE)
/** PAGE_BYTES = the PTE_SIZE * ENTRY_AMOUNT aka 2^POBITS */
#define PAGE_BYTES (0x1 << POBITS)

#endif //CSO2_MLPT_H
