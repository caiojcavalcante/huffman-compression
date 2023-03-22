typedef unsigned char byte;

typedef struct node
{
    byte data;
    int freq;
    struct node *left;
    struct node *right;
} node_t;

typedef struct pq_node
{
    int priority;
    node_t *data;
    struct pq_node *next;
} pq_node_t;

typedef struct linked_list
{
    node_t *data;
    struct linked_list *next;
} list;
