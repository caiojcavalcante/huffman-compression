typedef unsigned char byte;

typedef struct heap_node
{
    int priority;
    struct tree *tree;
} Heap_node;
/**
 * @brief A struct to represent a tree node.
*/
typedef struct tree
{
    unsigned char data;
    struct tree *left;
    struct tree *right;
} Tree;

typedef struct
{
    struct heap_node *data;
    int size;
    int capacity;
} Heap;
