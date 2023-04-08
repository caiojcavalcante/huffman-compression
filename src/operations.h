int *count_frequencies(unsigned char data[100], int size)
{
    int *frequencies = calloc(256, sizeof(int));

    for (int i = 0; i < size; i++)
        frequencies[data[i]]++;

    return frequencies;
}

Heap *create_heap()
{
    Heap *heap = malloc(sizeof(Heap));
    heap->data = malloc(256 * sizeof(Heap_node));
    heap->size = 0;
    heap->capacity = 256;

    return heap;
}

Tree *create_tree(unsigned char data)
{
    Tree *tree = malloc(sizeof(Tree));
    tree->data = data;
    tree->left = NULL;
    tree->right = NULL;

    return tree;
}

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
        heap->data[current_node].priority > heap->data[parent_node].priority)
    {
        printf("current_node: %d\n", current_node);
        printf("parent_node: %d\n", current_node >> 1);
        printf("linha: %d\n", __LINE__);
        temp = heap->data[current_node];
        printf("linha: %d\n", __LINE__);
        heap->data[current_node] = heap->data[current_node >> 1];
        printf("linha: %d\n", __LINE__);
        heap->data[parent_node] = temp;
        printf("linha: %d\n", __LINE__);
        current_node >>= 1;
        printf("linha: %d\n", __LINE__);
        parent_node >>= 1;
    }
}