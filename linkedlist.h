#include "core.h"
#include "pool.h"

typedef struct Node Node;
struct Node {
    u32 next;
    u32 data;
};

typedef struct LinkedList LinkedList;
struct LinkedList {
    Pool *pool;
};

LinkedList linkedlist_alloc(Arena *arena, u32 len, u32 slot_size, u32 align);

void linkedlist_push(LinkedList *linkedlist, u32 data);
void linkedlist_pop(LinkedList *linkedlist, u32 data);
Node *linkedlist_get(LinkedList *linkedlist, u32 data);
