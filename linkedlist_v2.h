#include "core.h"
#include "arena.h"

typedef struct NodeV2 NodeV2;
struct NodeV2 {
    NodeV2 *next;
    u32     data;
};

typedef struct LinkedListV2 LinkedListV2;
struct LinkedListV2 {
    Arena  *arena;
    NodeV2 *first;
    NodeV2 *free;
};

LinkedListV2 linkedlist_v2_alloc(Arena *arena);

NodeV2 *linkedlist_v2_push(LinkedListV2 *linkedlist, u32 data);
void linkedlist_v2_pop(LinkedListV2 *linkedlist, u32 data);
NodeV2 *linkedlist_v2_get(LinkedListV2 *linkedlist, u32 data);
