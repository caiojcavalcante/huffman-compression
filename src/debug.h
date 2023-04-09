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
void print_binary(int n, int size)
{
    for (int i = sizeof(n) * size - 1; i >= 0; i--)
    {
        if (n & (1 << i))
            printf("1");
        else
            printf("0");
    }
}