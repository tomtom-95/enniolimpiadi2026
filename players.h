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

#define ENTITY_IDX_TO_BIT(idx)  ((idx) - 1)
#define BIT_TO_ENTITY_IDX(bit)  ((bit) + 1)

typedef enum TournamentFormat {
    FORMAT_KNOCKOUT,        // Pure single elimination
    FORMAT_GROUP_KNOCKOUT,  // Groups then knockout (World Cup style)
} TournamentFormat;

typedef enum TournamentPhase {
    PHASE_REGISTRATION,  // Players can register/unregister
    PHASE_GROUP,         // Group phase (only valid for FORMAT_GROUP_KNOCKOUT)
    PHASE_KNOCKOUT,      // Knockout phase, bracket matches
    PHASE_FINISHED,      // Tournament completed
} TournamentPhase;

typedef enum MatchResult {
    MATCH_RESULT_TBD,   // The match must still be played
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

typedef struct Entity Entity;
struct Entity {
    u32 nxt;
    u32 prv;
    String8 name;

    // I am assuming the entity will never have more than 64 registration
    u64 registrations;

    // Used only if Entity is a tournament
    u8 medals[3];
    TournamentPhase phase;
    TournamentFormat format;

    // Tournament bracket tree (heap-style: children of i at 2*i+1 and 2*i+2)
    // Stores player indices. 0 means empty slot.
    // Used when the tournament is a single elimination
    u8 bracket[BRACKET_SIZE];

    // GroupPhase struct used when the tournament as a group phase
    GroupPhase group_phase;
};

typedef struct EntityList EntityList;
struct EntityList {
    Entity *entities;
    u32 first_free_idx;
    u32 len;
};

s32 find_first_empty_slot(u32 bitmap);
u32 find_all_filled_slots(u64 bitmap, s32 positions[64]);

EntityList entity_list_init(Arena *arena, u32 len);

u32 entity_list_find(EntityList *entity_list, String8 name);

u32  entity_list_add(EntityList *entity_list, String8 name);
u32  entity_list_count(EntityList *list);

void entity_list_rename(EntityList *entity_list, u32 idx, String8 new_name);
void entity_list_remove(EntityList *list1, EntityList *list2, String8 name);
void entity_list_register(EntityList *list1, EntityList *list2, String8 name1, String8 name2);
void entity_list_unregister(EntityList *list1, EntityList *list2, String8 name1, String8 name2);

void tournament_construct_bracket(Entity *tournament);
void tournament_construct_groups(Entity *tournament);
void tournament_populate_bracket_from_groups(Entity *tournament);

// Save/Load functions
b32 olympiad_save(Arena *arena, EntityList *players, EntityList *tournaments);
b32 olympiad_load(Arena *arena, EntityList *players, EntityList *tournaments);

#endif // PLAYERS_H
