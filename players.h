#ifndef PLAYERS_H
#define PLAYERS_H

#include "core.h"
#include "arena.h"
#include "string.h"

// TODO IMPORTANT! Document this header and all the subtle details
// I do not want anymore to look at code and wonder why and how and if it works!

#define MAX_NUM_ENTITIES 64
#define BRACKET_SIZE 127  // 64 + 32 + 16 + 8 + 4 + 2 + 1

#define ENTITY_IDX_TO_BIT(idx)  ((idx) - 1)
#define BIT_TO_ENTITY_IDX(bit)  ((bit) + 1)

typedef enum MedalsEnum {
    MEDAL_GOLD   = 0,
    MEDAL_SILVER = 1,
    MEDAL_BRONZE = 2,
} MedalsEnum;

typedef enum TournamentState {
    TOURNAMENT_REGISTRATION = 0,  // Players can register/unregister
    TOURNAMENT_IN_PROGRESS  = 1,  // Bracket is locked, matches can be played
    TOURNAMENT_FINISHED     = 2,  // Tournament completed
} TournamentState;

typedef struct Entity Entity;
struct Entity {
    u32 nxt;
    u32 prv;
    String8 name;

    // I am assuming the entity will never have more than 64 registration
    u64 registrations;

    // Used only if Entity is a tournament
    u8 medals[3];
    TournamentState state;  // Tournament phase (registration, in_progress, finished)

    // Tournament bracket tree (heap-style: children of i at 2*i+1 and 2*i+2)
    // Stores player indices. 0 means empty slot.
    u8 bracket[BRACKET_SIZE];
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
void entity_list_remove(EntityList *list1, EntityList *list2, String8 name);
void entity_list_register(EntityList *list1, EntityList *list2, String8 name1, String8 name2);
void entity_list_unregister(EntityList *list1, EntityList *list2, String8 name1, String8 name2);

void tournament_construct_bracket(EntityList *tournaments, String8 tournament_name);

#endif // PLAYERS_H
