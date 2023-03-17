#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#define true 1
#define false 0
#define MAX 257
#define BIT_SIZE 9
#define DEBUG if (0)
#define LEFT '0'
#define RIGHT '1'
#define ENDL printf("\n")

typedef unsigned char byte;

typedef struct node
{
    byte data;
    int freq;
    struct node *left;
    struct node *right;
} node_t;

typedef struct pq_node
{
    int priority;
    node_t *data;
    struct pq_node *next;
} pq_node_t;

int *count_frequency(byte *data)
{
    int *freq = (int *)malloc(sizeof(int) * MAX);
    memset(freq, 0, sizeof(int) * MAX);

    for (int i = 0;; i++)
    {
        if (data[i] == '\0')
            break;
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

void print_tree(node_t *root, int space)
{
    // base case
    if (root == NULL)
        return;

    // increase distance between levels
    space += 10;

    // print right child first
    print_tree(root->right, space);

    // print current node after space
    printf("\n");
    for (int i = 10; i < space; i++)
        printf(" ");
    printf("'%c' (%d)\n", root->data, root->freq);

    // print left child
    print_tree(root->left, space);
}

int max_(int a, int b) // max is a macro in windows.h, but I use bash
{
    return (a > b) ? a : b;
}

int get_tree_depth(node_t *root)
{
    if (root == NULL)
        return -1;

    int l = get_tree_depth(root->left);
    int r = get_tree_depth(root->right);

    return 1 + max_(l, r);
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

void print_binary(short n, int size)
{
    for (int i = sizeof(n) * size - 1; i >= 0; i--)
    {
        if (n & (1 << i))
            printf("1");
        else
            printf("0");
    }
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

void compress(const char *filename, byte data[], int encoded_data_size, short tree_size, short trash_size, char tree[])
{
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        printf("Error opening file '%s'\n", filename);
        return;
    }
    // 3 bits for trash, 13 bits for tree size, tree, data

    byte buffer;

    tree_size <<= 3;
    tree_size >>= 3;
    trash_size <<= 13;

    short header = trash_size | tree_size;

    buffer = header >> 8;
    fwrite(&buffer, sizeof(byte), 1, fp);

    buffer = header & 0xFF;
    fwrite(&buffer, sizeof(byte), 1, fp);

    fwrite(tree + 1, sizeof(char), tree_size, fp);

    fwrite(data, sizeof(byte), encoded_data_size, fp);
    fclose(fp);
}

node_t *generate_huffman_tree_from_inorder(char *tree, short *index)
{
    node_t *node;
    printf("recebi %c index: %hd\n", tree[*index], *index);

    int is_leaf = false;
    if (tree[*index] == '\\')
    {
        is_leaf = true;
        printf("escape character found");
        (*index)++; // skip the escape character
    }

    char data = tree[*index];
    node = create_node(data, 1);
    (*index)++;

    if (!is_leaf && data == '*')
    {
        printf("é galho");
        ENDL;
        node->left = generate_huffman_tree_from_inorder(tree, index);
        node->right = generate_huffman_tree_from_inorder(tree, index);
    }
    ENDL;
    return node;
}

void decode(node_t *root, byte encoded_data[], int encoded_data_size)
{
    node_t *current_node = root;
    int current_bit_position = 0;

    byte current_byte = 0;

    printf("Decoded data: ");
    for (int i = 0; i < encoded_data_size; i++)
    {
        current_byte = encoded_data[i];
        current_bit_position = 7;

        while (current_bit_position >= 0)
        {
            if (current_byte & (1 << current_bit_position))
            {
                current_node = current_node->right;
            }
            else
            {
                current_node = current_node->left;
            }
            current_bit_position--;

            if (current_node->left == NULL && current_node->right == NULL)
            {
                printf("%c", current_node->data);
                current_node = root;
            }
        }
    }
    printf("\n");
}

void decompress(char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        printf("Error opening file '%s'\n", filename);
        return;
    }

    int size = get_file_size(filename);

    printf("File size: %d\n", size);

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

    printf("Trash size: %d\n", trash_size);
    printf("Tree size: %d\n", tree_size);

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

    int encoded_data_size = size - 2 - tree_size;
    byte *encoded_data = (byte *)malloc(encoded_data_size * sizeof(byte));

    fread(encoded_data, sizeof(byte), encoded_data_size, fp);

    decode(root, encoded_data, encoded_data_size);

    fclose(fp);
}

void encode_data(byte data[], char dict[MAX][BIT_SIZE], byte encoded_data[], int *encoded_data_size, int *trash_size)
{
    int data_len = strlen(data);
    int current_bit = 0;
    byte current_byte = 0;

    for (int i = 0; i < data_len; i++)
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

    (*tree_size)++;

    char data = root->data;

    int is_leaf = root->left == NULL && root->right == NULL;

    if ((data == '*' || data == '\\') && is_leaf)
    {
        tree[*tree_size] = '\\';
        (*tree_size)++;
    }

    tree[*tree_size] = data;

    inorder_traversal(root->left, tree, tree_size);
    inorder_traversal(root->right, tree, tree_size);
}

void print_encoded_data(byte encoded_data[], int encoded_data_size)
{
    printf("Encoded data: ");
    for (int i = 0; i < encoded_data_size; i++)
    {
        print_binary(encoded_data[i], sizeof(byte) * 8);
    }
    printf("\n");
}


int main()
{
    if (0)
    {

        byte *data = "caralho\\ marcio * esse codigo funcionou * de primeira\\";

        // Count the frequency of each character
        int *freq = count_frequency(data);

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

        char dict[MAX][BIT_SIZE];
        char code[BIT_SIZE];

        // generate the dictionary like a = 00, b = 01, etc...save it in dict
        generate_huffman_codes(root, dict, code, 0);

        byte encoded_data[100];
        int encoded_data_size = 0;
        int trash_size = 0;

        encode_data(data, dict, encoded_data, &encoded_data_size, &trash_size);

        int tree_size = 0;
        char *tree = (char *)malloc(100 * sizeof(char));

        // save the tree in the array tree
        inorder_traversal(root, tree, &tree_size);
        // print the tree

        compress("compressed.huff", encoded_data, encoded_data_size, tree_size, trash_size, tree);
    }
    else
    {
        decompress("compressed.huff");
        printf("\n");
    }
    return 0;
}