#include <stdio.h>
#include <stdlib.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "structs.h"
#include "operations.h"

void test_heap() {
    Heap* heap = create_heap();
    CU_ASSERT(heap != NULL);
    CU_ASSERT(heap->data != NULL);
    CU_ASSERT(heap->size == 0);
    CU_ASSERT(heap->capacity == 256);

    insert(heap, 3, create_tree('M'));
    CU_ASSERT(heap->data[0].tree->data == 'M');
    CU_ASSERT(heap->data[0].priority == 3);

    insert(heap, 5, create_tree('A'));
    CU_ASSERT(heap->data[0].tree->data == 'A');
    CU_ASSERT(heap->data[0].priority == 5);
}

int main() {
    //flag : -lcunit
    CU_initialize_registry();               // Initialize the CUnit test registry
    CU_pSuite suite = CU_add_suite("Suite", NULL, NULL);  // Create a test suite named "Suite"
    CU_add_test(suite, "heap pointer", test_heap);       // Add the test function to the test suite
    CU_basic_set_mode(CU_BRM_VERBOSE);      // Set the test runner to verbose mode
    CU_basic_run_tests();                   // Run the tests using the basic interface
    CU_cleanup_registry();                  // Clean up the CUnit test registry
    return 0;                               // Return 0 to indicate successful execution
}
