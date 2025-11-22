#ifndef LAYOUT_H
#define LAYOUT_H

#include "clay.h"
#include "arena.h"

typedef struct {
    intptr_t offset;
    intptr_t memory;
} ClayVideoDemo_Arena;

// typedef struct {
//     int32_t selectedDocumentIndex;
//     float yOffset;
//     ClayVideoDemo_Arena frameArena;
// } ClayVideoDemo_Data;

typedef struct ClayVideoDemo_Data ClayVideoDemo_Data;
struct ClayVideoDemo_Data {
    Arena *frameArena;
    float yOffset;
    
    u8 selectedHeaderButton;
};

typedef struct {
    int32_t requestedDocumentIndex;
} SidebarClickData;


#endif // LAYOUT_H
