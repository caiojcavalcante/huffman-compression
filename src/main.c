#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "debug.h"
#include "operations.h"

#define MAX 256

void compress()
{
    char input_file_name[100];
    scanf("%s", input_file_name);

    FILE *input_file = fopen(input_file_name, "rb");

    if(input_file == NULL)
    {
        printf("File not found.\n");
        return;
    }

    long file_size = get_file_size(input_file);

    printf("file size: %ld\n", file_size);

    int *frequencies = count_frequencies(input_file, file_size); 

    Heap *heap = create_heap();

    int i = 0;

    for (short byte = 0; byte < 256; byte++)
    {
        if (frequencies[byte] > 0)
        {
            i++;
            insert(heap, frequencies[byte], create_tree(byte, NULL, NULL));
        }
    }

    printf("rodei: %d\n", i);

    Tree *left, *right;

    int left_priority, right_priority;

    printf("Heap size: %d\n", heap->size);

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
    printf("underflow?\n");
    Tree *root = pop(heap);

    int max_code_size = get_tree_depth(root) + 1;

    unsigned char dict[MAX][max_code_size];
    unsigned char code[max_code_size];

    generate_huffman_codes(
        root,
        max_code_size,
        dict,
        code,
        0);

    free(frequencies);
    free(heap->data);
    free(heap);

    FILE *output_file = fopen("a.huff", "wb");

    /*
        For performance reasons, we will write the tree
        part of the header first, then the data and then
        the trash and tree sizes.
    */

    fseek(output_file, 2, SEEK_SET);

    short tree_size = save_tree(
        root,
        output_file);

    short trash_size = save_data(
        input_file,
        output_file,
        max_code_size,
        dict,
        file_size,
        tree_size);

    save_header(
        output_file,
        trash_size,
        tree_size);
    
    free(root);
}

int main()
{
    compress();

    return 0;
}