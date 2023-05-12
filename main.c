#include <stdio.h>
#include "AVL.h"
#include "NormalTree.h"

// typedef struct binary_tree 
// {
//     int item;
//     int h;
//     binary_tree * left;
//     binary_tree * right;
// } binary_tree;

int compara_avl(binary_tree * bt, int i, int j)
{
    if (bt == NULL) 
    {
        return j;
    } 
    else 
    {
        if (bt -> item == i)
        {
            return j;
        }
        else if (bt -> item > i)
        {
            return compara_avl(bt -> left, i, j + 1);
        }
        else
        {
            return compara_avl(bt -> right, i, j + 1);
        }
    }
}

int compara_normal_tree(NormalTree * bt, int i, int j)
{
    if (bt == NULL) 
    {
        return j;
    } 
    else 
    {
        if (bt -> data == i)
        {
            return j;
        }
        else if (bt -> data > i)
        {
            return compara_normal_tree(bt -> left, i, j + 1);
        }
        else
        {
            return compara_normal_tree(bt -> right, i, j + 1);
        }
    }
}

int main()
{
    

    //crie uma arvore avl e uma normal com cada linha do arquivo numeros.txt
    FILE * file = fopen("numeros.txt", "r");
    FILE * saida = fopen("saida.txt", "a");

    for(int i = 1; i <= 100; i++)
    {
        binary_tree * bt = create_empty_binary_tree();

        int num;
        fscanf(file, "%d", &num);
        while (num != -1)
        {
            bt = add(bt, num);
            fscanf(file, "%d", &num);
        }
       
        int numero = i;
        
        int avl = compara_avl(bt, numero, 0);
        
        fprintf(saida, "%d, ", avl);
        destroy_avl(bt);
    }
    fclose(file);
    fprintf(saida, "\n");
    //faça a mesma coisa uma uma normal tree
    file = fopen("numeros.txt", "r");
    for(int i = 1; i <= 100; i++)
    {
        NormalTree * bt = createNormalTree_empty();

        int num;
        fscanf(file, "%d", &num);
        while (num != -1)
        {
            bt = add_normal_tree(bt, num);
            fscanf(file, "%d", &num);
        }
       
        int numero = i;
        
        int normal = compara_normal_tree(bt, numero, 0);
        
        fprintf(saida, "%d, ", normal);
        destroyNormalTree(bt);
    }
    fclose(file);
    return 0;
}