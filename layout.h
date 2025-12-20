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
    u8 selectedPlayerIdx;

    PlayersList players;
    EventsList tournaments;

    // Text inputs
    TextBoxEnum focusedTextbox;
    TextInput textInputs[TEXTBOX_COUNT];

    // Mouse cursor (set once per frame to avoid flickering)
    int mouseCursor;

    // Zoom state for tournament chart
    float chartZoomLevel;
    float groupMatrixZoomLevel;

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

    // Panel visibility for Group+Knockout tournaments
    bool groupsPanelVisible;
    bool knockoutPanelVisible;
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

///////////////////////////////////////////////////////////////////////////////
// Extern declarations for global variables

extern const int FONT_ID_BODY_16;
extern const int FONT_ID_PRESS_START_2P;

extern const float SLOT_PLAYER_FONT_SIZE;
extern const float TEXTBOX_WIDTH;
extern const float TEXTBOX_HEIGHT;
extern const float TEXTBOX_PADDING;

extern Clay_Sizing layoutExpand;
extern LayoutData data;

// Colors
extern Clay_Color COLOR_WHITE;
extern Clay_Color COLOR_OFF_WHITE;
extern Clay_Color COLOR_BLACK;
extern Clay_Color COLOR_RED;
extern Clay_Color COLOR_GREEN;
extern Clay_Color COLOR_BLUE;

///////////////////////////////////////////////////////////////////////////////
// Utility Functions

Clay_String str8_to_clay(String8 str);
Clay_String str8_to_clay_truncated(Arena *arena, String8 str, u64 max_len);
void RenderDuplicateWarning(Clay_String parentId, Clay_String message, u32 uniqueId);

///////////////////////////////////////////////////////////////////////////////
// Event Handlers

void HandleHeaderButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleGoBackInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleEditTournament(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleOpenPlayer(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleGoBackFromPlayer(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandlePlayerEventRowClick(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleDeleteTournament(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleDeletePlayer(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleOuterContainerInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleZoomableHover(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleTextInput(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleAddEventButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleAddPlayerButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleTogglePlayerRegistration(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleAdvanceWinner(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleStartTournament(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleReturnToGroupPhase(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleReturnToRegistrationPhase(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleConfirmReturnToRegistration(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleCancelReturnToRegistration(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleConfirmReturnToGroupPhase(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleCancelReturnToGroupPhase(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleTerminateGroupPhase(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleConfirmDeleteTournament(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleCancelDeleteTournament(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleConfirmDeletePlayer(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleCancelDeletePlayer(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleSelectTournamentFormat(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleStartRenameEvent(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleConfirmRenameEvent(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleCancelRenameEvent(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleStartRenamePlayer(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleConfirmRenamePlayer(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleCancelRenamePlayer(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleIncrementGroupSize(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleDecrementGroupSize(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleIncrementAdvancePerGroup(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleDecrementAdvancePerGroup(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleMatrixCellClick(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleCancelScoreModal(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleConfirmScoreModal(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleToggleGroupsPanel(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);
void HandleToggleKnockoutPanel(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData);

///////////////////////////////////////////////////////////////////////////////
// TextBox Functions

void TextInput_ProcessKeyboard(TextInput *input);
void TextInput_Render(TextBoxEnum textBoxEnum, Clay_String elementId, Clay_String scrollId, Clay_String placeholder);
void TextInput_RenderCursor(TextInput *input, Clay_BoundingBox inputBox, Clay_ScrollContainerData scrollData);

///////////////////////////////////////////////////////////////////////////////
// Layout / Render Functions

void RenderHeaderButton(Clay_String text, Page page);
void RenderDashboard(void);

// Tournament
void RenderGoBackButton(void);
void RenderTournamentBanner(u32 tournament_idx);
void RenderFormatOption(TournamentFormat format, TournamentFormat current_format, Clay_String name, Clay_String description, u32 id);
void RenderRegistrationPanel(u32 tournament_idx, Event *tournament, s32 *registered_positions, u32 registered_count);
void RenderInProgressPanel(s32 *registered_positions, u32 registered_count);

// Match / Bracket Rendering
void RenderMatchSlot(Clay_String player1_name, Clay_String player2_name, u8 player1_idx, u8 player2_idx, u32 bracket_pos1, u32 bracket_pos2, u32 match_id, float zoom);
void RenderByeSlot(u32 match_id, float zoom);
void RenderGroupPhaseHeader(void);
void RenderKnockoutHeader(void);
void RenderKnockoutChart(u8 *bracket, u32 num_players);
void RenderGroupMatrix(Event *tournament, u32 group_idx, u32 players_in_group);
void RenderGroupsKnockoutChart(Event *tournament);

// Tournament Panels
void RenderTournamentLeftPanel(u32 tournament_idx);
void RenderTournamentRightPanel(u32 tournament_idx);
void RenderTournamentChart(u32 tournament_idx);

// Events Page
void RenderEventsActions(u32 tournament_idx);
void RenderEventsBanner(void);
void RenderEventsHeader(void);
void RenderEventsList(void);
void RenderEvents(void);

// Players Page
void RenderPlayersActions(u32 player_idx);
void RenderPlayersBanner(void);
void RenderPlayersHeader(void);
void RenderPlayersList(void);

// Player Detail Page
void RenderPlayerStatCard(u32 idx, Clay_String label, Clay_String value, Clay_Color accentColor);
void RenderPlayerEventRow(u32 tournament_idx, u32 player_idx);
void RenderPlayerGoBackButton(void);
void RenderPlayerBanner(u32 player_idx);
void RenderPlayerDetail(u32 player_idx);
void RenderAvailableEventRow(u32 tournament_idx, u32 player_idx);

// Main Pages
void RenderPlayers(void);
void RenderResults(void);

// Modal Overlays
void RenderConfirmationModal(void);
void RenderRenameModal(void);
void RenderRegisterScoreModal(void);

// Main Layout Entry Point
Clay_RenderCommandArray CreateLayout(void);

#endif // LAYOUT_H
