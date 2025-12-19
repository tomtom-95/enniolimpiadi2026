#include <stdio.h>

#include "core.h"
#include "arena.c"
#include "string.c"
#include "players.c"

int main(void)
{
    // Allocate arena for all allocations
    Arena *arena = arena_alloc(MegaByte(16));

    // Initialize lists
    PlayersList players = players_list_init(arena, MAX_NUM_ENTITIES);
    EventsList events = events_list_init(arena, MAX_NUM_ENTITIES);

    // Add players
    players_list_add(&players, str8_lit("Alice"));
    players_list_add(&players, str8_lit("Bob"));
    players_list_add(&players, str8_lit("Charlie"));

    // Add events
    events_list_add(&events, str8_lit("Tennis"));
    events_list_add(&events, str8_lit("Chess"));

    // Register players to events
    // Alice -> Tennis, Chess
    register_player_to_event(&players, &events, str8_lit("Alice"), str8_lit("Tennis"));
    register_player_to_event(&players, &events, str8_lit("Alice"), str8_lit("Chess"));

    // Bob -> Tennis
    register_player_to_event(&players, &events, str8_lit("Bob"), str8_lit("Tennis"));

    // Charlie -> Chess
    register_player_to_event(&players, &events, str8_lit("Charlie"), str8_lit("Chess"));

    // Save to file
    b32 success = olympiad_save(arena, &players, &events);

    if (success)
    {
        printf("Players: %u\n", players_list_count(&players));
        printf("Events: %u\n", events_list_count(&events));
    }

    return success ? 0 : 1;
}
