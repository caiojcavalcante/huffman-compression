/**
 * @brief Prints the tree in a pretty way.
 *
 * @param root The root of the tree.
 * @param space The space between the levels. (Start with 0)
 */
void print_tree(Tree *root, int space)
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

    printf("'%c'\n", root->data);

    // print left child
    print_tree(root->left, space);
}
/**
 * @brief Prints a number in binary base.
 *
 * @param n The number to print.
 * @param size The size of the number in bytes.
 */

void print_binary(unsigned char n)
{
    for (int i = sizeof(n); i >= 0; i--)
    {
        if (n & (1 << i))
            printf("1");
        else
            printf("0");
    }
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
