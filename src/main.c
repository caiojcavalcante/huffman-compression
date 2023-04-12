#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "debug.h"
#include "operations.h"

#define MAX 256

void compress(FILE *input_file, FILE *output_file);
void decompress(FILE *input_file, FILE *output_file);

/**
 * @brief The main function.
 *
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return int The exit code.
 */
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Usage: %s <mode> <input file> <output file>\n", argv[0]);
        return 1;
    }

    FILE *input_file = fopen(argv[2], "rb");
    FILE *output_file = fopen(argv[3], "wb");

    if (input_file == NULL)
    {
        printf("File not found.\n");
        return 2;
    }
    if (output_file == NULL)
    {
        printf("Couldn't save to output file.\n");
        return 3;
    }

    if (argv[1][1] == 'c')
        compress(input_file, output_file);

    else if (argv[1][1] == 'd')
        decompress(input_file, output_file);

    else
    {
        printf("Invalid mode.\n");
        return 4;
    }

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

    printf("Heap size: %d\n", heap->size);

    int number_of_nodes = heap->size;

    Tree *root = mount_huffman_tree(heap);

    free(frequencies);
    free(heap->data);
    free(heap);

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
