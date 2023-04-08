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