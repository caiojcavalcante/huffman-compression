#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "debug.h"
#include "operations.h"

#define MAX 256

void compress()
{
    FILE *input_file = fopen("texto.txt", "rb");
    int file_size = get_file_size(input_file);

    int *frequencies = count_frequencies(input_file, file_size);

    Heap *heap = create_heap();

    for (short byte = 0; byte < 256; byte++)
    {
        if (frequencies[byte] > 0)
        {
            insert(heap, frequencies[byte], create_tree(byte, NULL, NULL));
        }
    }

    Tree *left, *right;

    int left_priority, right_priority;

    while (heap->size > 1)
    {
        left_priority = heap->data[0].priority;
        left = pop(heap);

        right_priority = heap->data[0].priority;
        right = pop(heap);

        insert(
            heap,
            left_priority + right_priority,
            create_tree('*', left, right));
    }

    Tree *root = pop(heap);

    int max_code_size = get_tree_depth(root) + 1;

    unsigned char dict[MAX][max_code_size];
    unsigned char code[max_code_size];

    generate_huffman_codes(root, max_code_size, dict, code, 0);

    free(frequencies);
    free(heap->data);
    free(heap);

    // read file compressing

    FILE *output_file = fopen("OUTPUT.txt", "wb");

    int tree_size = 10;

    /*
        For performance reasons, we will write the tree
        part of the header first, then the data and then
        the trash and tree sizes.
    */

   int tree_size = inorder_traversal(root, output_file);

    int trash_size = encode_data(
        input_file,
        output_file,
        max_code_size,
        dict,
        file_size,
        tree_size);
}

int main()
{
    compress();

    return 0;
}