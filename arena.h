#ifndef ARENA_H
#define ARENA_H

#include "core.h"

#define ARENA_HEADER_SIZE 128

typedef struct Arena Arena;
struct Arena {
    u64 len;
    u64 pos;
};
StaticAssert(sizeof(Arena) <= ARENA_HEADER_SIZE, arena_header_size_check);

typedef struct Temp Temp;
struct Temp {
    Arena *arena;
    u64 pos;
};

typedef struct Ctx Ctx;
struct Ctx {
    Arena *arenas[2]; // scratch arenas
};

Arena *arena_alloc(u64 len);

void *arena_push(Arena *arena, u64 align, u64 size);
void arena_pop_to(Arena *arena, u64 pos);

u64 arena_pos(Arena *arena);

Temp temp_begin(Arena *arena);
void temp_end(Temp temp);

void ctx_init(void);

//- rjf: push helper macros
#define push_array_no_zero_aligned(a, T, c, align) (T *)arena_push((a), sizeof(T)*(c), (align))
#define push_array_aligned(a, T, c, align) (T *)arena_push((a), sizeof(T)*(c), (align))
#define push_array_no_zero(a, T, c) push_array_no_zero_aligned(a, T, c, Max(8, AlignOf(T)))
#define push_array(a, T, c) push_array_aligned(a, T, c, Max(8, AlignOf(T)))

#endif // ARENA_H
