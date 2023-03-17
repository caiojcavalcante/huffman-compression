#define CAPACITY 10

typedef struct Heap
{
    int size;
    int capacity;
    int *array;
} Heap;

int main()
{
    Heap *heap = (Heap *)malloc(sizeof(Heap));
    heap->size = 0;
    heap->capacity = CAPACITY;
    heap->array = (int *)malloc(sizeof(int) * heap->capacity);
    
    
    return 0;
}