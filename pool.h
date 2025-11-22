#ifndef POOL_H
#define POOL_H

#include "core.h"
#include "arena.h"

#define POOL_HEADER_SIZE 128

typedef struct Pool Pool;
struct Pool {
    u32 len;
    u32 slot_size;
    u32 first_free;
};
StaticAssert(sizeof(Pool) <= POOL_HEADER_SIZE, pool_header_size_check);

Pool *pool_alloc(Arena *arena, u32 len, u32 slot_size, u32 align);

u32   pool_push(Pool *pool, void *data);
void  pool_pop(Pool *pool, u32 idx);
void *pool_get(Pool *pool, u32 idx);

#define pool_init(a, T, c) pool_alloc((a), c, sizeof(T), Max(8, AlignOf(T)))

#endif // POOL_H
