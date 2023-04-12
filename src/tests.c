#include <stdio.h>
#include <stdlib.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "structs.h"
#include "operations.h"

void test_tree()
{
    Tree *tree = create_tree('A', NULL, NULL);
    CU_ASSERT(tree != NULL);
    CU_ASSERT(tree->data == 'A');
    CU_ASSERT(tree->left == NULL);
    CU_ASSERT(tree->right == NULL);
    tree->right = create_tree('B', NULL, NULL);
    CU_ASSERT(tree->right != NULL);
    CU_ASSERT(tree->right->data == 'B');
    CU_ASSERT(tree->left == NULL);
    CU_ASSERT(tree->right->left == NULL);
    CU_ASSERT(tree->right->right == NULL);

    free_tree(tree);

    // Test getting depth of a leaf node
    Tree *leaf = create_tree('A', NULL, NULL);
    int depth = get_tree_depth(leaf);
    CU_ASSERT_EQUAL(depth, 1);
    free(leaf);

    // Test getting depth of an internal node
    Tree *left = create_tree('B', NULL, NULL);
    Tree *right = create_tree('C', NULL, NULL);
    Tree *root = create_tree('D', left, right);
    depth = get_tree_depth(root);
    CU_ASSERT_EQUAL(depth, 2);
    free(left);
    free(right);
    free(root);

    // Test getting depth of a deeper tree
    root = create_tree('A',
        create_tree('B',
         create_tree('C',
          create_tree('D',
           create_tree('E',
             create_tree('F', NULL, NULL),
           NULL),
          NULL),
         NULL),
        NULL),
       NULL);

    depth = get_tree_depth(root);
    CU_ASSERT_EQUAL(depth, 6);
    Tree* aux = root;
    while(aux->left != NULL){
        aux = aux->left;
        free(root);
        root = aux;
    }
}

void test_heap()
{
    Heap *heap = create_heap();
    CU_ASSERT(heap != NULL);
    CU_ASSERT(heap->data != NULL);
    CU_ASSERT(heap->size == 0);
    CU_ASSERT(heap->capacity == 257);

    insert(heap, 3, create_tree('M', NULL, NULL));
    CU_ASSERT(heap->size == 1);
    CU_ASSERT(heap->capacity == 257);
    CU_ASSERT(heap->data[0].tree->data == 'M');
    CU_ASSERT(heap->data[0].priority == 3);

    insert(heap, 5, create_tree('A', NULL, NULL));
    CU_ASSERT(heap->size == 2);
    CU_ASSERT(heap->capacity == 257);
    CU_ASSERT(heap->data[0].tree->data == 'M');
    CU_ASSERT(heap->data[0].priority == 3);
    CU_ASSERT(heap->data[1].tree->data == 'A');
    CU_ASSERT(heap->data[1].priority == 5);

    insert(heap, 1, create_tree('B', NULL, NULL));
    CU_ASSERT(heap->data[0].tree->data == 'B');
    CU_ASSERT(heap->data[0].priority == 1);

    while (heap->size > 0)
    {
        CU_ASSERT(heap->data[0].tree == pop(heap));
    }
}

int main()
{
    // flag : -lcunit
    CU_initialize_registry();                            // Initialize the CUnit test registry
    CU_pSuite suite = CU_add_suite("Suite", NULL, NULL); // Create a test suite named "Suite"
    CU_add_test(suite, "Heap test", test_heap);          // Add the test function to the test suite
    CU_add_test(suite, "Tree test", test_tree);          // Add the test function to the test suite
    CU_basic_set_mode(CU_BRM_VERBOSE);                   // Set the test runner to verbose mode
    CU_basic_run_tests();                                // Run the tests using the basic interface
    CU_cleanup_registry();                               // Clean up the CUnit test registry
    return 0;                                            // Return 0 to indicate successful execution
}
