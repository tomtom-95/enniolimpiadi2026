#ifndef PLAYERS_H
#define PLAYERS_H

#include "core.h"
#include "arena.h"
#include "string.h"

// TODO IMPORTANT! Document this header and all the subtle details
// I do not want anymore to look at code and wonder why and how and if it works!

#define MAX_NUM_ENTITIES 64
#define BRACKET_SIZE 127

#define MAX_GROUPS 16
#define MAX_GROUP_SIZE 8
#define GROUP_NONE 0xFF  // Sentinel for "player not assigned to any group"

#define MAX_STRING_SIZE 64 // Maximum size players and tournaments names

typedef enum TournamentFormat {
    FORMAT_KNOCKOUT  = 0,   // Pure single elimination
    FORMAT_GROUP_KNOCKOUT,  // Groups then knockout (World Cup style)
} TournamentFormat;

typedef enum TournamentPhase {
    PHASE_REGISTRATION = 0,  // Players can register/unregister
    PHASE_GROUP,             // Group phase (only valid for FORMAT_GROUP_KNOCKOUT)
    PHASE_KNOCKOUT,          // Knockout phase, bracket matches
    PHASE_FINISHED,          // Tournament completed
} TournamentPhase;

typedef enum MatchResult {
    MATCH_RESULT_TBD = 0,   // The match must still be played
    MATCH_RESULT_WIN,
    MATCH_RESULT_DRAW,
    MATCH_RESULT_LOSE,
} MatchResult;

typedef struct MatchScore {
    u16 row_score;  // Score of the row player
    u16 col_score;  // Score of the column player
} MatchScore;

typedef struct GroupPhase GroupPhase;
typedef struct GroupPhase {
    u8 num_groups;
    u8 group_size;
    u8 advance_per_group;

    // Forward: groups[g][local] = global_player_idx
    u8 groups[MAX_GROUPS][MAX_GROUP_SIZE];

    // Reverse: given global player idx, get (group, local position)
    // player_group[global_idx] = group index (GROUP_NONE = not in any group)
    // player_slot[global_idx] = local position within that group
    u8 player_group[MAX_NUM_ENTITIES + 1];  // +1 because entity indices are 1-based
    u8 player_slot[MAX_NUM_ENTITIES + 1];

    // Results matrix (uses local indices)
    // results[g][row][col] stores the score from row player's match against col player
    // Storing the upper diagonal is enough information, the lower one is redundant
    MatchScore  scores[MAX_GROUPS][MAX_GROUP_SIZE][MAX_GROUP_SIZE];
    MatchResult results[MAX_GROUPS][MAX_GROUP_SIZE][MAX_GROUP_SIZE];

    // Elimination bracket for players who advance from groups
    // Uses heap-style layout: children of i at 2*i+1 and 2*i+2
    // Stores player indices. 0 means empty slot.
    u8 bracket[BRACKET_SIZE];
} GroupPhase;

typedef struct Player Player;
struct Player {
    u32 prv;
    u32 nxt;
    String8 name;

    // bitmask of tournament indices
    u64 registrations;
};

typedef struct Event Event;
struct Event {
    u32 prv;
    u32 nxt;
    String8 name;

    // bitmask of player indices
    u64 registrations;

    TournamentPhase phase;
    TournamentFormat format;
    u8 bracket[BRACKET_SIZE];
    GroupPhase group_phase;
};

typedef struct PlayersList {                                                                                                                                                                                                                      
    Player *players;
    u32 first_free_idx;
    u32 len;
} PlayersList;
                                                                                                                                                                                                                                                   
typedef struct EventsList {                                                                                                                                                                                                                  
    Event *events;
    u32 first_free_idx;
    u32 len;
} EventsList;

s32 find_first_empty_slot(u32 bitmap);
u32 find_all_filled_slots(u64 bitmap, s32 positions[64]);

// Players list functions
PlayersList players_list_init(Arena *arena, u32 len);
u32  players_list_find(PlayersList *list, String8 name);
u32  players_list_add(PlayersList *list, String8 name);
u32  players_list_count(PlayersList *list);
void players_list_rename(PlayersList *list, u32 idx, String8 new_name);
void players_list_remove(PlayersList *players, EventsList *events, String8 name);

// Events list functions
EventsList events_list_init(Arena *arena, u32 len);
u32  events_list_find(EventsList *list, String8 name);
u32  events_list_add(EventsList *list, String8 name);
u32  events_list_count(EventsList *list);
void events_list_rename(EventsList *list, u32 idx, String8 new_name);
void events_list_remove(EventsList *events, PlayersList *players, String8 name);

// Registration functions
void register_player_to_event(PlayersList *players, EventsList *events, String8 player_name, String8 event_name);
void unregister_player_from_event(PlayersList *players, EventsList *events, String8 player_name, String8 event_name);

// Tournament functions
void tournament_construct_bracket(Event *event);
void tournament_construct_groups(Event *event);
void calculate_group_standings(Event *event, u32 group_idx, u8 *standings, u32 players_in_group);
void tournament_populate_bracket_from_groups(Event *event);

// Save/Load functions
b32 olympiad_save(Arena *arena, PlayersList *players, EventsList *events);
b32 olympiad_load(Arena *arena, PlayersList *players, EventsList *events);

#endif // PLAYERS_H
