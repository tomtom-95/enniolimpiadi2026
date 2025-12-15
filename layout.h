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

typedef enum Page
{
    PAGE_Dashboard = 0,
    PAGE_Events    = 1,
    PAGE_Players   = 2,
    PAGE_Results   = 3,
}
Page;

#define TEXTBOX_LIST                                                         \
    X(TEXTBOX_NULL,        "",                    "")                        \
    X(TEXTBOX_Events,      "EventNameInput",      "EventNameInputScroll")    \
    X(TEXTBOX_Players,     "PlayerNameInput",     "PlayerNameInputScroll")   \
    X(TEXTBOX_EventRename, "EventRenameInput",    "EventRenameInputScroll")  \
    X(TEXTBOX_PlayerRename,"PlayerRenameInput",   "PlayerRenameInputScroll") \
    X(TEXTBOX_Score1,      "Score1Input",         "Score1InputScroll")       \
    X(TEXTBOX_Score2,      "Score2Input",         "Score2InputScroll")

typedef enum TextBoxEnum
{
#define X(name, input_id, scroll_id) name,
    TEXTBOX_LIST
#undef X
    TEXTBOX_COUNT,
}
TextBoxEnum;

typedef enum ConfirmationModal
{
    MODAL_NULL = 0,

    MODAL_DELETE_TOURNAMENT,
    MODAL_DELETE_PLAYER,
    MODAL_RETURN_TO_REGISTRATION,
    MODAL_RETURN_TO_GROUP_PHASE
}
ConfirmationModal;

typedef enum DuplicateWarning
{
    DUPLICATE_NONE = 0,
    DUPLICATE_PLAYER_ADD,
    DUPLICATE_EVENT_ADD,
    DUPLICATE_PLAYER_RENAME,
    DUPLICATE_EVENT_RENAME,
}
DuplicateWarning;

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


typedef struct LayoutData LayoutData;
struct LayoutData {
    Arena *arena;      // Persistent arena for allocations
    Arena *frameArena; // Per-frame temporary arena
    float yOffset;
    Font *fonts;

    Page selectedHeaderButton;

    u8 selectedTournamentIdx;

    EntityList players;
    EntityList tournaments;

    // Text inputs
    TextBoxEnum focusedTextbox;
    TextInput textInputs[TEXTBOX_COUNT];

    // Mouse cursor (set once per frame to avoid flickering)
    int mouseCursor;

    // Zoom state for tournament chart
    float chartZoomLevel;

    // Active modal (MODAL_NULL = no modal)
    ConfirmationModal confirmationModal;

    // Data for delete tournament modal
    u32 deleteTournamentIdx;

    // Inline rename state for events (0 = not renaming)
    u32 renamingEventIdx;

    // Data for delete player modal
    u32 deletePlayerIdx;

    // Inline rename state for players (0 = not renaming)
    u32 renamingPlayerIdx;

    // Whether or not are we changing score
    bool modalScoreActive;

    // Data for score registration modal
    u32 scoreModalGroupIdx;
    u32 scoreModalRowIdx;   // row player slot index
    u32 scoreModalColIdx;   // column player slot index

    // Double-click tracking
    double lastClickTime;
    u32 lastClickCellId;

    // Duplicate name warning (shown until user interacts with textbox)
    DuplicateWarning duplicateWarning;
};

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

#endif // LAYOUT_H
