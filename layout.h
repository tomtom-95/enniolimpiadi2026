#ifndef LAYOUT_H
#define LAYOUT_H

#include "clay.h"
#include "arena.h"
#include "players.h"
#include "string.h"
#include "raylib/raylib.h"

#define TEXT_INPUT_MAX_LEN 256

typedef struct TextInput TextInput;
struct TextInput {
    char buffer[TEXT_INPUT_MAX_LEN];
    u32 len;
    u32 cursorPos;
    float blinkTimer;
};

typedef struct {
    intptr_t offset;
    intptr_t memory;
} ClayVideoDemo_Arena;

typedef enum Page
{
    PAGE_Dashboard,
    PAGE_Events,
    PAGE_Players,
    PAGE_Results,
}
Page;

typedef enum TextBoxEnum
{
    TEXTBOX_NULL,
    TEXTBOX_Events,
    TEXTBOX_Players,
    TEXTBOX_COUNT,
}
TextBoxEnum;


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
    TextBoxEnum focusedTextbox;
    TextInput textInputs[TEXTBOX_COUNT];
};

typedef struct {
    int32_t requestedDocumentIndex;
} SidebarClickData;

// // Utility Functions
// Clay_String str8_to_clay(String8 str);
// 
// // Event Handlers
// void HandleHeaderButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData);
// void HandleEventElementInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData);
// void HandleGoBack(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData);
// void HandleTextInputHover(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData);
// 
// // Texbox functions
// void TextInput_ProcessKeyboard(TextInput *input);
// void TextInput_UpdateCursorFromClick(TextInput *input, float clickRelativeX, Font *fonts, int fontId);
// void TextInput_HandleClick(TextInput *input, Clay_BoundingBox inputBox, Font *fonts, int fontId);
// void TextInput_Render(TextInput *input, Clay_String elementId, Clay_String scrollId, Clay_String placeholder, Font *fonts, int fontId);
// void TextInput_RenderCursor(TextInput *input, Clay_BoundingBox inputBox, Clay_ScrollContainerData scrollData, Font *fonts, int fontId);
// 
// // Layout functions
// void RenderHeaderButton(Clay_String text, Page page);
// void RenderDashboard(void);
// void RenderEventsHeaderElement(Clay_String string);
// void RenderEventElement(Clay_String element);
// void RenderEventsActionsButtons(void);
// void RenderMatchSlot(Clay_String player1_name, Clay_String player2_name, bool player1_is_tbd, bool player2_is_tbd, u32 match_id);
// void RenderByeSlot(u32 match_id);
// void RenderTournamentChart(u32 tournament_idx);
// void RenderEvents(void);
// void RenderPlayers(void);
// void RenderResults(void);
// 
// Clay_RenderCommandArray CreateLayout(void);

#endif // LAYOUT_H
