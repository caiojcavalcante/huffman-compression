/**
 * @file operations.h
 * @brief Contains the functions that perform the operations.
 */

#define max(a, b) ((a) > (b) ? (a) : (b))
#define MAX 256
#define BYTE_SIZE 8
#define BIT_SIZE 9
/**
 * @brief Gets the file size.
 *
 * @param file The file to get the size.
 * @return The size of the file.
 */
long get_file_size(FILE *file)
{
    if (file == NULL)
        return -1;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    return size;
}
/**
 * @brief Counts the frequencies of each byte in the data.
 *
 * @param file The file to read.
 * @param size The size of the data.
 * @return An array with the frequencies of each byte.
 */
int *count_frequencies(FILE *file, unsigned long size)
{
    int *frequencies = calloc(MAX, sizeof(int));
    unsigned char byte;
    for (int i = 0; i < size; i++)
    {
        fread(&byte, sizeof(unsigned char), 1, file);
        frequencies[byte]++;
    }
    rewind(file); // reset file pointer
    return frequencies;
}
/**
 * @brief Swaps two addresses.
 *
 * @param a The first address.
 * @param b The second address.
 * @param size The size of the data.
 */
void swap(void *a, void *b, size_t size)
{
    void *temp = malloc(size);
    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
    free(temp);
}
/**
 * @brief Creates a new binary tree node with the specified data.
 *
 * @param data The data to store in the new node.
 * @param left The left child of the node.
 * @param right The right child of the node.
 * @return A pointer to the newly created node.
 */
Tree *create_tree(unsigned char data, Tree *left, Tree *right)
{
    Tree *tree = malloc(sizeof(Tree));
    tree->data = data;
    tree->left = left;
    tree->right = right;

    return tree;
}
/**
 * @brief Recursively frees the root nodes.
 *
 * @param root The tree to be freed.
 */
void free_tree(Tree *root)
{
    if (root == NULL)
        return;

    free_tree(root->left);
    free_tree(root->right);

    free(root);
}
/**
 * @brief Gets the tree depth.
 *
 * @param tree The tree to get the depth.
 * @return An integer value of the tree depth.
 */
int get_tree_depth(Tree *tree)
{
    if (tree == NULL)
        return 0;

    int left_depth = get_tree_depth(tree->left);
    int right_depth = get_tree_depth(tree->right);

    return 1 + max(left_depth, right_depth);
}
/**
 * @brief Saves the tree in the output file and returns the tree size
 *
 * @param root The root of the tree
 * @param output_file The output file
 * @return The tree size
 */
short save_tree(Tree *root, FILE *output_file)
{
    if (root == NULL)
        return 0;

    int is_escape_leaf =
        (root->data == '*' || root->data == '\\') &&
        root->left == NULL && root->right == NULL;

    if (is_escape_leaf)
        fwrite("\\", sizeof(unsigned char), 1, output_file);

    fwrite(&root->data, sizeof(unsigned char), 1, output_file);
    int left = save_tree(root->left, output_file);
    int right = save_tree(root->right, output_file);
    return 1 + left + right + is_escape_leaf;
}
/**
 * @brief Saves the header in the output file
 *
 * @param output_file The output file
 * @param trash_size The trash size
 * @param tree_size The tree size
 */
void save_header(FILE *output_file, unsigned short trash_size, unsigned short tree_size)
{
    trash_size <<= 13;
    short header = trash_size | tree_size;
    unsigned char buffer = header >> 8;
    fwrite(&buffer, sizeof(unsigned char), 1, output_file);
    buffer = header;
    fwrite(&buffer, sizeof(unsigned char), 1, output_file);
}
/**
 * @brief Reads the input file and compress to the output file
 *
 * @param input_file The input file
 * @param output_file The output file
 * @param max_code_size The maximum code size (depth of the huffman tree)
 * @param dict The dictionary with the codes
 * @param file_size The size of the input file
 * @param tree_size The size of the huffman tree
 * @return int The trash size
 */
int save_data(FILE *input_file, FILE *output_file, int max_code_size, unsigned char dict[MAX][max_code_size], int file_size, int tree_size)
{
    int current_bit = 0;
    int trash_size = 0;
    unsigned char write_buffer = 0;
    unsigned char read_buffer;

    fseek(output_file, tree_size + 2, SEEK_SET); // offset for header

    for (int i = 0; i < file_size; i++)
    {
        fread(&read_buffer, sizeof(unsigned char), 1, input_file);
        unsigned char *code = dict[read_buffer];

        for (int j = 0; code[j] != '\0'; j++)
        {
            write_buffer <<= 1;
            if (code[j] == '1')
            {
                write_buffer |= 1; // set the last bit to 1
            }

            current_bit++; // increment the current bit

            if (current_bit == 8)
            {
                fwrite(&write_buffer, sizeof(unsigned char), 1, output_file);
                write_buffer = 0; // reset the byte
                current_bit = 0;
            }
        }
    }

    if (current_bit > 0) // if there are remaining bits
    {
        write_buffer <<= 8 - current_bit;                             // shift the remaining bits to the left
        fwrite(&write_buffer, sizeof(unsigned char), 1, output_file); // save the byte
        // we are filling the remaining bits with 0s
        trash_size = 8 - current_bit;
    }
    fclose(input_file);  // we won't use the input file anymore
    rewind(output_file); // takes the input_file back to first position
    return trash_size;
}
/**
 * @brief Generate the huffman codes.
 *
 * @param root The root of the tree.
 * @param max_code_size The maximum code size.
 * @param dict The dictionary to store the codes.
 * @param code The current code.
 * @param code_size The current code size.
 */
void generate_huffman_codes(Tree *root, int max_code_size, unsigned char dict[MAX][max_code_size], unsigned char code[max_code_size], int code_size)
{
    if (root == NULL)
        return;

    if (root->left == NULL && root->right == NULL)
    {
        // we have a leaf node
        code[code_size] = '\0';         // null terminate the string
        strcpy((char *)dict[root->data], (const char*)code); // copy the code to the dictionary
        return;
    }
    // traverse left subtree
    code[code_size] = '0';
    generate_huffman_codes(root->left, max_code_size, dict, code, code_size + 1);

    // traverse right subtree
    code[code_size] = '1';
    generate_huffman_codes(root->right, max_code_size, dict, code, code_size + 1);
}
/**
 * @brief Creates the huffman tree from a given header.
 *
 * @param input_file The input file.
 * @param tree_size The tree size.
 * @return Tree* The huffman tree.
 */
Tree *get_tree_from_header(FILE *input_file, unsigned short *tree_size)
{
    unsigned char buffer;
    fread(&buffer, sizeof(unsigned char), 1, input_file);
    int is_leaf = 0;
    if (*tree_size == 0)
        return NULL;
    (*tree_size)--;

    if (buffer == '\\')
    {
        (*tree_size)--;
        fread(&buffer, sizeof(unsigned char), 1, input_file);
        is_leaf = 1;
    }
    if (buffer != '*')
    {
        is_leaf = 1;
    }
    if (is_leaf)
    {
        return create_tree(buffer, NULL, NULL);
    }
    Tree *left = get_tree_from_header(input_file, tree_size);
    Tree *right = get_tree_from_header(input_file, tree_size);
    return create_tree('*', left, right);
}
/**
 * @brief Reads the header from the input file.
 *
 * @param input_file The input file.
 * @param trash_size The trash size.
 * @param tree_size The tree size.
 */
void read_header(FILE *input_file, unsigned char *trash_size, unsigned short *tree_size)
{

    unsigned char buffer;
    fread(&buffer, sizeof(char), 1, input_file);

    unsigned short header = buffer << 8;

    fread(&buffer, sizeof(char), 1, input_file);

    header |= buffer;

    *trash_size = header >> 13;
    *tree_size = header & 0x1FFF;

    printf("Trash size: %d\n", *trash_size);
    printf("Tree size: %d\n", *tree_size);
}
/**
 * @brief Uncompresses the input file and saves it to the output file.
 *
 * @param input_file The input file.
 * @param output_file The output file.
 * @param file_size The file size.
 * @param trash_size The trash size.
 * @param tree_size The tree size.
 */
void decode_to_file(FILE *input_file, FILE *output_file, long file_size, unsigned char trash_size, unsigned short tree_size)
{
    file_size -= tree_size + 2;
    file_size <<= 3;
    file_size -= trash_size;

    Tree *root = get_tree_from_header(
        input_file,
        &tree_size);

    Tree *aux = root;
    unsigned char buffer;
    int current_bit = 0;

    while (file_size--)
    {
        if (current_bit == 0)
        {
            fread(&buffer, sizeof(unsigned char), 1, input_file);
            current_bit = 8;
        }

        if (buffer & (1 << --current_bit))
        {
            aux = aux->right;
        }
        else
        {
            aux = aux->left;
        }

        if (aux->left == NULL && aux->right == NULL)
        {
            fwrite(&(aux->data), sizeof(unsigned char), 1, output_file);
            aux = root;
        }
    }
}
/**
 * @brief Creates a new heap.
 *
 * @return A pointer to the newly created heap.
 */
Heap *create_heap()
{
    Heap *heap = malloc(sizeof(Heap));
    heap->data = malloc(257 * sizeof(Heap_node));
    heap->size = 0;
    heap->capacity = 257;

    return heap;
}
/**
 * @brief Heapifies the heap (min).
 *
 * @param heap The heap to heapify.
 * @param index The index of the node to heapify.
 */
void heapify(Heap *heap, int index)
{
    int left_child = index << 1;
    int right_child = left_child + 1;
    int smallest = index;

    if (left_child < heap->size && heap->data[left_child].priority < heap->data[smallest].priority)
        smallest = left_child;

    if (right_child < heap->size && heap->data[right_child].priority < heap->data[smallest].priority)
        smallest = right_child;

    if (smallest != index)
    {
        swap(&heap->data[index], &heap->data[smallest], sizeof(Heap_node));
        heapify(heap, smallest);
    }
}
/**
 * @brief Inserts a new node in the heap.
 *
 * @param heap The heap to insert the node.
 * @param priority The priority of the node.
 * @param tree The tree to store in the node.
 */
void insert(Heap *heap, int priority, Tree *tree)
{
    if (heap->size == heap->capacity)
        return;

    Heap_node node = {priority, tree};
    Heap_node temp;

    heap->data[heap->size++] = node;

    int current_node = heap->size - 1;
    int parent_node = current_node >> 1;

    while (
        current_node > 0 &&
        heap->data[current_node].priority < heap->data[parent_node].priority)
    {
        temp = heap->data[current_node];
        heap->data[current_node] = heap->data[current_node >> 1];
        heap->data[parent_node] = temp;
        current_node >>= 1;
        parent_node >>= 1;
    }
}
/**
 * @brief Removes the root node from the heap.
 *
 * @param heap The heap to remove the root node.
 */
Tree *pop(Heap *heap)
{
    if (heap->size == 0)
    {
        printf("Heap Underflow!\n");
        return NULL;
    }
    Tree *tree = heap->data[0].tree;
    heap->data[0] = heap->data[--heap->size];
    heapify(heap, 0);
    return tree;
}