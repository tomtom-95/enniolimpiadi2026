#include <stdlib.h>
#include <stdbool.h>

#include "core.h"
#include "arena.h"

// Global context
Ctx ctx;

Arena *
arena_alloc(u64 len)
{
    u8 *buffer = malloc(len);
    Arena *arena = (Arena *)buffer;

    arena->len = len;
    arena->pos = ARENA_HEADER_SIZE;

    return arena;
}

void *
arena_push(Arena *arena, u64 size, u64 align)
{
    u64 pos_pre = AlignPow2(arena->pos, align);
    u64 pos_pst = pos_pre + size;


    arena->pos = pos_pst;

    void *result = (u8 *)arena + pos_pre;
    MemoryZero(result, pos_pst - pos_pre);

    return result;
}

u64
arena_pos(Arena *arena)
{
    return arena->pos;
}

void
arena_pop_to(Arena *arena, u64 pos)
{
    arena->pos = ClampBot(ARENA_HEADER_SIZE, pos);
}

//- rjf: temporary arena scopes

Temp
temp_begin(Arena *arena)
{
  u64 pos = arena_pos(arena);
  Temp temp = {arena, pos};
  return temp;
}

void
temp_end(Temp temp)
{
  arena_pop_to(temp.arena, temp.pos);
}

void
ctx_init(void)
{
    for (u64 i = 0; i < ArrayCount(ctx.arenas); ++i)
    {
        ctx.arenas[i] = arena_alloc(MegaByte(32));
    }
}

Temp
scratch_get(Arena **conflicts, u64 count)
{
    Arena *result = 0;
    Arena **arena_ptr = ctx.arenas;
    for (u64 i = 0; i < ArrayCount(ctx.arenas); ++i, ++arena_ptr)
    {
        bool has_conflict = false;
        Arena **conflict_ptr = conflicts;
        for (u64 j = 0; j < count; ++j, ++conflict_ptr)
        {
            if (*arena_ptr == *conflict_ptr)
            {
                has_conflict = true;
                break;
            }
        }
        if (!has_conflict)
        {
            result = *arena_ptr;
            break;
        }
    }
    return temp_begin(result);
}

void
scratch_release(Temp temp)
{
    temp_end(temp);
}
