#include <stdio.h>

#include "arena.h"
#include "core.h"
#include "arena.c"
#include "string.c"
#include "players.c"

int main(void)
{
    ctx_init();

    // Allocate arena for all allocations
    Arena *arena = arena_alloc(MegaByte(16));

    // Initialize lists
    PlayersList players = players_list_init(arena, MAX_NUM_ENTITIES);
    EventsList events = events_list_init(arena, MAX_NUM_ENTITIES);

    // Add players
    players_list_add(&players, str8_lit("Alice"));
    players_list_add(&players, str8_lit("Bob"));
    players_list_add(&players, str8_lit("Charlie"));
    players_list_add(&players, str8_lit("Player01"));
    players_list_add(&players, str8_lit("Player02"));
    players_list_add(&players, str8_lit("Player03"));
    players_list_add(&players, str8_lit("Player04"));
    players_list_add(&players, str8_lit("Player05"));
    players_list_add(&players, str8_lit("Player06"));
    players_list_add(&players, str8_lit("Player07"));
    players_list_add(&players, str8_lit("Player08"));
    players_list_add(&players, str8_lit("Player09"));
    players_list_add(&players, str8_lit("Player10"));
    players_list_add(&players, str8_lit("Player11"));
    players_list_add(&players, str8_lit("Player12"));
    players_list_add(&players, str8_lit("Player13"));
    players_list_add(&players, str8_lit("Player14"));

    // Add events
    events_list_add(&events, str8_lit("Tennis"));
    events_list_add(&events, str8_lit("Chess"));
    events_list_add(&events, str8_lit("Torneo01"));
    events_list_add(&events, str8_lit("Torneo02"));
    events_list_add(&events, str8_lit("Torneo03"));
    events_list_add(&events, str8_lit("Torneo04"));
    events_list_add(&events, str8_lit("Torneo05"));
    events_list_add(&events, str8_lit("Torneo06"));
    events_list_add(&events, str8_lit("Torneo07"));
    events_list_add(&events, str8_lit("Torneo08"));
    events_list_add(&events, str8_lit("Torneo09"));
    events_list_add(&events, str8_lit("Torneo10"));
    events_list_add(&events, str8_lit("Torneo11"));
    events_list_add(&events, str8_lit("Torneo12"));
    events_list_add(&events, str8_lit("Torneo13"));
    events_list_add(&events, str8_lit("Torneo14"));
    events_list_add(&events, str8_lit("Torneo15"));
    events_list_add(&events, str8_lit("Torneo16"));

    // Register players to events
    // Alice -> Tennis, Chess
    register_player_to_event(&players, &events, str8_lit("Alice"), str8_lit("Tennis"));
    register_player_to_event(&players, &events, str8_lit("Alice"), str8_lit("Chess"));

    // Bob -> Tennis
    register_player_to_event(&players, &events, str8_lit("Bob"), str8_lit("Tennis"));

    // Charlie -> Chess
    register_player_to_event(&players, &events, str8_lit("Charlie"), str8_lit("Chess"));

    // Save to file
    b32 success = olympiad_save(&players, &events);

    if (success)
    {
        printf("Players: %u\n", players_list_count(&players));
        printf("Events: %u\n", events_list_count(&events));
    }

    return success ? 0 : 1;
}
