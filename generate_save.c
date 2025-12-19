#include <stdio.h>

#include "core.h"
#include "arena.c"
#include "string.c"
#include "players.c"

int main(void)
{
    // Allocate arena for all allocations
    Arena *arena = arena_alloc(MegaByte(16));

    // Initialize entity lists
    EntityList players = entity_list_init(arena, MAX_NUM_ENTITIES);
    EntityList tournaments = entity_list_init(arena, MAX_NUM_ENTITIES);

    // Add players
    entity_list_add(&players, str8_lit("Alice"));
    entity_list_add(&players, str8_lit("Bob"));
    entity_list_add(&players, str8_lit("Charlie"));

    // Add tournaments
    entity_list_add(&tournaments, str8_lit("Tennis"));
    entity_list_add(&tournaments, str8_lit("Chess"));

    // Register players to tournaments
    // Alice -> Tennis, Chess
    entity_list_register(&players, &tournaments, str8_lit("Alice"), str8_lit("Tennis"));
    entity_list_register(&players, &tournaments, str8_lit("Alice"), str8_lit("Chess"));

    // Bob -> Tennis
    entity_list_register(&players, &tournaments, str8_lit("Bob"), str8_lit("Tennis"));

    // Charlie -> Chess
    entity_list_register(&players, &tournaments, str8_lit("Charlie"), str8_lit("Chess"));

    // Save to file
    b32 success = olympiad_save(arena, &players, &tournaments);

    if (success)
    {
        printf("Players: %u\n", entity_list_count(&players));
        printf("Tournaments: %u\n", entity_list_count(&tournaments));
    }

    return success ? 0 : 1;
}
