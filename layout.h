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

#define TEXTBOX_LIST                                                     \
    X(TEXTBOX_NULL,        "",                    "")                     \
    X(TEXTBOX_Events,      "EventNameInput",      "EventNameInputScroll") \
    X(TEXTBOX_Players,     "PlayerNameInput",     "PlayerNameInputScroll")\
    X(TEXTBOX_EventRename, "EventRenameInput",    "EventRenameInputScroll")

typedef enum TextBoxEnum
{
#define X(name, input_id, scroll_id) name,
    TEXTBOX_LIST
#undef X
    TEXTBOX_COUNT,
}
TextBoxEnum;

static const char *TextBoxInputIds[] = {
#define X(name, input_id, scroll_id) input_id,
    TEXTBOX_LIST
#undef X
};

static const char *TextBoxScrollIds[] = {
#define X(name, input_id, scroll_id) scroll_id,
    TEXTBOX_LIST
#undef X
};


typedef struct ClayVideoDemo_Data ClayVideoDemo_Data;
struct ClayVideoDemo_Data {
    Arena *arena;      // Persistent arena for allocations
    Arena *frameArena; // Per-frame temporary arena
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

    // Mouse cursor (set once per frame to avoid flickering)
    int mouseCursor;

    // Zoom state for tournament chart
    float chartZoomLevel;

    // Confirmation dialog for returning to registration
    bool showReturnToRegistrationConfirm;

    // Confirmation dialog for deleting a tournament
    bool showDeleteTournamentConfirm;
    u32 deleteTournamentIdx;

    // Inline rename state for events (0 = not renaming)
    u32 renamingEventIdx;
};

typedef struct {
    int32_t requestedDocumentIndex;
} SidebarClickData;

typedef enum
{
    CUSTOM_LAYOUT_ELEMENT_TYPE_3D_MODEL,
    CUSTOM_LAYOUT_ELEMENT_TYPE_BRACKET_CONNECTIONS
} CustomLayoutElementType;

typedef struct
{
    Model model;
    float scale;
    Vector3 position;
    Matrix rotation;
} CustomLayoutElement_3DModel;

typedef struct
{
    u32 num_players;
    float zoom;
    float yOffset;
} CustomLayoutElement_BracketConnections;

typedef struct
{
    CustomLayoutElementType type;
    union {
        CustomLayoutElement_3DModel model;
        CustomLayoutElement_BracketConnections bracketConnections;
    } customData;
} CustomLayoutElement;

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
// void TextInput_HandleClick(TextInput *input, Clay_BoundingBox inputBox, Font *fonts, int fontId);
// void TextInput_Render(TextInput *input, Clay_String elementId, Clay_String scrollId, Clay_String placeholder, Font *fonts, int fontId);
// void TextInput_RenderCursor(TextInput *input, Clay_BoundingBox inputBox, Clay_ScrollContainerData scrollData, Font *fonts, int fontId);
// 
// // Layout functions
// void RenderHeaderButton(Clay_String text, Page page);
// void RenderDashboard(void);
// void RenderEventsActionsButtons(void);
// void RenderMatchSlot(Clay_String player1_name, Clay_String player2_name, bool player1_is_tbd, bool player2_is_tbd, u32 match_id);
// void RenderByeSlot(u32 match_id);
// void RenderTournamentChart(u32 tournament_idx);
// void RenderEvents(void);
// void RenderPlayers(void);
// void RenderResults(void);
//
// Clay_RenderCommandArray CreateLayout(void);

// Draws bezier curves connecting match slots in single elimination brackets
// Call this after CreateLayout() and Clay_Raylib_Render(), between BeginDrawing/EndDrawing
// void DrawBracketConnections(void);

#endif // LAYOUT_H
