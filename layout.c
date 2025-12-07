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
Clay_Color headerBackgroundColor  = { 255, 255, 255, 255 };


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

ClayVideoDemo_Data data = {0};

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

///////////////////////////////////////////////////////////////////////////////
// Event Handlers

void
HandleHeaderButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    Page page = (Page)userData;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        data.selectedHeaderButton = page;
    }
}

void
HandleGoBackInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        data.selectedTournamentIdx = 0;
    }
}

void
HandleEditTournament(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        u32 tournament_idx = (u32)userData;
        data.selectedTournamentIdx = tournament_idx;
    }
}

void
HandleDeleteTournament(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        u32 tournament_idx = (u32)userData;
        data.deleteTournamentIdx = tournament_idx;
        data.showDeleteTournamentConfirm = true;
    }
}

void
HandleDeletePlayer(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        u32 player_idx = (u32)userData;
        data.deletePlayerIdx = player_idx;
        data.showDeletePlayerConfirm = true;
    }
}

void
HandleOuterContainerInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        data.focusedTextbox = TEXTBOX_NULL;
    }
}

void
HandleChartHover(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    // Handle zoom with Cmd+/Cmd- while hovering over chart
    bool cmdPressed = IsKeyDown(KEY_LEFT_SUPER) || IsKeyDown(KEY_RIGHT_SUPER);

    if (cmdPressed)
    {
        if (IsKeyPressed(KEY_RIGHT_BRACKET)) // Which is actually the "+" on the Mac with italian keyboard layout
        {
            // Zoom in (Cmd +)
            data.chartZoomLevel += 0.1f;
            if (data.chartZoomLevel > 3.0f) data.chartZoomLevel = 3.0f;
        }
        if (IsKeyPressed(KEY_SLASH)) // Which is actually the "-" on the Mac with italian keyboard layout
        {
            // Zoom out (Cmd -)
            data.chartZoomLevel -= 0.1f;
            if (data.chartZoomLevel < 0.5f) data.chartZoomLevel = 0.5f;
        }
    }
}

void
HandleTextInput(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    data.mouseCursor = MOUSE_CURSOR_IBEAM;

    TextBoxEnum textBoxEnum = (TextBoxEnum)userData;

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
HandleAddEventButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        TextInput *textInput = &data.textInputs[TEXTBOX_Events];
        String8 eventName = str8((u8 *)textInput->buffer, textInput->len);
        // Copy string to persistent arena so it survives after text input changes
        String8 eventNameCopy = str8_copy(data.arena, eventName);
        entity_list_add(&data.tournaments, eventNameCopy);
    }
}

void
HandleAddPlayerButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        TextInput *textInput = &data.textInputs[TEXTBOX_Players];
        String8 playerName = str8((u8 *)textInput->buffer, textInput->len);
        // Copy string to persistent arena so it survives after text input changes
        String8 playerNameCopy = str8_copy(data.arena, playerName);
        entity_list_add(&data.players, playerNameCopy);
    }
}

void
HandleTogglePlayerRegistration(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        u32 player_idx = (u32)userData;
        Entity *player = data.players.entities + player_idx;
        Entity *tournament = data.tournaments.entities + data.selectedTournamentIdx;

        // Check if player is already registered to this tournament
        bool is_registered = (player->registrations >> ENTITY_IDX_TO_BIT(data.selectedTournamentIdx)) & 1;

        if (is_registered)
        {
            entity_list_unregister(&data.players, &data.tournaments, player->name, tournament->name);
        }
        else
        {
            entity_list_register(&data.players, &data.tournaments, player->name, tournament->name);
        }
    }
}

void
HandleAdvanceWinner(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;

    // Decode userData: lower 8 bits = player_idx, upper bits = bracket_pos
    u8 player_idx = (u8)(userData & 0xFF);
    u32 bracket_pos = (u32)(userData >> 8);

    Entity *tournament = data.tournaments.entities + data.selectedTournamentIdx;

    // Only allow changes when tournament is in progress
    if (tournament->state != TOURNAMENT_IN_PROGRESS) return;

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
            if (tournament->bracket[parent] == player_idx)
            {
                tournament->bracket[parent] = 0;
            }
            pos = parent;
        }
    }
    // Left-click: advance non-TBD player
    else if (player_idx != 0 && pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        tournament->bracket[parent_pos] = player_idx;
    }
}

void
HandleStartTournament(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        Entity *tournament = data.tournaments.entities + data.selectedTournamentIdx;

        // Only start if we have at least 2 players
        s32 positions[64];
        u32 num_players = find_all_filled_slots(tournament->registrations, positions);
        if (num_players >= 2)
        {
            tournament->state = TOURNAMENT_IN_PROGRESS;
        }
    }
}

void
HandleReturnToRegistration(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        data.showReturnToRegistrationConfirm = true;
    }
}

void
HandleConfirmReturnToRegistration(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        Entity *tournament = data.tournaments.entities + data.selectedTournamentIdx;
        tournament->state = TOURNAMENT_REGISTRATION;
        data.showReturnToRegistrationConfirm = false;
    }
}

void
HandleCancelReturnToRegistration(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        data.showReturnToRegistrationConfirm = false;
    }
}

void
HandleConfirmDeleteTournament(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        Entity *tournament = data.tournaments.entities + data.deleteTournamentIdx;
        entity_list_remove(&data.tournaments, &data.players, tournament->name);
        data.deleteTournamentIdx = 0;
        data.showDeleteTournamentConfirm = false;
    }
}

void
HandleCancelDeleteTournament(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        data.deleteTournamentIdx = 0;
        data.showDeleteTournamentConfirm = false;
    }
}

void
HandleConfirmDeletePlayer(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        Entity *player = data.players.entities + data.deletePlayerIdx;
        entity_list_remove(&data.players, &data.tournaments, player->name);
        data.deletePlayerIdx = 0;
        data.showDeletePlayerConfirm = false;
    }
}

void
HandleCancelDeletePlayer(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        data.deletePlayerIdx = 0;
        data.showDeletePlayerConfirm = false;
    }
}

void
HandleToggleTournamentFormat(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        Entity *tournament = data.tournaments.entities + data.selectedTournamentIdx;
        // Toggle between single elimination and groups
        if (tournament->format == FORMAT_SINGLE_ELIMINATION)
        {
            tournament->format = FORMAT_GROUPS_THEN_BRACKET;
        }
        else
        {
            tournament->format = FORMAT_SINGLE_ELIMINATION;
        }
    }
}

void
HandleStartRenameEvent(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        u32 event_idx = (u32)userData;
        data.renamingEventIdx = event_idx;
        data.focusedTextbox = TEXTBOX_EventRename;

        // Pre-fill the textbox with the current event name
        Entity *tournament = data.tournaments.entities + event_idx;
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
HandleConfirmRenameEvent(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
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
                entity_list_rename(&data.tournaments, data.renamingEventIdx, new_name);
            }

            // Clear rename state
            data.renamingEventIdx = 0;
            data.focusedTextbox = TEXTBOX_NULL;
        }
    }
}

void
HandleCancelRenameEvent(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        data.renamingEventIdx = 0;
        data.focusedTextbox = TEXTBOX_NULL;
    }
}

void
HandleStartRenamePlayer(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        u32 player_idx = (u32)userData;
        data.renamingPlayerIdx = player_idx;
        data.focusedTextbox = TEXTBOX_PlayerRename;

        // Pre-fill the textbox with the current player name
        Entity *player = data.players.entities + player_idx;
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
HandleConfirmRenamePlayer(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
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
                entity_list_rename(&data.players, data.renamingPlayerIdx, new_name);
            }

            // Clear rename state
            data.renamingPlayerIdx = 0;
            data.focusedTextbox = TEXTBOX_NULL;
        }
    }
}

void
HandleCancelRenamePlayer(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        data.renamingPlayerIdx = 0;
        data.focusedTextbox = TEXTBOX_NULL;
    }
}

void
HandleIncrementGroupSize(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        Entity *tournament = data.tournaments.entities + data.selectedTournamentIdx;
        if (tournament->group_phase.group_size < MAX_GROUP_SIZE)
        {
            tournament->group_phase.group_size++;
        }
    }
}

void
HandleDecrementGroupSize(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        Entity *tournament = data.tournaments.entities + data.selectedTournamentIdx;
        if (tournament->group_phase.group_size > 2)
        {
            tournament->group_phase.group_size--;
        }
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
        Clay_OnHover(HandleTextInput, textBoxEnum);

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

    CLAY(CLAY_IDI("HeaderButton", page), {
        .layout = { .padding = { 16, 16, 8, 8 }},
        .backgroundColor = Clay_Hovered() ? headerButtonHoverColor : headerButtonColor,
        .cornerRadius = CLAY_CORNER_RADIUS(5)
    }) {
        Clay_OnHover(HandleHeaderButtonInteraction, (intptr_t)page);
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 24,
            .textColor = isSelected ? headerButtonStringClickColor: stringColor
        }));
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
        // Welcome banner
        CLAY(CLAY_ID("WelcomeBanner"), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                .padding = { 24, 24, 20, 20 },
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = dashAccentPurple,
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
                    u32 tournaments_count = entity_list_count(&data.tournaments);
                    String8 tournaments_count_str8 = str8_u32(data.frameArena, tournaments_count);
                    Clay_String tournaments_count_clay = str8_to_clay(tournaments_count_str8);
                    CLAY_TEXT(tournaments_count_clay, CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_PRESS_START_2P,
                        .fontSize = 48,
                        .textColor = dashAccentCoral
                    }));
                }
            }

            // Players card with teal accent
            CLAY(CLAY_ID("PlayersCardOuter"), {
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
                    u32 players_count = entity_list_count(&data.players);
                    String8 players_count_str8 = str8_u32(data.frameArena, players_count);
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

static void
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
            Clay_OnHover(HandleGoBackInteraction, 0);
            CLAY_TEXT(CLAY_STRING("< Go back"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 14,
                .textColor = COLOR_WHITE
            }));
        }
    }
}

static void
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
        CLAY_TEXT(str8_to_clay((data.tournaments.entities + tournament_idx)->name), CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_PRESS_START_2P,
            .fontSize = 28,
            .textColor = COLOR_WHITE
        }));
    }
}

static void
RenderRegistrationPanel(u32 tournament_idx, Entity *tournament,
    s32 *registered_positions, u32 registered_count)
{
    // Tournament format toggle button
    Clay_String format_text = tournament->format == FORMAT_SINGLE_ELIMINATION
        ? CLAY_STRING("Format: Single Elim")
        : CLAY_STRING("Format: Groups");

    CLAY(CLAY_ID("FormatToggleButton"), {
        .layout = {
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
            .padding = { 10, 10, 8, 8 },
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
        },
        .backgroundColor = Clay_Hovered() ? dashAccentPurple : dashAccentTeal,
        .cornerRadius = CLAY_CORNER_RADIUS(8)
    }) {
        Clay_OnHover(HandleToggleTournamentFormat, 0);
        CLAY_TEXT(format_text, CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 14,
            .textColor = COLOR_WHITE
        }));
    }

    // Group size selector (only for groups format)
    if (tournament->format == FORMAT_GROUPS_THEN_BRACKET)
    {
        CLAY(CLAY_ID("GroupSizeSelector"), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                .padding = { 8, 8, 6, 6 },
                .childGap = 8,
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = dashCardBg,
            .cornerRadius = CLAY_CORNER_RADIUS(8),
            .border = { .width = {1, 1, 1, 1}, .color = textInputBorderColor }
        }) {
            // Decrement button
            CLAY(CLAY_ID("GroupSizeDecrement"), {
                .layout = {
                    .sizing = {.width = CLAY_SIZING_FIXED(28), .height = CLAY_SIZING_FIXED(28)},
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = Clay_Hovered() ? dashAccentPurple : dashAccentTeal,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                Clay_OnHover(HandleDecrementGroupSize, 0);
                CLAY_TEXT(CLAY_STRING("-"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 18,
                    .textColor = COLOR_WHITE
                }));
            }

            // Group size label and value
            CLAY(CLAY_ID("GroupSizeValue"), {
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                    .childGap = 4,
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                }
            }) {
                CLAY_TEXT(CLAY_STRING("Group size:"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 12,
                    .textColor = dashLabelText
                }));
                String8 size_str = str8_u32(data.frameArena, tournament->group_phase.group_size);
                CLAY_TEXT(str8_to_clay(size_str), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 14,
                    .textColor = stringColor
                }));
            }

            // Increment button
            CLAY(CLAY_ID("GroupSizeIncrement"), {
                .layout = {
                    .sizing = {.width = CLAY_SIZING_FIXED(28), .height = CLAY_SIZING_FIXED(28)},
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = Clay_Hovered() ? dashAccentPurple : dashAccentTeal,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                Clay_OnHover(HandleIncrementGroupSize, 0);
                CLAY_TEXT(CLAY_STRING("+"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 18,
                    .textColor = COLOR_WHITE
                }));
            }
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
            Clay_OnHover(HandleStartTournament, 0);
            CLAY_TEXT(CLAY_STRING("Start Tournament"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 14,
                .textColor = COLOR_WHITE
            }));
        }
    }

    // List of registered players (click to unregister)
    CLAY(CLAY_ID("RegisteredPlayersList"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
            .childGap = 4
        },
    }) {
        for (u32 i = 0; i < registered_count; i++)
        {
            u32 player_idx = BIT_TO_ENTITY_IDX(registered_positions[i]);
            Entity *player = data.players.entities + player_idx;

            CLAY(CLAY_IDI("RegisteredPlayer", player_idx), {
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                    .padding = { 8, 8, 6, 6 },
                    .childGap = 6
                },
                .backgroundColor = Clay_Hovered() ? playerRowHoverColor : playerRowColor,
                .cornerRadius = CLAY_CORNER_RADIUS(4),
                .border = { .width = {1, 1, 1, 1}, .color = textInputBorderColor }
            }) {
                Clay_OnHover(HandleTogglePlayerRegistration, (intptr_t)player_idx);
                CLAY_TEXT(CLAY_STRING("-"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 16,
                    .textColor = removeTextColor
                }));
                CLAY_TEXT(str8_to_clay_truncated(data.frameArena, player->name, MAX_DISPLAY_NAME_LEN), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 16,
                    .textColor = stringColor
                }));
            }
        }
    }

    // List of not registered players (click to register)
    CLAY(CLAY_ID("NotRegisteredPlayersList"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
            .childGap = 4
        },
    }) {
        u32 idx_tail = data.players.len + 1;
        u32 idx = (data.players.entities)->nxt;
        while (idx != idx_tail)
        {
            Entity *player = data.players.entities + idx;
            bool is_registered = (player->registrations >> ENTITY_IDX_TO_BIT(tournament_idx)) & 1;

            if (!is_registered)
            {
                CLAY(CLAY_IDI("NotRegisteredPlayer", idx), {
                    .layout = {
                        .layoutDirection = CLAY_LEFT_TO_RIGHT,
                        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                        .padding = { 8, 8, 6, 6 },
                        .childGap = 6
                    },
                    .backgroundColor = Clay_Hovered() ? playerRowHoverColor : playerRowColor,
                    .cornerRadius = CLAY_CORNER_RADIUS(4),
                    .border = { .width = {1, 1, 1, 1}, .color = textInputBorderColor }
                }) {
                    Clay_OnHover(HandleTogglePlayerRegistration, (intptr_t)idx);
                    CLAY_TEXT(CLAY_STRING("+"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 16,
                        .textColor = addButtonColor
                    }));
                    CLAY_TEXT(str8_to_clay_truncated(data.frameArena, player->name, MAX_DISPLAY_NAME_LEN), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 16,
                        .textColor = stringColor
                    }));
                }
            }

            idx = player->nxt;
        }
    }
}

// Render in-progress phase left panel content
static void
RenderInProgressPanel(s32 *registered_positions, u32 registered_count)
{
    // Show tournament status
    CLAY_TEXT(CLAY_STRING("Tournament In Progress"), CLAY_TEXT_CONFIG({
        .fontId = FONT_ID_BODY_16,
        .fontSize = 16,
        .textColor = dashAccentTeal
    }));

    // Return to Registration button (dialog is rendered as modal overlay elsewhere)
    CLAY(CLAY_ID("ReturnToRegistrationButton"), {
        .layout = {
            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
            .padding = { 10, 10, 8, 8 },
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
        },
        .backgroundColor = Clay_Hovered() ? dashAccentCoral : dashAccentPurple,
        .cornerRadius = CLAY_CORNER_RADIUS(8)
    }) {
        Clay_OnHover(HandleReturnToRegistration, 0);
        CLAY_TEXT(CLAY_STRING("Return to Registration"), CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 14,
            .textColor = COLOR_WHITE
        }));
    }

    CLAY_TEXT(CLAY_STRING("Left click a player name to advance them to the next round."), CLAY_TEXT_CONFIG({
        .fontId = FONT_ID_BODY_16,
        .fontSize = 14,
        .textColor = matchVsColor
    }));

    CLAY_TEXT(CLAY_STRING("Right click a player name to undo his advancement"), CLAY_TEXT_CONFIG({
        .fontId = FONT_ID_BODY_16,
        .fontSize = 14,
        .textColor = matchVsColor
    }));

    // Show list of players (read-only)
    CLAY_TEXT(CLAY_STRING("Players:"), CLAY_TEXT_CONFIG({
        .fontId = FONT_ID_BODY_16,
        .fontSize = 16,
        .textColor = matchVsColor
    }));

    for (u32 i = 0; i < registered_count; i++)
    {
        u32 player_idx = BIT_TO_ENTITY_IDX(registered_positions[i]);
        Entity *player = data.players.entities + player_idx;

        CLAY_TEXT(str8_to_clay_truncated(data.frameArena, player->name, MAX_DISPLAY_NAME_LEN), CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 14,
            .textColor = stringColor
        }));
    }
}

/////////////////////////////////////////////////
// Single elimination tournament

void
RenderMatchSlot(Clay_String player1_name, Clay_String player2_name,
    u8 player1_idx, u8 player2_idx,
    u32 bracket_pos1, u32 bracket_pos2,
    u32 match_id, float zoom)
{
    bool player1_is_tbd = (player1_idx == 0);
    bool player2_is_tbd = (player2_idx == 0);
    Clay_Color name1Color = player1_is_tbd ? matchVsColor : stringColor;
    Clay_Color name2Color = player2_is_tbd ? matchVsColor : stringColor;

    // Encode data for click handlers: (bracket_pos << 8) | player_idx
    intptr_t player1_data = (intptr_t)((bracket_pos1 << 8) | player1_idx);
    intptr_t player2_data = (intptr_t)((bracket_pos2 << 8) | player2_idx);

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
                    Clay_OnHover(HandleAdvanceWinner, player1_data);
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
                    Clay_OnHover(HandleAdvanceWinner, player2_data);
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

// Render single elimination bracket
static void
RenderSingleEliminationBracket(Entity *tournament, u32 num_players)
{
    // Only reconstruct bracket during registration phase
    if (tournament->state == TOURNAMENT_REGISTRATION)
    {
        tournament_construct_bracket(&data.tournaments, tournament->name);
    }

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

                    u8 player1_idx = tournament->bracket[pos1];
                    u8 player2_idx = tournament->bracket[pos2];

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
                                Entity *player1 = data.players.entities + player1_idx;
                                name1 = str8_to_clay_truncated(data.frameArena, player1->name, MAX_DISPLAY_NAME_LEN);
                            }
                            if (player2_idx != 0)
                            {
                                Entity *player2 = data.players.entities + player2_idx;
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

/////////////////////////////////////////////////
// Tournament with group phase

void
RenderGroupMatrix(Entity *tournament, u32 group_idx, u32 players_in_group)
{
    // Use accent colors that cycle per group to match header
    Clay_Color groupAccentColors[] = {
        dashAccentTeal,
        dashAccentCoral,
        dashAccentPurple,
        dashAccentOrange,
        dashAccentGold
    };
    u32 numAccentColors = sizeof(groupAccentColors) / sizeof(groupAccentColors[0]);
    Clay_Color groupAccent = groupAccentColors[group_idx % numAccentColors];

    CLAY(CLAY_IDI("GroupMatrix", group_idx), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0) }
        },
        .cornerRadius = CLAY_CORNER_RADIUS(8),
        .border = { .width = {1, 1, 1, 1}, .color = textInputBorderColor }
    }) {
        // Header row with player names as columns
        CLAY(CLAY_IDI("MatrixHeaderRow", group_idx), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) }
            },
            .backgroundColor = groupAccent,
            .cornerRadius = { 8, 8, 0, 0 }
        }) {
            // Empty corner cell
            CLAY(CLAY_IDI("MatrixCorner", group_idx), {
                .layout = {
                    .sizing = { .width = CLAY_SIZING_FIXED(100), .height = CLAY_SIZING_FIT(0) },
                    .padding = { 8, 8, 10, 10 },
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                }
            }) {
                CLAY_TEXT(CLAY_STRING("vs"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 12,
                    .textColor = COLOR_WHITE
                }));
            }

            // Column headers (player names)
            for (u32 col = 0; col < players_in_group; col++)
            {
                u8 player_idx = tournament->group_phase.groups[group_idx][col];
                if (player_idx != 0)
                {
                    Entity *player = data.players.entities + player_idx;
                    u32 header_id = group_idx * MAX_GROUP_SIZE * 2 + col;
                    CLAY(CLAY_IDI("MatrixColHeader", header_id), {
                        .layout = {
                            .sizing = { .width = CLAY_SIZING_FIXED(100), .height = CLAY_SIZING_FIT(0) },
                            .padding = { 8, 8, 10, 10 },
                            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                        },
                        .border = {.width = {1, 0, 0, 0}, .color = { 255, 255, 255, 80 } }
                    }) {
                        CLAY_TEXT(str8_to_clay_truncated(data.frameArena, player->name, MAX_DISPLAY_NAME_LEN), CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_BODY_16,
                            .fontSize = 14,
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
                Entity *row_player = data.players.entities + row_player_idx;
                u32 row_id = group_idx * MAX_GROUP_SIZE + row;
                bool isLastRow = (row == players_in_group - 1);

                CLAY(CLAY_IDI("MatrixRow", row_id), {
                    .layout = {
                        .layoutDirection = CLAY_LEFT_TO_RIGHT,
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) }
                    },
                    .backgroundColor = (row % 2 == 0) ? dashCardBg : dashBgGradientTop,
                    .cornerRadius = isLastRow ? (Clay_CornerRadius){ 0, 0, 8, 8 } : (Clay_CornerRadius){ 0, 0, 0, 0 }
                }) {
                    // Row header (player name) with accent background
                    CLAY(CLAY_IDI("MatrixRowHeader", row_id), {
                        .layout = {
                            .sizing = { .width = CLAY_SIZING_FIXED(100), .height = CLAY_SIZING_FIT(0) },
                            .padding = { 10, 10, 10, 10 },
                            .childAlignment = { .x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER }
                        },
                        .backgroundColor = groupAccent,
                        .cornerRadius = isLastRow ? (Clay_CornerRadius){ 0, 0, 0, 8 } : (Clay_CornerRadius){ 0, 0, 0, 0 }
                    }) {
                        CLAY_TEXT(str8_to_clay_truncated(data.frameArena, row_player->name, MAX_DISPLAY_NAME_LEN), CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_BODY_16,
                            .fontSize = 14,
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
                            Clay_Color cell_bg = isDiagonal ? textInputBorderColor : (row % 2 == 0) ? dashCardBg : dashBgGradientTop;
                            bool isLastCol = (col == players_in_group - 1);

                            CLAY(CLAY_IDI("MatrixCell", cell_id), {
                                .layout = {
                                    .sizing = { .width = CLAY_SIZING_FIXED(100), .height = CLAY_SIZING_FIT(0) },
                                    .padding = { 8, 8, 10, 10 },
                                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                                },
                                .border = { .width = {1, 0, 0, 0}, .color = textInputBorderColor },
                                .backgroundColor = cell_bg,
                                .cornerRadius = (isLastRow && isLastCol) ? (Clay_CornerRadius){ 0, 0, 8, 0 } : (Clay_CornerRadius){ 0, 0, 0, 0 }
                            }) {
                                // Diagonal cells (player vs self) show dash
                                if (isDiagonal)
                                {
                                    CLAY_TEXT(CLAY_STRING("-"), CLAY_TEXT_CONFIG({
                                        .fontId = FONT_ID_BODY_16,
                                        .fontSize = 14,
                                        .textColor = matchVsColor
                                    }));
                                }
                                else
                                {
                                    // Other cells will show match results (empty for now)
                                    CLAY_TEXT(CLAY_STRING("TBD"), CLAY_TEXT_CONFIG({
                                        .fontId = FONT_ID_BODY_16,
                                        .fontSize = 14,
                                        .textColor = dashAccentPurple
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

static void
RenderGroupsChart(Entity *tournament)
{
    // Only reconstruct groups during registration phase
    if (tournament->state == TOURNAMENT_REGISTRATION)
    {
        tournament_construct_groups(tournament);
    }

    // Get group info from the constructed group phase
    u32 num_groups = tournament->group_phase.num_groups;

    // Cycle through accent colors for each group
    Clay_Color groupAccentColors[] = {
        dashAccentTeal,
        dashAccentCoral,
        dashAccentPurple,
        dashAccentOrange,
        dashAccentGold
    };
    u32 numAccentColors = sizeof(groupAccentColors) / sizeof(groupAccentColors[0]);

    // Calculate groups per row based on number of groups
    // 1-2 groups: 1 row, 3-4 groups: 2 per row, 5+ groups: 3 per row
    u32 groups_per_row = (num_groups <= 2) ? num_groups : (num_groups <= 4) ? 2 : 3;
    u32 num_rows = (num_groups + groups_per_row - 1) / groups_per_row;

    // Render groups in a grid layout (rows of groups)
    CLAY(CLAY_ID("GroupsContainer"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0) },
            .childGap = 24,
            .padding = { 16, 16, 16, 16 }
        }
    }) {
        for (u32 row = 0; row < num_rows; row++)
        {
            // Each row container
            CLAY(CLAY_IDI("GroupRow", row), {
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0) },
                    .childGap = 24,
                    .childAlignment = { .y = CLAY_ALIGN_Y_TOP }
                }
            }) {
                // Groups in this row
                u32 start_g = row * groups_per_row;
                u32 end_g = start_g + groups_per_row;
                if (end_g > num_groups) end_g = num_groups;

                for (u32 g = start_g; g < end_g; g++)
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

                    // Outer container with accent bar (matching app style)
                    CLAY(CLAY_IDI("GroupOuter", g), {
                        .layout = {
                            .layoutDirection = CLAY_TOP_TO_BOTTOM,
                            .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0) }
                        },
                        .cornerRadius = CLAY_CORNER_RADIUS(12)
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
                                .childGap = 12
                            },
                            .backgroundColor = dashCardBg,
                            .cornerRadius = { 0, 0, 12, 12 }
                        }) {
                            // Group header with styled badge
                            CLAY(CLAY_IDI("GroupHeader", g), {
                                .layout = {
                                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) },
                                    .padding = { 12, 12, 8, 8 },
                                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                                },
                                .backgroundColor = groupAccent,
                                .cornerRadius = CLAY_CORNER_RADIUS(8)
                            }) {
                                String8 group_prefix = str8_lit("GROUP ");
                                String8 group_num = str8_u32(data.frameArena, g + 1);
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
    }
}

void
RenderTournamentLeftPanel(u32 tournament_idx)
{
    // Left panel outer - with accent bar
    CLAY(CLAY_ID("LeftPanelOuter"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = {.width = CLAY_SIZING_FIXED(220), .height = CLAY_SIZING_GROW(0)}
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
                .childGap = 12
            },
            .backgroundColor = dashCardBg,
            .cornerRadius = { 0, 0, 12, 12 },
            .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() }
        }) {
            Entity *panel_tournament = data.tournaments.entities + tournament_idx;
            s32 registered_positions[64] = {0};
            u32 registered_count = find_all_filled_slots(panel_tournament->registrations, registered_positions);

            if (panel_tournament->state == TOURNAMENT_REGISTRATION)
            {
                RenderRegistrationPanel(tournament_idx, panel_tournament, registered_positions, registered_count);
            }
            else if (panel_tournament->state == TOURNAMENT_IN_PROGRESS)
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
                .sizing = layoutExpand,
                .childAlignment = { .x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_TOP }
            },
            .backgroundColor = dashCardBg,
            .cornerRadius = { 0, 0, 12, 12 },
            .clip = { .horizontal = true, .vertical = true, .childOffset = Clay_GetScrollOffset() }
        }) {
            Clay_OnHover(HandleChartHover, 0);
            Entity *tournament = data.tournaments.entities + tournament_idx;

            // Get the number of players in the tournament
            s32 _positions[64] = {0};
            u32 num_players = find_all_filled_slots(tournament->registrations, _positions);

            if (tournament->format == FORMAT_SINGLE_ELIMINATION)
            {
                RenderSingleEliminationBracket(tournament, num_players);

                // Custom element for drawing bezier curve connections
                // This is rendered as part of Clay's render commands, in the correct z-order
                static CustomLayoutElement bracketConnectionsElement;
                bracketConnectionsElement.type = CUSTOM_LAYOUT_ELEMENT_TYPE_BRACKET_CONNECTIONS;
                bracketConnectionsElement.customData.bracketConnections.num_players = num_players;
                bracketConnectionsElement.customData.bracketConnections.zoom = data.chartZoomLevel;
                bracketConnectionsElement.customData.bracketConnections.yOffset = data.yOffset;

                CLAY(CLAY_ID("BracketConnections"), {
                    .layout = { .sizing = { .width = CLAY_SIZING_FIXED(0), .height = CLAY_SIZING_FIXED(0) } },
                    .custom = { .customData = &bracketConnectionsElement }
                }) {}
            }
            else // FORMAT_GROUPS_THEN_BRACKET
            {
                RenderGroupsChart(tournament);
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
            Clay_OnHover(HandleEditTournament, (intptr_t)tournament_idx);

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
            Clay_OnHover(HandleStartRenameEvent, (intptr_t)tournament_idx);

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
            Clay_OnHover(HandleDeleteTournament, (intptr_t)tournament_idx);

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

            // Add button
            CLAY(CLAY_ID("AddEventButton"), {
                .layout = {
                    .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIXED(44)},
                    .padding = { 24, 24, 0, 0 },
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = Clay_Hovered() ? dashAccentTeal : dashAccentCoral,
                .cornerRadius = CLAY_CORNER_RADIUS(10)
            }) {
                Clay_OnHover(HandleAddEventButtonInteraction, 0);
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

            // Event rows
            u32 idx_tail = data.tournaments.len + 1;
            u32 idx = (data.tournaments.entities)->nxt;
            while (idx != idx_tail)
            {
                Entity *tournament = data.tournaments.entities + idx;

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
                        CLAY_TEXT(str8_to_clay(str8_u32(data.frameArena, count)), CLAY_TEXT_CONFIG({
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
            .clip = { .horizontal = true, .childOffset = Clay_GetScrollOffset() }
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
    CLAY(CLAY_IDI("PlayerActions", player_idx), {
        .layout = {
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .sizing = { .width = CLAY_SIZING_FIXED(220) },
            .childGap = 8
        }
    }) {
        // Open button (not implemented yet)
        CLAY_AUTO_ID({
            .layout = {
                .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                .padding = { 12, 12, 6, 6 },
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = Clay_Hovered() ? dashAccentPurple : dashAccentTeal,
            .cornerRadius = CLAY_CORNER_RADIUS(8)
        }) {
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
            Clay_OnHover(HandleStartRenamePlayer, (intptr_t)player_idx);

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
            Clay_OnHover(HandleDeletePlayer, (intptr_t)player_idx);

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

            // Add button
            CLAY(CLAY_ID("AddPlayerButton"), {
                .layout = {
                    .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIXED(44)},
                    .padding = { 24, 24, 0, 0 },
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = Clay_Hovered() ? dashAccentTeal : dashAccentCoral,
                .cornerRadius = CLAY_CORNER_RADIUS(10)
            }) {
                Clay_OnHover(HandleAddPlayerButtonInteraction, 0);
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
            .cornerRadius = { 0, 0, 12, 12 }
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

            // Player rows
            u32 idx_tail = data.players.len + 1;
            u32 idx = (data.players.entities)->nxt;
            while (idx != idx_tail)
            {
                Entity *player = data.players.entities + idx;

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
                        CLAY_TEXT(str8_to_clay(str8_u32(data.frameArena, count)), CLAY_TEXT_CONFIG({
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

void
RenderPlayers(void)
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
        .clip = { .horizontal = true, .vertical = true, .childOffset = Clay_GetScrollOffset() }
    }) {
        RenderPlayersBanner();
        RenderPlayersHeader();
        RenderPlayersList();
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
// Modal Overlay (pure Clay implementation)

static Clay_Color modalOverlayColor = { 0, 0, 0, 150 };

void
RenderConfirmationModal(void)
{
    bool showReturnModal = data.showReturnToRegistrationConfirm;
    bool showDeleteTournamentModal = data.showDeleteTournamentConfirm;
    bool showDeletePlayerModal = data.showDeletePlayerConfirm;

    if (!showReturnModal && !showDeleteTournamentModal && !showDeletePlayerModal) return;

    // Determine modal content based on which type is being shown
    Clay_String titleText;
    Clay_String messageText;
    Clay_String confirmText;

    if (showDeleteTournamentModal) {
        titleText = CLAY_STRING("Delete Tournament?");
        messageText = CLAY_STRING("This tournament will be permanently deleted.");
        confirmText = CLAY_STRING("Yes, delete");
    } else if (showDeletePlayerModal) {
        titleText = CLAY_STRING("Delete Player?");
        messageText = CLAY_STRING("This player will be permanently deleted.");
        confirmText = CLAY_STRING("Yes, delete");
    } else {
        titleText = CLAY_STRING("Are you sure?");
        messageText = CLAY_STRING("All tournament progress will be lost.");
        confirmText = CLAY_STRING("Yes, reset");
    }

    // Get entity name for delete modals
    Clay_String entityName = CLAY_STRING("");
    if (showDeleteTournamentModal && data.deleteTournamentIdx != 0)
    {
        Entity *tournament = data.tournaments.entities + data.deleteTournamentIdx;
        entityName = str8_to_clay(tournament->name);
    }
    else if (showDeletePlayerModal && data.deletePlayerIdx != 0)
    {
        Entity *player = data.players.entities + data.deletePlayerIdx;
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
            if ((showDeleteTournamentModal || showDeletePlayerModal) && entityName.length > 0)
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
                    if (showDeleteTournamentModal) {
                        Clay_OnHover(HandleConfirmDeleteTournament, 0);
                    } else if (showDeletePlayerModal) {
                        Clay_OnHover(HandleConfirmDeletePlayer, 0);
                    } else {
                        Clay_OnHover(HandleConfirmReturnToRegistration, 0);
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
                    if (showDeleteTournamentModal) {
                        Clay_OnHover(HandleCancelDeleteTournament, 0);
                    } else if (showDeletePlayerModal) {
                        Clay_OnHover(HandleCancelDeletePlayer, 0);
                    } else {
                        Clay_OnHover(HandleCancelReturnToRegistration, 0);
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
    bool renamingEvent = data.renamingEventIdx != 0;
    bool renamingPlayer = data.renamingPlayerIdx != 0;

    if (!renamingEvent && !renamingPlayer) return;

    Clay_String currentName;
    Clay_String titleText;
    TextBoxEnum textbox;

    if (renamingEvent) {
        Entity *tournament = data.tournaments.entities + data.renamingEventIdx;
        currentName = str8_to_clay(tournament->name);
        titleText = CLAY_STRING("Rename Event");
        textbox = TEXTBOX_EventRename;
    } else {
        Entity *player = data.players.entities + data.renamingPlayerIdx;
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
            if (renamingEvent) {
                TextInput_Render(TEXTBOX_EventRename, CLAY_STRING("EventRenameInput"),
                    CLAY_STRING("EventRenameInputScroll"), CLAY_STRING("Enter new name..."));
            } else {
                TextInput_Render(TEXTBOX_PlayerRename, CLAY_STRING("PlayerRenameInput"),
                    CLAY_STRING("PlayerRenameInputScroll"), CLAY_STRING("Enter new name..."));
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
                    if (renamingEvent) {
                        Clay_OnHover(HandleConfirmRenameEvent, 0);
                    } else {
                        Clay_OnHover(HandleConfirmRenamePlayer, 0);
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
                    if (renamingEvent) {
                        Clay_OnHover(HandleCancelRenameEvent, 0);
                    } else {
                        Clay_OnHover(HandleCancelRenamePlayer, 0);
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
        Clay_OnHover(HandleOuterContainerInteraction, 0);

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
            .cornerRadius = CLAY_CORNER_RADIUS(8),
            .clip = { .horizontal = true, .childOffset = Clay_GetScrollOffset() }
        }) {
            RenderHeaderButton(CLAY_STRING("Dashboard"), PAGE_Dashboard);
            RenderHeaderButton(CLAY_STRING("Events"), PAGE_Events);
            RenderHeaderButton(CLAY_STRING("Players"), PAGE_Players);
            RenderHeaderButton(CLAY_STRING("Results"), PAGE_Results);
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
        RenderConfirmationModal();
        RenderRenameModal();
    }

    Clay_RenderCommandArray renderCommands = Clay_EndLayout();

    for (int32_t i = 0; i < renderCommands.length; i++) {
        Clay_RenderCommandArray_Get(&renderCommands, i)->boundingBox.y += data.yOffset;
    }
    return renderCommands;
}
