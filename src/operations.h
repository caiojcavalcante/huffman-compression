/**
 * @file operations.h
 * @brief Contains the functions that perform the operations.
 */

#define max(a, b) ((a) > (b) ? (a) : (b))

/**
 * @brief Counts the frequencies of each byte in the data.
 *
 * @param data The data to count the frequencies.
 * @param size The size of the data.
 * @return An array with the frequencies of each byte.
 */
int *count_frequencies(unsigned char data[100], int size)
{
    int *frequencies = calloc(256, sizeof(int));

    for (int i = 0; i < size; i++)
        frequencies[data[i]]++;

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
 * @brief Creates a new binary tree node with the specified data.
 *
 * @param data The data to store in the new node.
 * @param left The left child of the node.
 * @param right The right child of the node.
 * @return A pointer to the newly created node.
 */
Tree *create_tree(unsigned char data, Tree *left, Tree* right)
{
    Tree *tree = malloc(sizeof(Tree));
    tree->data = data;
    tree->left = left;
    tree->right = right;

    return tree;
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
/**
 * @brief Prints the heap.
 *
 * @param heap The heap to print.
 */
void print_heap(Heap *heap)
{
    if (heap->size <= 0)
    {
        return;
    }

    int level_size = 1;
    int i = 0;

    while (i < heap->size)
    {
        // print spaces before the first element of each level
        for (int k = 0; k < (heap->size - i) * (heap->size - i) / 8; k++)
        {
            printf(" ");
        }

        // print the elements of the current level
        for (int j = i; j < i + level_size && j < heap->size; j++)
        {
            printf("%c: %d ", heap->data[j].tree->data, heap->data[j].priority);

            // print spaces between elements
            for (int k = 0; k < (heap->size - j) / 2; k++)
            {
                printf(" ");
            }
        }

        printf("\n");
        i += level_size;
        level_size *= 2;
    }
}
