#include <assert.h>

#include "arena.h"
#include "core.h"
#include "string.h"
#include "players.h"

/**
 * Find all filled slots in a bitmap and store their positions.
 *
 * Iterates through a 64-bit bitmap and extracts the position of each bit
 * that is set to 1, storing them in the positions array.
 *
 * @param bitmap    The 64-bit bitmap to scan for set bits
 * @param positions Output array to store the positions of set bits (0-63)
 * @return          The number of filled slots found
 */
u32
find_all_filled_slots(u64 bitmap, s32 positions[64])
{
    u32 count = 0;

    while (bitmap != 0)
    {
        s32 pos = __builtin_ctzll(bitmap);
        positions[count++] = pos;
        bitmap &= bitmap - 1;
    }

    // Return number of filled slots
    return count;
}

EntityList
entity_list_init(Arena *arena, u32 len)
{
    assert(len <= MAX_NUM_ENTITIES);

    // Allocate 2 nodes more than MAX_NUM_PLAYERS for the dummy head and tail nodes
    Entity *entities = push_array(arena, Entity, len + 2);

    u32 tail_idx = len + 1;

    // Link head and tail sentinel
    entities->nxt = len + 1;
    (entities + tail_idx)->prv = 0;

    EntityList entity_list = {.entities = entities, .first_free_idx = 1, .len = len};

    // Initialize the free list
    for (u32 i = 1; i < len + 1; ++i)
    {
        entity_list.entities[i].nxt = (i + 1) % tail_idx;
    }

    return entity_list;
}

u32
entity_list_find(EntityList *entity_list, String8 name)
{
    u32 idx_tail = entity_list->len + 1;

    u32 idx = entity_list->entities->nxt;
    while (idx != idx_tail)
    {
        Entity *entity = entity_list->entities + idx;
        if (str8_cmp(name, entity->name))
        {
            return idx;
        }

        idx = entity->nxt;
    }

    return idx;
}

u32
entity_list_count(EntityList *list)
{
    u32 idx_tail = list->len + 1;
    u32 idx = list->entities->nxt;

    u32 count = 0;
    while (idx != idx_tail)
    {
        idx = (list->entities + idx)->nxt;
        ++count;
    }

    return count;
}

u32
entity_list_add(EntityList *entity_list, String8 name)
{
    u32 idx_tail = entity_list->len + 1;

    // Make sure there is not already a player with this name
    assert(entity_list_find(entity_list, name) == idx_tail);

    u32 first_free_idx = entity_list->first_free_idx;

    // There are no elements in the free list
    // TODO: instead of hard crash must be implemented a mechanism for growing the entities array
    //       which is to move the backing array into a new allocation (2x bigger)
    assert(first_free_idx != 0);

    // Pointer to the node that will be used
    Entity *entity = entity_list->entities + first_free_idx;

    // Move the first free
    entity_list->first_free_idx = (entity_list->entities + first_free_idx)->nxt;

    // Add player on top of the list
    u32 idx = (entity_list->entities)->nxt;

    (entity_list->entities)->nxt = first_free_idx;

    entity->prv = 0;
    entity->nxt = idx;

    (entity_list->entities + idx)->prv = first_free_idx;

    // Fill the node with data
    entity->name.len = name.len;
    entity->name.str = name.str;

    // Player entity is not registered to anything
    entity->registrations = 0;

    return first_free_idx;
}

void
entity_list_remove(EntityList *list1, EntityList *list2, String8 name)
{
    u32 idx_tail = list1->len + 1;

    u32 idx = entity_list_find(list1, name);
    assert(idx != idx_tail);

    Entity *entity = list1->entities + idx;

    s32 positions[64] = {0};
    u32 count = find_all_filled_slots((list1->entities + idx)->registrations, positions);
    for (u32 i = 0; i < count; ++i)
    {
        // Set a bit at position (idx - 1) to 0
        (list2->entities + positions[i] + 1)->registrations &= ~(1ULL << ENTITY_IDX_TO_BIT(idx));
    }

    (list1->entities + entity->prv)->nxt = entity->nxt;
    (list1->entities + entity->nxt)->prv = entity->prv;

    entity->nxt = list1->first_free_idx;
    list1->first_free_idx = idx;
}

void
entity_list_register(EntityList *list1, EntityList *list2,
    String8 name1, String8 name2)
{
    u32 idx_tail1 = list1->len + 1;
    u32 idx_tail2 = list2->len + 1;

    u32 idx1 = entity_list_find(list1, name1);
    assert(idx1 != idx_tail1);

    u32 idx2 = entity_list_find(list2, name2);
    assert(idx2 != idx_tail2);

    // Set registration bits
    (list1->entities + idx1)->registrations |= (1ULL << ENTITY_IDX_TO_BIT(idx2));
    (list2->entities + idx2)->registrations |= (1ULL << ENTITY_IDX_TO_BIT(idx1));
}

void
entity_list_unregister(EntityList *list1, EntityList *list2,
    String8 name1, String8 name2)
{
    u32 idx_tail1 = list1->len + 1;
    u32 idx_tail2 = list2->len + 1;

    u32 idx1 = entity_list_find(list1, name1);
    assert(idx1 != idx_tail1);

    u32 idx2 = entity_list_find(list2, name2);
    assert(idx2 != idx_tail2);

    // Unset the registration bits
    (list1->entities + idx1)->registrations &= ~(1ULL << ENTITY_IDX_TO_BIT(idx2));
    (list2->entities + idx2)->registrations &= ~(1ULL << ENTITY_IDX_TO_BIT(idx1));
}
