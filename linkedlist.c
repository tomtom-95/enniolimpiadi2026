#include <assert.h>

#include "core.h"
#include "arena.h"
#include "pool.h"
#include "linkedlist.h"

// Having a general linked list start to be too much of a pain
// New experiment: implement the linked list directly on top of the arena 
// This means the the linked list is responsible both for the next index for
// the node actually used and the keeping of the free list

LinkedList
linkedlist_alloc(Arena *arena, u32 len, u32 slot_size, u32 align)
{
    assert(slot_size >= sizeof(u32));

    Pool *pool = pool_alloc(arena, len, slot_size, align);
    LinkedList linkedlist = { .pool = pool };

    // Push a sentinel node in the linked list
    // This node will never be popped and will always be at idx = 0
    pool_push(pool, &((Node){ .next = len }));

    return linkedlist;
}

Node *
linkedlist_get(LinkedList *linkedlist, u32 data)
{
    // Get the sentinel node
    Node *node = pool_get(linkedlist->pool, 0);

    while (node->next < linkedlist->pool->len)
    {
        node = pool_get(linkedlist->pool, node->next);
        if (node->data == data)
        {
            return node;
        }
    }

    return NULL;
}

void
linkedlist_push(LinkedList *linkedlist, u32 data)
{
    // Get the sentinal node
    Node *sentinel = pool_get(linkedlist->pool, 0);

    // The new allocated node points to the sentinel->next
    Node node = { .data = data, .next = sentinel->next };

    u32 idx = pool_push(linkedlist->pool, &node);
    sentinel->next = idx;
}

void
linkedlist_pop(LinkedList *linkedlist, u32 data)
{
    // Start with a pointer to the sentinel's next field
    u32 *next_ptr = &(((Node *)pool_get(linkedlist->pool, 0))->next);

    while (*next_ptr < linkedlist->pool->len)
    {
        Node *curr = pool_get(linkedlist->pool, *next_ptr);

        if (curr->data == data)
        {
            u32 tmp = *next_ptr;
            *next_ptr = curr->next;
            pool_pop(linkedlist->pool, tmp);
            return;
        }

        // Move to current node's next field
        next_ptr = &(curr->next);
    }
}
