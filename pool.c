#include <assert.h>

#include "core.h"
#include "arena.h"
#include "pool.h"

Pool *
pool_alloc(Arena *arena, u32 len, u32 slot_size, u32 align)
{
    // NOTE: align must be a power of 2 or the code breaks
    //       the code also breaks when POOL_HEADER_SIZE is not a multiple of Max(AlignOf(Pool), align)

    assert(sizeof(u32) <= slot_size);

    // In the unlikely case an alignment bigger than POOL_HEADER_SIZE is required the code would break
    assert(align <= POOL_HEADER_SIZE);

    Pool *pool = arena_push(arena, POOL_HEADER_SIZE + len * slot_size, Max(AlignOf(Pool), align));

    pool->len = len;
    pool->slot_size  = slot_size;
    pool->first_free = 0;

    u8 *p = (u8 *)pool + POOL_HEADER_SIZE;
    for (u32 i = 0; i < len; ++i)
    {
        *((u32 *)p) = i + 1;
        p += slot_size;
    }

    return pool;
}

u32
pool_push(Pool *pool, void *data)
{
    assert(pool->first_free < pool->len && "There are no free slot!");

    u32 idx = pool->first_free;

    void *slot = (void *)((u8 *)pool + POOL_HEADER_SIZE + idx * pool->slot_size);
    pool->first_free = *((u32 *)slot);

    MemoryCopy(slot, data, pool->slot_size);

    return idx;
}

void
pool_pop(Pool *pool, u32 idx)
{
    assert(idx < pool->len);

    void *slot = (void *)((u8 *)pool + POOL_HEADER_SIZE + idx * pool->slot_size);

    u32 tmp = pool->first_free;
    pool->first_free = idx;
    *((u32 *)slot) = tmp;
}

void *
pool_get(Pool *pool, u32 idx)
{
    assert(idx < pool->len);

    return (void *)((u8 *)pool + POOL_HEADER_SIZE + idx * pool->slot_size);
}
