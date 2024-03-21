//
// Created by sebastian-f on 3/21/24.
//

#ifndef CSO2_PAGETABLES_TEST_H
#define CSO2_PAGETABLES_TEST_H
#include <math.h>
#include <stdlib.h>
#include <time.h>
/**
 * @brief Tests either translate or page_allocate
 * @details The test run is determined by the test_version.
 *  0 for translate,
 *  1 for page_allocate
 * @param test_version which test the function will run
 */
#define TESTS 4
// Enable DEBUG from config.h
void test(int test_version);
#endif //CSO2_PAGETABLES_TEST_H
