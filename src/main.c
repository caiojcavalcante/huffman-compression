#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "operations.h"
// #include "debug.h"

void compress()
{
    unsigned char data[100] = "Marcos ribeiro vosse esta solteiro?";

    int *frequencies = count_frequencies(data, strlen(data));

    // for (int i = 0; i < 256; i++)
    //     if (frequencies[i] > 0)
    //         printf("%c: %d\n", i, frequencies[i]);

    Heap *heap = create_heap();

    // insert(heap, 3, create_tree('M'));
    // for (int i = 0; i < heap->size; i++)
    //     printf("%c: %d\n", heap->data[i].tree->data, heap->data[i].priority);
    // printf("\n");
    // insert(heap, 5, create_tree('A'));
    // for (int i = 0; i < heap->size; i++)
    //     printf("%c: %d\n", heap->data[i].tree->data, heap->data[i].priority);
    // printf("\n");

    // insert(heap, 2, create_tree('R'));
    // insert(heap, 6, create_tree('C'));

    int teste = 0;

    for (unsigned char byte = 0; byte < 256; byte++)
    {
        if (frequencies[byte] > 0)
        {
            insert(heap, frequencies[byte], create_tree(byte));
            // if (teste++ > 10)
            //     break;
        }
    }

    // print the heap
    for (int i = 0; i < heap->size; i++)
        printf("%c: %d\n", heap->data[i].tree->data, heap->data[i].priority);

    printf("marcos ribeiro\n");
}

int main()
{
    compress();

    return 0;
}