#ifndef LAYOUT_H
#define LAYOUT_H

#include "clay.h"
#include "arena.h"
#include "players.h"
#include "string.h"

typedef struct {
    intptr_t offset;
    intptr_t memory;
} ClayVideoDemo_Arena;

typedef enum {
    PAGE_Dashboard,
    PAGE_Events,
    PAGE_Players,
    PAGE_Results,
} Page;

typedef struct ClayVideoDemo_Data ClayVideoDemo_Data;
struct ClayVideoDemo_Data {
    Arena *frameArena;
    float yOffset;

    Page selectedHeaderButton;

    u8 selectedTournamentIdx;

    EntityList players;
    EntityList tournaments;
};

typedef struct {
    int32_t requestedDocumentIndex;
} SidebarClickData;

// Utility Functions
Clay_String str8_to_clay(String8 str);

#endif // LAYOUT_H
