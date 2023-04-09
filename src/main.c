#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "operations.h"
#include "debug.h"

/**
 * @brief does the compressing
 */
void compress()
{
    unsigned char data[100] = "abcd";

    int *frequencies = count_frequencies(data, strlen(data));

    Heap *heap = create_heap();

    int teste = 0;

    for (short byte = 0; byte < 256; byte++)
    {
        if (frequencies[byte] > 0)
        {
            insert(heap, frequencies[byte], create_tree(byte, NULL, NULL));
        }
    }
    print_heap(heap);

    Tree *left;
    Tree *right;

    int left_priority;
    int right_priority;

    while (heap->size > 1)
    {
        left_priority = heap->data[0].priority;
        left = pop(heap);

        right_priority = heap->data[0].priority;
        right = pop(heap);

        // printf("merging '%c: %d' with '%c: %d'\n", left->data, left_priority, right->data, right_priority);

        insert(heap, left_priority + right_priority, create_tree('*', left, right));

        // print_heap(heap);
    }

    Tree *root = pop(heap);

    print_tree(root, 0);

    int tree_depth = get_tree_depth(root);

    printf("Tree depth: %d\n", tree_depth);

    int *codes[256];
}

int main()
{
    compress();

    return 0;
}