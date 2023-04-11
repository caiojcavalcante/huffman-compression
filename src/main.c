#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "debug.h"
#include "operations.h"

#define MAX 256

void compress(FILE *input_file, FILE *output_file);
void decompress(FILE *input_file, FILE *output_file);

int main()
{
    int mode = 0;
    char input_file_name[100];
    char output_file_name[100];

    printf("Select mode:\n1-Compress\n0-Decompress\n");
    scanf("%d", &mode);

    printf("Enter the input file name(with extension):\n");
    scanf("%s", input_file_name);

    FILE *input_file = fopen(input_file_name, "rb");

    if (input_file == NULL)
    {
        printf("File not found.\n");
        return 1;
    }

    printf("Enter the output file name(with extension):\n");
    scanf("%s", output_file_name);

    FILE *output_file = fopen(output_file_name, "wb");

    if (output_file == NULL)
    {
        printf("Couldn't save to output file.\n");
        return 2;
    }

    if (mode)
        compress(input_file, output_file);
    else
        decompress(input_file, output_file);

    return 0;
}

/**
 * @brief Decompresses a file using the huffman algorithm.
 *
 * @param input_file The file to be decompressed.
 * @param output_file The file to save the decompressed data.
 */
void decompress(FILE *input_file, FILE *output_file)
{
    long file_size = get_file_size(input_file);

    unsigned char trash_size;
    unsigned short tree_size;

    read_header(input_file, &trash_size, &tree_size);

    decode_to_file(
        input_file,
        output_file,
        file_size,
        trash_size,
        tree_size);
}

/**
 * @brief Compresses a file using the huffman algorithm.
 *
 * @param input_file The file to be compressed.
 * @param output_file The file to save the compressed data.
 */
void compress(FILE *input_file, FILE *output_file)
{
    long file_size = get_file_size(input_file);

    printf("File size: %ld\n", file_size);

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

    printf("Heap size: %d\n", heap->size);

    int number_of_nodes = heap->size;

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

    printf("Max code size: %d\n", max_code_size);

    if (max_code_size < 12 && number_of_nodes > 250)
    {
        printf("The huffman tree is well balanced\nThe file might not shrink.\n");
    }

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

    /*
        For performance reasons, we will write the tree
        part of the header first, then the data and then
        the trash and tree sizes on the first two bytes.
    */

    fseek(output_file, 2, SEEK_SET); // skip 2 first bytes

    short tree_size = save_tree(
        root,
        output_file);
    printf("Tree size: %d\n", tree_size);

    short trash_size = save_data(
        input_file,
        output_file,
        max_code_size,
        dict,
        file_size,
        tree_size);
    printf("Trash size: %d\n", trash_size);

    save_header(
        output_file,
        trash_size,
        tree_size);

    free(root);
}
