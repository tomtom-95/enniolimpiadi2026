#ifndef LAYOUT_H
#define LAYOUT_H

#include "clay.h"

Clay_Sizing layoutExpand = {
    .width = CLAY_SIZING_GROW(0),
    .height = CLAY_SIZING_GROW(0)
};

Clay_Color contentBackgroundColor = { 90, 90, 90, 255 };

typedef struct {
    Clay_String title;
    Clay_String contents;
} Document;

typedef struct {
    Document *documents;
    uint32_t length;
} DocumentArray;

Document documentsRaw[5];

DocumentArray documents = {
    .length = 5,
    .documents = documentsRaw
};

typedef struct {
    intptr_t offset;
    intptr_t memory;
} ClayVideoDemo_Arena;

typedef struct {
    int32_t selectedDocumentIndex;
    float yOffset;
    ClayVideoDemo_Arena frameArena;
} ClayVideoDemo_Data;

typedef struct {
    int32_t requestedDocumentIndex;
    int32_t* selectedDocumentIndex;
} SidebarClickData;


#endif // LAYOUT_H
