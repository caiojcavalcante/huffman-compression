#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "debug.h"
#define true 1
#define false 0
#define MAX 257
#define BIT_SIZE 9
#define DEBUG if (0)
#define LEFT '0'
#define RIGHT '1'
#define ENDL printf("\n")
#define LINE printf("linha %d\n", __LINE__);

int *count_frequency(byte *data, int data_size)
{
    int *freq = (int *)malloc(sizeof(int) * MAX);
    memset(freq, 0, sizeof(int) * MAX);

    for (int i = 0; i < data_size; i++)
    {
        freq[data[i]]++;
    }

    return freq;
}

node_t *create_node(byte data, int freq)
{
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    new_node->data = data;
    new_node->freq = freq;
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

void enqueue(pq_node_t **pq, node_t *data)
{
    int priority = data->freq;
    pq_node_t *new_node = (pq_node_t *)malloc(sizeof(pq_node_t));
    new_node->data = data;
    new_node->priority = priority;
    new_node->next = NULL;

    if (*pq == NULL)
    {
        *pq = new_node;
        return;
    }

    if ((*pq)->priority > priority)
    {
        new_node->next = *pq;
        *pq = new_node;
        return;
    }

    pq_node_t *current = *pq;

    while (current->next != NULL && current->next->priority <= priority)
    {
        current = current->next;
    }

    new_node->next = current->next;
    current->next = new_node;
}

int initialize_queue(pq_node_t **pq, int *freq, int *queue_size)
{
    for (int i = 0; i < MAX; i++)
    {
        if (freq[i] > 0)
        {
            node_t *new_node = create_node(i, freq[i]);
            enqueue(pq, new_node);
            (*queue_size)++;
        }
    }
}

node_t *dequeue(pq_node_t **pq)
{
    if (*pq == NULL)
    {
        // queue is empty
        return NULL;
    }

    pq_node_t *temp = *pq;
    *pq = (*pq)->next;
    node_t *data = temp->data;
    free(temp);
    return data;
}

void generate_huffman_tree(pq_node_t **pq, int queue_size)
{
    while (queue_size > 1)
    {
        node_t *left = dequeue(pq);
        node_t *right = dequeue(pq);
        node_t *new_node = create_node('*', left->freq + right->freq);
        new_node->left = left;
        new_node->right = right;
        enqueue(pq, new_node);
        queue_size--;
    }
}

int max_(int a, int b) // max is a macro in windows.h, but I use bash
{
    return (a > b) ? a : b;
}

int get_tree_depth(const node_t *root)
{
    if (root == NULL)
        return 0;
    return 1 + max_(get_tree_depth(root->left), get_tree_depth(root->right));
}

void generate_huffman_codes(const node_t *root, char dict[MAX][BIT_SIZE], char code[BIT_SIZE], int code_size)
{
    if (root == NULL)
        return;

    if (root->left == NULL && root->right == NULL)
    {
        // we have a leaf node
        code[code_size] = '\0';         // null terminate the string
        strcpy(dict[root->data], code); // copy the code to the dictionary
        DEBUG printf("%c: %s\n", root->data, dict[root->data]);
        return;
    }
    // traverse left subtree
    code[code_size] = '0';
    generate_huffman_codes(root->left, dict, code, code_size + 1);

    // traverse right subtree
    code[code_size] = '1';
    generate_huffman_codes(root->right, dict, code, code_size + 1);
}

int get_file_size(char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        return -10;
    }
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    fclose(fp);
    return size;
}

void write(char *ending_filename, byte *data, int encoded_data_size, short tree_size, short trash_size, char tree[])
{
    FILE *fp = fopen(ending_filename, "wb");
    if (fp == NULL)
    {
        printf("Error opening file '%s'\n", ending_filename);
        return;
    }
    LINE;
    // 3 bits for trash, 13 bits for tree size, tree, data
    byte buffer;

    tree_size <<= 3;
    tree_size >>= 3;
    trash_size <<= 13;
    LINE;

    short header = trash_size | tree_size;

    LINE;
    buffer = header >> 8;
    fwrite(&buffer, sizeof(byte), 1, fp);
    LINE;

    buffer = header & 0xFF;
    fwrite(&buffer, sizeof(byte), 1, fp);

    fwrite(tree, sizeof(char), tree_size, fp);

    fwrite(data, sizeof(byte), encoded_data_size, fp);
    fclose(fp);
    LINE;
}

node_t *generate_huffman_tree_from_inorder(char *tree, short *index)
{
    node_t *node;

    int is_leaf = false;
    if (tree[*index] == '\\')
    {
        is_leaf = true;
        (*index)++; // skip the escape character
    }

    char data = tree[*index];
    node = create_node(data, 1);
    (*index)++;

    if (!is_leaf && data == '*')
    {
        node->left = generate_huffman_tree_from_inorder(tree, index);
        node->right = generate_huffman_tree_from_inorder(tree, index);
    }
    return node;
}

void decode(node_t *root, byte encoded_data[], int encoded_data_size, short trash_size, char *ending_filename)
{
    FILE *fp = fopen(ending_filename, "wb");
    if (fp == NULL)
    {
        printf("Error opening file '%s'\n", ending_filename);
        return;
    }

    node_t *current = root;
    int current_bit = 0;
    byte current_byte = 0;
    int num_processed_bits = 0;

    for (int i = 0; i < encoded_data_size; i++)
    {
        current_byte = encoded_data[i];
        for (int j = 0; j < 8; j++)
        {
            if (num_processed_bits < trash_size)
            {
                num_processed_bits++;
                continue;
            }

            if (current_byte & 0x80)
            {
                current = current->right;
            }
            else
            {
                current = current->left;
            }

            if (current->left == NULL && current->right == NULL)
            {
                fwrite(&current->data, sizeof(char), 1, fp);
                current = root;
            }

            current_byte <<= 1;
            num_processed_bits++;
        }
    }

    fclose(fp);
}

void encode_data(byte data[], char dict[MAX][BIT_SIZE], int original_data_size, byte encoded_data[], int *encoded_data_size, int *trash_size)
{
    int current_bit = 0;
    byte current_byte = 0;

    for (int i = 0; i < original_data_size; i++)
    {
        char *code = dict[data[i]];

        for (int j = 0; code[j] != '\0'; j++)
        {
            current_byte <<= 1;
            if (code[j] == '1')
            {
                current_byte |= 1; // set the last bit to 1
            }

            current_bit++; // increment the current bit

            if (current_bit == 8)
            {
                encoded_data[(*encoded_data_size)++] = current_byte; // save the byte
                current_byte = 0;                                    // reset the byte
                current_bit = 0;
            }
        }
    }

    if (current_bit > 0) // if there are remaining bits
    {
        current_byte <<= 8 - current_bit;                    // shift the remaining bits to the left
        encoded_data[(*encoded_data_size)++] = current_byte; // save the byte
        // we are filling the remaining bits with 0s
        *trash_size = 8 - current_bit;
    }
}

void inorder_traversal(node_t *root, char *tree, int *tree_size)
{
    if (root == NULL)
        return;

    char data = root->data;


    int is_escape_leaf =
        (data == '*' || data == '\\') && root->left == NULL && root->right == NULL;

    if (is_escape_leaf)
    {
        // tree[*tree_size] = '\\';
        (*tree_size)++;
    }

    // tree[*tree_size] = data;
    (*tree_size)++;
    // printf("colocando %c no indice %d\n", data, *tree_size);

    inorder_traversal(root->left, tree, tree_size);
    inorder_traversal(root->right, tree, tree_size);
}

void print_encoded_data(byte encoded_data[], int encoded_data_size)
{
    for (int i = 0; i < encoded_data_size; i++)
    {
        print_binary(encoded_data[i], sizeof(byte) * 8);
    }
    printf("\n");
}

int get_tree_size(node_t *root)
{
    if (root == NULL)
        return 0;

    return 1 + get_tree_size(root->left) + get_tree_size(root->right);
}

void free_queue(pq_node_t *pq)
{
    while(pq != NULL)
    {
        pq_node_t *aux = pq;
        pq = pq->next;
        free(aux);
    }
}

void compress(char *starting_filename, char *ending_filename)
{
    int data_size = get_file_size(starting_filename);

    byte *data = (byte *)malloc(data_size * sizeof(byte));

    printf("File size: %d bytes\n", data_size);

    FILE *fp = fopen(starting_filename, "rb");
    if (fp == NULL)
    {
        printf("Error opening file '%s'\n", starting_filename);
        return;
    }

    fread(data, sizeof(byte), data_size, fp);

    fclose(fp);

    // Count the frequency of each character
    int *freq = count_frequency(data, data_size);

    // Create a priority queue
    pq_node_t *pq = NULL;
    int queue_size = 0;

    initialize_queue(&pq, freq, &queue_size);

    generate_huffman_tree(&pq, queue_size);

    // we have our huffman tree as the only node in the queue
    node_t *root = dequeue(&pq);

    // print the tree to check if it is correct
    print_tree(root, 0);
    ENDL;

    // free the memory
    free(freq);
    free_queue(pq);

    int max_code_size = get_tree_depth(root) + 1;

    char dict[MAX][max_code_size];
    char code[max_code_size];

    // generate the dictionary like a = 00, b = 01, etc...save it in dict
    generate_huffman_codes(root, dict, code, 0);

    byte *encoded_data = (byte *)malloc(data_size * sizeof(byte));
    int encoded_data_size = 0;
    int trash_size = 0;

    encode_data(data, dict, data_size, encoded_data, &encoded_data_size, &trash_size);

    int tree_size = get_tree_size(root) + 3;
    printf("Tree size: %d\n", tree_size);
    char *tree = (char *)malloc(tree_size * sizeof(char));

    tree_size = 0;//reset it because we didn't count escape characters

    // save the tree in the array tree
    inorder_traversal(root, tree, &tree_size);
    tree_size--;

    printf("Tree size: %d\n", tree_size);

    printf("tree: %s\n", tree);
    write(ending_filename, encoded_data, encoded_data_size, tree_size + 1, trash_size, tree);
    return;
}

void decompress(char *starting_filename, char *ending_filename)
{
    FILE *fp = fopen(starting_filename, "rb");
    if (fp == NULL)
    {
        printf("Error opening file '%s'\n", starting_filename);
        return;
    }

    int file_size = get_file_size(starting_filename);

    // read the data from the file
    byte buffer = 0;

    // read the header
    short header;
    fread(&buffer, sizeof(byte), 1, fp);
    header = buffer << 8;
    fread(&buffer, sizeof(byte), 1, fp);
    header |= buffer;

    short trash_size = header >> 13;
    short tree_size = header & 0x1FFF;

    // read the tree

    char *tree = (char *)malloc(tree_size * sizeof(char));

    printf("Tree: ");
    for (int i = 0; i < tree_size; i++)
    {
        fread(&buffer, sizeof(byte), 1, fp);
        tree[i] = buffer;
        printf("%c", buffer);
    }
    ENDL;

    // read the data

    short i = 0;
    node_t *root = generate_huffman_tree_from_inorder(tree, &i);

    print_tree(root, 0);

    char dict[MAX][BIT_SIZE];
    char code[BIT_SIZE];

    generate_huffman_codes(root, dict, code, 0);

    int encoded_data_size = file_size - 2 - tree_size;

    byte *encoded_data = (byte *)malloc(encoded_data_size * sizeof(byte));

    fread(encoded_data, sizeof(byte), encoded_data_size, fp);

    decode(root, encoded_data, encoded_data_size, trash_size + 1, ending_filename);

    fclose(fp);
}

int main()
{
    int choice = 0;
    char starting_filename[100];
    char ending_filename[100];

    // Loop until the user enters a valid choice
    while (choice != 1 && choice != 2)
    {
        printf("Enter 1 to compress a file, or 2 to decompress a file: ");
        scanf("%d", &choice);
    }

    // Get the filename from the user
    // printf("Enter the starting filename: ");
    // scanf("%s", starting_filename);
    // printf("Enter the ending filename: ");
    // scanf("%s", ending_filename);

    strcpy(starting_filename, "teste.txt");
    strcpy(ending_filename, "teste.huff");

    // Call the appropriate function based on the user's choice
    if (choice == 1)
    {
        compress(starting_filename, ending_filename);
        LINE;
    }
    else
    {
        decompress(starting_filename, ending_filename);
    }

    return 0;
}