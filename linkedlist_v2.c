#include "arena.h"
#include "linkedlist_v2.h"

LinkedListV2
linkedlist_v2_alloc(Arena *arena)
{
    LinkedListV2 linkedlist = { .arena = arena, .free = NULL };
    return linkedlist;
}

NodeV2 *
linkedlist_v2_push(LinkedListV2 *linkedlist, u32 data)
{
    NodeV2 *node = linkedlist->free;

    if (node)
    {
        linkedlist->free = linkedlist->free->next;
    }
    else
    {
        node = push_array(linkedlist->arena, NodeV2, 1);
    }

    node->data = data;
    node->next = linkedlist->first;
    linkedlist->first = node;

    return node;
}

void
linkedlist_v2_pop(LinkedListV2 *linkedlist, u32 data)
{
    NodeV2 *node = linkedlist->first;

    if (node->data == data)
    {
        linkedlist->first = linkedlist->first->next;
    }
    else
    {
        while (node->next->data != data)
        {
            node = node->next;
        }

        NodeV2 *to_pop = node->next;

        node->next = to_pop->next;
        to_pop->next = linkedlist->free;
        linkedlist->free = to_pop;
    }
}

NodeV2 *
linkedlist_v2_get(LinkedListV2 *linkedlist, u32 data)
{
    NodeV2 *node = linkedlist->first;

    while (node->data != data)
    {
        node = node->next;
    }

    return node;
}
