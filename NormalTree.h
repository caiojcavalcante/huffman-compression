#include <stdlib.h>

typedef struct _NormalTree 
{
    int data;
    struct _NormalTree * left;
    struct _NormalTree * right;
} NormalTree;

NormalTree * createNormalTree_empty();
NormalTree * createNormalTree(int data, NormalTree *left, NormalTree *right);
void destroyNormalTree(NormalTree * tree);
NormalTree * add_normal_tree(NormalTree * tree, int item);

NormalTree * createNormalTree_empty()
{
    return NULL;
}

NormalTree * createNormalTree(int data, NormalTree * left, NormalTree * right)
{
    NormalTree * newTree = (NormalTree *) malloc(sizeof(NormalTree));
    newTree -> data = data;
    newTree -> left = left;
    newTree -> right = right;
    return newTree;
}

NormalTree * add_normal_tree(NormalTree * tree, int item)
{
    if (tree == NULL)
        return createNormalTree(item, NULL, NULL);
    else if (item < tree -> data)
        tree -> left = add_normal_tree(tree -> left, item);
    else if (item > tree -> data)
        tree -> right = add_normal_tree(tree -> right, item);
    return tree;
}

void destroyNormalTree(NormalTree * tree)
{
    if (tree != NULL)
    {
        destroyNormalTree(tree -> left);
        destroyNormalTree(tree -> right);
        free(tree);
    }
} 
