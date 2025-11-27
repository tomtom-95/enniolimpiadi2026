#ifndef LAYOUT_H
#define LAYOUT_H

#include "clay.h"
#include "arena.h"
#include "players.h"
#include "string.h"
#include "raylib/raylib.h"
#include "textbox.h"

typedef struct {
    intptr_t offset;
    intptr_t memory;
} ClayVideoDemo_Arena;

typedef enum Page {
    PAGE_Dashboard,
    PAGE_Events,
    PAGE_Players,
    PAGE_Results,
} Page;

typedef struct ClayVideoDemo_Data ClayVideoDemo_Data;
struct ClayVideoDemo_Data {
    Arena *frameArena;
    float yOffset;
    Font *fonts;

    Page selectedHeaderButton;

    u8 selectedTournamentIdx;

    EntityList players;
    EntityList tournaments;

    MedalsEnum medals[MAX_NUM_ENTITIES];

    // Text inputs
    TextInput eventNameInput;
    TextInput playerNameInput;
};

typedef struct {
    int32_t requestedDocumentIndex;
} SidebarClickData;

// Utility Functions
Clay_String str8_to_clay(String8 str);

#endif // LAYOUT_H
