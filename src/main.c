#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define byte unsigned char
#define true 1
#define false 0
#define MAX 256
#define DEBUG 0 &&
#define LEFT '0'
#define RIGHT '1'

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

node_t *create_node(byte data, int freq)
{
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    new_node->data = data;
    new_node->freq = freq;
    new_node->left = NULL;
    new_node->right = NULL;
    DEBUG printf("node criado\n");
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

node_t *dequeue(pq_node_t **pq)
{
    if (*pq == NULL)
    {
        return NULL;
    }

    pq_node_t *temp = *pq;
    *pq = (*pq)->next;
    node_t *data = temp->data;
    free(temp);
    return data;
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

int max_(int a, int b)
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

void generate_huffman_codes(node_t *root, char dict[MAX][8], char code[8], int code_size)
{
    if (root == NULL)
        return;

    if (root->left == NULL && root->right == NULL)
    {
        // we have a leaf node
        strcpy(dict[root->data], code);
        printf("%c: %s\n", root->data, dict[root->data]);
        return;
    }
    // traverse left subtree
    code[code_size] = '0';
    generate_huffman_codes(root->left, dict, code, code_size + 1);

    // traverse right subtree
    code[code_size] = '1';
    generate_huffman_codes(root->right, dict, code, code_size + 1);
}

void print_binary(byte n)
{
    for (int i = sizeof(n) * 8 - 1; i >= 0; i--)
    {
        if (n & (1u << i))
            printf("1");
        else
            printf("0");
    }
}

int main()
{
    byte *data = "hello marcio ribeiro!";

    // Count the frequency of each character

    int *freq = (int *)malloc(sizeof(int) * MAX);
    memset(freq, 0, sizeof(int) * MAX);

    for (int i = 0; i < strlen(data); i++)
    {
        freq[data[i]]++;
    }

    pq_node_t *pq = NULL;
    int queue_size = 0;

    for (int i = 0; i < MAX; i++)
    {
        if (freq[i] > 0)
        {
            node_t *new_node = create_node(i, freq[i]);
            enqueue(&pq, new_node);
            queue_size++;
        }
    }

    while (queue_size > 1)
    {
        node_t *left = dequeue(&pq);
        node_t *right = dequeue(&pq);
        node_t *new_node = create_node('+', left->freq + right->freq);
        new_node->left = left;
        new_node->right = right;
        enqueue(&pq, new_node);
        queue_size--;
    }

    // we have our huffman tree as the only node in the queue
    node_t *root = dequeue(&pq);

    print_tree(root, 0);

    // free the memory
    free(freq);

    // get the depth of the tree
    int depth = get_tree_depth(root);

    printf("Depth: %d\n", depth);

    char dict[MAX][8];
    char code[8];

    generate_huffman_codes(root, dict, code, 0);

    // printf("Encoded data: ");
    // for (int i = 0; i < strlen(data); i++)
    // {
    //     printf("%s", dict[data[i]]);
    // }

    printf("%s\n", data);

    // print original data in binary
    printf("Original data: ");
    for (int i = 0; i < strlen(data); i++)
    {
        print_binary(data[i]);
    }
    printf("\n");
    printf("\n");

    byte encoded_data[100];
    int encoded_data_size = 0;

    int data_len = strlen(data);
    int current_bit = 0;
    byte current_byte = 0;

    for (int i = 0; i < data_len; i++)
    {
        char *code = dict[data[i]];
        int code_len = strlen(code);

        for (int j = 0; j < code_len; j++)
        {
            current_byte <<= 1;
            if (code[j] == '1')
            {
                current_byte |= 1;
            }

            current_bit++;

            if (current_bit == 8)
            {
                encoded_data[encoded_data_size++] = current_byte;
                current_byte = 0;
                current_bit = 0;
            }
        }
    }

    if (current_bit > 0)
    {
        current_byte <<= 8 - current_bit;
        encoded_data[encoded_data_size++] = current_byte;
    }

    printf("Encoded data: ");
    for (int i = 0; i < encoded_data_size; i++)
    {
        print_binary(encoded_data[i]);
    }
    printf("\n");
    printf("\n");

    printf("Original data size: %ld bytes\n", strlen(data) * sizeof(char));
    printf("Encoded data size: %d bytes\n", encoded_data_size);

    return 0;
}