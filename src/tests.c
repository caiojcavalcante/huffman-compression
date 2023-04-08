#include <stdio.h>
#include <stdlib.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "structs.h"
#include "operations.h"

void test() {
    CU_ASSERT(1 == 1);
}

void test_heap() {
    int *p = malloc(100);
    CU_ASSERT(p != NULL);
    free(p);
}

int main() {
    CU_initialize_registry();               // Initialize the CUnit test registry
    CU_pSuite suite = CU_add_suite("Suite", NULL, NULL);  // Create a test suite named "Suite"
    CU_add_test(suite, "test", test);       // Add the test function to the test suite
    CU_add_test(suite, "heap pointer", test_heap);       // Add the test function to the test suite
    CU_basic_set_mode(CU_BRM_VERBOSE);      // Set the test runner to verbose mode
    CU_basic_run_tests();                   // Run the tests using the basic interface
    CU_cleanup_registry();                  // Clean up the CUnit test registry
    return 0;                               // Return 0 to indicate successful execution
}
