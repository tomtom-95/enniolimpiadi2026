#include <stdlib.h>
#include <assert.h>

#include "clay.h"
#include "core.h"
#include "arena.h"
#include "string.h"
#include "players.h"
#include "layout.h"

#include "raylib/raylib.h"

///////////////////////////////////////////////////////////////////////////////
// Colors

Clay_Color COLOR_WHITE     = { 255, 255, 255, 255};
Clay_Color COLOR_OFF_WHITE = { 245, 245, 245, 255};

Clay_Color COLOR_BLACK     = { 0, 0, 0, 255};
Clay_Color COLOR_RED       = { 255, 0, 0, 255};
Clay_Color COLOR_GREEN     = { 0, 255, 0, 255};
Clay_Color COLOR_BLUE      = { 0, 0, 255, 255};

Clay_Color stringColor            = { 0, 0, 0, 255};
Clay_Color stringColorOnClick     = { 135, 206, 235, 255 };
Clay_Color contentBackgroundColor = { 90, 90, 90, 255 };
Clay_Color headerBackgroundColor  = { 245, 245, 245, 255 };


Clay_Color headerButtonColor      = { 255, 255, 255, 255};
Clay_Color headerButtonHoverColor = { 240, 240, 240, 255 };
Clay_Color headerButtonStringClickColor = { 0, 0, 255, 255 };

Clay_Color dashboardWindowColor  = { 255, 255, 255, 255};

Clay_Color eventElementColor      = { 255, 255, 255, 255};
Clay_Color eventElementHoverColor = { 250, 250, 250, 255};

Clay_Color matchBorderColor       = { 200, 200, 200, 255};
Clay_Color matchVsColor           = { 150, 150, 150, 255};
Clay_Color matrixBorderColor      = { 160, 170, 180, 255};

Clay_Color goBackButtonColor      = { 230, 240, 250, 255};
Clay_Color goBackButtonHoverColor = { 200, 220, 245, 255};
Clay_Color goBackTextColor        = { 60, 100, 160, 255};
Clay_Color tournamentTitleColor   = { 40, 40, 60, 255};

// Dashboard colors
Clay_Color dashAccentCoral        = { 255, 107, 107, 255 };  // Coral red
Clay_Color dashAccentTeal         = { 72, 219, 195, 255 };   // Teal
Clay_Color dashAccentPurple       = { 155, 89, 182, 255 };   // Purple
Clay_Color dashAccentGold         = { 241, 196, 15, 255 };   // Golden
Clay_Color dashAccentOrange       = { 255, 159, 67, 255 };   // Orange
Clay_Color dashCardShadow         = { 0, 0, 0, 25 };         // Subtle shadow
Clay_Color dashStatNumber         = { 45, 52, 54, 255 };     // Dark slate for numbers
Clay_Color dashLabelText          = { 99, 110, 114, 255 };   // Gray for labels
Clay_Color dashCardBg             = { 253, 253, 253, 255 };  // Slightly warm white
Clay_Color dashBgGradientTop      = { 250, 247, 255, 255 };  // Light lavender
Clay_Color dashBgGradientBot      = { 255, 250, 245, 255 };  // Warm cream

// Accent colors for cycling through groups, header buttons, and other lists
Clay_Color groupAccentColors[] = {
    { 72, 219, 195, 255 },   // Teal (dashAccentTeal)
    { 255, 107, 107, 255 },  // Coral red (dashAccentCoral)
    { 155, 89, 182, 255 },   // Purple (dashAccentPurple)
    { 255, 159, 67, 255 },   // Orange (dashAccentOrange)
    { 241, 196, 15, 255 }    // Golden (dashAccentGold)
};

Clay_Color groupAccentColorsHover[] = {
    { 56, 195, 172, 255 },   // Darker teal (from 72,219,195)
    { 235, 77, 77, 255 },    // Darker coral (from 255,107,107)
    { 135, 69, 162, 255 },   // Darker purple (from 155,89,182)
    { 235, 139, 47, 255 },   // Darker orange (from 255,159,67)
    { 221, 176, 0, 255 }     // Darker gold (from 241,196,15)
};

Clay_Color textInputBackgroundColor = { 250, 250, 250, 255};
Clay_Color textInputBorderColor     = { 200, 200, 200, 255};
Clay_Color addButtonColor           = { 76, 175, 80, 255};
Clay_Color addButtonHoverColor      = { 56, 142, 60, 255};

Clay_Color removeButtonColor        = { 255, 235, 238, 255};  // Light red/pink
Clay_Color removeButtonHoverColor   = { 255, 205, 210, 255};  // Darker pink on hover
Clay_Color removeTextColor          = { 211, 47, 47, 255};    // Red text
Clay_Color addButtonTextColor       = { 255, 255, 255, 255};
Clay_Color playerRowColor           = { 255, 255, 255, 255};
Clay_Color playerRowHoverColor      = { 245, 245, 245, 255};

///////////////////////////////////////////////////////////////////////////////
// Globals

const int FONT_ID_BODY_16 = 0;
const int FONT_ID_PRESS_START_2P  = 1;

const float SLOT_PLAYER_FONT_SIZE = 16.0f;

const float TEXTBOX_WIDTH   = 300.0f;
const float TEXTBOX_HEIGHT  = 40.0f;
const float TEXTBOX_PADDING = 12.0f;

Clay_Sizing layoutExpand = {
    .width = CLAY_SIZING_GROW(0),
    .height = CLAY_SIZING_GROW(0)
};

LayoutData data = {0};

///////////////////////////////////////////////////////////////////////////////
// Utility Functions

Clay_String
str8_to_clay(String8 str)
{
    return (Clay_String){
        .length = (int)str.len,
        .chars = (const char *)str.str,
        .isStaticallyAllocated = false
    };
}

#define MAX_DISPLAY_NAME_LEN 14

Clay_String
str8_to_clay_truncated(Arena *arena, String8 str, u64 max_len)
{
    if (str.len <= max_len) {
        return str8_to_clay(str);
    }
    String8 truncated = str8_cat(arena, str8(str.str, max_len - 3), str8_lit("..."));
    return str8_to_clay(truncated);
}

void
RenderDuplicateWarning(Clay_String parentId, Clay_String message, u32 uniqueId)
{
    CLAY(CLAY_IDI("DuplicateWarning", uniqueId), {
        .layout = {
            .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
            .padding = { 6, 6, 4, 4 }
        },
        .backgroundColor = { 255, 240, 240, 255 },
        .cornerRadius = CLAY_CORNER_RADIUS(4),
        .border = { .width = {1, 1, 1, 1}, .color = COLOR_RED },
        .floating = {
            .attachTo = CLAY_ATTACH_TO_ELEMENT_WITH_ID,
            .parentId = Clay_GetElementId(parentId).id,
            .attachPoints = {
                .element = CLAY_ATTACH_POINT_LEFT_TOP,
                .parent = CLAY_ATTACH_POINT_LEFT_BOTTOM
            },
            .offset = { 0, 4 }
        }
    }) {
        CLAY_TEXT(message, CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 12,
            .textColor = COLOR_RED
        }));
    }
}

///////////////////////////////////////////////////////////////////////////////
// Event Handlers

void
HandleHeaderButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    Page page = *(Page *)userData;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        data.selectedHeaderButton = page;
    }
}

void
HandleGoBackInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    (void)userData;
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        data.selectedTournamentIdx = 0;
    }
}

void
HandleEditTournament(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        u32 tournament_idx = *(u32 *)userData;
        data.selectedTournamentIdx = tournament_idx;
    }
}

void
HandleOpenPlayer(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        u32 player_idx = *(u32 *)userData;
        data.selectedPlayerIdx = player_idx;
    }
}

void
HandleGoBackFromPlayer(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    (void)userData;
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        data.selectedPlayerIdx = 0;
    }
}

void
HandlePlayerEventRowClick(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        u32 tournament_idx = *(u32 *)userData;

        // Navigate to Events page and select this tournament
        data.selectedHeaderButton = PAGE_Events;
        data.selectedTournamentIdx = tournament_idx;
    }
}

void
HandleEnrollPlayerFromDetail(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        // Decode userData: lower 16 bits = player_idx, upper 16 bits = tournament_idx
        intptr_t encoded = *(intptr_t *)userData;
        u32 player_idx = (u32)(encoded & 0xFFFF);
        u32 tournament_idx = (u32)((encoded >> 16) & 0xFFFF);

        Player *player = data.players.players + player_idx;
        Event *tournament = data.tournaments.events + tournament_idx;

        register_player_to_event(&data.players, &data.tournaments, player->name, tournament->name);
    }
}

void
HandleUnregisterPlayerFromDetail(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        // Decode userData: lower 16 bits = player_idx, upper 16 bits = tournament_idx
        intptr_t encoded = *(intptr_t *)userData;
        u32 player_idx = (u32)(encoded & 0xFFFF);
        u32 tournament_idx = (u32)((encoded >> 16) & 0xFFFF);

        Player *player = data.players.players + player_idx;
        Event *tournament = data.tournaments.events + tournament_idx;

        // Only allow unregistration during registration phase
        if (tournament->phase == PHASE_REGISTRATION)
        {
            unregister_player_from_event(&data.players, &data.tournaments, player->name, tournament->name);
        }
    }
}

void
HandleDeleteTournament(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        u32 tournament_idx = *(u32 *)userData;
        data.deleteTournamentIdx = tournament_idx;
        data.confirmationModal = MODAL_DELETE_TOURNAMENT;
    }
}

void
HandleDeletePlayer(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        u32 player_idx = *(u32 *)userData;
        data.deletePlayerIdx = player_idx;
        data.confirmationModal = MODAL_DELETE_PLAYER;
    }
}

void
HandleOuterContainerInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    (void)userData;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        data.focusedTextbox = TEXTBOX_NULL;
    }
}

void
HandleZoomableHover(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    float *zoomLevel = (float *)userData;

    // Handle zoom with Cmd+/Cmd- while hovering over zoomable area
    bool cmdPressed = IsKeyDown(KEY_LEFT_SUPER) || IsKeyDown(KEY_RIGHT_SUPER);

    if (cmdPressed)
    {
        if (IsKeyPressed(KEY_RIGHT_BRACKET)) // Which is actually the "+" on the Mac with italian keyboard layout
        {
            // Zoom in (Cmd +)
            *zoomLevel += 0.1f;
            if (*zoomLevel > 3.0f)
            {
                *zoomLevel = 3.0f;
            }
        }
        if (IsKeyPressed(KEY_SLASH)) // Which is actually the "-" on the Mac with italian keyboard layout
        {
            // Zoom out (Cmd -)
            *zoomLevel -= 0.1f;
            if (*zoomLevel < 0.5f)
            {
                *zoomLevel = 0.5f;
            }
        }
    }
}

void
HandleTextInput(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_IBEAM;

    TextBoxEnum *pTextBoxEnum = (TextBoxEnum *)userData;
    TextBoxEnum textBoxEnum = *pTextBoxEnum;

    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        data.focusedTextbox = textBoxEnum;

        Clay_BoundingBox inputBox = Clay_GetElementData(elementId).boundingBox;
        Vector2 mousePos = GetMousePosition();

        // Get scroll offset for this textbox
        const char *scrollIdStr = TextBoxScrollIds[textBoxEnum];
        Clay_ElementId scrollId = Clay_GetElementId((Clay_String){ .length = strlen(scrollIdStr), .chars = scrollIdStr });
        Clay_ScrollContainerData scrollData = Clay_GetScrollContainerData(scrollId);
        float scrollOffsetX = (scrollData.found && scrollData.scrollPosition) ? scrollData.scrollPosition->x : 0.0f;

        float clickRelativeX = mousePos.x - inputBox.x - TEXTBOX_PADDING - scrollOffsetX;

        // Update cursor
        TextInput *input = &data.textInputs[textBoxEnum];

        if (clickRelativeX < 0)
        {
            clickRelativeX = 0;
        }

        u32 newCursorPos = 0;
        float prevWidth = 0;
        for (u32 i = 0; i <= input->len; i++) {
            char tempBuffer[TEXT_INPUT_MAX_LEN];
            for (u32 j = 0; j < i; j++) {
                tempBuffer[j] = input->buffer[j];
            }
            tempBuffer[i] = '\0';

            Vector2 textSize = MeasureTextEx(data.fonts[FONT_ID_BODY_16], tempBuffer, 16.0f, 0);

            if (clickRelativeX <= textSize.x) {
                float midpoint = (prevWidth + textSize.x) / 2.0f;
                if (clickRelativeX < midpoint) {
                    newCursorPos = i > 0 ? i - 1 : 0;
                } else {
                    newCursorPos = i;
                }
                break;
            }
            prevWidth = textSize.x;
            newCursorPos = i;
        }
        input->cursorPos = newCursorPos;
        input->blinkTimer = 0.0f;
    }
}

void
HandleAddEventButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        TextInput *textInput = &data.textInputs[TEXTBOX_Events];
        String8 eventName = str8_copy(data.arena, str8((u8 *)textInput->buffer, textInput->len));
        String8 eventNameStripped = str8_strip_whitespace(eventName);
        if (eventNameStripped.len != 0)
        {
            // Check if event with this name already exists
            u32 idx_tail = data.tournaments.len - 1;
            u32 existing_idx = events_list_find(&data.tournaments, eventNameStripped);
            if (existing_idx != idx_tail)
            {
                // Event already exists, show warning
                data.duplicateWarning = DUPLICATE_EVENT_ADD;
            }
            else
            {
                events_list_add(&data.tournaments, eventNameStripped);
            }
        }
    }
}

void
HandleAddPlayerButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        TextInput *textInput = &data.textInputs[TEXTBOX_Players];
        String8 playerName = str8_copy(data.arena, str8((u8 *)textInput->buffer, textInput->len));
        String8 playerNameStripped = str8_strip_whitespace(playerName);
        if (playerNameStripped.len != 0)
        {
            // Check if player with this name already exists
            u32 idx_tail = data.players.len - 1;
            u32 existing_idx = players_list_find(&data.players, playerNameStripped);
            if (existing_idx != idx_tail)
            {
                // Player already exists, show warning
                data.duplicateWarning = DUPLICATE_PLAYER_ADD;
            }
            else
            {
                players_list_add(&data.players, playerNameStripped);
            }
        }
    }
}

void
HandleTogglePlayerRegistration(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        u32 player_idx = *(u32 *)userData;
        Player *player = data.players.players + player_idx;
        Event *tournament = data.tournaments.events + data.selectedTournamentIdx;

        // Check if player is already registered to this tournament
        bool is_registered = (player->registrations >> data.selectedTournamentIdx) & 1;

        if (is_registered)
        {
            unregister_player_from_event(&data.players, &data.tournaments, player->name, tournament->name);
        }
        else
        {
            register_player_to_event(&data.players, &data.tournaments, player->name, tournament->name);
        }
    }
}

void
HandleAdvanceWinner(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;

    // Decode userData: lower 8 bits = player_idx, upper bits = bracket_pos
    intptr_t encoded = *(intptr_t *)userData;
    u8 player_idx = (u8)(encoded & 0xFF);
    u32 bracket_pos = (u32)(encoded >> 8);

    Event *tournament = data.tournaments.events + data.selectedTournamentIdx;

    // Only allow changes when tournament is in progress
    if (tournament->phase == PHASE_REGISTRATION) return;

    // Select the appropriate bracket based on tournament format
    u8 *bracket = (tournament->format == FORMAT_GROUP_KNOCKOUT)
        ? tournament->group_phase.bracket
        : tournament->bracket;

    // Calculate parent position in the bracket tree
    u32 parent_pos = (bracket_pos - 1) / 2;

    // Right-click: undo advancement (cascade reset up the tree)
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    {
        // Traverse up the tree and reset all ancestor slots that contain this player
        u32 pos = bracket_pos;
        while (pos > 0)
        {
            u32 parent = (pos - 1) / 2;
            if (bracket[parent] == player_idx)
            {
                bracket[parent] = 0;
            }
            pos = parent;
        }
    }
    // Left-click: advance non-TBD player
    else if (player_idx != 0 && pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        bracket[parent_pos] = player_idx;
    }
}

void
HandleStartTournament(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        Event *tournament = data.tournaments.events + data.selectedTournamentIdx;

        // Only start if we have at least 2 players
        s32 positions[64];
        u32 num_players = find_all_filled_slots(tournament->registrations, positions);
        if (num_players >= 2)
        {
            tournament->phase = (tournament->format == FORMAT_GROUP_KNOCKOUT)
                ? PHASE_GROUP
                : PHASE_KNOCKOUT;
        }
    }
}

void
HandleReturnToGroupPhase(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        data.confirmationModal = MODAL_RETURN_TO_GROUP_PHASE;
    }
}

void
HandleReturnToRegistrationPhase(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        data.confirmationModal = MODAL_RETURN_TO_REGISTRATION;
    }
}

void
HandleConfirmReturnToRegistration(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        Event *tournament = data.tournaments.events + data.selectedTournamentIdx;
        tournament->phase = PHASE_REGISTRATION;
        data.confirmationModal = MODAL_NULL;

        tournament_construct_groups(tournament);
        tournament_construct_bracket(tournament);
        tournament_populate_bracket_from_groups(tournament);
    }
}

void
HandleCancelReturnToRegistration(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        data.confirmationModal = MODAL_NULL;
    }
}

void
HandleConfirmReturnToGroupPhase(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        Event *tournament = data.tournaments.events + data.selectedTournamentIdx;
        tournament->phase = PHASE_GROUP;
        data.confirmationModal = MODAL_NULL;

        tournament_construct_bracket(tournament);
        tournament_populate_bracket_from_groups(tournament);
    }
}

void
HandleCancelReturnToGroupPhase(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        data.confirmationModal = MODAL_NULL;
    }
}

void
HandleTerminateGroupPhase(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        Event *tournament = data.tournaments.events + data.selectedTournamentIdx;
        tournament->phase = PHASE_KNOCKOUT;
    }
}

void
HandleConfirmDeleteTournament(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    (void)userData;
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        Event *tournament = data.tournaments.events + data.deleteTournamentIdx;
        events_list_remove(&data.tournaments, &data.players, tournament->name);
        data.deleteTournamentIdx = 0;
        data.confirmationModal = MODAL_NULL;
    }
}

void
HandleCancelDeleteTournament(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    (void)userData;
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        data.deleteTournamentIdx = 0;
        data.confirmationModal = MODAL_NULL;
    }
}

void
HandleConfirmDeletePlayer(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    (void)userData;
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        Player *player = data.players.players + data.deletePlayerIdx;
        players_list_remove(&data.players, &data.tournaments, player->name);
        data.deletePlayerIdx = 0;
        data.confirmationModal = MODAL_NULL;
    }
}

void
HandleCancelDeletePlayer(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    (void)userData;
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        data.deletePlayerIdx = 0;
        data.confirmationModal = MODAL_NULL;
    }
}

void
HandleSelectTournamentFormat(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        TournamentFormat format = *(TournamentFormat *)userData;
        Event *tournament = data.tournaments.events + data.selectedTournamentIdx;
        tournament->format = format;
    }
}

void
HandleStartRenameEvent(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        u32 event_idx = *(u32 *)userData;
        data.renamingEventIdx = event_idx;
        data.focusedTextbox = TEXTBOX_EventRename;

        // Pre-fill the textbox with the current event name
        Event *tournament = data.tournaments.events + event_idx;
        TextInput *input = &data.textInputs[TEXTBOX_EventRename];
        u32 copy_len = tournament->name.len < TEXT_INPUT_MAX_LEN - 1
            ? (u32)tournament->name.len
            : TEXT_INPUT_MAX_LEN - 1;
        for (u32 i = 0; i < copy_len; i++)
        {
            input->buffer[i] = (char)tournament->name.str[i];
        }
        input->buffer[copy_len] = '\0';
        input->len = copy_len;
        input->cursorPos = copy_len;
        input->blinkTimer = 0.0f;
    }
}

void
HandleConfirmRenameEvent(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        if (data.renamingEventIdx != 0)
        {
            TextInput *input = &data.textInputs[TEXTBOX_EventRename];

            if (input->len > 0)
            {
                // NOTE: this is a memory leak but who cares
                String8 new_name = str8_copy(data.arena, str8((u8 *)input->buffer, input->len));

                // Check if another event with this name already exists
                u32 idx_tail = data.tournaments.len - 1;
                u32 existing_idx = events_list_find(&data.tournaments, new_name);
                if (existing_idx != idx_tail && existing_idx != data.renamingEventIdx)
                {
                    // Event already exists, show warning
                    data.duplicateWarning = DUPLICATE_EVENT_RENAME;
                    return;
                }

                events_list_rename(&data.tournaments, data.renamingEventIdx, new_name);
            }

            // Clear rename state
            data.renamingEventIdx = 0;
            data.focusedTextbox = TEXTBOX_NULL;
            data.duplicateWarning = DUPLICATE_NONE;
        }
    }
}

void
HandleCancelRenameEvent(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    (void)userData;
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        data.renamingEventIdx = 0;
        data.focusedTextbox = TEXTBOX_NULL;
        data.duplicateWarning = DUPLICATE_NONE;
    }
}

void
HandleStartRenamePlayer(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        u32 player_idx = *(u32 *)userData;
        data.renamingPlayerIdx = player_idx;
        data.focusedTextbox = TEXTBOX_PlayerRename;

        // Pre-fill the textbox with the current player name
        Player *player = data.players.players + player_idx;
        TextInput *input = &data.textInputs[TEXTBOX_PlayerRename];
        u32 copy_len = player->name.len < TEXT_INPUT_MAX_LEN - 1
            ? (u32)player->name.len
            : TEXT_INPUT_MAX_LEN - 1;
        for (u32 i = 0; i < copy_len; i++)
        {
            input->buffer[i] = (char)player->name.str[i];
        }
        input->buffer[copy_len] = '\0';
        input->len = copy_len;
        input->cursorPos = copy_len;
        input->blinkTimer = 0.0f;
    }
}

void
HandleConfirmRenamePlayer(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    (void)userData;
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        if (data.renamingPlayerIdx != 0)
        {
            TextInput *input = &data.textInputs[TEXTBOX_PlayerRename];

            if (input->len > 0)
            {
                // NOTE: this is a memory leak but who cares
                String8 new_name = str8_copy(data.arena, str8((u8 *)input->buffer, input->len));

                // Check if another player with this name already exists
                u32 idx_tail = data.players.len - 1;
                u32 existing_idx = players_list_find(&data.players, new_name);
                if (existing_idx != idx_tail && existing_idx != data.renamingPlayerIdx)
                {
                    // Player already exists, show warning
                    data.duplicateWarning = DUPLICATE_PLAYER_RENAME;
                    return;
                }

                players_list_rename(&data.players, data.renamingPlayerIdx, new_name);
            }

            // Clear rename state
            data.renamingPlayerIdx = 0;
            data.focusedTextbox = TEXTBOX_NULL;
            data.duplicateWarning = DUPLICATE_NONE;
        }
    }
}

void
HandleCancelRenamePlayer(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    (void)userData;
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        data.renamingPlayerIdx = 0;
        data.focusedTextbox = TEXTBOX_NULL;
        data.duplicateWarning = DUPLICATE_NONE;
    }
}

void
HandleIncrementGroupSize(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    // (void)userData;
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        Event *tournament = data.tournaments.events + data.selectedTournamentIdx;
        if (tournament->group_phase.group_size < MAX_GROUP_SIZE)
        {
            tournament->group_phase.group_size++;
        }

        tournament_construct_groups(tournament);
    }
}

void
HandleDecrementGroupSize(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    // (void)userData;
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        Event *tournament = data.tournaments.events + data.selectedTournamentIdx;
        if (tournament->group_phase.group_size > 2)
        {
            tournament->group_phase.group_size--;
        }

        tournament_construct_groups(tournament);
    }
}

void
HandleIncrementAdvancePerGroup(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    (void)elementId;
    (void)userData;
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        Event *tournament = data.tournaments.events + data.selectedTournamentIdx;
        // Cannot advance more players than are in the group
        if (tournament->group_phase.advance_per_group < tournament->group_phase.group_size - 1)
        {
            tournament->group_phase.advance_per_group++;
        }
    }
}

void
HandleDecrementAdvancePerGroup(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    (void)elementId;
    (void)userData;
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        Event *tournament = data.tournaments.events + data.selectedTournamentIdx;
        if (tournament->group_phase.advance_per_group > 1)
        {
            tournament->group_phase.advance_per_group--;
        }
    }
}

typedef struct {
    u32 group_idx;
    u32 row_idx;
    u32 col_idx;
    u32 cell_id;
} MatrixCellData;

#define DOUBLE_CLICK_THRESHOLD 0.3  // 300ms for double-click

void
HandleMatrixCellClick(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    MatrixCellData *cellData = (MatrixCellData *)userData;

    // Only allow score entry when tournament is in progress
    Event *tournament = data.tournaments.events + data.selectedTournamentIdx;
    if (tournament->phase == PHASE_REGISTRATION)
    {
        return;
    }

    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;

    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        double currentTime = GetTime();

        // Check for double-click on the same cell
        if (data.lastClickCellId == cellData->cell_id &&
            (currentTime - data.lastClickTime) < DOUBLE_CLICK_THRESHOLD)
        {
            // Double-click detected - open score modal
            data.modalScoreActive = true;

            data.scoreModalGroupIdx = cellData->group_idx;
            data.scoreModalRowIdx   = cellData->row_idx;
            data.scoreModalColIdx   = cellData->col_idx;

            // Clear score input textboxes
            data.textInputs[TEXTBOX_Score1].buffer[0] = '\0';
            data.textInputs[TEXTBOX_Score1].len = 0;
            data.textInputs[TEXTBOX_Score1].cursorPos = 0;
            data.textInputs[TEXTBOX_Score2].buffer[0] = '\0';
            data.textInputs[TEXTBOX_Score2].len = 0;
            data.textInputs[TEXTBOX_Score2].cursorPos = 0;

            // Focus the first textbox
            data.focusedTextbox = TEXTBOX_Score1;

            // Reset to prevent triple-click triggering again
            data.lastClickTime = 0;
            data.lastClickCellId = 0;
        }
        else
        {
            // First click - record time and cell
            data.lastClickTime = currentTime;
            data.lastClickCellId = cellData->cell_id;
        }
    }
}

void
HandleCancelScoreModal(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        data.modalScoreActive = false;
        data.focusedTextbox = TEXTBOX_NULL;
    }
}

void
HandleConfirmScoreModal(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        // Parse scores from textboxes
        TextInput *input1 = &data.textInputs[TEXTBOX_Score1];
        TextInput *input2 = &data.textInputs[TEXTBOX_Score2];

        u16 score1 = 0;
        u16 score2 = 0;

        // Parse score 1 (simple atoi-style)
        for (u32 i = 0; i < input1->len; i++)
        {
            char c = input1->buffer[i];
            if (c >= '0' && c <= '9')
            {
                score1 = score1 * 10 + (c - '0');
            }
        }

        // Parse score 2
        for (u32 i = 0; i < input2->len; i++)
        {
            char c = input2->buffer[i];
            if (c >= '0' && c <= '9')
            {
                score2 = score2 * 10 + (c - '0');
            }
        }

        // Save to the tournament's group phase
        Event *tournament = data.tournaments.events + data.selectedTournamentIdx;
        u32 g = data.scoreModalGroupIdx;
        u32 row = data.scoreModalRowIdx;
        u32 col = data.scoreModalColIdx;

        // Store the score for this match
        tournament->group_phase.scores[g][row][col].row_score = score1;
        tournament->group_phase.scores[g][row][col].col_score = score2;

        // Also store the mirror entry (col vs row) with swapped scores
        tournament->group_phase.scores[g][col][row].row_score = score2;
        tournament->group_phase.scores[g][col][row].col_score = score1;

        // Close the modal
        data.modalScoreActive = false;
        data.focusedTextbox = TEXTBOX_NULL;

        tournament_populate_bracket_from_groups(tournament);
    }
}

void
HandleToggleGroupsPanel(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    (void)elementId;
    (void)userData;
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        data.groupsPanelVisible = !data.groupsPanelVisible;
    }
}

void
HandleToggleKnockoutPanel(Clay_ElementId elementId, Clay_PointerData pointerData, void *userData)
{
    (void)elementId;
    (void)userData;
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        data.knockoutPanelVisible = !data.knockoutPanelVisible;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Texbox functions

void
TextInput_ProcessKeyboard(TextInput *input)
{
    float dt = GetFrameTime();

    // Update blink timer
    input->blinkTimer += dt;
    if (input->blinkTimer > 1.0f) {
        input->blinkTimer -= 1.0f;
    }

    // Handle character input - insert at cursor position
    const float maxTextWidth = TEXTBOX_WIDTH - (TEXTBOX_PADDING * 2);

    int key = GetCharPressed();
    while (key > 0)
    {
        // Clear duplicate warning when user types
        data.duplicateWarning = DUPLICATE_NONE;

        // TODO: this must be refactored when I want to really get a good textbox done
        if (key >= 32 && key <= 125 && input->len < TEXT_INPUT_MAX_LEN - 1)
        {
            // Build temporary buffer with the new character inserted
            char tempBuffer[TEXT_INPUT_MAX_LEN];
            for (u32 i = 0; i < input->cursorPos; i++)
            {
                tempBuffer[i] = input->buffer[i];
            }
            tempBuffer[input->cursorPos] = (char)key;
            for (u32 i = input->cursorPos; i < input->len; i++)
            {
                tempBuffer[i + 1] = input->buffer[i];
            }
            tempBuffer[input->len + 1] = '\0';

            // Check if the new text would fit
            Vector2 textSize = MeasureTextEx(data.fonts[FONT_ID_BODY_16], tempBuffer, 16.0f, 0);

            char tempBuffer2[TEXT_INPUT_MAX_LEN];
            for (u32 i = 0; i < input->len; i++)
            {
                tempBuffer2[i] = tempBuffer[i];
            }
            tempBuffer2[input->len] = '\0';
            Vector2 textSize2 = MeasureTextEx(data.fonts[FONT_ID_BODY_16], tempBuffer2, 16.0f, 0);
            Vector2 wideCharSize = MeasureTextEx(data.fonts[FONT_ID_BODY_16], "M", 16.0f, 0);

            if (textSize2.x + wideCharSize.x < maxTextWidth)
            {
                // Shift characters after cursor to make room
                for (u32 i = input->len; i > input->cursorPos; i--)
                {
                    input->buffer[i] = input->buffer[i - 1];
                }
                input->buffer[input->cursorPos] = (char)key;
                input->len++;
                input->cursorPos++;
                input->buffer[input->len] = '\0';
                input->blinkTimer = 0.0f;
            }
        }
        key = GetCharPressed();
    }

    // Handle backspace - delete character before cursor
    if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
        // Clear duplicate warning when user deletes
        data.duplicateWarning = DUPLICATE_NONE;
        if (input->cursorPos > 0) {
            for (u32 i = input->cursorPos - 1; i < input->len - 1; i++) {
                input->buffer[i] = input->buffer[i + 1];
            }
            input->len--;
            input->cursorPos--;
            input->buffer[input->len] = '\0';
            input->blinkTimer = 0.0f;
        }
    }

    // Handle delete - delete character at cursor
    if (IsKeyPressed(KEY_DELETE) || IsKeyPressedRepeat(KEY_DELETE)) {
        // Clear duplicate warning when user deletes
        data.duplicateWarning = DUPLICATE_NONE;
        if (input->cursorPos < input->len) {
            for (u32 i = input->cursorPos; i < input->len - 1; i++) {
                input->buffer[i] = input->buffer[i + 1];
            }
            input->len--;
            input->buffer[input->len] = '\0';
            input->blinkTimer = 0.0f;
        }
    }

    // Handle left arrow
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)) {
        if (input->cursorPos > 0) {
            input->cursorPos--;
            input->blinkTimer = 0.0f;
        }
    }

    // Handle right arrow
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)) {
        if (input->cursorPos < input->len) {
            input->cursorPos++;
            input->blinkTimer = 0.0f;
        }
    }

    // Handle Home key
    if (IsKeyPressed(KEY_HOME)) {
        input->cursorPos = 0;
        input->blinkTimer = 0.0f;
    }

    // Handle End key
    if (IsKeyPressed(KEY_END)) {
        input->cursorPos = input->len;
        input->blinkTimer = 0.0f;
    }
}

void
TextInput_Render(TextBoxEnum textBoxEnum, Clay_String elementId,
    Clay_String scrollId, Clay_String placeholder)
{
    bool isFocused = (data.focusedTextbox == textBoxEnum);
    TextInput *input = &data.textInputs[textBoxEnum];

    // Check if current text is at max width
    const float maxTextWidth = TEXTBOX_WIDTH - (TEXTBOX_PADDING * 2);
    bool isAtMaxWidth = false;
    if (input->len > 0)
    {
        Vector2 textSize = MeasureTextEx(data.fonts[FONT_ID_BODY_16], input->buffer, 16.0f, 0);

        // Consider at max if adding even a narrow character would overflow
        // Use 'i' as reference for the narrowest typical character
        Vector2 narrowCharSize = MeasureTextEx(data.fonts[FONT_ID_BODY_16], "i", 16.0f, 0);
        Vector2 wideCharSize = MeasureTextEx(data.fonts[FONT_ID_BODY_16], "M", 16.0f, 0);
        isAtMaxWidth = (textSize.x + wideCharSize.x > maxTextWidth);
    }

    // Use red border when at max width, otherwise normal border color
    Clay_Color inputBorderColor = isAtMaxWidth ? COLOR_RED :
                                  (isFocused ? COLOR_BLUE : textInputBorderColor);

    // Outer container: styling (background, border, corner radius)
    CLAY(Clay_GetElementId(elementId), {
        .layout = {
            .sizing = {.width = CLAY_SIZING_FIXED(TEXTBOX_WIDTH), .height = CLAY_SIZING_FIXED(TEXTBOX_HEIGHT)},
            .padding = { (u16)TEXTBOX_PADDING, (u16)TEXTBOX_PADDING, 0, 0 },
        },
        .backgroundColor = textInputBackgroundColor,
        .cornerRadius = CLAY_CORNER_RADIUS(4),
        .border = { .width = {1, 1, 1, 1}, .color = inputBorderColor }
    }) {
        TextBoxEnum *pTextBoxEnum = push_array(data.frameArena, TextBoxEnum, 1);
        *pTextBoxEnum = textBoxEnum;
        Clay_OnHover(HandleTextInput, pTextBoxEnum);

        // Inner container with scroll
        CLAY(Clay_GetElementId(scrollId), {
            .layout = {
                .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
                .childAlignment = { .x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER }
            },
        }) {
            if (input->len > 0)
            {
                Clay_String inputText = {
                    .length = (int)input->len,
                    .chars = input->buffer,
                    .isStaticallyAllocated = false
                };
                CLAY_TEXT(inputText, CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 16,
                    .textColor = stringColor
                }));
            }
            else if (textBoxEnum != data.focusedTextbox)
            {
                // Show placeholder when empty and not focused
                CLAY_TEXT(placeholder, CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 16,
                    .textColor = matchVsColor
                }));
            }
        }

        // Floating warning message - doesn't affect layout
        if (isAtMaxWidth)
        {
            CLAY(CLAY_IDI("TextInputWarning", textBoxEnum), {
                .layout = {
                    .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                    .padding = { 6, 6, 4, 4 }
                },
                .backgroundColor = { 255, 240, 240, 255 },
                .cornerRadius = CLAY_CORNER_RADIUS(4),
                .border = { .width = {1, 1, 1, 1}, .color = COLOR_RED },
                .floating = {
                    .attachTo = CLAY_ATTACH_TO_ELEMENT_WITH_ID,
                    .parentId = Clay_GetElementId(elementId).id,
                    .attachPoints = {
                        .element = CLAY_ATTACH_POINT_LEFT_TOP,
                        .parent = CLAY_ATTACH_POINT_LEFT_BOTTOM
                    },
                    .offset = { 0, 4 }
                }
            }) {
                CLAY_TEXT(CLAY_STRING("Maximum length reached"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 12,
                    .textColor = COLOR_RED
                }));
            }
        }
    }
}

void
TextInput_RenderCursor(TextInput *input, Clay_BoundingBox inputBox,
    Clay_ScrollContainerData scrollData)
{
    bool cursorVisible = input->blinkTimer < 0.5f;
    if (!cursorVisible) return;

    // Calculate cursor X offset based on text before cursor
    float cursorOffsetX = 0;
    if (input->cursorPos > 0)
    {
        char beforeCursor[TEXT_INPUT_MAX_LEN];
        for (u32 i = 0; i < input->cursorPos; i++) {
            beforeCursor[i] = input->buffer[i];
        }
        beforeCursor[input->cursorPos] = '\0';
        Vector2 textSize = MeasureTextEx(data.fonts[FONT_ID_BODY_16], beforeCursor, 16.0f, 0);
        cursorOffsetX = textSize.x;
    }

    float scrollOffsetX = (scrollData.found && scrollData.scrollPosition) ? scrollData.scrollPosition->x : 0.0f;
    float cursorX = inputBox.x + TEXTBOX_PADDING + cursorOffsetX + scrollOffsetX;
    float cursorY = inputBox.y + (inputBox.height - 18.0f) / 2.0f;

    // Clip cursor to textbox bounds
    float clipLeft = inputBox.x + TEXTBOX_PADDING;
    float clipRight = inputBox.x + inputBox.width - TEXTBOX_PADDING;

    if (cursorX >= clipLeft && cursorX <= clipRight)
    {
        DrawRectangle((int)cursorX, (int)cursorY, 1, 18, BLACK);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Layout functions

void
RenderHeaderButton(Clay_String text, Page page)
{
    bool isSelected = (data.selectedHeaderButton == page);
    Clay_Color accentColor = groupAccentColors[page % ArrayCount(groupAccentColors)];
    Clay_Color accentColorHover = groupAccentColorsHover[page % ArrayCount(groupAccentColorsHover)];

    // Outer container with accent bar on top
    CLAY(CLAY_IDI("HeaderButtonOuter", page), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0) }
        },
        .cornerRadius = CLAY_CORNER_RADIUS(8)
    }) {
        CLAY(CLAY_IDI("HeaderButton", page), {
            .layout = { .padding = { 16, 16, 8, 8 }},
            .backgroundColor = Clay_Hovered() ? accentColorHover: accentColor,
            .cornerRadius = { 8, 8, 0, 0 }
        }) {
            Page *pPage = push_array(data.frameArena, Page, 1);
            *pPage = page;
            Clay_OnHover(HandleHeaderButtonInteraction, pPage);
            CLAY_TEXT(text, CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_PRESS_START_2P,
                .fontSize = 24,
                .textColor = COLOR_WHITE
            }));
        }
    }
}

void
RenderDashboard(void)
{
    CLAY(CLAY_ID("Dashboard"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = layoutExpand,
            .childGap = 20,
            .padding = { 8, 8, 8, 8 },
            .childAlignment = {
                .y = CLAY_ALIGN_Y_TOP
            }
        },
        .clip = { .horizontal = true, .childOffset = Clay_GetScrollOffset() },
        .backgroundColor = dashBgGradientTop
    }) {
        CLAY(CLAY_ID("WelcomeBanner"), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                .padding = { 24, 24, 20, 20 },
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = groupAccentColors[PAGE_Dashboard],
            .cornerRadius = CLAY_CORNER_RADIUS(16)
        }) {
            CLAY_TEXT(CLAY_STRING("ENNIOLIMPIADI 2026"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_PRESS_START_2P,
                .fontSize = 36,
                .textColor = COLOR_WHITE
            }));
        }

        // Stats row - horizontal cards
        CLAY(CLAY_ID("StatsRow"), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                .childGap = 16
            }
        }) {
            // Events card with coral accent
            CLAY(CLAY_ID("EventsCardOuter"), {
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)}
                },
                .cornerRadius = CLAY_CORNER_RADIUS(12)
            }) {
                // Colored accent bar
                CLAY(CLAY_ID("EventsAccent"), {
                    .layout = {
                        .sizing = {.height = CLAY_SIZING_FIXED(6), .width = CLAY_SIZING_GROW(0)}
                    },
                    .backgroundColor = dashAccentCoral,
                    .cornerRadius = { 12, 12, 0, 0 }
                }) {}
                // Card content
                CLAY(CLAY_ID("EventsCard"), {
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                        .padding = { 20, 20, 16, 20 },
                        .childGap = 8,
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                    },
                    .backgroundColor = dashCardBg,
                    .cornerRadius = { 0, 0, 12, 12 }
                }) {
                    CLAY_TEXT(CLAY_STRING("EVENTS"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 12,
                        .textColor = dashLabelText
                    }));
                    u32 tournaments_count = events_list_count(&data.tournaments);
                    String8 tournaments_count_str8 = str8_from_u32(data.frameArena, tournaments_count);
                    Clay_String tournaments_count_clay = str8_to_clay(tournaments_count_str8);
                    CLAY_TEXT(tournaments_count_clay, CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_PRESS_START_2P,
                        .fontSize = 48,
                        .textColor = dashAccentCoral
                    }));
                }
            }

            // Players card with teal accent
            CLAY(CLAY_ID("DashboardPlayersCardOuter"), {
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)}
                },
                .cornerRadius = CLAY_CORNER_RADIUS(12)
            }) {
                CLAY(CLAY_ID("PlayersAccent"), {
                    .layout = {
                        .sizing = {.height = CLAY_SIZING_FIXED(6), .width = CLAY_SIZING_GROW(0)}
                    },
                    .backgroundColor = dashAccentTeal,
                    .cornerRadius = { 12, 12, 0, 0 }
                }) {}
                CLAY(CLAY_ID("PlayersCard"), {
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                        .padding = { 20, 20, 16, 20 },
                        .childGap = 8,
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                    },
                    .backgroundColor = dashCardBg,
                    .cornerRadius = { 0, 0, 12, 12 }
                }) {
                    CLAY_TEXT(CLAY_STRING("PLAYERS"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 12,
                        .textColor = dashLabelText
                    }));
                    u32 players_count = players_list_count(&data.players);
                    String8 players_count_str8 = str8_from_u32(data.frameArena, players_count);
                    Clay_String players_count_clay = str8_to_clay(players_count_str8);
                    CLAY_TEXT(players_count_clay, CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_PRESS_START_2P,
                        .fontSize = 48,
                        .textColor = dashAccentTeal
                    }));
                }
            }

            // Finished events card with gold accent
            CLAY(CLAY_ID("FinishedCardOuter"), {
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)}
                },
                .cornerRadius = CLAY_CORNER_RADIUS(12)
            }) {
                CLAY(CLAY_ID("FinishedAccent"), {
                    .layout = {
                        .sizing = {.height = CLAY_SIZING_FIXED(6), .width = CLAY_SIZING_GROW(0)}
                    },
                    .backgroundColor = dashAccentGold,
                    .cornerRadius = { 12, 12, 0, 0 }
                }) {}
                CLAY(CLAY_ID("FinishedCard"), {
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                        .padding = { 20, 20, 16, 20 },
                        .childGap = 8,
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                    },
                    .backgroundColor = dashCardBg,
                    .cornerRadius = { 0, 0, 12, 12 }
                }) {
                    CLAY_TEXT(CLAY_STRING("COMPLETED"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 12,
                        .textColor = dashLabelText
                    }));
                    CLAY_TEXT(CLAY_STRING("?"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_PRESS_START_2P,
                        .fontSize = 48,
                        .textColor = dashAccentGold
                    }));
                }
            }
        }

        // Medagliere section - podium style
        CLAY(CLAY_ID("MedagliereSection"), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                .childGap = 16
            }
        }) {
            // Section header
            CLAY(CLAY_ID("MedagliereHeader"), {
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                    .padding = { 16, 16, 12, 12 },
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = dashAccentOrange,
                .cornerRadius = CLAY_CORNER_RADIUS(10)
            }) {
                CLAY_TEXT(CLAY_STRING("MEDAGLIERE"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_PRESS_START_2P,
                    .fontSize = 24,
                    .textColor = COLOR_WHITE
                }));
            }

            // Medal cards row
            CLAY(CLAY_ID("MedalRow"), {
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                    .childGap = 12
                }
            }) {
                // Gold medal
                CLAY(CLAY_ID("GoldMedal"), {
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                        .padding = { 16, 16, 20, 20 },
                        .childGap = 8,
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                    },
                    .backgroundColor = dashCardBg,
                    .cornerRadius = CLAY_CORNER_RADIUS(12),
                    .border = { .width = {3, 3, 3, 3}, .color = dashAccentGold }
                }) {
                    CLAY_TEXT(CLAY_STRING("ORO"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 14,
                        .textColor = dashAccentGold
                    }));
                    CLAY_TEXT(CLAY_STRING("1st"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_PRESS_START_2P,
                        .fontSize = 32,
                        .textColor = dashAccentGold
                    }));
                    CLAY_TEXT(CLAY_STRING("---"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 16,
                        .textColor = dashLabelText
                    }));
                }

                // Silver medal
                CLAY(CLAY_ID("SilverMedal"), {
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                        .padding = { 16, 16, 20, 20 },
                        .childGap = 8,
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                    },
                    .backgroundColor = dashCardBg,
                    .cornerRadius = CLAY_CORNER_RADIUS(12),
                    .border = { .width = {3, 3, 3, 3}, .color = { 192, 192, 192, 255 } }
                }) {
                    CLAY_TEXT(CLAY_STRING("ARGENTO"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 14,
                        .textColor = { 140, 140, 140, 255 }
                    }));
                    CLAY_TEXT(CLAY_STRING("2nd"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_PRESS_START_2P,
                        .fontSize = 32,
                        .textColor = { 140, 140, 140, 255 }
                    }));
                    CLAY_TEXT(CLAY_STRING("---"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 16,
                        .textColor = dashLabelText
                    }));
                }

                // Bronze medal
                CLAY(CLAY_ID("BronzeMedal"), {
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                        .padding = { 16, 16, 20, 20 },
                        .childGap = 8,
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                    },
                    .backgroundColor = dashCardBg,
                    .cornerRadius = CLAY_CORNER_RADIUS(12),
                    .border = { .width = {3, 3, 3, 3}, .color = { 205, 127, 50, 255 } }
                }) {
                    CLAY_TEXT(CLAY_STRING("BRONZO"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 14,
                        .textColor = { 180, 110, 45, 255 }
                    }));
                    CLAY_TEXT(CLAY_STRING("3rd"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_PRESS_START_2P,
                        .fontSize = 32,
                        .textColor = { 180, 110, 45, 255 }
                    }));
                    CLAY_TEXT(CLAY_STRING("---"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 16,
                        .textColor = dashLabelText
                    }));
                }
            }
        }

        // Quick actions / fun section
        CLAY(CLAY_ID("QuickActions"), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                .padding = { 20, 20, 16, 16 },
                .childGap = 16,
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = dashAccentPurple,
            .cornerRadius = CLAY_CORNER_RADIUS(12)
        }) {
            CLAY_TEXT(CLAY_STRING("Savio merda!"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_PRESS_START_2P,
                .fontSize = 20,
                .textColor = COLOR_WHITE
            }));
        }
    }
}

/////////////////////////////////////////////////
// Tournament

void
RenderGoBackButton(void)
{
    CLAY(CLAY_ID("GoBackRow"), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)}
        }
    }) {
        CLAY(CLAY_ID("GoBack"), {
            .layout = {
                .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                .padding = { 12, 12, 8, 8 },
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = Clay_Hovered() ? dashAccentPurple : dashAccentTeal,
            .cornerRadius = CLAY_CORNER_RADIUS(10)
        }) {
            Clay_OnHover(HandleGoBackInteraction, NULL);
            CLAY_TEXT(CLAY_STRING("< Go back"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 14,
                .textColor = COLOR_WHITE
            }));
        }
    }
}

void
RenderTournamentBanner(u32 tournament_idx)
{
    CLAY(CLAY_ID("TournamentBanner"), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
            .padding = { 24, 24, 20, 20 },
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
        },
        .backgroundColor = dashAccentOrange,
        .cornerRadius = CLAY_CORNER_RADIUS(16)
    }) {
        CLAY_TEXT(str8_to_clay((data.tournaments.events + tournament_idx)->name), CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_PRESS_START_2P,
            .fontSize = 28,
            .textColor = COLOR_WHITE
        }));
    }
}

void
RenderFormatOption(TournamentFormat format, TournamentFormat current_format,
    Clay_String name, Clay_String description, u32 id)
{
    bool is_selected = (current_format == format);

    TournamentFormat *pFormat = push_array(data.frameArena, TournamentFormat, 1);
    *pFormat = format;

    Clay_Color border_color = is_selected ? dashAccentTeal : textInputBorderColor;
    u16 border_width = is_selected ? 2 : 1;

    CLAY(CLAY_IDI("FormatOption", id), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
            .padding = { 10, 10, 8, 8 },
            .childGap = 8,
            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
        },
        .backgroundColor = Clay_Hovered() ? playerRowHoverColor : playerRowColor,
        .cornerRadius = CLAY_CORNER_RADIUS(6),
        .border = { .width = {border_width, border_width, border_width, border_width}, .color = border_color }
    }) {
        Clay_OnHover(HandleSelectTournamentFormat, pFormat);
        CLAY(CLAY_IDI("RadioIndicator", id), {
            .layout = {
                .sizing = {.width = CLAY_SIZING_FIXED(16), .height = CLAY_SIZING_FIXED(16)},
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = is_selected ? dashAccentTeal : COLOR_WHITE,
            .cornerRadius = CLAY_CORNER_RADIUS(8),
            .border = { .width = {2, 2, 2, 2}, .color = is_selected ? dashAccentTeal : textInputBorderColor }
        }) {
            if (is_selected) {
                CLAY(CLAY_IDI("RadioDot", id), {
                    .layout = {
                        .sizing = {.width = CLAY_SIZING_FIXED(6), .height = CLAY_SIZING_FIXED(6)}
                    },
                    .backgroundColor = COLOR_WHITE,
                    .cornerRadius = CLAY_CORNER_RADIUS(3)
                }) {}
            }
        }
        CLAY(CLAY_IDI("FormatLabel", id), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                .childGap = 2
            }
        }) {
            CLAY_TEXT(name, CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 14,
                .textColor = is_selected ? dashAccentTeal : stringColor
            }));
            CLAY_TEXT(description, CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 11,
                .textColor = dashLabelText
            }));
        }
    }
}

void
RenderRegistrationPanel(u32 tournament_idx, Event *tournament,
    s32 *registered_positions, u32 registered_count)
{
    // Tournament format selector (radio-style list)
    CLAY(CLAY_ID("FormatSelector"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
            .childGap = 4
        }
    }) {
        RenderFormatOption(FORMAT_KNOCKOUT, tournament->format,
            CLAY_STRING("Single Elimination"),
            CLAY_STRING("Direct knockout bracket"),
            FORMAT_KNOCKOUT);

        RenderFormatOption(FORMAT_GROUP_KNOCKOUT, tournament->format,
            CLAY_STRING("Groups + Knockout"),
            CLAY_STRING("Round-robin groups, then bracket"),
            FORMAT_GROUP_KNOCKOUT);
    }

    // Group settings cards (only for groups format)
    if (tournament->format == FORMAT_GROUP_KNOCKOUT)
    {
        CLAY(CLAY_ID("GroupSettingsRow"), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                .childGap = 8
            }
        }) {
            // Group size card with controls
            CLAY(CLAY_ID("GroupSizeCardOuter"), {
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)}
                },
                .cornerRadius = CLAY_CORNER_RADIUS(8)
            }) {
                // Accent bar
                CLAY(CLAY_ID("GroupSizeAccent"), {
                    .layout = {
                        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(4)}
                    },
                    .backgroundColor = dashAccentTeal,
                    .cornerRadius = { 8, 8, 0, 0 }
                }) {}
                // Card content
                CLAY(CLAY_ID("GroupSizeCard"), {
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                        .padding = { 8, 8, 6, 8 },
                        .childGap = 4,
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                    },
                    .backgroundColor = dashCardBg,
                    .cornerRadius = { 0, 0, 8, 8 }
                }) {
                    CLAY_TEXT(CLAY_STRING("GROUP SIZE"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 10,
                        .textColor = dashLabelText
                    }));
                    // Value with +/- controls
                    CLAY(CLAY_ID("GroupSizeControls"), {
                        .layout = {
                            .layoutDirection = CLAY_LEFT_TO_RIGHT,
                            .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                            .childGap = 8,
                            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                        }
                    }) {
                        // Decrement button
                        CLAY(CLAY_ID("GroupSizeDecrement"), {
                            .layout = {
                                .sizing = {.width = CLAY_SIZING_FIXED(24), .height = CLAY_SIZING_FIXED(24)},
                                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                            },
                            .backgroundColor = Clay_Hovered() ? dashAccentPurple : dashAccentTeal,
                            .cornerRadius = CLAY_CORNER_RADIUS(12)
                        }) {
                            Clay_OnHover(HandleDecrementGroupSize, NULL);
                            CLAY_TEXT(CLAY_STRING("-"), CLAY_TEXT_CONFIG({
                                .fontId = FONT_ID_BODY_16,
                                .fontSize = 16,
                                .textColor = COLOR_WHITE
                            }));
                        }
                        // Value
                        String8 size_str = str8_from_u32(data.frameArena, tournament->group_phase.group_size);
                        CLAY_TEXT(str8_to_clay(size_str), CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_PRESS_START_2P,
                            .fontSize = 20,
                            .textColor = dashAccentTeal
                        }));
                        // Increment button
                        CLAY(CLAY_ID("GroupSizeIncrement"), {
                            .layout = {
                                .sizing = {.width = CLAY_SIZING_FIXED(24), .height = CLAY_SIZING_FIXED(24)},
                                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                            },
                            .backgroundColor = Clay_Hovered() ? dashAccentPurple : dashAccentTeal,
                            .cornerRadius = CLAY_CORNER_RADIUS(12)
                        }) {
                            Clay_OnHover(HandleIncrementGroupSize, NULL);
                            CLAY_TEXT(CLAY_STRING("+"), CLAY_TEXT_CONFIG({
                                .fontId = FONT_ID_BODY_16,
                                .fontSize = 16,
                                .textColor = COLOR_WHITE
                            }));
                        }
                    }
                }
            }

            // Advance per group card with controls
            CLAY(CLAY_ID("AdvanceCardOuter"), {
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)}
                },
                .cornerRadius = CLAY_CORNER_RADIUS(8)
            }) {
                // Accent bar
                CLAY(CLAY_ID("AdvanceAccent"), {
                    .layout = {
                        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(4)}
                    },
                    .backgroundColor = dashAccentOrange,
                    .cornerRadius = { 8, 8, 0, 0 }
                }) {}
                // Card content
                CLAY(CLAY_ID("AdvanceCard"), {
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                        .padding = { 8, 8, 6, 8 },
                        .childGap = 4,
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                    },
                    .backgroundColor = dashCardBg,
                    .cornerRadius = { 0, 0, 8, 8 }
                }) {
                    CLAY_TEXT(CLAY_STRING("ADVANCE"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 10,
                        .textColor = dashLabelText
                    }));
                    // Value with +/- controls
                    CLAY(CLAY_ID("AdvanceControls"), {
                        .layout = {
                            .layoutDirection = CLAY_LEFT_TO_RIGHT,
                            .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                            .childGap = 8,
                            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                        }
                    }) {
                        // Decrement button
                        CLAY(CLAY_ID("AdvancePerGroupDecrement"), {
                            .layout = {
                                .sizing = {.width = CLAY_SIZING_FIXED(24), .height = CLAY_SIZING_FIXED(24)},
                                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                            },
                            .backgroundColor = Clay_Hovered() ? dashAccentPurple : dashAccentOrange,
                            .cornerRadius = CLAY_CORNER_RADIUS(12)
                        }) {
                            Clay_OnHover(HandleDecrementAdvancePerGroup, NULL);
                            CLAY_TEXT(CLAY_STRING("-"), CLAY_TEXT_CONFIG({
                                .fontId = FONT_ID_BODY_16,
                                .fontSize = 16,
                                .textColor = COLOR_WHITE
                            }));
                        }
                        // Value
                        String8 advance_str = str8_from_u32(data.frameArena, tournament->group_phase.advance_per_group);
                        CLAY_TEXT(str8_to_clay(advance_str), CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_PRESS_START_2P,
                            .fontSize = 20,
                            .textColor = dashAccentOrange
                        }));
                        // Increment button
                        CLAY(CLAY_ID("AdvancePerGroupIncrement"), {
                            .layout = {
                                .sizing = {.width = CLAY_SIZING_FIXED(24), .height = CLAY_SIZING_FIXED(24)},
                                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                            },
                            .backgroundColor = Clay_Hovered() ? dashAccentPurple : dashAccentOrange,
                            .cornerRadius = CLAY_CORNER_RADIUS(12)
                        }) {
                            Clay_OnHover(HandleIncrementAdvancePerGroup, NULL);
                            CLAY_TEXT(CLAY_STRING("+"), CLAY_TEXT_CONFIG({
                                .fontId = FONT_ID_BODY_16,
                                .fontSize = 16,
                                .textColor = COLOR_WHITE
                            }));
                        }
                    }
                }
            }
        }

        // VIEW section - toggle visibility of Groups and Knockout panels
        CLAY(CLAY_ID("RegViewSectionHeader"), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                .padding = { 0, 0, 8, 0 }
            }
        }) {
            CLAY_TEXT(CLAY_STRING("VIEW"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 10,
                .textColor = dashLabelText
            }));
        }
    }

    // Show "Start Tournament" button if we have enough players
    if (registered_count >= 2)
    {
        CLAY(CLAY_ID("StartTournamentButton"), {
            .layout = {
                .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                .padding = { 10, 10, 8, 8 },
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
            },
            .backgroundColor = Clay_Hovered() ? dashAccentTeal : dashAccentCoral,
            .cornerRadius = CLAY_CORNER_RADIUS(8)
        }) {
            Clay_OnHover(HandleStartTournament, NULL);
            CLAY_TEXT(CLAY_STRING("Start Tournament"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 14,
                .textColor = COLOR_WHITE
            }));
        }
    }

    CLAY(CLAY_ID("RegisteredHeader"), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
            .padding = { 12, 12, 6, 6 },
            .childGap = 8,
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
        },
        .backgroundColor = dashAccentTeal,
        .cornerRadius = { 8, 8, 0, 0 }
    }) {
        CLAY_TEXT(CLAY_STRING("REGISTERED"), CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 12,
            .textColor = COLOR_WHITE
        }));
        // Count badge
        CLAY(CLAY_ID("RegisteredCount"), {
            .layout = {
                .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                .padding = { 6, 6, 2, 2 }
            },
            .backgroundColor = COLOR_WHITE,
            .cornerRadius = CLAY_CORNER_RADIUS(8)
        }) {
            String8 count_str = str8_from_u32(data.frameArena, registered_count);
            CLAY_TEXT(str8_to_clay(count_str), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 10,
                .textColor = dashAccentTeal
            }));
        }
    }

    // Players list container
    CLAY(CLAY_ID("RegisteredListCard"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
            .childGap = 4
        },
        .backgroundColor = dashCardBg,
        .cornerRadius = { 0, 0, 8, 8 }
    }) {
        if (registered_count == 0)
        {
            CLAY_TEXT(CLAY_STRING("Click players below to add"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 12,
                .textColor = dashLabelText
            }));
        }
        for (u32 i = 0; i < registered_count; i++)
        {
            u32 player_idx = registered_positions[i];
            Player *player = data.players.players + player_idx;

            CLAY(CLAY_IDI("RegisteredPlayer", player_idx), {
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                    .padding = { 8, 8, 6, 6 },
                    .childGap = 8,
                    .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = Clay_Hovered() ? playerRowHoverColor : playerRowColor,
                .cornerRadius = CLAY_CORNER_RADIUS(4),
                .border = { .width = {1, 1, 1, 1}, .color = textInputBorderColor }
            }) {
                u32 *pPlayerIdx = push_array(data.frameArena, u32, 1);
                *pPlayerIdx = player_idx;
                Clay_OnHover(HandleTogglePlayerRegistration, pPlayerIdx);
                // Number badge
                CLAY(CLAY_IDI("RegPlayerBadge", player_idx), {
                    .layout = {
                        .sizing = {.width = CLAY_SIZING_FIXED(20), .height = CLAY_SIZING_FIXED(20)},
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = dashAccentTeal,
                    .cornerRadius = CLAY_CORNER_RADIUS(10)
                }) {
                    String8 num_str = str8_from_u32(data.frameArena, i + 1);
                    CLAY_TEXT(str8_to_clay(num_str), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 10,
                        .textColor = COLOR_WHITE
                    }));
                }
                // Player name
                CLAY(CLAY_IDI("RegPlayerName", player_idx), {
                    .layout = {
                        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)}
                    }
                }) {
                    CLAY_TEXT(str8_to_clay_truncated(data.frameArena, player->name, MAX_DISPLAY_NAME_LEN), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 14,
                        .textColor = stringColor
                    }));
                }
                // Remove indicator
                CLAY_TEXT(CLAY_STRING("-"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 16,
                    .textColor = removeTextColor
                }));
            }
        }
    }

    // Purple header bar
    CLAY(CLAY_ID("AvailableHeader"), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
            .padding = { 12, 12, 6, 6 },
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
        },
        .backgroundColor = dashAccentPurple,
        .cornerRadius = { 8, 8, 0, 0 }
    }) {
        CLAY_TEXT(CLAY_STRING("AVAILABLE"), CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 12,
            .textColor = COLOR_WHITE
        }));
    }

    // Players list container
    CLAY(CLAY_ID("AvailableListCard"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
            .childGap = 4
        },
        .backgroundColor = dashCardBg,
        .cornerRadius = { 0, 0, 8, 8 }
    }) {
        u32 idx_tail = data.players.len - 1;
        u32 idx = (data.players.players)->nxt;
        while (idx != idx_tail)
        {
            Player *player = data.players.players + idx;
            bool is_registered = (player->registrations >> tournament_idx) & 1;

            if (!is_registered)
            {
                CLAY(CLAY_IDI("AvailablePlayer", idx), {
                    .layout = {
                        .layoutDirection = CLAY_LEFT_TO_RIGHT,
                        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                        .padding = { 8, 8, 6, 6 },
                        .childGap = 8,
                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = Clay_Hovered() ? playerRowHoverColor : playerRowColor,
                    .cornerRadius = CLAY_CORNER_RADIUS(4),
                    .border = { .width = {1, 1, 1, 1}, .color = textInputBorderColor }
                }) {
                    u32 *pIdx = push_array(data.frameArena, u32, 1);
                    *pIdx = idx;
                    Clay_OnHover(HandleTogglePlayerRegistration, pIdx);
                    // Add indicator
                    CLAY(CLAY_IDI("AvailPlayerAdd", idx), {
                        .layout = {
                            .sizing = {.width = CLAY_SIZING_FIXED(20), .height = CLAY_SIZING_FIXED(20)},
                            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                        },
                        .backgroundColor = addButtonColor,
                        .cornerRadius = CLAY_CORNER_RADIUS(10)
                    }) {
                        CLAY_TEXT(CLAY_STRING("+"), CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_BODY_16,
                            .fontSize = 12,
                            .textColor = COLOR_WHITE
                        }));
                    }
                    // Player name
                    CLAY_TEXT(str8_to_clay_truncated(data.frameArena, player->name, MAX_DISPLAY_NAME_LEN), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 14,
                        .textColor = stringColor
                    }));
                }
            }

            idx = player->nxt;
        }
    }
}

// Render in-progress phase left panel content
void
RenderInProgressPanel(s32 *registered_positions, u32 registered_count)
{
    // Show tournament status
    CLAY_TEXT(CLAY_STRING("Tournament In Progress"), CLAY_TEXT_CONFIG({
        .fontId = FONT_ID_BODY_16,
        .fontSize = 16,
        .textColor = dashAccentTeal
    }));

    Event *tournament = data.tournaments.events + data.selectedTournamentIdx;

    // CONTINUE section - forward/advance actions (only show when there's a forward action)
    if (tournament->format == FORMAT_GROUP_KNOCKOUT &&
        tournament->phase == PHASE_GROUP)
    {
        // Terminate Group Phase button - primary action (teal/green)
        CLAY(CLAY_ID("TerminateGroupPhase"), {
            .layout = {
                .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                .padding = { 10, 10, 10, 10 },
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
            },
            .backgroundColor = Clay_Hovered() ? dashAccentPurple : dashAccentTeal,
            .cornerRadius = CLAY_CORNER_RADIUS(8)
        }) {
            Clay_OnHover(HandleTerminateGroupPhase, NULL);
            CLAY_TEXT(CLAY_STRING("Start Knockout Phase"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 14,
                .textColor = COLOR_WHITE
            }));
        }
    }

    // Return to Group Phase button (only in knockout phase)
    if (tournament->format == FORMAT_GROUP_KNOCKOUT && tournament->phase == PHASE_KNOCKOUT)
    {
        CLAY(CLAY_ID("ReturnToGroupPhaseButton"), {
            .layout = {
                .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                .padding = { 10, 10, 8, 8 },
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
            },
            .backgroundColor = Clay_Hovered() ? dashAccentCoral : COLOR_WHITE,
            .cornerRadius = CLAY_CORNER_RADIUS(8),
            .border = { .width = {2, 2, 2, 2}, .color = dashAccentOrange }
        }) {
            Clay_OnHover(HandleReturnToGroupPhase, NULL);
            CLAY_TEXT(CLAY_STRING("Return to Group Phase"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 13,
                .textColor = Clay_Hovered() ? COLOR_WHITE : dashAccentOrange
            }));
        }
    }

    // Return to Registration button - secondary action (outlined style)
    CLAY(CLAY_ID("ReturnToRegistrationPhaseButton"), {
        .layout = {
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
            .padding = { 10, 10, 8, 8 },
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
        },
        .backgroundColor = Clay_Hovered() ? dashAccentCoral : COLOR_WHITE,
        .cornerRadius = CLAY_CORNER_RADIUS(8),
        .border = { .width = {2, 2, 2, 2}, .color = dashAccentOrange }
    }) {
        Clay_OnHover(HandleReturnToRegistrationPhase, NULL);
        CLAY_TEXT(CLAY_STRING("Return to Registration"), CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 13,
            .textColor = Clay_Hovered() ? COLOR_WHITE : dashAccentOrange
        }));
    }

    if (tournament->format == FORMAT_GROUP_KNOCKOUT)
    {
        // Read-only group settings info - styled cards
        CLAY(CLAY_ID("InProgressGroupSettingsRow"), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                .childGap = 8
            }
        }) {
            // Group size card
            CLAY(CLAY_ID("InProgressGroupSizeCardOuter"), {
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)}
                },
                .cornerRadius = CLAY_CORNER_RADIUS(8)
            }) {
                // Accent bar
                CLAY(CLAY_ID("InProgressGroupSizeAccent"), {
                    .layout = {
                        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(4)}
                    },
                    .backgroundColor = dashAccentTeal,
                    .cornerRadius = { 8, 8, 0, 0 }
                }) {}
                // Card content
                CLAY(CLAY_ID("InProgressGroupSizeCard"), {
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                        .padding = { 12, 12, 8, 12 },
                        .childGap = 2,
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                    },
                    .backgroundColor = dashCardBg,
                    .cornerRadius = { 0, 0, 8, 8 }
                }) {
                    CLAY_TEXT(CLAY_STRING("GROUP SIZE"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 10,
                        .textColor = dashLabelText
                    }));
                    String8 size_str = str8_from_u32(data.frameArena, tournament->group_phase.group_size);
                    CLAY_TEXT(str8_to_clay(size_str), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_PRESS_START_2P,
                        .fontSize = 24,
                        .textColor = dashAccentTeal
                    }));
                }
            }

            // Advance per group card
            CLAY(CLAY_ID("InProgressAdvanceCardOuter"), {
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)}
                },
                .cornerRadius = CLAY_CORNER_RADIUS(8)
            }) {
                // Accent bar
                CLAY(CLAY_ID("InProgressAdvanceAccent"), {
                    .layout = {
                        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(4)}
                    },
                    .backgroundColor = dashAccentOrange,
                    .cornerRadius = { 8, 8, 0, 0 }
                }) {}
                // Card content
                CLAY(CLAY_ID("InProgressAdvanceCard"), {
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                        .padding = { 12, 12, 8, 12 },
                        .childGap = 2,
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                    },
                    .backgroundColor = dashCardBg,
                    .cornerRadius = { 0, 0, 8, 8 }
                }) {
                    CLAY_TEXT(CLAY_STRING("ADVANCE"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 10,
                        .textColor = dashLabelText
                    }));
                    String8 advance_str = str8_from_u32(data.frameArena, tournament->group_phase.advance_per_group);
                    CLAY_TEXT(str8_to_clay(advance_str), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_PRESS_START_2P,
                        .fontSize = 24,
                        .textColor = dashAccentOrange
                    }));
                }
            }
        }

        // VIEW section - toggle visibility of Groups and Knockout panels
        CLAY(CLAY_ID("InProgressViewSectionHeader"), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                .padding = { 0, 0, 8, 0 }
            }
        }) {
            CLAY_TEXT(CLAY_STRING("VIEW"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 10,
                .textColor = dashLabelText
            }));
        }

    }

    // Players card
    CLAY(CLAY_ID("InProgressPlayersCardOuter"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)}
        },
        .cornerRadius = CLAY_CORNER_RADIUS(8)
    }) {
        // Purple accent bar with title
        CLAY(CLAY_ID("PlayersAccentBar"), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                .padding = { 12, 12, 6, 6 },
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = dashAccentPurple,
            .cornerRadius = { 8, 8, 0, 0 }
        }) {
            CLAY_TEXT(CLAY_STRING("PLAYERS"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 12,
                .textColor = COLOR_WHITE
            }));
        }
        // Players list container
        CLAY(CLAY_ID("PlayersListCard"), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
                .padding = { 8, 8, 8, 8 },
                .childGap = 4
            },
            .backgroundColor = dashCardBg,
            .cornerRadius = { 0, 0, 8, 8 }
        }) {
            for (u32 i = 0; i < registered_count; i++)
            {
                u32 player_idx = registered_positions[i];
                Player *player = data.players.players + player_idx;

                CLAY(CLAY_IDI("PlayerListItem", player_idx), {
                    .layout = {
                        .layoutDirection = CLAY_LEFT_TO_RIGHT,
                        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                        .padding = { 8, 8, 6, 6 },
                        .childGap = 8,
                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = playerRowColor,
                    .cornerRadius = CLAY_CORNER_RADIUS(4),
                    .border = { .width = {1, 1, 1, 1}, .color = textInputBorderColor }
                }) {
                    // Player number badge
                    CLAY(CLAY_IDI("PlayerBadge", player_idx), {
                        .layout = {
                            .sizing = {.width = CLAY_SIZING_FIXED(20), .height = CLAY_SIZING_FIXED(20)},
                            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                        },
                        .backgroundColor = dashAccentPurple,
                        .cornerRadius = CLAY_CORNER_RADIUS(10)
                    }) {
                        String8 num_str = str8_from_u32(data.frameArena, i + 1);
                        CLAY_TEXT(str8_to_clay(num_str), CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_BODY_16,
                            .fontSize = 10,
                            .textColor = COLOR_WHITE
                        }));
                    }
                    // Player name
                    CLAY_TEXT(str8_to_clay_truncated(data.frameArena, player->name, MAX_DISPLAY_NAME_LEN), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 14,
                        .textColor = stringColor
                    }));
                }
            }
        }
    }
}

/////////////////////////////////////////////////
// Single elimination tournament

void
RenderMatchSlot(Clay_String player1_name, Clay_String player2_name,
    u8 player1_idx, u8 player2_idx, u32 bracket_pos1, u32 bracket_pos2, u32 match_id, float zoom)
{
    bool player1_is_tbd = (player1_idx == 0);
    bool player2_is_tbd = (player2_idx == 0);
    Clay_Color name1Color = player1_is_tbd ? matchVsColor : stringColor;
    Clay_Color name2Color = player2_is_tbd ? matchVsColor : stringColor;

    // Encode data for click handlers: (bracket_pos << 8) | player_idx
    intptr_t *pPlayer1Data = push_array(data.frameArena, intptr_t, 1);
    *pPlayer1Data = (intptr_t)((bracket_pos1 << 8) | player1_idx);
    intptr_t *pPlayer2Data = push_array(data.frameArena, intptr_t, 1);
    *pPlayer2Data = (intptr_t)((bracket_pos2 << 8) | player2_idx);

    float slotWidth = 160 * zoom;
    float accentHeight = 4 * zoom;
    float cornerRadius = 8 * zoom;
    u16 paddingH = (u16)(12 * zoom);
    u16 paddingV = (u16)(10 * zoom);
    float fontSize = SLOT_PLAYER_FONT_SIZE * zoom;
    float vsFontSize = 12 * zoom;

    // Outer container with accent bar
    CLAY(CLAY_IDI("MatchBorder", match_id), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0) }
        },
        .cornerRadius = CLAY_CORNER_RADIUS(cornerRadius)
    }) {
        // Teal accent bar at top
        CLAY(CLAY_IDI("MatchAccent", match_id), {
            .layout = {
                .sizing = { .width = CLAY_SIZING_FIXED(slotWidth), .height = CLAY_SIZING_FIXED(accentHeight) }
            },
            .backgroundColor = dashAccentTeal,
            .cornerRadius = { cornerRadius, cornerRadius, 0, 0 }
        }) {}
        CLAY(CLAY_IDI("Match", match_id), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = { .width = CLAY_SIZING_FIXED(slotWidth), .height = CLAY_SIZING_FIT(0) },
                .padding = { paddingH, paddingH, paddingV, paddingV },
                .childGap = (u16)(2 * zoom),
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
            },
            .backgroundColor = dashCardBg,
            .cornerRadius = { 0, 0, cornerRadius, cornerRadius },
            .border = { .width = { 1, 1, 1, 1 }, .color = textInputBorderColor }
        }) {
            // Player 1 - clickable to advance
            CLAY(CLAY_IDI("Player1Slot", match_id), {
                .layout = {
                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) },
                    .padding = { (u16)(4 * zoom), (u16)(4 * zoom), (u16)(2 * zoom), (u16)(2 * zoom) },
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                },
                .backgroundColor = Clay_Hovered() && !player1_is_tbd ? dashAccentTeal : dashCardBg,
                .cornerRadius = CLAY_CORNER_RADIUS(4 * zoom)
            }) {
                if (!player1_is_tbd) {
                    Clay_OnHover(HandleAdvanceWinner, pPlayer1Data);
                }
                CLAY_TEXT(player1_name, CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = fontSize,
                    .textColor = name1Color
                }));
            }

            // VS separator with lines
            CLAY_AUTO_ID({
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) },
                    .padding = { 0, 0, (u16)(4 * zoom), (u16)(4 * zoom) },
                    .childGap = (u16)(8 * zoom),
                    .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                }
            }) {
                // Left line
                CLAY_AUTO_ID({
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(1) }
                    },
                    .backgroundColor = dashAccentPurple
                }) {}
                // VS text
                CLAY_TEXT(CLAY_STRING("vs"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = vsFontSize,
                    .textColor = dashAccentPurple
                }));
                // Right line
                CLAY_AUTO_ID({
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(1) }
                    },
                    .backgroundColor = dashAccentPurple
                }) {}
            }

            // Player 2 - clickable to advance
            CLAY(CLAY_IDI("Player2Slot", match_id), {
                .layout = {
                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) },
                    .padding = { (u16)(4 * zoom), (u16)(4 * zoom), (u16)(2 * zoom), (u16)(2 * zoom) },
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                },
                .backgroundColor = Clay_Hovered() && !player2_is_tbd ? dashAccentTeal : dashCardBg,
                .cornerRadius = CLAY_CORNER_RADIUS(4 * zoom)
            }) {
                if (!player2_is_tbd) {
                    Clay_OnHover(HandleAdvanceWinner, pPlayer2Data);
                }
                CLAY_TEXT(player2_name, CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = fontSize,
                    .textColor = name2Color
                }));
            }
        }
    }
}

void
RenderByeSlot(u32 match_id, float zoom)
{
    float slotWidth = 160 * zoom;
    float accentHeight = 4 * zoom;
    float cornerRadius = 8 * zoom;
    u16 paddingH = (u16)(12 * zoom);
    u16 paddingV = (u16)(10 * zoom);
    float fontSize = SLOT_PLAYER_FONT_SIZE * zoom;
    float byeFontSize = 12 * zoom;

    // Render a bye slot with the same dimensions as a regular match slot
    CLAY(CLAY_IDI("MatchBorder", match_id), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0) }
        },
        .cornerRadius = CLAY_CORNER_RADIUS(cornerRadius)
    }) {
        // Orange accent bar at top (to differentiate from regular matches)
        CLAY(CLAY_IDI("ByeAccent", match_id), {
            .layout = {
                .sizing = { .width = CLAY_SIZING_FIXED(slotWidth), .height = CLAY_SIZING_FIXED(accentHeight) }
            },
            .backgroundColor = dashAccentOrange,
            .cornerRadius = { cornerRadius, cornerRadius, 0, 0 }
        }) {}
        CLAY(CLAY_IDI("Match", match_id), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = { .width = CLAY_SIZING_FIXED(slotWidth), .height = CLAY_SIZING_FIT(0) },
                .padding = { paddingH, paddingH, paddingV, paddingV },
                .childGap = (u16)(2 * zoom),
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
            },
            .backgroundColor = dashCardBg,
            .cornerRadius = { 0, 0, cornerRadius, cornerRadius },
            .border = { .width = { 1, 1, 1, 1 }, .color = textInputBorderColor }
        }) {
            // Empty slot 1 - fixed height container to match player name height
            CLAY_AUTO_ID({
                .layout = {
                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(fontSize + 4 * zoom) }
                }
            }) {}

            // BYE separator with lines
            CLAY_AUTO_ID({
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) },
                    .padding = { 0, 0, (u16)(4 * zoom), (u16)(4 * zoom) },
                    .childGap = (u16)(8 * zoom),
                    .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                }
            }) {
                // Left line
                CLAY_AUTO_ID({
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(1) }
                    },
                    .backgroundColor = dashAccentOrange
                }) {}
                // BYE text
                CLAY_TEXT(CLAY_STRING("BYE"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = byeFontSize,
                    .textColor = dashAccentOrange
                }));
                // Right line
                CLAY_AUTO_ID({
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(1) }
                    },
                    .backgroundColor = dashAccentOrange
                }) {}
            }

            // Empty slot 2 - fixed height container to match player name height
            CLAY_AUTO_ID({
                .layout = {
                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(fontSize + 4 * zoom) }
                }
            }) {}
        }
    }
}

void
RenderGroupPhaseHeader(void)
{
    CLAY(CLAY_ID("GroupPhaseHeader"), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) },
            .padding = { 0, 0, 16, 16 },
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
        }
    }) {
        CLAY(CLAY_ID("GroupPhaseHeaderBadge"), {
            .layout = {
                .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0) },
                .padding = { 16, 16, 10, 10 }
            },
            .backgroundColor = dashAccentCoral,
            .cornerRadius = CLAY_CORNER_RADIUS(8)
        }) {
            CLAY_TEXT(CLAY_STRING("GROUP PHASE"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_PRESS_START_2P,
                .fontSize = 16,
                .textColor = COLOR_WHITE
            }));
        }
    }
}

void
RenderKnockoutHeader(void)
{
    CLAY(CLAY_ID("KnockoutHeader"), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) },
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
        }
    }) {
        CLAY(CLAY_ID("KnockoutHeaderBadge"), {
            .layout = {
                .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0) },
                .padding = { 16, 16, 10, 10 }
            },
            .backgroundColor = dashAccentCoral,
            .cornerRadius = CLAY_CORNER_RADIUS(8)
        }) {
            CLAY_TEXT(CLAY_STRING("KNOCKOUT PHASE"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_PRESS_START_2P,
                .fontSize = 16,
                .textColor = COLOR_WHITE
            }));
        }
    }
}

void
RenderKnockoutChart(u8 *bracket, u32 num_players)
{
    CLAY(CLAY_ID("KnockoutBracketContainer"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .padding = { 16, 16, 16, 16 }
        },
    }) {
        Clay_OnHover(HandleZoomableHover, &data.chartZoomLevel);

        static CustomLayoutElement bracketConnectionsElement;
        bracketConnectionsElement.type = CUSTOM_LAYOUT_ELEMENT_TYPE_BRACKET_CONNECTIONS;
        bracketConnectionsElement.customData.bracketConnections.num_players = num_players;
        bracketConnectionsElement.customData.bracketConnections.zoom = data.chartZoomLevel;
        bracketConnectionsElement.customData.bracketConnections.yOffset = data.yOffset;

        // Use floating element so it doesn't move with childOffset/scroll
        // This prevents Clay from culling it when the bracket is panned
        CLAY(CLAY_ID("BracketConnections"), {
            .layout = { .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) } },
            .floating = {
                .attachTo = CLAY_ATTACH_TO_PARENT,
                .pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_PASSTHROUGH,
                .clipTo = CLAY_CLIP_TO_ATTACHED_PARENT
            },
            .custom = { .customData = &bracketConnectionsElement }
        }) {}

        // Calculate bracket size (next power of 2 >= num_players)
        u32 bracket_size = 1;
        while (bracket_size < num_players)
        {
            bracket_size <<= 1;
        }

        // Calculate number of rounds based on bracket size
        u32 num_rounds = 0;
        u32 temp = bracket_size;
        while (temp > 1)
        {
            temp >>= 1;
            num_rounds++;
        }

        // Create horizontal layout for rounds (left to right)
        float zoom = data.chartZoomLevel;
        CLAY(CLAY_ID("RoundsContainer"), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
                .childGap = (u16)(32 * zoom),
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
            },
        }) {
            for (u32 round = 0; round < num_rounds; round++)
            {
                u32 matches_in_round = bracket_size >> (round + 1);
                // Each slot grows by 2^round so matches align with parent matches
                u32 grow_factor = 1 << round;

                // Calculate bracket level for this visual round
                // Visual round 0 (leftmost) = deepest level, round (num_rounds-1) = level 1 (finals)
                u32 bracket_level = num_rounds - round;
                u32 level_base = (1u << bracket_level) - 1;  // First index at this level

                CLAY(CLAY_IDI("Round", round), {
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                    }
                }) {
                    // Matches in this round
                    for (u32 match = 0; match < matches_in_round; match++)
                    {
                        u32 match_id = round * 100 + match;

                        // Get the two bracket positions for this match
                        u32 pos1 = level_base + match * 2;
                        u32 pos2 = level_base + match * 2 + 1;

                        u8 player1_idx = bracket[pos1];
                        u8 player2_idx = bracket[pos2];

                        // Slot container that grows proportionally and centers the match
                        CLAY(CLAY_IDI("MatchSlot", match_id), {
                            .layout = {
                                .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_GROW(grow_factor) },
                                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                            }
                        }) {
                            // BYE slots only exist in the first round
                            if (round == 0 && player1_idx == 0 && player2_idx == 0)
                            {
                                RenderByeSlot(match_id, zoom);
                            }
                            else
                            {
                                // Render match slot (with players or TBD)
                                Clay_String name1 = CLAY_STRING("TBD");
                                Clay_String name2 = CLAY_STRING("TBD");

                                if (player1_idx != 0)
                                {
                                    Player *player1 = data.players.players + player1_idx;
                                    name1 = str8_to_clay_truncated(data.frameArena, player1->name, MAX_DISPLAY_NAME_LEN);
                                }
                                if (player2_idx != 0)
                                {
                                    Player *player2 = data.players.players + player2_idx;
                                    name2 = str8_to_clay_truncated(data.frameArena, player2->name, MAX_DISPLAY_NAME_LEN);
                                }

                                RenderMatchSlot(name1, name2, player1_idx, player2_idx, pos1, pos2, match_id, zoom);
                            }
                        }
                    }
                }
            }
        }
    }
}

/////////////////////////////////////////////////
// Tournament with group phase

void
RenderGroupMatrix(Event *tournament, u32 group_idx, u32 players_in_group)
{
    float zoom = data.groupMatrixZoomLevel;
    u16 cellWidth = (u16)(100 * zoom);
    u16 cellGap = (u16)(4 * zoom);
    u16 padH = (u16)(8 * zoom);
    u16 padV = (u16)(10 * zoom);
    u16 cornerRad = (u16)(8 * zoom);
    u16 fontSizeSmall = (u16)(12 * zoom);
    u16 fontSizeMed = (u16)(14 * zoom);

    Clay_Color groupAccent = groupAccentColors[group_idx % ArrayCount(groupAccentColors)];

    CLAY(CLAY_IDI("GroupMatrix", group_idx), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0) },
            .childGap = cellGap
        }
    }) {
        Clay_OnHover(HandleZoomableHover, &data.groupMatrixZoomLevel);

        // Header row with player names as columns
        CLAY(CLAY_IDI("MatrixHeaderRow", group_idx), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0) },
                .childGap = cellGap
            }
        }) {
            // Empty corner cell
            CLAY(CLAY_IDI("MatrixCorner", group_idx), {
                .layout = {
                    .sizing = { .width = CLAY_SIZING_FIXED(cellWidth), .height = CLAY_SIZING_FIT(0) },
                    .padding = { padH, padH, padV, padV },
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = groupAccent,
                .cornerRadius = CLAY_CORNER_RADIUS(cornerRad)
            }) {
                CLAY_TEXT(CLAY_STRING("vs"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = fontSizeSmall,
                    .textColor = COLOR_WHITE
                }));
            }

            // Column headers (player names)
            for (u32 col = 0; col < players_in_group; col++)
            {
                u8 player_idx = tournament->group_phase.groups[group_idx][col];
                if (player_idx != 0)
                {
                    Player *player = data.players.players + player_idx;
                    u32 header_id = group_idx * MAX_GROUP_SIZE * 2 + col;
                    CLAY(CLAY_IDI("MatrixColHeader", header_id), {
                        .layout = {
                            .sizing = { .width = CLAY_SIZING_FIXED(cellWidth), .height = CLAY_SIZING_FIT(0) },
                            .padding = { padH, padH, padV, padV },
                            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                        },
                        .backgroundColor = groupAccent,
                        .cornerRadius = CLAY_CORNER_RADIUS(cornerRad)
                    }) {
                        CLAY_TEXT(str8_to_clay_truncated(data.frameArena, player->name, MAX_DISPLAY_NAME_LEN), CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_BODY_16,
                            .fontSize = fontSizeMed,
                            .textColor = COLOR_WHITE
                        }));
                    }
                }
            }
        }

        // Data rows (one per player)
        for (u32 row = 0; row < players_in_group; row++)
        {
            u8 row_player_idx = tournament->group_phase.groups[group_idx][row];
            if (row_player_idx != 0)
            {
                Player *row_player = data.players.players + row_player_idx;
                u32 row_id = group_idx * MAX_GROUP_SIZE + row;

                CLAY(CLAY_IDI("MatrixRow", row_id), {
                    .layout = {
                        .layoutDirection = CLAY_LEFT_TO_RIGHT,
                        .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0) },
                        .childGap = cellGap
                    }
                }) {
                    // Row header (player name) with accent background
                    CLAY(CLAY_IDI("MatrixRowHeader", row_id), {
                        .layout = {
                            .sizing = { .width = CLAY_SIZING_FIXED(cellWidth), .height = CLAY_SIZING_FIT(0) },
                            .padding = { padV, padV, padV, padV },
                            .childAlignment = { .x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER }
                        },
                        .backgroundColor = groupAccent,
                        .cornerRadius = CLAY_CORNER_RADIUS(cornerRad)
                    }) {
                        CLAY_TEXT(str8_to_clay_truncated(data.frameArena, row_player->name, MAX_DISPLAY_NAME_LEN), CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_BODY_16,
                            .fontSize = fontSizeMed,
                            .textColor = COLOR_WHITE
                        }));
                    }

                    // Result cells
                    for (u32 col = 0; col < players_in_group; col++)
                    {
                        u8 col_player_idx = tournament->group_phase.groups[group_idx][col];
                        if (col_player_idx != 0)
                        {
                            u32 cell_id = group_idx * MAX_GROUP_SIZE * MAX_GROUP_SIZE + row * MAX_GROUP_SIZE + col;
                            bool isDiagonal = (row == col);
                            bool isGroupPhase = (tournament->phase == PHASE_GROUP);
                            bool _showHover = (!isDiagonal && isGroupPhase);
                            Clay_Color cell_bg_normal = isDiagonal ? textInputBorderColor : dashCardBg;

                            CLAY(CLAY_IDI("MatrixCell", cell_id), {
                                .layout = {
                                    .sizing = { .width = CLAY_SIZING_FIXED(cellWidth), .height = CLAY_SIZING_FIT(0) },
                                    .padding = { padH, padH, padV, padV },
                                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                                },
                                .backgroundColor = (_showHover && Clay_Hovered()) ? dashAccentTeal : cell_bg_normal,
                                .cornerRadius = CLAY_CORNER_RADIUS(cornerRad)
                            }) {
                                // Diagonal cells (player vs self) show dash
                                if (isDiagonal)
                                {
                                    CLAY_TEXT(CLAY_STRING("-"), CLAY_TEXT_CONFIG({
                                        .fontId = FONT_ID_BODY_16,
                                        .fontSize = fontSizeMed,
                                        .textColor = matchVsColor
                                    }));
                                }
                                else
                                {
                                    // Register double-click handler for score entry (only in group phase)
                                    if (isGroupPhase)
                                    {
                                        MatrixCellData *pCellData = push_array(data.frameArena, MatrixCellData, 1);
                                        pCellData->group_idx = group_idx;
                                        pCellData->row_idx = row;
                                        pCellData->col_idx = col;
                                        pCellData->cell_id = cell_id;
                                        Clay_OnHover(HandleMatrixCellClick, pCellData);
                                    }

                                    // Get the score for this cell
                                    MatchScore score = tournament->group_phase.scores[group_idx][row][col];
                                    bool hasScore = (score.row_score != 0 || score.col_score != 0);

                                    if (hasScore)
                                    {
                                        // Display score as "X - Y"
                                        String8 str8_row_score = str8_from_u32(data.frameArena, (u32)score.row_score);
                                        String8 str8_col_score = str8_from_u32(data.frameArena, (u32)score.col_score);
                                        String8 separator = str8_lit_comp(" - ");
                                        String8 res = str8_cat(data.frameArena, str8_cat(data.frameArena, str8_row_score, separator), str8_col_score);
                                        CLAY_TEXT(str8_to_clay(res), CLAY_TEXT_CONFIG({
                                            .fontId = FONT_ID_BODY_16,
                                            .fontSize = fontSizeMed,
                                            .textColor = (_showHover && Clay_Hovered())  ? COLOR_WHITE : dashAccentTeal
                                        }));
                                    }
                                    else
                                    {
                                        // No score yet - show TBD
                                        CLAY_TEXT(CLAY_STRING("TBD"), CLAY_TEXT_CONFIG({
                                            .fontId = FONT_ID_BODY_16,
                                            .fontSize = fontSizeMed,
                                            .textColor = (_showHover && Clay_Hovered()) ? COLOR_WHITE : dashAccentPurple
                                        }));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void
RenderGroupsKnockoutChart(Event *tournament)
{
    // Get group info from the constructed group phase
    u32 num_groups = tournament->group_phase.num_groups;

    u32 numAccentColors = sizeof(groupAccentColors) / sizeof(groupAccentColors[0]);

    // Calculate number of qualifiers for the bracket
    u32 num_qualifiers = num_groups * tournament->group_phase.advance_per_group;

    // Determine sizing based on which panels are visible
    bool showGroups = data.groupsPanelVisible;
    bool showKnockout = data.knockoutPanelVisible && (num_qualifiers >= 2);

    // Main container: groups on left, knockout bracket on right
    CLAY(CLAY_ID("GroupsAndBracketContainer"), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
            .childGap = 32,
            .childAlignment = { .x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_TOP }
        },
        .clip = { .vertical = true, .horizontal = true, .childOffset = Clay_GetScrollOffset() }
    }) {
        // Left side: Groups stacked vertically with styled container
        if (showGroups)
        {
            // Use grow sizing if knockout is hidden, otherwise use fit with percentage
            Clay_Sizing groupsSizing = showKnockout
                ? (Clay_Sizing){ .width = CLAY_SIZING_FIT(GetScreenWidth() * 0.4f), .height = CLAY_SIZING_FIT(0) }
                : (Clay_Sizing){ .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) };

            // Horizontal container for Groups panel + hide strip
            CLAY(CLAY_ID("GroupsPanelRow"), {
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = groupsSizing,
                    .childGap = 0
                }
            }) {
            CLAY(CLAY_ID("GroupsContainerOuter"), {
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) }
                },
                .cornerRadius = CLAY_CORNER_RADIUS(12)
            }) {
                // Teal accent bar for groups section
                CLAY(CLAY_ID("GroupsAccentBar"), {
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(6) }
                    },
                    .backgroundColor = dashAccentTeal,
                    .cornerRadius = { 12, 12, 0, 0 }
                }) {}

                // Groups header
                CLAY(CLAY_ID("GroupsHeader"), {
                    .layout = {
                        .layoutDirection = CLAY_LEFT_TO_RIGHT,
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) },
                        .padding = { 16, 16, 12, 12 },
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = { 245, 250, 250, 255 }
                }) {
                    CLAY(CLAY_ID("GroupsHeaderBadge"), {
                        .layout = {
                            .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0) },
                            .padding = { 12, 12, 8, 8 }
                        },
                        .backgroundColor = dashAccentTeal,
                        .cornerRadius = CLAY_CORNER_RADIUS(8)
                    }) {
                        CLAY_TEXT(CLAY_STRING("GROUP STAGE"), CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_PRESS_START_2P,
                            .fontSize = 14,
                            .textColor = COLOR_WHITE
                        }));
                    }
                }

                // Groups content
                CLAY(CLAY_ID("GroupsContainer"), {
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) },
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER },
                        .childGap = 16,
                        .padding = { 16, 16, 16, 16 }
                    },
                    .backgroundColor = { 245, 250, 250, 255 },
                    .cornerRadius = { 0, 0, 12, 12 },
                }) {
                    for (u32 g = 0; g < num_groups; g++)
                    {
                        // Count actual players in this group
                        u32 players_in_group = 0;
                        for (u32 slot = 0; slot < MAX_GROUP_SIZE; slot++)
                        {
                            if (tournament->group_phase.groups[g][slot] != 0)
                            {
                                players_in_group++;
                            }
                        }

                        Clay_Color groupAccent = groupAccentColors[g % numAccentColors];

                        // Outer container with accent bar
                        CLAY(CLAY_IDI("GroupOuter", g), {
                            .layout = {
                                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                                .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0) }
                            },
                            .cornerRadius = CLAY_CORNER_RADIUS(12),
                        }) {
                            // Colored accent bar at top
                            CLAY(CLAY_IDI("GroupAccent", g), {
                                .layout = {
                                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(6) }
                                },
                                .backgroundColor = groupAccent,
                                .cornerRadius = { 12, 12, 0, 0 }
                            }) {}

                            // Group content card
                            CLAY(CLAY_IDI("Group", g), {
                                .layout = {
                                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                                    .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0) },
                                    .padding = { 16, 16, 16, 16 },
                                    .childGap = 12,
                                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                                },
                                .backgroundColor = dashCardBg,
                                .cornerRadius = { 0, 0, 12, 12 }
                            }) {
                                // Group header with styled badge - clickable to toggle matrix
                                CLAY(CLAY_IDI("GroupHeader", g), {
                                    .layout = {
                                        .layoutDirection = CLAY_LEFT_TO_RIGHT,
                                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) },
                                        .padding = { 12, 12, 8, 8 },
                                        .childGap = 8,
                                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                                    },
                                    .backgroundColor = groupAccent,
                                    .cornerRadius = CLAY_CORNER_RADIUS(8)
                                }) {
                                    String8 group_prefix = str8_lit("GROUP ");
                                    String8 group_num = str8_from_u32(data.frameArena, g + 1);
                                    String8 group_label = str8_cat(data.frameArena, group_prefix, group_num);
                                    CLAY_TEXT(str8_to_clay(group_label), CLAY_TEXT_CONFIG({
                                        .fontId = FONT_ID_PRESS_START_2P,
                                        .fontSize = 16,
                                        .textColor = COLOR_WHITE
                                    }));
                                }

                                RenderGroupMatrix(tournament, g, players_in_group);
                            }
                        }
                    }
                }
            }

            // Hide Groups strip - vertical strip on right edge
            CLAY(CLAY_ID("HideGroupsStrip"), {
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = { .width = CLAY_SIZING_FIXED(32), .height = CLAY_SIZING_GROW(0) },
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = Clay_Hovered() ? groupAccentColorsHover[0] : groupAccentColors[0],
                .cornerRadius = CLAY_CORNER_RADIUS(8)
            }) {
                Clay_OnHover(HandleToggleGroupsPanel, NULL);
                CLAY_TEXT(CLAY_STRING("<"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 20,
                    .textColor = COLOR_WHITE
                }));
            }
            } // end GroupsPanelRow
        } // end if (showGroups)
        else
        {
            // Collapsed Groups indicator - vertical strip taking full height
            CLAY(CLAY_ID("CollapsedGroupsIndicator"), {
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = { .width = CLAY_SIZING_FIXED(36), .height = CLAY_SIZING_GROW(0) },
                    .padding = { 8, 8, 12, 12 },
                    .childGap = 4,
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = Clay_Hovered() ? groupAccentColorsHover[0] : groupAccentColors[0],
                .cornerRadius = CLAY_CORNER_RADIUS(8)
            }) {
                Clay_OnHover(HandleToggleGroupsPanel, NULL);
                CLAY_TEXT(CLAY_STRING(">"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 16,
                    .textColor = COLOR_WHITE
                }));
                CLAY_TEXT(CLAY_STRING("G"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_PRESS_START_2P,
                    .fontSize = 12,
                    .textColor = COLOR_WHITE
                }));
                CLAY_TEXT(CLAY_STRING("R"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_PRESS_START_2P,
                    .fontSize = 12,
                    .textColor = COLOR_WHITE
                }));
                CLAY_TEXT(CLAY_STRING("O"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_PRESS_START_2P,
                    .fontSize = 12,
                    .textColor = COLOR_WHITE
                }));
                CLAY_TEXT(CLAY_STRING("U"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_PRESS_START_2P,
                    .fontSize = 12,
                    .textColor = COLOR_WHITE
                }));
                CLAY_TEXT(CLAY_STRING("P"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_PRESS_START_2P,
                    .fontSize = 12,
                    .textColor = COLOR_WHITE
                }));
                CLAY_TEXT(CLAY_STRING("S"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_PRESS_START_2P,
                    .fontSize = 12,
                    .textColor = COLOR_WHITE
                }));
            }
        }

        // Right side: Knockout bracket
        if (showKnockout)
        {
            // Horizontal container for hide strip + Knockout panel
            CLAY(CLAY_ID("KnockoutPanelRow"), {
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) },
                    .childGap = 0
                }
            }) {
            // Hide Knockout strip - vertical strip on left edge
            CLAY(CLAY_ID("HideKnockoutStrip"), {
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = { .width = CLAY_SIZING_FIXED(32), .height = CLAY_SIZING_GROW(0) },
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = Clay_Hovered() ? groupAccentColorsHover[1] : groupAccentColors[1],
                .cornerRadius = CLAY_CORNER_RADIUS(8)
            }) {
                Clay_OnHover(HandleToggleKnockoutPanel, NULL);
                CLAY_TEXT(CLAY_STRING(">"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 20,
                    .textColor = COLOR_WHITE
                }));
            }

            CLAY(CLAY_ID("KnockoutContainerOuter"), {
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) }
                },
                .cornerRadius = CLAY_CORNER_RADIUS(12)
            }) {
                // Coral accent bar for knockout section
                CLAY(CLAY_ID("KnockoutAccentBar"), {
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(6) }
                    },
                    .backgroundColor = dashAccentCoral,
                    .cornerRadius = { 12, 12, 0, 0 }
                }) {}

                // Knockout header
                CLAY(CLAY_ID("KnockoutSectionHeader"), {
                    .layout = {
                        .layoutDirection = CLAY_LEFT_TO_RIGHT,
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) },
                        .padding = { 16, 16, 12, 12 },
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = { 255, 248, 248, 255 }
                }) {
                    CLAY(CLAY_ID("KnockoutHeaderBadgeInner"), {
                        .layout = {
                            .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0) },
                            .padding = { 12, 12, 8, 8 }
                        },
                        .backgroundColor = dashAccentCoral,
                        .cornerRadius = CLAY_CORNER_RADIUS(8)
                    }) {
                        CLAY_TEXT(CLAY_STRING("KNOCKOUT STAGE"), CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_PRESS_START_2P,
                            .fontSize = 14,
                            .textColor = COLOR_WHITE
                        }));
                    }
                }

                // Knockout content wrapper
                CLAY(CLAY_ID("KnockoutContentWrapper"), {
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) }
                    },
                    .backgroundColor = { 255, 248, 248, 255 },
                    .cornerRadius = { 0, 0, 12, 12 }
                }) {
                    RenderKnockoutChart(tournament->group_phase.bracket, num_qualifiers);
                }
            }
            } // end KnockoutPanelRow
        }
        else if (num_qualifiers >= 2)
        {
            // Collapsed Knockout indicator - vertical strip taking full height
            CLAY(CLAY_ID("CollapsedKnockoutIndicator"), {
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = { .width = CLAY_SIZING_FIXED(36), .height = CLAY_SIZING_GROW(0) },
                    .padding = { 8, 8, 12, 12 },
                    .childGap = 4,
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = Clay_Hovered() ? groupAccentColorsHover[1] : groupAccentColors[1],
                .cornerRadius = CLAY_CORNER_RADIUS(8)
            }) {
                Clay_OnHover(HandleToggleKnockoutPanel, NULL);
                CLAY_TEXT(CLAY_STRING("<"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 16,
                    .textColor = COLOR_WHITE
                }));
                CLAY_TEXT(CLAY_STRING("K"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_PRESS_START_2P,
                    .fontSize = 12,
                    .textColor = COLOR_WHITE
                }));
                CLAY_TEXT(CLAY_STRING("N"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_PRESS_START_2P,
                    .fontSize = 12,
                    .textColor = COLOR_WHITE
                }));
                CLAY_TEXT(CLAY_STRING("O"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_PRESS_START_2P,
                    .fontSize = 12,
                    .textColor = COLOR_WHITE
                }));
                CLAY_TEXT(CLAY_STRING("C"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_PRESS_START_2P,
                    .fontSize = 12,
                    .textColor = COLOR_WHITE
                }));
                CLAY_TEXT(CLAY_STRING("K"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_PRESS_START_2P,
                    .fontSize = 12,
                    .textColor = COLOR_WHITE
                }));
                CLAY_TEXT(CLAY_STRING("O"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_PRESS_START_2P,
                    .fontSize = 12,
                    .textColor = COLOR_WHITE
                }));
                CLAY_TEXT(CLAY_STRING("U"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_PRESS_START_2P,
                    .fontSize = 12,
                    .textColor = COLOR_WHITE
                }));
                CLAY_TEXT(CLAY_STRING("T"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_PRESS_START_2P,
                    .fontSize = 12,
                    .textColor = COLOR_WHITE
                }));
            }
        }
    }
}

void
RenderTournamentLeftPanel(u32 tournament_idx)
{
    // Left panel outer - with accent bar
    CLAY(CLAY_ID("LeftPanelOuter"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {.width = CLAY_SIZING_FIXED(250), .height = CLAY_SIZING_GROW(0)}
        },
        .cornerRadius = CLAY_CORNER_RADIUS(12)
    }) {
        // Teal accent bar
        CLAY(CLAY_ID("LeftPanelAccent"), {
            .layout = {
                .sizing = {.height = CLAY_SIZING_FIXED(6), .width = CLAY_SIZING_GROW(0)}
            },
            .backgroundColor = dashAccentTeal,
            .cornerRadius = { 12, 12, 0, 0 }
        }) {}
        // Left panel content
        CLAY(CLAY_ID("LeftPanel"), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
                .padding = { 16, 16, 16, 16 },
                .childGap = 12,
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
            },
            .backgroundColor = dashCardBg,
            .cornerRadius = { 0, 0, 12, 12 },
            .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() }
        }) {
            Event *panel_tournament = data.tournaments.events + tournament_idx;
            s32 registered_positions[64] = {0};
            u32 registered_count = find_all_filled_slots(panel_tournament->registrations, registered_positions);

            if (panel_tournament->phase == PHASE_REGISTRATION)
            {
                RenderRegistrationPanel(tournament_idx, panel_tournament, registered_positions, registered_count);
            }
            else
            {
                RenderInProgressPanel(registered_positions, registered_count);
            }
        }
    }
}

void
RenderTournamentRightPanel(u32 tournament_idx)
{
    // Right panel outer - with accent bar
    CLAY(CLAY_ID("TournamentChartOuter"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = layoutExpand
        },
        .cornerRadius = CLAY_CORNER_RADIUS(12)
    }) {
        // Purple accent bar
        CLAY(CLAY_ID("TournamentChartAccent"), {
            .layout = {
                .sizing = {.height = CLAY_SIZING_FIXED(6), .width = CLAY_SIZING_GROW(0)}
            },
            .backgroundColor = dashAccentPurple,
            .cornerRadius = { 12, 12, 0, 0 }
        }) {}
        // Right panel - Tournament chart
        CLAY(CLAY_ID("TournamentChart"), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = layoutExpand,
                .childAlignment = { .x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_TOP }
            },
            .backgroundColor = dashCardBg,
            .cornerRadius = { 0, 0, 12, 12 },
            // .clip = { .horizontal = true, .vertical = true, .childOffset = Clay_GetScrollOffset() }
        }) {
            Event *tournament = data.tournaments.events + tournament_idx;

            // Get the number of players in the tournament
            s32 _positions[64] = {0};
            u32 num_players = find_all_filled_slots(tournament->registrations, _positions);

            if (tournament->format == FORMAT_KNOCKOUT)
            {
                RenderKnockoutChart(tournament->bracket, num_players);
            }
            else // FORMAT_GROUP_KNOCKOUT
            {
                RenderGroupsKnockoutChart(tournament);
            }
        }
    }
}

void
RenderTournamentChart(u32 tournament_idx)
{
    CLAY(CLAY_ID("Tournament"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = layoutExpand,
            .childGap = 20,
            .padding = { 8, 8, 8, 8 },
            .childAlignment = {
                .y = CLAY_ALIGN_Y_TOP
            },
        },
        .backgroundColor = dashBgGradientTop
    }) {
        RenderGoBackButton();
        RenderTournamentBanner(tournament_idx);

        // Two-column layout: left panel + right panel chart
        CLAY(CLAY_ID("TournamentContent"), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = layoutExpand,
                .childGap = 16
            }
        }) {
            RenderTournamentLeftPanel(tournament_idx);
            RenderTournamentRightPanel(tournament_idx);
        }
    }
}

/////////////////////////////////////////////////
// Events main page

void
RenderEventsActions(u32 tournament_idx)
{
    u32 *pTournamentIdx = push_array(data.frameArena, u32, 1);
    *pTournamentIdx = tournament_idx;

    CLAY(CLAY_IDI("EventActions", tournament_idx), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = { .width = CLAY_SIZING_FIXED(220) },
            .childGap = 8
        }
    }) {
        // Edit button (opens tournament details)
        CLAY_AUTO_ID({
            .layout = {
                .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                .padding = { 12, 12, 6, 6 },
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = Clay_Hovered() ? dashAccentPurple : dashAccentTeal,
            .cornerRadius = CLAY_CORNER_RADIUS(8)
        }) {
            Clay_OnHover(HandleEditTournament, pTournamentIdx);

            CLAY_TEXT(CLAY_STRING("Open"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 14,
                .textColor = COLOR_WHITE
            }));
        }
        // Rename button
        CLAY_AUTO_ID({
            .layout = {
                .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                .padding = { 12, 12, 6, 6 },
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = Clay_Hovered() ? dashAccentPurple : dashAccentOrange,
            .cornerRadius = CLAY_CORNER_RADIUS(8)
        }) {
            Clay_OnHover(HandleStartRenameEvent, pTournamentIdx);

            CLAY_TEXT(CLAY_STRING("Rename"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 14,
                .textColor = COLOR_WHITE
            }));
        }
        // Delete button
        CLAY_AUTO_ID({
            .layout = {
                .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                .padding = { 12, 12, 6, 6 },
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = Clay_Hovered() ? removeButtonHoverColor : removeButtonColor,
            .cornerRadius = CLAY_CORNER_RADIUS(8)
        }) {
            Clay_OnHover(HandleDeleteTournament, pTournamentIdx);

            CLAY_TEXT(CLAY_STRING("Delete"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 14,
                .textColor = removeTextColor
            }));
        }
    }
}

void
RenderEventsBanner(void)
{
    CLAY(CLAY_ID("EventsBanner"), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
            .padding = { 24, 24, 20, 20 },
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
        },
        .backgroundColor = dashAccentCoral,
        .cornerRadius = CLAY_CORNER_RADIUS(16)
    }) {
        CLAY_TEXT(CLAY_STRING("EVENTS"), CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_PRESS_START_2P,
            .fontSize = 36,
            .textColor = COLOR_WHITE
        }));
    }
}

void
RenderEventsHeader(void)
{
    // Header with input field and add button
    CLAY(CLAY_ID("EventsInputHeaderOuter"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)}
        },
        .cornerRadius = CLAY_CORNER_RADIUS(12)
    }) {
        // Accent bar
        CLAY(CLAY_ID("EventsInputAccent"), {
            .layout = {
                .sizing = {.height = CLAY_SIZING_FIXED(6), .width = CLAY_SIZING_GROW(0)}
            },
            .backgroundColor = dashAccentTeal,
            .cornerRadius = { 12, 12, 0, 0 }
        }) {}
        CLAY(CLAY_ID("EventsInputHeader"), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                .padding = { 20, 20, 16, 16 },
                .childGap = 16,
                .childAlignment = { .x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = dashCardBg,
            .cornerRadius = { 0, 0, 12, 12 }
        }) {
            // Process keyboard input when focused
            if (data.focusedTextbox == TEXTBOX_Events)
            {
                TextInput_ProcessKeyboard(&data.textInputs[TEXTBOX_Events]);
            }

            // Render text input
            TextInput_Render(TEXTBOX_Events, CLAY_STRING("EventNameInput"),
                CLAY_STRING("EventNameInputScroll"), CLAY_STRING("Enter event name..."));

            // Show duplicate name warning
            if (data.duplicateWarning == DUPLICATE_EVENT_ADD)
            {
                RenderDuplicateWarning(CLAY_STRING("EventNameInput"),
                    CLAY_STRING("Event already exists"), DUPLICATE_EVENT_ADD);
            }

            // Add button
            CLAY(CLAY_ID("AddEventButton"), {
                .layout = {
                    .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIXED(40)},
                    .padding = { 24, 24, 0, 0 },
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = Clay_Hovered() ? dashAccentTeal : dashAccentCoral,
                .cornerRadius = CLAY_CORNER_RADIUS(10)
            }) {
                Clay_OnHover(HandleAddEventButtonInteraction, NULL);
                CLAY_TEXT(CLAY_STRING("+ Add Event"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 16,
                    .textColor = COLOR_WHITE
                }));
            }
        }
    }
}

void
RenderEventsList(void)
{
    // Events list container with accent bar
    CLAY(CLAY_ID("EventsListOuter"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = layoutExpand
        },
        .cornerRadius = CLAY_CORNER_RADIUS(12)
    }) {
        // Purple accent bar
        CLAY(CLAY_ID("EventsListAccent"), {
            .layout = {
                .sizing = {.height = CLAY_SIZING_FIXED(6), .width = CLAY_SIZING_GROW(0)}
            },
            .backgroundColor = dashAccentPurple,
            .cornerRadius = { 12, 12, 0, 0 }
        }) {}
        CLAY(CLAY_ID("EventsList"), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = layoutExpand,
                .childGap = 2,
            },
            .backgroundColor = dashCardBg,
            .cornerRadius = { 0, 0, 12, 12 },
        }) {
            // List header
            CLAY(CLAY_ID("EventsListHeader"), {
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                    .padding = { 20, 20, 12, 12 }
                },
                .backgroundColor = dashCardBg
            }) {
                CLAY(CLAY_ID("EventNameHeader"), {
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_GROW(0) }
                    }
                }) {
                    CLAY_TEXT(CLAY_STRING("EVENT NAME"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 12,
                        .textColor = dashLabelText
                    }));
                }
                CLAY(CLAY_ID("EventPlayersHeader"), {
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_FIXED(150) }
                    }
                }) {
                    CLAY_TEXT(CLAY_STRING("PLAYERS"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 12,
                        .textColor = dashLabelText
                    }));
                }
                CLAY(CLAY_ID("EventActionsHeader"), {
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_FIXED(220) }
                    }
                }) {
                    CLAY_TEXT(CLAY_STRING("ACTIONS"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 12,
                        .textColor = dashLabelText
                    }));
                }
            }

            CLAY(CLAY_ID("EventListRows"), {
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = layoutExpand
                },
                .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() }
            }) {
                // Event rows
                u32 idx_tail = data.tournaments.len - 1;
                u32 idx = (data.tournaments.events)->nxt;
                while (idx != idx_tail)
                {
                    Event *tournament = data.tournaments.events + idx;

                    CLAY(CLAY_IDI("EventRow", idx), {
                        .layout = {
                            .layoutDirection = CLAY_LEFT_TO_RIGHT,
                            .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER },
                            .padding = { 20, 20, 12, 12 }
                        },
                        .backgroundColor = Clay_Hovered() ? dashBgGradientTop : dashCardBg
                    }) {
                        // Event name
                        CLAY(CLAY_IDI("EventName", idx), {
                            .layout = {
                                .sizing = { .width = CLAY_SIZING_GROW(0) },
                                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                            }
                        }) {
                            CLAY_TEXT(str8_to_clay(tournament->name), CLAY_TEXT_CONFIG({
                                .fontId = FONT_ID_BODY_16,
                                .fontSize = 18,
                                .textColor = dashAccentPurple
                            }));
                        }

                        // Number of players registered
                        CLAY(CLAY_IDI("EventPlayers", idx), {
                            .layout = {
                                .sizing = { .width = CLAY_SIZING_FIXED(150) },
                                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                            }
                        }) {
                            s32 positions[64];
                            u32 count = find_all_filled_slots(tournament->registrations, positions);
                            CLAY_TEXT(str8_to_clay(str8_from_u32(data.frameArena, count)), CLAY_TEXT_CONFIG({
                                .fontId = FONT_ID_PRESS_START_2P,
                                .fontSize = 24,
                                .textColor = dashAccentTeal
                            }));
                        }

                        RenderEventsActions(idx);
                    }

                    idx = tournament->nxt;
                }
            }
        }
    }
}

void
RenderEvents(void)
{
    if (data.selectedTournamentIdx != 0)
    {
        RenderTournamentChart(data.selectedTournamentIdx);
    }
    else
    {
        CLAY(CLAY_ID("Events"), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = layoutExpand,
                .childAlignment = {
                    .y = CLAY_ALIGN_Y_TOP
                },
                .childGap = 20,
                .padding = { 8, 8, 8, 8 }
            },
            .backgroundColor = dashBgGradientTop,
        }) {
            RenderEventsBanner();
            RenderEventsHeader();
            RenderEventsList();
        }
    }
}

/////////////////////////////////////////////////
// Players main page

void
RenderPlayersActions(u32 player_idx)
{
    u32 *pPlayerIdx = push_array(data.frameArena, u32, 1);
    *pPlayerIdx = player_idx;

    CLAY(CLAY_IDI("PlayerActions", player_idx), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = { .width = CLAY_SIZING_FIXED(220) },
            .childGap = 8
        }
    }) {
        // Open button
        CLAY_AUTO_ID({
            .layout = {
                .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                .padding = { 12, 12, 6, 6 },
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = Clay_Hovered() ? dashAccentPurple : dashAccentTeal,
            .cornerRadius = CLAY_CORNER_RADIUS(8)
        }) {
            Clay_OnHover(HandleOpenPlayer, pPlayerIdx);

            CLAY_TEXT(CLAY_STRING("Open"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 14,
                .textColor = COLOR_WHITE
            }));
        }
        // Rename button
        CLAY_AUTO_ID({
            .layout = {
                .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                .padding = { 12, 12, 6, 6 },
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = Clay_Hovered() ? dashAccentPurple : dashAccentOrange,
            .cornerRadius = CLAY_CORNER_RADIUS(8)
        }) {
            Clay_OnHover(HandleStartRenamePlayer, pPlayerIdx);

            CLAY_TEXT(CLAY_STRING("Rename"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 14,
                .textColor = COLOR_WHITE
            }));
        }
        // Delete button
        CLAY_AUTO_ID({
            .layout = {
                .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                .padding = { 12, 12, 6, 6 },
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = Clay_Hovered() ? removeButtonHoverColor : removeButtonColor,
            .cornerRadius = CLAY_CORNER_RADIUS(8)
        }) {
            Clay_OnHover(HandleDeletePlayer, pPlayerIdx);

            CLAY_TEXT(CLAY_STRING("Delete"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 14,
                .textColor = removeTextColor
            }));
        }
    }
}

void
RenderPlayersBanner(void)
{
    CLAY(CLAY_ID("PlayersBanner"), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
            .padding = { 24, 24, 20, 20 },
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
        },
        .backgroundColor = dashAccentPurple,
        .cornerRadius = CLAY_CORNER_RADIUS(16)
    }) {
        CLAY_TEXT(CLAY_STRING("PLAYERS"), CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_PRESS_START_2P,
            .fontSize = 36,
            .textColor = COLOR_WHITE
        }));
    }
}

void
RenderPlayersHeader(void)
{
    // Header with input field and add button
    CLAY(CLAY_ID("PlayersInputHeaderOuter"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)}
        },
        .cornerRadius = CLAY_CORNER_RADIUS(12)
    }) {
        // Accent bar
        CLAY(CLAY_ID("PlayersInputAccent"), {
            .layout = {
                .sizing = {.height = CLAY_SIZING_FIXED(6), .width = CLAY_SIZING_GROW(0)}
            },
            .backgroundColor = dashAccentCoral,
            .cornerRadius = { 12, 12, 0, 0 }
        }) {}
        CLAY(CLAY_ID("PlayersInputHeader"), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                .padding = { 20, 20, 16, 16 },
                .childGap = 16,
                .childAlignment = { .x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = dashCardBg,
            .cornerRadius = { 0, 0, 12, 12 }
        }) {
            // Process keyboard input when focused
            if (data.focusedTextbox == TEXTBOX_Players)
            {
                TextInput_ProcessKeyboard(&data.textInputs[TEXTBOX_Players]);
            }

            // Render text input
            TextInput_Render(TEXTBOX_Players, CLAY_STRING("PlayerNameInput"),
                CLAY_STRING("PlayerNameInputScroll"), CLAY_STRING("Enter player name..."));

            // Show duplicate name warning
            if (data.duplicateWarning == DUPLICATE_PLAYER_ADD)
            {
                RenderDuplicateWarning(CLAY_STRING("PlayerNameInput"),
                    CLAY_STRING("Player already exists"), DUPLICATE_PLAYER_ADD);
            }

            // Add button
            CLAY(CLAY_ID("AddPlayerButton"), {
                .layout = {
                    .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIXED(40)},
                    .padding = { 24, 24, 0, 0 },
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = Clay_Hovered() ? dashAccentTeal : dashAccentCoral,
                .cornerRadius = CLAY_CORNER_RADIUS(10)
            }) {
                Clay_OnHover(HandleAddPlayerButtonInteraction, NULL);
                CLAY_TEXT(CLAY_STRING("+ Add Player"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 16,
                    .textColor = COLOR_WHITE
                }));
            }
        }
    }
}

void
RenderPlayersList(void)
{
    // Players list container with accent bar
    CLAY(CLAY_ID("PlayersListOuter"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = layoutExpand
        },
        .cornerRadius = CLAY_CORNER_RADIUS(12)
    }) {
        // Teal accent bar
        CLAY(CLAY_ID("PlayersListAccent"), {
            .layout = {
                .sizing = {.height = CLAY_SIZING_FIXED(6), .width = CLAY_SIZING_GROW(0)}
            },
            .backgroundColor = dashAccentTeal,
            .cornerRadius = { 12, 12, 0, 0 }
        }) {}
        CLAY(CLAY_ID("PlayersList"), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = layoutExpand,
                .childGap = 2,
            },
            .backgroundColor = dashCardBg,
            .cornerRadius = { 0, 0, 12, 12 },
        }) {
            // List header
            CLAY(CLAY_ID("PlayersListHeader"), {
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                    .padding = { 20, 20, 12, 12 }
                },
                .backgroundColor = dashCardBg
            }) {
                CLAY(CLAY_ID("PlayerNameHeader"), {
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_GROW(0) }
                    }
                }) {
                    CLAY_TEXT(CLAY_STRING("PLAYER NAME"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 12,
                        .textColor = dashLabelText
                    }));
                }
                CLAY(CLAY_ID("PlayerRegistrationsHeader"), {
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_FIXED(150) }
                    }
                }) {
                    CLAY_TEXT(CLAY_STRING("REGISTRATIONS"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 12,
                        .textColor = dashLabelText
                    }));
                }
                CLAY(CLAY_ID("PlayerActionsHeader"), {
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_FIXED(220) }
                    }
                }) {
                    CLAY_TEXT(CLAY_STRING("ACTIONS"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 12,
                        .textColor = dashLabelText
                    }));
                }
            }

            CLAY(CLAY_ID("PlayerListRows"), {
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = layoutExpand
                },
                .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() }
            }) {
                // Player rows
                u32 idx_tail = data.players.len - 1;
                u32 idx = (data.players.players)->nxt;
                while (idx != idx_tail)
                {
                    Player *player = data.players.players + idx;

                    CLAY(CLAY_IDI("PlayerRow", idx), {
                        .layout = {
                            .layoutDirection = CLAY_LEFT_TO_RIGHT,
                            .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER },
                            .padding = { 20, 20, 12, 12 }
                        },
                        .backgroundColor = Clay_Hovered() ? dashBgGradientTop : dashCardBg
                    }) {
                        // Player name
                        CLAY(CLAY_IDI("PlayerName", idx), {
                            .layout = {
                                .sizing = { .width = CLAY_SIZING_GROW(0) },
                                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                            }
                        }) {
                            CLAY_TEXT(str8_to_clay(player->name), CLAY_TEXT_CONFIG({
                                .fontId = FONT_ID_BODY_16,
                                .fontSize = 18,
                                .textColor = dashAccentPurple
                            }));
                        }

                        // Number of registrations
                        CLAY(CLAY_IDI("PlayerRegistrations", idx), {
                            .layout = {
                                .sizing = { .width = CLAY_SIZING_FIXED(150) },
                                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                            }
                        }) {
                            s32 positions[64];
                            u32 count = find_all_filled_slots(player->registrations, positions);
                            CLAY_TEXT(str8_to_clay(str8_from_u32(data.frameArena, count)), CLAY_TEXT_CONFIG({
                                .fontId = FONT_ID_PRESS_START_2P,
                                .fontSize = 24,
                                .textColor = dashAccentTeal
                            }));
                        }

                        RenderPlayersActions(idx);
                    }

                    idx = player->nxt;
                }
            }
        }
    }
}

/////////////////////////////////////////////////
// Player detail page

// Stat card for player detail page (idx used for unique CLAY_IDI)
void
RenderPlayerStatCard(u32 idx, Clay_String label, Clay_String value, Clay_Color accentColor)
{
    CLAY(CLAY_IDI("PlayerStatCard", idx), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)}
        },
        .cornerRadius = CLAY_CORNER_RADIUS(12)
    }) {
        // Colored accent bar at top
        CLAY(CLAY_IDI("PlayerStatAccent", idx), {
            .layout = {
                .sizing = {.height = CLAY_SIZING_FIXED(6), .width = CLAY_SIZING_GROW(0)}
            },
            .backgroundColor = accentColor,
            .cornerRadius = { 12, 12, 0, 0 }
        }) {}
        // Card content
        CLAY(CLAY_IDI("PlayerStatContent", idx), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                .padding = { 16, 16, 12, 16 },
                .childGap = 6,
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
            },
            .backgroundColor = dashCardBg,
            .cornerRadius = { 0, 0, 12, 12 }
        }) {
            CLAY_TEXT(label, CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 11,
                .textColor = dashLabelText
            }));
            CLAY_TEXT(value, CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_PRESS_START_2P,
                .fontSize = 28,
                .textColor = accentColor
            }));
        }
    }
}

// Event row for player detail page
void
RenderPlayerEventRow(u32 tournament_idx, u32 player_idx)
{
    Event *tournament = data.tournaments.events + tournament_idx;

    // Determine phase color and label
    Clay_Color statusColor;
    Clay_String statusLabel;
    switch (tournament->phase)
    {
        case PHASE_REGISTRATION:
            statusColor = dashAccentTeal;
            statusLabel = CLAY_STRING("Iscrizioni");
            break;
        case PHASE_GROUP:
            statusColor = dashAccentOrange;
            statusLabel = CLAY_STRING("Gruppi");
            break;
        case PHASE_KNOCKOUT:
            statusColor = dashAccentPurple;
            statusLabel = CLAY_STRING("Eliminazione");
            break;
        case PHASE_FINISHED:
            statusColor = dashAccentGold;
            statusLabel = CLAY_STRING("Completato");
            break;
    }

    // Get format label
    Clay_String formatLabel;
    if (tournament->format == FORMAT_KNOCKOUT)
    {
        formatLabel = CLAY_STRING("Eliminazione diretta");
    }
    else
    {
        formatLabel = CLAY_STRING("Gruppi + Eliminazione");
    }

    bool can_unregister = (tournament->phase == PHASE_REGISTRATION);

    // Encode both indices for the unregister handler
    intptr_t *pEncoded = push_array(data.frameArena, intptr_t, 1);
    *pEncoded = (intptr_t)player_idx | ((intptr_t)tournament_idx << 16);

    u32 *pTournamentIdx = push_array(data.frameArena, u32, 1);
    *pTournamentIdx = tournament_idx;

    // Outer container with accent bar
    CLAY(CLAY_IDI("PD_EnrolledOuter", tournament_idx), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)}
        },
        .cornerRadius = CLAY_CORNER_RADIUS(10)
    }) {
        // Left accent bar
        CLAY(CLAY_IDI("PD_EnrolledAccent", tournament_idx), {
            .layout = {
                .sizing = {.width = CLAY_SIZING_FIXED(6), .height = CLAY_SIZING_GROW(0)}
            },
            .backgroundColor = statusColor,
            .cornerRadius = { 10, 0, 0, 10 }
        }) {}

        // Main content
        CLAY(CLAY_IDI("PD_EnrolledRow", tournament_idx), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                .padding = { 14, 14, 12, 12 },
                .childGap = 12,
                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = playerRowColor,
            .cornerRadius = { 0, 10, 10, 0 }
        }) {
            // Event info (clickable area for navigation)
            CLAY(CLAY_IDI("PD_EnrolledInfo", tournament_idx), {
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                    .padding = { 4, 4, 4, 4 },
                    .childGap = 4
                },
                .backgroundColor = Clay_Hovered() ? playerRowHoverColor : playerRowColor,
                .cornerRadius = CLAY_CORNER_RADIUS(6)
            }) {
                Clay_OnHover(HandlePlayerEventRowClick, pTournamentIdx);

                // Event name row with status badge
                CLAY(CLAY_IDI("PD_EnrolledName", tournament_idx), {
                    .layout = {
                        .layoutDirection = CLAY_LEFT_TO_RIGHT,
                        .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_FIT(0)},
                        .childGap = 10,
                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                    }
                }) {
                    CLAY_TEXT(str8_to_clay(tournament->name), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 15,
                        .textColor = dashStatNumber
                    }));
                    // Status badge
                    CLAY(CLAY_IDI("PD_EnrolledBadge", tournament_idx), {
                        .layout = {
                            .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_FIT(0)},
                            .padding = { 8, 8, 3, 3 }
                        },
                        .backgroundColor = statusColor,
                        .cornerRadius = CLAY_CORNER_RADIUS(8)
                    }) {
                        CLAY_TEXT(statusLabel, CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_BODY_16,
                            .fontSize = 9,
                            .textColor = COLOR_WHITE
                        }));
                    }
                }
                // Format
                CLAY_TEXT(formatLabel, CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 11,
                    .textColor = dashLabelText
                }));
            }

            // Right: Unregister button (only active during registration phase)
            if (can_unregister)
            {
                CLAY(CLAY_IDI("PD_EnrolledUnreg", tournament_idx), {
                    .layout = {
                        .sizing = {.width = CLAY_SIZING_FIXED(32), .height = CLAY_SIZING_FIXED(32)},
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = Clay_Hovered() ? dashAccentCoral : removeButtonColor,
                    .cornerRadius = CLAY_CORNER_RADIUS(16)
                }) {
                    Clay_OnHover(HandleUnregisterPlayerFromDetail, pEncoded);
                    CLAY_TEXT(CLAY_STRING("-"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 18,
                        .textColor = Clay_Hovered() ? COLOR_WHITE : removeTextColor
                    }));
                }
            }
            else
            {
                // Disabled unregister button (grayed out)
                CLAY(CLAY_IDI("PD_EnrolledUnregDis", tournament_idx), {
                    .layout = {
                        .sizing = {.width = CLAY_SIZING_FIXED(32), .height = CLAY_SIZING_FIXED(32)},
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = { 235, 235, 235, 255 },
                    .cornerRadius = CLAY_CORNER_RADIUS(16)
                }) {
                    CLAY_TEXT(CLAY_STRING("-"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 18,
                        .textColor = { 190, 190, 190, 255 }
                    }));
                }
            }
        }
    }
}

void
RenderAvailableEventRow(u32 tournament_idx, u32 player_idx)
{
    Event *tournament = data.tournaments.events + tournament_idx;

    // Determine phase color and label (matching enrolled events style)
    Clay_Color statusColor;
    Clay_String statusLabel;
    switch (tournament->phase)
    {
        case PHASE_REGISTRATION:
            statusColor = dashAccentTeal;
            statusLabel = CLAY_STRING("Iscrizioni");
            break;
        case PHASE_GROUP:
            statusColor = dashAccentOrange;
            statusLabel = CLAY_STRING("Gruppi");
            break;
        case PHASE_KNOCKOUT:
            statusColor = dashAccentPurple;
            statusLabel = CLAY_STRING("Eliminazione");
            break;
        case PHASE_FINISHED:
            statusColor = dashAccentGold;
            statusLabel = CLAY_STRING("Completato");
            break;
    }

    // Get format label
    Clay_String formatLabel;
    if (tournament->format == FORMAT_KNOCKOUT)
    {
        formatLabel = CLAY_STRING("Eliminazione diretta");
    }
    else
    {
        formatLabel = CLAY_STRING("Gruppi + Eliminazione");
    }

    bool can_enroll = (tournament->phase == PHASE_REGISTRATION);

    // Encode both indices for the handler
    intptr_t *pEncoded = push_array(data.frameArena, intptr_t, 1);
    *pEncoded = (intptr_t)player_idx | ((intptr_t)tournament_idx << 16);

    u32 *pTournamentIdx = push_array(data.frameArena, u32, 1);
    *pTournamentIdx = tournament_idx;

    // Outer container with accent bar (matching enrolled events style)
    CLAY(CLAY_IDI("PD_AvailOuter", tournament_idx), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)}
        },
        .cornerRadius = CLAY_CORNER_RADIUS(10)
    }) {
        // Left accent bar (lighter version of status color)
        CLAY(CLAY_IDI("PD_AvailAccent", tournament_idx), {
            .layout = {
                .sizing = {.width = CLAY_SIZING_FIXED(6), .height = CLAY_SIZING_GROW(0)}
            },
            .backgroundColor = {
                (u8)(statusColor.r + (255 - statusColor.r) / 2),
                (u8)(statusColor.g + (255 - statusColor.g) / 2),
                (u8)(statusColor.b + (255 - statusColor.b) / 2),
                255
            },
            .cornerRadius = { 10, 0, 0, 10 }
        }) {}

        // Main content
        CLAY(CLAY_IDI("PD_AvailRow", tournament_idx), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                .padding = { 14, 14, 12, 12 },
                .childGap = 12,
                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = playerRowColor,
            .cornerRadius = { 0, 10, 10, 0 }
        }) {
            // Event info (clickable area for navigation)
            CLAY(CLAY_IDI("PD_AvailInfo", tournament_idx), {
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                    .padding = { 4, 4, 4, 4 },
                    .childGap = 4
                },
                .backgroundColor = Clay_Hovered() ? playerRowHoverColor : (Clay_Color){ 0, 0, 0, 0 },
                .cornerRadius = CLAY_CORNER_RADIUS(6)
            }) {
                Clay_OnHover(HandlePlayerEventRowClick, pTournamentIdx);

                // Event name row with status badge
                CLAY(CLAY_IDI("PD_AvailName", tournament_idx), {
                    .layout = {
                        .layoutDirection = CLAY_LEFT_TO_RIGHT,
                        .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_FIT(0)},
                        .childGap = 10,
                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                    }
                }) {
                    CLAY_TEXT(str8_to_clay(tournament->name), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 15,
                        .textColor = dashLabelText
                    }));
                    // Status badge (same as enrolled events)
                    CLAY(CLAY_IDI("PD_AvailBadge", tournament_idx), {
                        .layout = {
                            .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_FIT(0)},
                            .padding = { 8, 8, 3, 3 }
                        },
                        .backgroundColor = statusColor,
                        .cornerRadius = CLAY_CORNER_RADIUS(8)
                    }) {
                        CLAY_TEXT(statusLabel, CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_BODY_16,
                            .fontSize = 9,
                            .textColor = COLOR_WHITE
                        }));
                    }
                }
                // Format
                CLAY_TEXT(formatLabel, CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 11,
                    .textColor = { 160, 160, 160, 255 }
                }));
            }

            // Right: Add button (active or disabled based on phase)
            if (can_enroll)
            {
                CLAY(CLAY_IDI("PD_AvailAdd", tournament_idx), {
                    .layout = {
                        .sizing = {.width = CLAY_SIZING_FIXED(32), .height = CLAY_SIZING_FIXED(32)},
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = Clay_Hovered() ? dashAccentTeal : addButtonColor,
                    .cornerRadius = CLAY_CORNER_RADIUS(16)
                }) {
                    Clay_OnHover(HandleEnrollPlayerFromDetail, pEncoded);
                    CLAY_TEXT(CLAY_STRING("+"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 18,
                        .textColor = COLOR_WHITE
                    }));
                }
            }
            else
            {
                // Disabled add button (grayed out - tournament not in registration phase)
                CLAY(CLAY_IDI("PD_AvailAddDis", tournament_idx), {
                    .layout = {
                        .sizing = {.width = CLAY_SIZING_FIXED(32), .height = CLAY_SIZING_FIXED(32)},
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = { 235, 235, 235, 255 },
                    .cornerRadius = CLAY_CORNER_RADIUS(16)
                }) {
                    CLAY_TEXT(CLAY_STRING("+"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 18,
                        .textColor = { 190, 190, 190, 255 }
                    }));
                }
            }
        }
    }
}

void
RenderPlayerGoBackButton(void)
{
    CLAY(CLAY_ID("PlayerGoBackRow"), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)}
        }
    }) {
        CLAY(CLAY_ID("PlayerGoBack"), {
            .layout = {
                .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                .padding = { 12, 12, 8, 8 },
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = Clay_Hovered() ? dashAccentPurple : dashAccentTeal,
            .cornerRadius = CLAY_CORNER_RADIUS(10)
        }) {
            Clay_OnHover(HandleGoBackFromPlayer, NULL);
            CLAY_TEXT(CLAY_STRING("< Go back"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 14,
                .textColor = COLOR_WHITE
            }));
        }
    }
}

void
RenderPlayerBanner(u32 player_idx)
{
    Player *player = data.players.players + player_idx;

    CLAY(CLAY_ID("PlayerBanner"), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
            .padding = { 24, 24, 20, 20 },
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
        },
        .backgroundColor = dashAccentPurple,
        .cornerRadius = CLAY_CORNER_RADIUS(16)
    }) {
        CLAY_TEXT(str8_to_clay(player->name), CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_PRESS_START_2P,
            .fontSize = 28,
            .textColor = COLOR_WHITE
        }));
    }
}

void
RenderPlayerDetail(u32 player_idx)
{
    Player *player = data.players.players + player_idx;

    // Count registrations and get tournament indices
    s32 registered_tournaments[64];
    u32 registration_count = find_all_filled_slots(player->registrations, registered_tournaments);

    CLAY(CLAY_ID("PlayerDetail"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = layoutExpand,
            .childGap = 20,
            .padding = { 8, 8, 8, 8 },
            .childAlignment = {
                .y = CLAY_ALIGN_Y_TOP
            },
        },
        .backgroundColor = dashBgGradientTop
    }) {
        RenderPlayerGoBackButton();
        RenderPlayerBanner(player_idx);

        // Stats cards row (mimic StatCard from player-profile.jsx)
        CLAY(CLAY_ID("PlayerStatsRow"), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                .childGap = 12
            }
        }) {
            // Registrations stat card
            String8 reg_count_str = str8_from_u32(data.frameArena, registration_count);
            RenderPlayerStatCard(0, CLAY_STRING("ISCRIZIONI"), str8_to_clay(reg_count_str), dashAccentTeal);

            // Placeholder stat cards - in a full implementation these would show actual match stats
            RenderPlayerStatCard(1, CLAY_STRING("VITTORIE"), CLAY_STRING("-"), dashAccentGold);
            RenderPlayerStatCard(2, CLAY_STRING("SCONFITTE"), CLAY_STRING("-"), dashAccentCoral);
            RenderPlayerStatCard(3, CLAY_STRING("PUNTI"), CLAY_STRING("-"), dashAccentPurple);
        }

        // Two-column layout
        CLAY(CLAY_ID("PlayerContent"), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = {.height = CLAY_SIZING_GROW(0), .width = CLAY_SIZING_GROW(0)},
                .childGap = 16
            }
        }) {
            // Left column: Events section
            CLAY(CLAY_ID("PlayerEventsSection"), {
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                    .childGap = 12
                }
            }) {
                // Section header
                CLAY(CLAY_ID("PlayerEventsSectionHeader"), {
                    .layout = {
                        .layoutDirection = CLAY_LEFT_TO_RIGHT,
                        .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                        .padding = { 16, 16, 12, 12 },
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = dashAccentOrange,
                    .cornerRadius = CLAY_CORNER_RADIUS(10)
                }) {
                    CLAY_TEXT(CLAY_STRING("PARTECIPAZIONI EVENTI"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_PRESS_START_2P,
                        .fontSize = 16,
                        .textColor = COLOR_WHITE
                    }));
                }

                // Enrolled events list container
                CLAY(CLAY_ID("PlayerEventsList"), {
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                        .padding = { 12, 12, 12, 12 },
                        .childGap = 8
                    },
                    .backgroundColor = dashCardBg,
                    .cornerRadius = CLAY_CORNER_RADIUS(12),
                    .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() }
                }) {
                    if (registration_count == 0)
                    {
                        CLAY(CLAY_ID("NoEventsMessage"), {
                            .layout = {
                                .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                                .padding = { 16, 16, 24, 24 },
                                .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                            }
                        }) {
                            CLAY_TEXT(CLAY_STRING("Nessuna iscrizione"), CLAY_TEXT_CONFIG({
                                .fontId = FONT_ID_BODY_16,
                                .fontSize = 14,
                                .textColor = dashLabelText
                            }));
                        }
                    }
                    else
                    {
                        for (u32 i = 0; i < registration_count; i++)
                        {
                            u32 tournament_idx = registered_tournaments[i];
                            RenderPlayerEventRow(tournament_idx, player_idx);
                        }
                    }
                }

                // Available events section header
                CLAY(CLAY_ID("AvailableEventsSectionHeader"), {
                    .layout = {
                        .layoutDirection = CLAY_LEFT_TO_RIGHT,
                        .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                        .padding = { 16, 16, 12, 12 },
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = dashAccentTeal,
                    .cornerRadius = CLAY_CORNER_RADIUS(10)
                }) {
                    CLAY_TEXT(CLAY_STRING("EVENTI DISPONIBILI"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_PRESS_START_2P,
                        .fontSize = 16,
                        .textColor = COLOR_WHITE
                    }));
                }

                // Available events list container
                CLAY(CLAY_ID("AvailableEventsList"), {
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                        .padding = { 12, 12, 12, 12 },
                        .childGap = 8
                    },
                    .backgroundColor = dashCardBg,
                    .cornerRadius = CLAY_CORNER_RADIUS(12),
                    .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() }
                }) {
                    // Iterate through all events and show ones player is NOT enrolled in
                    u32 idx_tail = data.tournaments.len - 1;
                    u32 idx = data.tournaments.events->nxt;
                    u32 available_count = 0;

                    while (idx != idx_tail)
                    {
                        Event *tournament = data.tournaments.events + idx;
                        // Check if player is NOT registered to this tournament
                        bool is_registered = (player->registrations >> idx) & 1;
                        if (!is_registered)
                        {
                            RenderAvailableEventRow(idx, player_idx);
                            available_count++;
                        }
                        idx = tournament->nxt;
                    }

                    if (available_count == 0)
                    {
                        CLAY(CLAY_ID("NoAvailableEventsMessage"), {
                            .layout = {
                                .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                                .padding = { 16, 16, 24, 24 },
                                .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                            }
                        }) {
                            CLAY_TEXT(CLAY_STRING("Iscritto a tutti gli eventi"), CLAY_TEXT_CONFIG({
                                .fontId = FONT_ID_BODY_16,
                                .fontSize = 14,
                                .textColor = dashLabelText
                            }));
                        }
                    }
                }
            }

            // Right column: Additional info section (placeholder for future features)
            CLAY(CLAY_ID("PlayerInfoSection"), {
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                    .childGap = 12
                }
            }) {
                // Section header
                CLAY(CLAY_ID("PlayerInfoSectionHeader"), {
                    .layout = {
                        .layoutDirection = CLAY_LEFT_TO_RIGHT,
                        .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                        .padding = { 16, 16, 12, 12 },
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = dashAccentPurple,
                    .cornerRadius = CLAY_CORNER_RADIUS(10)
                }) {
                    CLAY_TEXT(CLAY_STRING("STATISTICHE"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_PRESS_START_2P,
                        .fontSize = 16,
                        .textColor = COLOR_WHITE
                    }));
                }

                // Info card container
                CLAY(CLAY_ID("PlayerInfoCard"), {
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                        .padding = { 16, 16, 24, 24 },
                        .childGap = 12,
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                    },
                    .backgroundColor = dashCardBg,
                    .cornerRadius = CLAY_CORNER_RADIUS(12)
                }) {
                    CLAY_TEXT(CLAY_STRING("Statistiche dettagliate"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 14,
                        .textColor = dashLabelText
                    }));
                    CLAY_TEXT(CLAY_STRING("in arrivo..."), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 12,
                        .textColor = dashLabelText
                    }));
                }
            }
        }
    }
}

/////////////////////////////////////////////////
// Players list page

void
RenderPlayers(void)
{
    if (data.selectedPlayerIdx != 0)
    {
        RenderPlayerDetail(data.selectedPlayerIdx);
    }
    else
    {
        CLAY(CLAY_ID("Players"), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = layoutExpand,
                .childAlignment = {
                    .y = CLAY_ALIGN_Y_TOP
                },
                .childGap = 20,
                .padding = { 8, 8, 8, 8 }
            },
            .backgroundColor = dashBgGradientTop,
        }) {
            RenderPlayersBanner();
            RenderPlayersHeader();
            RenderPlayersList();
        }
    }
}

void
RenderResults(void)
{
    CLAY(CLAY_ID("Results"), {
        .layout = {
            .sizing = layoutExpand,
            .padding = { 16, 16, 0, 0 },
            .childGap = 16,
            .childAlignment = {
                .y = CLAY_ALIGN_Y_CENTER
            }
        },
        .backgroundColor = COLOR_BLACK
    }) {

    }
}

///////////////////////////////////////////////////////////////////////////////
// Modal Overlay

static Clay_Color modalOverlayColor = { 0, 0, 0, 150 };

void
RenderConfirmationModal(void)
{
    ConfirmationModal modal = data.confirmationModal;

    Clay_String titleText;
    Clay_String messageText;
    Clay_String confirmText;

    switch (modal)
    {
        case MODAL_DELETE_TOURNAMENT:
        {
            titleText = CLAY_STRING("Delete Tournament?");
            messageText = CLAY_STRING("This tournament will be permanently deleted.");
            confirmText = CLAY_STRING("Yes, delete");
            break;
        }
        case MODAL_DELETE_PLAYER:
        {
            titleText = CLAY_STRING("Delete Player?");
            messageText = CLAY_STRING("This player will be permanently deleted.");
            confirmText = CLAY_STRING("Yes, delete");
            break;
        }
        case MODAL_RETURN_TO_REGISTRATION:
        {
            titleText = CLAY_STRING("Are you sure?");
            messageText = CLAY_STRING("All tournament progress will be lost.");
            confirmText = CLAY_STRING("Yes, reset");
            break;
        }
        case MODAL_RETURN_TO_GROUP_PHASE:
        {
            titleText = CLAY_STRING("Return to Group Phase?");
            messageText = CLAY_STRING("All knockout progress will be lost.");
            confirmText = CLAY_STRING("Yes, return");
            break;
        }
        default:
            return;
    }

    // Get entity name for delete modals
    Clay_String entityName = CLAY_STRING("");
    if (modal == MODAL_DELETE_TOURNAMENT)
    {
        assert(data.deleteTournamentIdx != 0);

        Event *tournament = data.tournaments.events + data.deleteTournamentIdx;
        entityName = str8_to_clay(tournament->name);
    }
    else if (modal == MODAL_DELETE_PLAYER)
    {
        assert(data.deletePlayerIdx != 0);

        Player *player = data.players.players + data.deletePlayerIdx;
        entityName = str8_to_clay(player->name);
    }

    // Full-screen overlay that blocks all interactions
    CLAY(CLAY_ID("ModalOverlay"), {
        .layout = {
            .sizing = layoutExpand,
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
        },
        .backgroundColor = modalOverlayColor,
        .floating = {
            .attachTo = CLAY_ATTACH_TO_ROOT,
            .attachPoints = { .element = CLAY_ATTACH_POINT_CENTER_CENTER, .parent = CLAY_ATTACH_POINT_CENTER_CENTER },
            .pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE
        }
    }) {
        // Confirmation dialog box
        CLAY(CLAY_ID("ConfirmDialog"), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                .padding = { 24, 24, 20, 20 },
                .childGap = 12,
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
            },
            .backgroundColor = dashCardBg,
            .cornerRadius = CLAY_CORNER_RADIUS(12),
            .border = { .width = {3, 3, 3, 3}, .color = dashAccentCoral }
        }) {
            // Title row (centered)
            CLAY(CLAY_ID("ModalTitleRow"), {
                .layout = {
                    .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                }
            }) {
                CLAY_TEXT(titleText, CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_PRESS_START_2P,
                    .fontSize = 18,
                    .textColor = dashAccentCoral,
                    .wrapMode = CLAY_TEXT_WRAP_NONE 
                }));
            }

            // Entity name (only for delete modals)
            if ((modal == MODAL_DELETE_TOURNAMENT || modal == MODAL_DELETE_PLAYER) && entityName.length > 0)
            {
                CLAY(CLAY_ID("ModalEntityNameRow"), {
                    .layout = {
                        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                        .padding = { 12, 12, 8, 8 },
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                    },
                    .backgroundColor = dashAccentOrange,
                    .cornerRadius = CLAY_CORNER_RADIUS(6)
                }) {
                    CLAY_TEXT(entityName, CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_PRESS_START_2P,
                        .fontSize = 14,
                        .textColor = COLOR_WHITE
                    }));
                }
            }

            // Warning message row (centered)
            CLAY(CLAY_ID("ModalMessageRow"), {
                .layout = {
                    .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                }
            }) {
                CLAY_TEXT(messageText, CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 14,
                    .textColor = matchVsColor
                }));
            }

            // Buttons row
            CLAY(CLAY_ID("ModalButtonsRow"), {
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                    .childGap = 16
                }
            }) {
                // Yes button
                CLAY(CLAY_ID("ModalYesButton"), {
                    .layout = {
                        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                        .padding = { 16, 16, 10, 10 },
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                    },
                    .backgroundColor = Clay_Hovered() ? removeButtonHoverColor : dashAccentCoral,
                    .cornerRadius = CLAY_CORNER_RADIUS(8)
                }) {
                    switch (modal)
                    {
                        case MODAL_DELETE_TOURNAMENT:
                            Clay_OnHover(HandleConfirmDeleteTournament, NULL);
                            break;
                        case MODAL_DELETE_PLAYER:
                            Clay_OnHover(HandleConfirmDeletePlayer, NULL);
                            break;
                        case MODAL_RETURN_TO_GROUP_PHASE:
                            Clay_OnHover(HandleConfirmReturnToGroupPhase, NULL);
                            break;
                        case MODAL_RETURN_TO_REGISTRATION:
                            Clay_OnHover(HandleConfirmReturnToRegistration, NULL);
                            break;
                        default:
                            break;
                    }
                    CLAY_TEXT(confirmText, CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 14,
                        .textColor = COLOR_WHITE
                    }));
                }

                // Cancel button
                CLAY(CLAY_ID("ModalCancelButton"), {
                    .layout = {
                        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                        .padding = { 16, 16, 10, 10 },
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                    },
                    .backgroundColor = Clay_Hovered() ? dashAccentPurple : dashAccentTeal,
                    .cornerRadius = CLAY_CORNER_RADIUS(8)
                }) {
                    switch (modal)
                    {
                        case MODAL_DELETE_TOURNAMENT:
                            Clay_OnHover(HandleCancelDeleteTournament, NULL);
                            break;
                        case MODAL_DELETE_PLAYER:
                            Clay_OnHover(HandleCancelDeletePlayer, NULL);
                            break;
                        case MODAL_RETURN_TO_GROUP_PHASE:
                            Clay_OnHover(HandleCancelReturnToGroupPhase, NULL);
                            break;
                        case MODAL_RETURN_TO_REGISTRATION:
                            Clay_OnHover(HandleCancelReturnToRegistration, NULL);
                            break;
                        default:
                            break;
                    }
                    CLAY_TEXT(CLAY_STRING("Cancel"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 14,
                        .textColor = COLOR_WHITE
                    }));
                }
            }
        }
    }
}

void
RenderRenameModal(void)
{
    Clay_String currentName;
    Clay_String titleText;
    TextBoxEnum textbox;

    if (data.renamingEventIdx)
    {
        Event *tournament = data.tournaments.events + data.renamingEventIdx;
        currentName = str8_to_clay(tournament->name);
        titleText = CLAY_STRING("Rename Event");
        textbox = TEXTBOX_EventRename;
    }
    else
    {
        Player *player = data.players.players + data.renamingPlayerIdx;
        currentName = str8_to_clay(player->name);
        titleText = CLAY_STRING("Rename Player");
        textbox = TEXTBOX_PlayerRename;
    }

    // Process keyboard input when focused
    if (data.focusedTextbox == textbox)
    {
        TextInput_ProcessKeyboard(&data.textInputs[textbox]);
    }

    // Full-screen overlay that blocks all interactions
    CLAY(CLAY_ID("RenameModalOverlay"), {
        .layout = {
            .sizing = layoutExpand,
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
        },
        .backgroundColor = modalOverlayColor,
        .floating = {
            .attachTo = CLAY_ATTACH_TO_ROOT,
            .attachPoints = { .element = CLAY_ATTACH_POINT_CENTER_CENTER, .parent = CLAY_ATTACH_POINT_CENTER_CENTER },
            .pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE
        }
    }) {
        // Rename dialog box
        CLAY(CLAY_ID("RenameDialog"), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                .padding = { 24, 24, 20, 20 },
                .childGap = 12,
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
            },
            .backgroundColor = dashCardBg,
            .cornerRadius = CLAY_CORNER_RADIUS(12),
            .border = { .width = {3, 3, 3, 3}, .color = dashAccentOrange }
        }) {
            // Title row
            CLAY(CLAY_ID("RenameTitleRow"), {
                .layout = {
                    .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                }
            }) {
                CLAY_TEXT(titleText, CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_PRESS_START_2P,
                    .fontSize = 18,
                    .textColor = dashAccentOrange,
                    .wrapMode = CLAY_TEXT_WRAP_NONE
                }));
            }

            // Current name display
            CLAY(CLAY_ID("RenameCurrentNameRow"), {
                .layout = {
                    .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                    .padding = { 12, 12, 8, 8 },
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                },
                .backgroundColor = dashAccentPurple,
                .cornerRadius = CLAY_CORNER_RADIUS(6)
            }) {
                CLAY_TEXT(currentName, CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_PRESS_START_2P,
                    .fontSize = 14,
                    .textColor = COLOR_WHITE
                }));
            }

            // Text input for new name
            if (data.renamingEventIdx)
            {
                TextInput_Render(TEXTBOX_EventRename, CLAY_STRING("EventRenameInput"),
                    CLAY_STRING("EventRenameInputScroll"), CLAY_STRING("Enter new name..."));

                // Show duplicate name warning for event rename
                if (data.duplicateWarning == DUPLICATE_EVENT_RENAME)
                {
                    RenderDuplicateWarning(CLAY_STRING("EventRenameInput"),
                        CLAY_STRING("Event already exists"), DUPLICATE_EVENT_RENAME);
                }
            }
            else
            {
                TextInput_Render(TEXTBOX_PlayerRename, CLAY_STRING("PlayerRenameInput"),
                    CLAY_STRING("PlayerRenameInputScroll"), CLAY_STRING("Enter new name..."));

                // Show duplicate name warning for player rename
                if (data.duplicateWarning == DUPLICATE_PLAYER_RENAME)
                {
                    RenderDuplicateWarning(CLAY_STRING("PlayerRenameInput"),
                        CLAY_STRING("Player already exists"), DUPLICATE_PLAYER_RENAME);
                }
            }

            // Buttons row
            CLAY(CLAY_ID("RenameButtonsRow"), {
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                    .childGap = 16
                }
            }) {
                // OK button
                CLAY(CLAY_ID("RenameOkButton"), {
                    .layout = {
                        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                        .padding = { 16, 16, 10, 10 },
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                    },
                    .backgroundColor = Clay_Hovered() ? dashAccentTeal : addButtonColor,
                    .cornerRadius = CLAY_CORNER_RADIUS(8)
                }) {
                    if (data.renamingEventIdx)
                    {
                        Clay_OnHover(HandleConfirmRenameEvent, NULL);
                    }
                    else
                    {
                        Clay_OnHover(HandleConfirmRenamePlayer, NULL);
                    }

                    CLAY_TEXT(CLAY_STRING("OK"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 14,
                        .textColor = COLOR_WHITE
                    }));
                }

                // Cancel button
                CLAY(CLAY_ID("RenameCancelButton"), {
                    .layout = {
                        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                        .padding = { 16, 16, 10, 10 },
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                    },
                    .backgroundColor = Clay_Hovered() ? dashAccentPurple : dashAccentTeal,
                    .cornerRadius = CLAY_CORNER_RADIUS(8)
                }) {
                    if (data.renamingEventIdx)
                    {
                        Clay_OnHover(HandleCancelRenameEvent, NULL);
                    }
                    else
                    {
                        Clay_OnHover(HandleCancelRenamePlayer, NULL);
                    }
                    CLAY_TEXT(CLAY_STRING("Cancel"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 14,
                        .textColor = COLOR_WHITE
                    }));
                }
            }
        }
    }
}

void
RenderRegisterScoreModal(void)
{
    Event *tournament = data.tournaments.events + data.selectedTournamentIdx;
    u8 row_player_idx = tournament->group_phase.groups[data.scoreModalGroupIdx][data.scoreModalRowIdx];
    u8 col_player_idx = tournament->group_phase.groups[data.scoreModalGroupIdx][data.scoreModalColIdx];

    Player *row_player = data.players.players + row_player_idx;
    Player *col_player = data.players.players + col_player_idx;

    // Process keyboard input for score textboxes
    if (data.focusedTextbox == TEXTBOX_Score1)
    {
        TextInput_ProcessKeyboard(&data.textInputs[TEXTBOX_Score1]);
    }
    else if (data.focusedTextbox == TEXTBOX_Score2)
    {
        TextInput_ProcessKeyboard(&data.textInputs[TEXTBOX_Score2]);
    }

    // Full-screen overlay that blocks all interactions
    CLAY(CLAY_ID("ScoreModalOverlay"), {
        .layout = {
            .sizing = layoutExpand,
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
        },
        .backgroundColor = modalOverlayColor,
        .floating = {
            .attachTo = CLAY_ATTACH_TO_ROOT,
            .attachPoints = { .element = CLAY_ATTACH_POINT_CENTER_CENTER, .parent = CLAY_ATTACH_POINT_CENTER_CENTER },
            .pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE
        }
    }) {
        // Score dialog box
        CLAY(CLAY_ID("ScoreDialog"), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                .padding = { 24, 24, 20, 20 },
                .childGap = 16,
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
            },
            .backgroundColor = dashCardBg,
            .cornerRadius = CLAY_CORNER_RADIUS(12),
            .border = { .width = {3, 3, 3, 3}, .color = dashAccentTeal }
        }) {
            // Title row
            CLAY(CLAY_ID("ScoreTitleRow"), {
                .layout = {
                    .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                }
            }) {
                CLAY_TEXT(CLAY_STRING("Register Score"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_PRESS_START_2P,
                    .fontSize = 18,
                    .textColor = dashAccentTeal,
                    .wrapMode = CLAY_TEXT_WRAP_NONE
                }));
            }

            // Match info badge (Group X)
            String8 group_prefix = str8_lit("GROUP ");
            String8 group_num = str8_from_u32(data.frameArena, data.scoreModalGroupIdx + 1);
            String8 group_label = str8_cat(data.frameArena, group_prefix, group_num);
            CLAY(CLAY_ID("ScoreGroupBadge"), {
                .layout = {
                    .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                    .padding = { 12, 12, 6, 6 },
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                },
                .backgroundColor = dashAccentPurple,
                .cornerRadius = CLAY_CORNER_RADIUS(6)
            }) {
                CLAY_TEXT(str8_to_clay(group_label), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_PRESS_START_2P,
                    .fontSize = 12,
                    .textColor = COLOR_WHITE
                }));
            }

            // Player scores input section
            CLAY(CLAY_ID("ScoreInputsContainer"), {
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                    .childGap = 16,
                    .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                }
            }) {
                // Player 1 (row player)
                CLAY(CLAY_ID("Score1Container"), {
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                        .childGap = 8,
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                    }
                }) {
                    // Player 1 name
                    CLAY(CLAY_ID("Score1NameBadge"), {
                        .layout = {
                            .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                            .padding = { 10, 10, 6, 6 },
                            .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                        },
                        .backgroundColor = dashAccentOrange,
                        .cornerRadius = CLAY_CORNER_RADIUS(6)
                    }) {
                        CLAY_TEXT(str8_to_clay_truncated(data.frameArena, row_player->name, MAX_DISPLAY_NAME_LEN), CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_BODY_16,
                            .fontSize = 14,
                            .textColor = COLOR_WHITE
                        }));
                    }

                    // Score 1 textbox
                    TextInput_Render(TEXTBOX_Score1, CLAY_STRING("Score1Input"),
                        CLAY_STRING("Score1InputScroll"), CLAY_STRING("0"));
                }

                // VS text
                CLAY(CLAY_ID("ScoreVsText"), {
                    .layout = {
                        .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                        .padding = { 8, 8, 0, 0 }
                    }
                }) {
                    CLAY_TEXT(CLAY_STRING("vs"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_PRESS_START_2P,
                        .fontSize = 14,
                        .textColor = matchVsColor
                    }));
                }

                // Player 2 (col player)
                CLAY(CLAY_ID("Score2Container"), {
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                        .childGap = 8,
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                    }
                }) {
                    // Player 2 name
                    CLAY(CLAY_ID("Score2NameBadge"), {
                        .layout = {
                            .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                            .padding = { 10, 10, 6, 6 },
                            .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                        },
                        .backgroundColor = dashAccentCoral,
                        .cornerRadius = CLAY_CORNER_RADIUS(6)
                    }) {
                        CLAY_TEXT(str8_to_clay_truncated(data.frameArena, col_player->name, MAX_DISPLAY_NAME_LEN), CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_BODY_16,
                            .fontSize = 14,
                            .textColor = COLOR_WHITE
                        }));
                    }

                    // Score 2 textbox
                    TextInput_Render(TEXTBOX_Score2, CLAY_STRING("Score2Input"),
                        CLAY_STRING("Score2InputScroll"), CLAY_STRING("0"));
                }
            }

            // Buttons row
            CLAY(CLAY_ID("ScoreButtonsRow"), {
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                    .childGap = 16
                }
            }) {
                // OK button
                CLAY(CLAY_ID("ScoreOkButton"), {
                    .layout = {
                        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                        .padding = { 16, 16, 10, 10 },
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                    },
                    .backgroundColor = Clay_Hovered() ? dashAccentTeal : addButtonColor,
                    .cornerRadius = CLAY_CORNER_RADIUS(8)
                }) {
                    Clay_OnHover(HandleConfirmScoreModal, NULL);
                    CLAY_TEXT(CLAY_STRING("Save"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 14,
                        .textColor = COLOR_WHITE
                    }));
                }

                // Cancel button
                CLAY(CLAY_ID("ScoreCancelButton"), {
                    .layout = {
                        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                        .padding = { 16, 16, 10, 10 },
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                    },
                    .backgroundColor = Clay_Hovered() ? dashAccentPurple : dashAccentTeal,
                    .cornerRadius = CLAY_CORNER_RADIUS(8)
                }) {
                    Clay_OnHover(HandleCancelScoreModal, NULL);
                    CLAY_TEXT(CLAY_STRING("Cancel"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 14,
                        .textColor = COLOR_WHITE
                    }));
                }
            }
        }
    }
}

Clay_RenderCommandArray
CreateLayout(void)
{
    data.frameArena->pos = ARENA_HEADER_SIZE;

    Clay_BeginLayout();

    CLAY(CLAY_ID("OuterContainer"), {
        .backgroundColor = COLOR_OFF_WHITE,
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = layoutExpand,
            .padding = CLAY_PADDING_ALL(16),
            .childGap = 16
        }
    }) {
        Clay_OnHover(HandleOuterContainerInteraction, NULL);

        CLAY(CLAY_ID("HeaderBar"), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = {
                    .height = CLAY_SIZING_FIXED(60),
                    .width = CLAY_SIZING_GROW(0)
                },
                .padding = { 16, 16, 0, 0 },
                .childGap = 16,
                .childAlignment = {
                    .y = CLAY_ALIGN_Y_CENTER
                }
            },
            .backgroundColor = headerBackgroundColor,
            .cornerRadius = CLAY_CORNER_RADIUS(8)
        }) {
            RenderHeaderButton(CLAY_STRING("Dashboard"), PAGE_Dashboard);
            RenderHeaderButton(CLAY_STRING("Events"),    PAGE_Events);
            RenderHeaderButton(CLAY_STRING("Players"),   PAGE_Players);
            RenderHeaderButton(CLAY_STRING("Results"),   PAGE_Results);
        };
        switch (data.selectedHeaderButton)
        {
            case PAGE_Dashboard:
                RenderDashboard();
                break;
            case PAGE_Events:
                RenderEvents();
                break;
            case PAGE_Players:
                RenderPlayers();
                break;
            case PAGE_Results:
                RenderResults();
                break;
        }

        // Render modal overlays (floating, rendered last so they appear on top)
        if (data.confirmationModal)
        {
            RenderConfirmationModal();
        }
        if (data.renamingEventIdx || data.renamingPlayerIdx)
        {
            RenderRenameModal();
        }
        if (data.modalScoreActive)
        {
            RenderRegisterScoreModal();
        }
    }

    Clay_RenderCommandArray renderCommands = Clay_EndLayout();

    for (int32_t i = 0; i < renderCommands.length; i++) {
        Clay_RenderCommandArray_Get(&renderCommands, i)->boundingBox.y += data.yOffset;
    }
    return renderCommands;
}
