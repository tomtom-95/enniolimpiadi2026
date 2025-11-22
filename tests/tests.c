#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "../arena.c"
#include "../pool.c"
#include "../string.c"
#include "../linkedlist.c"
#include "../players.c"

///////////////////////////////////////////////////////////////
// arena tests

void
bar(Arena *arena)
{
    Temp temp = scratch_get(&arena, 1);
    u8 *num = push_array(arena, u8, 9);
    *num = 1;

    u8 *tmp_num = push_array(temp.arena, u8, 2);
    *tmp_num = 2;
    *(tmp_num + 1) = 3;

    scratch_release(temp);
}

void
foo(void)
{
    Temp temp = scratch_get(0, 0);
    bar(temp.arena);
    scratch_release(temp);
}

void
test_arena(void)
{
    ctx_init();
    foo();
}

///////////////////////////////////////////////////////////////
// string tests

void
test_str8_cmp(void)
{
    String8 s1 = str8_lit("Hello");
    String8 s2 = str8_lit("Hello");

    assert(str8_cmp(s1, s2));
}

void
test_str8_cat(void)
{
    String8 a = str8_lit("Hello, ");
    String8 b = str8_lit("World!");

    Arena *arena = arena_alloc(MegaByte(1));
    String8 c = str8_cat(arena, a, b);
}

void
test_str8_constructor(void)
{
    char *cstr = "Hello";

    String8 str = str8_cstr(cstr);
}

void
test_str8_copy(void)
{
    String8 a = str8_lit("Hello");

    Arena *arena = arena_alloc(MegaByte(1));
    String8 res = str8_copy(arena, a);
}

///////////////////////////////////////////////////////////////
// pool tests

void
test_pool_push(void)
{
    typedef struct Foo Foo;
    struct Foo {
        u32 data;
    };

    Arena *arena = arena_alloc(MegaByte(1));

    Pool *pool = pool_init(arena, Foo, 10);

    Foo foo1 = {.data = 0xaaaaaaaa};
    Foo foo2 = {.data = 0xbbbbbbbb};
    Foo foo3 = {.data = 0xcccccccc};

    pool_push(pool, &foo1);
    pool_push(pool, &foo2);
    pool_push(pool, &foo3);
}

void
test_pool_pop(void)
{
    typedef struct Foo Foo;
    struct Foo {
        u32 data;
    };

    Arena *arena = arena_alloc(MegaByte(1));

    Pool *pool = pool_init(arena, Foo, 10);

    Foo foo1 = {.data = 0xaaaaaaaa};
    Foo foo2 = {.data = 0xbbbbbbbb};
    Foo foo3 = {.data = 0xcccccccc};

    u32 idx1 = pool_push(pool, &foo1);
    pool_pop(pool, idx1);
}

void
test_pool_double_free(void)
{
    // NOTE: the current pool implementation of the pool allocator
    //       does not protect against double free.
    //       This function shows how calling pool_pop on an already
    //       freed index causes a bug
    typedef struct Foo Foo;
    struct Foo {
        u32 data;
    };

    Arena *arena = arena_alloc(MegaByte(1));

    Pool *pool = pool_init(arena, Foo, 10);

    Foo foo = {.data = 0xaaaaaaaa};

    u32 idx = pool_push(pool, &foo);
    pool_pop(pool, idx);
    pool_pop(pool, idx);
}

///////////////////////////////////////////////////////////////
// players tests

void
test_players(void)
{
    Arena *arena = arena_alloc(MegaByte(1));

    EntityList players = entity_list_init(arena, 64);
    EntityList tournaments = entity_list_init(arena, 64);

    String8 aldo     = str8_lit("Aldo");
    String8 giovanni = str8_lit("Giovanni");
    String8 giacomo  = str8_lit("Giacomo");

    String8 pingpong    = str8_lit("Ping Pong");
    String8 machiavelli = str8_lit("Machiavelli");
    String8 freccette   = str8_lit("Freccette");

    entity_list_add(&players, aldo);
    entity_list_add(&players, giovanni);
    entity_list_add(&players, giacomo);

    entity_list_add(&tournaments, pingpong);
    entity_list_add(&tournaments, machiavelli);
    entity_list_add(&tournaments, freccette);

    entity_list_register(&players, &tournaments, aldo, pingpong);

    u32 idx_aldo = entity_list_find(&players, aldo);
    assert((players.entities + idx_aldo)->registrations == 0x0000000000000001);

    u32 idx_pingpong = entity_list_find(&tournaments, pingpong);
    assert((tournaments.entities + idx_pingpong)->registrations == 0x0000000000000001);

    entity_list_remove(&players, &tournaments, aldo);
    assert((tournaments.entities + idx_pingpong)->registrations == 0x0000000000000000);
}

void
test_unregistration(void)
{
    Arena *arena = arena_alloc(MegaByte(1));

    EntityList players = entity_list_init(arena, 64);
    EntityList tournaments = entity_list_init(arena, 64);

    String8 aldo     = str8_lit("Aldo");
    String8 giovanni = str8_lit("Giovanni");
    String8 giacomo  = str8_lit("Giacomo");

    String8 pingpong    = str8_lit("Ping Pong");
    String8 machiavelli = str8_lit("Machiavelli");
    String8 freccette   = str8_lit("Freccette");

    entity_list_add(&players, aldo);
    entity_list_add(&players, giovanni);
    entity_list_add(&players, giacomo);

    entity_list_add(&tournaments, pingpong);
    entity_list_add(&tournaments, machiavelli);
    entity_list_add(&tournaments, freccette);

    u32 idx_aldo = entity_list_find(&players, aldo);
    u32 idx_pingpong = entity_list_find(&tournaments, pingpong);

    entity_list_register(&players, &tournaments, aldo, pingpong);
    assert((tournaments.entities + idx_aldo)->registrations == 1);
    assert((tournaments.entities + idx_pingpong)->registrations == 1);

    entity_list_unregister(&players, &tournaments, aldo, pingpong);
    assert((tournaments.entities + idx_aldo)->registrations == 0);
    assert((tournaments.entities + idx_pingpong)->registrations == 0);
}

int
main(void)
{
    test_players();
    test_unregistration();

    return 0;
}
