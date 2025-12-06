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

// Fun dashboard colors
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
    // SetMouseCursor(data.mouseCursor);
    Page page = (Page)userData;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        data.selectedHeaderButton = page;
    }
}

void
HandleEventRowInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    data.mouseCursor = MOUSE_CURSOR_POINTING_HAND;
    u8 event_idx = (u8)userData;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        data.selectedTournamentIdx = event_idx;
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
HandleOuterContainerInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        data.focusedTextbox = TEXTBOX_NULL;
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

        float inputPaddingLeft = 12.0f;
        float clickRelativeX = mousePos.x - inputBox.x - inputPaddingLeft - scrollOffsetX;

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
        Entity *tournament = data.tournaments.entities + data.selectedTournamentIdx;
        tournament->state = TOURNAMENT_REGISTRATION;
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

///////////////////////////////////////////////////////////////////////////////
// Texbox functions

void
TextInput_ProcessKeyboard(TextInput *input)
{
    // Update blink timer
    input->blinkTimer += GetFrameTime();
    if (input->blinkTimer > 1.0f) {
        input->blinkTimer -= 1.0f;
    }

    // Handle character input - insert at cursor position
    int key = GetCharPressed();
    while (key > 0)
    {
        if (key >= 32 && key <= 125 && input->len < TEXT_INPUT_MAX_LEN - 1)
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
    Clay_Color inputBorderColor = isFocused ? COLOR_BLUE : textInputBorderColor;

    // Outer container: styling (background, border, corner radius)
    CLAY(Clay_GetElementId(elementId), {
        .layout = {
            .sizing = {.width = CLAY_SIZING_FIXED(300), .height = CLAY_SIZING_FIXED(40)},
            .padding = { 12, 12, 0, 0 },
        },
        .backgroundColor = textInputBackgroundColor,
        .cornerRadius = CLAY_CORNER_RADIUS(4),
        .border = { .width = {1, 1, 1, 1}, .color = inputBorderColor }
    }) {
        Clay_OnHover(HandleTextInput, textBoxEnum);

        TextInput *input = &data.textInputs[textBoxEnum];

        // Inner container with scroll
        CLAY(Clay_GetElementId(scrollId), {
            .layout = {
                .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
                .childAlignment = { .x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER }
            },
            .clip = { .horizontal = true, .childOffset = Clay_GetScrollOffset() }
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

    float inputPaddingLeft = 12.0f;
    float scrollOffsetX = (scrollData.found && scrollData.scrollPosition) ? scrollData.scrollPosition->x : 0.0f;
    float cursorX = inputBox.x + inputPaddingLeft + cursorOffsetX + scrollOffsetX;
    float cursorY = inputBox.y + (inputBox.height - 18.0f) / 2.0f;

    // Clip cursor to textbox bounds
    float clipLeft = inputBox.x + inputPaddingLeft;
    float clipRight = inputBox.x + inputBox.width - inputPaddingLeft;

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

///////////////////////////////////////////////////////////////////////////////
// Function to render Events section

void
RenderEventsHeaderElement(Clay_String string)
{
    CLAY_AUTO_ID({
        .layout = {
            .sizing = { .width = CLAY_SIZING_GROW(0) },
            .padding = {.top = 8, .bottom = 8}
        }
    }) {
        CLAY_TEXT(string, CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 18,
            .textColor = stringColor
        }));
    }
}

void
RenderEventElement(Clay_String element)
{
    CLAY_AUTO_ID({
        .layout = {
            .sizing = {.height = CLAY_SIZING_FIT(0) , .width = CLAY_SIZING_GROW(0)},
            .padding = { .top = 8, .bottom = 8 }
        }
    }) {
        CLAY_TEXT(element, CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 18,
            .textColor = stringColor
        }));
    }
}

void
RenderEventsActionsButtons(void)
{
    CLAY_AUTO_ID({
        .layout = {
            .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
            .padding = { 12, 12, 6, 6 },
            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
        },
        .backgroundColor = Clay_Hovered() ? removeButtonHoverColor : removeButtonColor,
        .cornerRadius = CLAY_CORNER_RADIUS(8)
    }) {
        CLAY_TEXT(CLAY_STRING("Delete"), CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 14,
            .textColor = removeTextColor
        }));
    }
}

void
RenderMatchSlot(Clay_String player1_name, Clay_String player2_name,
    u8 player1_idx, u8 player2_idx,
    u32 bracket_pos1, u32 bracket_pos2, u32 match_id)
{
    bool player1_is_tbd = (player1_idx == 0);
    bool player2_is_tbd = (player2_idx == 0);
    Clay_Color name1Color = player1_is_tbd ? matchVsColor : stringColor;
    Clay_Color name2Color = player2_is_tbd ? matchVsColor : stringColor;

    // Encode data for click handlers: (bracket_pos << 8) | player_idx
    intptr_t player1_data = (intptr_t)((bracket_pos1 << 8) | player1_idx);
    intptr_t player2_data = (intptr_t)((bracket_pos2 << 8) | player2_idx);

    // Outer border container
    CLAY(CLAY_IDI("MatchBorder", match_id), {
        .layout = {
            .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0) },
            .padding = { 2, 2, 2, 2 }
        },
        .backgroundColor = matchBorderColor,
        .cornerRadius = CLAY_CORNER_RADIUS(8)
    }) {
        CLAY(CLAY_IDI("Match", match_id), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = { .width = CLAY_SIZING_FIXED(160), .height = CLAY_SIZING_FIT(0) },
                .padding = { 12, 12, 10, 10 },
                .childGap = 2,
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
            },
            .backgroundColor = COLOR_WHITE,
            .cornerRadius = CLAY_CORNER_RADIUS(6)
        }) {
            // Player 1 - clickable to advance
            CLAY(CLAY_IDI("Player1Slot", match_id), {
                .layout = {
                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) },
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                },
                .backgroundColor = Clay_Hovered() && !player1_is_tbd ? playerRowHoverColor : COLOR_WHITE,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                if (!player1_is_tbd) {
                    Clay_OnHover(HandleAdvanceWinner, player1_data);
                }
                CLAY_TEXT(player1_name, CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = SLOT_PLAYER_FONT_SIZE,
                    .textColor = name1Color
                }));
            }

            // VS separator with lines
            CLAY_AUTO_ID({
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) },
                    .padding = { 0, 0, 4, 4 },
                    .childGap = 8,
                    .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                }
            }) {
                // Left line
                CLAY_AUTO_ID({
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(1) }
                    },
                    .backgroundColor = matchBorderColor
                }) {}
                // VS text
                CLAY_TEXT(CLAY_STRING("vs"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 12,
                    .textColor = matchVsColor
                }));
                // Right line
                CLAY_AUTO_ID({
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(1) }
                    },
                    .backgroundColor = matchBorderColor
                }) {}
            }

            // Player 2 - clickable to advance
            CLAY(CLAY_IDI("Player2Slot", match_id), {
                .layout = {
                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) },
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                },
                .backgroundColor = Clay_Hovered() && !player2_is_tbd ? playerRowHoverColor : COLOR_WHITE,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                if (!player2_is_tbd) {
                    Clay_OnHover(HandleAdvanceWinner, player2_data);
                }
                CLAY_TEXT(player2_name, CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = SLOT_PLAYER_FONT_SIZE,
                    .textColor = name2Color
                }));
            }
        }
    }
}

void
RenderByeSlot(u32 match_id)
{
    // Render a bye slot with the same dimensions as a regular match slot
    CLAY(CLAY_IDI("MatchBorder", match_id), {
        .layout = {
            .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0) },
            .padding = { 2, 2, 2, 2 }
        },
        .backgroundColor = matchBorderColor,
        .cornerRadius = CLAY_CORNER_RADIUS(8)
    }) {
        CLAY(CLAY_IDI("Match", match_id), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = { .width = CLAY_SIZING_FIXED(160), .height = CLAY_SIZING_FIT(0) },
                .padding = { 12, 12, 10, 10 },
                .childGap = 2,
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
            },
            .backgroundColor = COLOR_WHITE,
            .cornerRadius = CLAY_CORNER_RADIUS(6)
        }) {
            // Empty slot 1 - fixed height container to match player name height
            CLAY_AUTO_ID({
                .layout = {
                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(SLOT_PLAYER_FONT_SIZE) }
                }
            }) {}

            // VS separator with lines (same as RenderMatchSlot)
            CLAY_AUTO_ID({
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) },
                    .padding = { 0, 0, 4, 4 },
                    .childGap = 8,
                    .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                }
            }) {
                // Left line
                CLAY_AUTO_ID({
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(1) }
                    },
                    .backgroundColor = matchBorderColor
                }) {}
                // BYE text instead of VS
                CLAY_TEXT(CLAY_STRING("BYE"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 12,
                    .textColor = matchVsColor
                }));
                // Right line
                CLAY_AUTO_ID({
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(1) }
                    },
                    .backgroundColor = matchBorderColor
                }) {}
            }

            // Empty slot 2 - fixed height container to match player name height
            CLAY_AUTO_ID({
                .layout = {
                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(SLOT_PLAYER_FONT_SIZE) }
                }
            }) {}
        }
    }
}

void
RenderGroupMatrix(Entity *tournament, u32 group_idx, u32 players_in_group)
{
    CLAY(CLAY_IDI("GroupMatrix", group_idx), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0) }
        }
    }) {
        // Header row with player names as columns
        CLAY(CLAY_IDI("MatrixHeaderRow", group_idx), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) }
            },
            .border = { .width = {1, 1, 1, 1}, .color = matrixBorderColor }
        }) {
            // Empty corner cell
            CLAY(CLAY_IDI("MatrixCorner", group_idx), {
                .layout = {
                    .sizing = { .width = CLAY_SIZING_FIXED(80), .height = CLAY_SIZING_FIT(0) },
                    .padding = { 4, 4, 4, 4 },
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = COLOR_OFF_WHITE,
            }) {}

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
                            .sizing = { .width = CLAY_SIZING_FIXED(80), .height = CLAY_SIZING_FIT(0) },
                            .padding = { 4, 4, 4, 4 },
                            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                        },
                        .backgroundColor = COLOR_OFF_WHITE,
                        .border = {.width = {1, 0, 0, 0}, .color = matrixBorderColor }
                    }) {
                        CLAY_TEXT(str8_to_clay_truncated(data.frameArena, player->name, MAX_DISPLAY_NAME_LEN), CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_BODY_16,
                            .fontSize = 14,
                            .textColor = stringColor
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

                CLAY(CLAY_IDI("MatrixRow", row_id), {
                    .layout = {
                        .layoutDirection = CLAY_LEFT_TO_RIGHT,
                        .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) }
                    },
                    .border = { .width = {1, 1, 0, 1}, .color = matrixBorderColor }
                }) {
                    // Row header (player name)
                    CLAY(CLAY_IDI("MatrixRowHeader", row_id), {
                        .layout = {
                            .sizing = { .width = CLAY_SIZING_FIXED(80), .height = CLAY_SIZING_FIT(0) },
                            .padding = { 8, 8, 4, 4 },
                            .childAlignment = { .x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER }
                        },
                        .backgroundColor = COLOR_OFF_WHITE,
                    }) {
                        CLAY_TEXT(str8_to_clay_truncated(data.frameArena, row_player->name, MAX_DISPLAY_NAME_LEN), CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_BODY_16,
                            .fontSize = 14,
                            .textColor = stringColor
                        }));
                    }

                    // Result cells
                    for (u32 col = 0; col < players_in_group; col++)
                    {
                        u8 col_player_idx = tournament->group_phase.groups[group_idx][col];
                        if (col_player_idx != 0)
                        {
                            u32 cell_id = group_idx * MAX_GROUP_SIZE * MAX_GROUP_SIZE + row * MAX_GROUP_SIZE + col;
                            Clay_Color cell_bg = (row == col) ? matchBorderColor : playerRowColor;

                            CLAY(CLAY_IDI("MatrixCell", cell_id), {
                                .layout = {
                                    .sizing = { .width = CLAY_SIZING_FIXED(80), .height = CLAY_SIZING_FIT(0) },
                                    .padding = { 4, 4, 4, 4 },
                                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                                },
                                .border = { .width = {1, 0, 0, 0}, .color = matrixBorderColor },
                                .backgroundColor = cell_bg,
                            }) {
                                // Diagonal cells (player vs self) show dash
                                if (row == col)
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
                                        .textColor = matchVsColor
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

void
RenderTournamentChart(u32 tournament_idx)
{
    CLAY(CLAY_ID("Tournament"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = layoutExpand,
            .childGap = 16,
            .childAlignment = {
                .y = CLAY_ALIGN_Y_TOP
            },
        },
        .backgroundColor = COLOR_OFF_WHITE
    }) {
        // Header with go back and tournament name
        CLAY(CLAY_ID("TournamentHeader"), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                .padding = { 12, 12, 8, 8 },
                .childGap = 16,
                .childAlignment = { .x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = COLOR_WHITE,
            .cornerRadius = CLAY_CORNER_RADIUS(6)
        }) {
            CLAY(CLAY_ID("GoBack"), {
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)},
                    .padding = { 10, 10, 6, 6 },
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = Clay_Hovered() ? goBackButtonHoverColor : goBackButtonColor,
                .cornerRadius = CLAY_CORNER_RADIUS(4),
                .border = { .width = {1, 1, 1, 1}, .color = goBackTextColor }
            }) {
                Clay_OnHover(HandleGoBackInteraction, 0);
                CLAY_TEXT(CLAY_STRING("< Go back"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 14,
                    .textColor = goBackTextColor
                }));
            }
            CLAY_TEXT(str8_to_clay((data.tournaments.entities + tournament_idx)->name), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 24,
                .textColor = tournamentTitleColor
            }));
        }

        // Two-column layout: left panel + chart
        CLAY(CLAY_ID("TournamentContent"), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = layoutExpand,
                .childGap = 16
            }
        }) {
            // Left panel - Player management
            CLAY(CLAY_ID("LeftPanel"), {
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = {.width = CLAY_SIZING_FIXED(200), .height = CLAY_SIZING_GROW(0)},
                    .padding = { 12, 12, 12, 12 },
                    .childGap = 12
                },
                .backgroundColor = COLOR_WHITE,
                .cornerRadius = CLAY_CORNER_RADIUS(6)
            }) {
                Entity *panel_tournament = data.tournaments.entities + tournament_idx;
                s32 registered_positions[64] = {0};
                u32 registered_count = find_all_filled_slots(panel_tournament->registrations, registered_positions);

                if (panel_tournament->state == TOURNAMENT_REGISTRATION)
                {
                    // Tournament format toggle button
                    Clay_String format_text = panel_tournament->format == FORMAT_SINGLE_ELIMINATION
                        ? CLAY_STRING("Format: Single Elim")
                        : CLAY_STRING("Format: Groups");

                    CLAY(CLAY_ID("FormatToggleButton"), {
                        .layout = {
                            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                            .padding = { 10, 10, 8, 8 },
                            .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                        },
                        .backgroundColor = Clay_Hovered() ? goBackButtonHoverColor : goBackButtonColor,
                        .cornerRadius = CLAY_CORNER_RADIUS(4),
                        .border = { .width = {1, 1, 1, 1}, .color = goBackTextColor }
                    }) {
                        Clay_OnHover(HandleToggleTournamentFormat, 0);
                        CLAY_TEXT(format_text, CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_BODY_16,
                            .fontSize = 14,
                            .textColor = goBackTextColor
                        }));
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
                            .backgroundColor = Clay_Hovered() ? addButtonHoverColor : addButtonColor,
                            .cornerRadius = CLAY_CORNER_RADIUS(4)
                        }) {
                            Clay_OnHover(HandleStartTournament, 0);
                            CLAY_TEXT(CLAY_STRING("Start Tournament"), CLAY_TEXT_CONFIG({
                                .fontId = FONT_ID_BODY_16,
                                .fontSize = 14,
                                .textColor = addButtonTextColor
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
                        .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() }
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

                        if (registered_count == 0)
                        {
                            CLAY_TEXT(CLAY_STRING("No players registered"), CLAY_TEXT_CONFIG({
                                .fontId = FONT_ID_BODY_16,
                                .fontSize = 16,
                                .textColor = matchVsColor
                            }));
                        }
                    }

                    // List of not registered players (click to register)
                    CLAY(CLAY_ID("NotRegisteredPlayersList"), {
                        .layout = {
                            .layoutDirection = CLAY_TOP_TO_BOTTOM,
                            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
                            .childGap = 4
                        },
                        .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() }
                    }) {
                        u32 not_registered_count = 0;
                        u32 idx_tail = data.players.len + 1;
                        u32 idx = (data.players.entities)->nxt;
                        while (idx != idx_tail)
                        {
                            Entity *player = data.players.entities + idx;
                            bool is_registered = (player->registrations >> ENTITY_IDX_TO_BIT(tournament_idx)) & 1;

                            if (!is_registered)
                            {
                                not_registered_count++;
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
                else if (panel_tournament->state == TOURNAMENT_IN_PROGRESS)
                {
                    // Show tournament status
                    CLAY_TEXT(CLAY_STRING("Tournament In Progress"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 16,
                        .textColor = addButtonColor
                    }));

                    // Return to Registration button
                    CLAY(CLAY_ID("ReturnToRegistrationButton"), {
                        .layout = {
                            .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                            .padding = { 10, 10, 8, 8 },
                            .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                        },
                        .backgroundColor = Clay_Hovered() ? removeButtonHoverColor : removeButtonColor,
                        .cornerRadius = CLAY_CORNER_RADIUS(4)
                    }) {
                        Clay_OnHover(HandleReturnToRegistration, 0);
                        CLAY_TEXT(CLAY_STRING("Return to Registration"), CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_BODY_16,
                            .fontSize = 14,
                            .textColor = removeTextColor
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
            }

            // Right panel - Tournament chart
            CLAY(CLAY_ID("TournamentChart"), {
                .layout = {
                    .sizing = layoutExpand,
                    .childAlignment = { .x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_TOP }
                },
                .clip = { .horizontal = true, .vertical = true, .childOffset = Clay_GetScrollOffset() },
                .backgroundColor = COLOR_OFF_WHITE
            }) {
                Entity *tournament = data.tournaments.entities + tournament_idx;

                // Get the number of players in the tournament
                s32 _positions[64] = {0};
                u32 num_players = find_all_filled_slots(tournament->registrations, _positions);

                if (num_players == 0)
                {
                    CLAY_TEXT(CLAY_STRING("No players registered yet"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 18,
                        .textColor = matchVsColor
                    }));
                }
                else if (tournament->format == FORMAT_SINGLE_ELIMINATION)
                {
                    // Only reconstruct bracket during registration phase
                    // Once tournament starts, the bracket is locked and only updated by match results
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
                    CLAY(CLAY_ID("RoundsContainer"), {
                        .layout = {
                            .layoutDirection = CLAY_LEFT_TO_RIGHT,
                            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0) },
                            .childGap = 32,
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
                                            RenderByeSlot(match_id);
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

                                            RenderMatchSlot(name1, name2, player1_idx, player2_idx, pos1, pos2, match_id);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else // FORMAT_GROUPS_THEN_BRACKET
                {
                    // Only reconstruct groups during registration phase
                    if (tournament->state == TOURNAMENT_REGISTRATION)
                    {
                        // Default group size of 4
                        tournament_construct_groups(tournament, 4);
                    }

                    // Get group info from the constructed group phase
                    u32 group_size = tournament->group_phase.group_size;
                    u32 num_groups = tournament->group_phase.num_groups;

                    // Render groups vertically (one below the other), centered
                    CLAY(CLAY_ID("GroupsContainer"), {
                        .layout = {
                            .layoutDirection = CLAY_TOP_TO_BOTTOM,
                            .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) },
                            .childGap = 24,
                            .padding = { 16, 16, 16, 16 },
                            .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
                        }
                    }) {
                        for (u32 g = 0; g < num_groups; g++)
                        {
                            // Count actual players in this group
                            u32 players_in_group = 0;
                            for (u32 slot = 0; slot < group_size; slot++)
                            {
                                if (tournament->group_phase.groups[g][slot] != 0)
                                {
                                    players_in_group++;
                                }
                            }

                            CLAY(CLAY_IDI("Group", g), {
                                .layout = {
                                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                                    .sizing = { .width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0) },
                                    .padding = { 12, 12, 12, 12 },
                                    .childGap = 8
                                },
                                .backgroundColor = COLOR_WHITE,
                                .cornerRadius = CLAY_CORNER_RADIUS(8),
                                .border = { .width = {1, 1, 1, 1}, .color = matchBorderColor }
                            }) {
                                // Group header
                                String8 group_prefix = str8_lit("Group ");
                                String8 group_num = str8_u32(data.frameArena, g + 1);
                                String8 group_label = str8_cat(data.frameArena, group_prefix, group_num);
                                CLAY_TEXT(str8_to_clay(group_label), CLAY_TEXT_CONFIG({
                                    .fontId = FONT_ID_BODY_16,
                                    .fontSize = 18,
                                    .textColor = tournamentTitleColor
                                }));

                                RenderGroupMatrix(tournament, g, players_in_group);
                            }
                        }
                    }
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
            .clip = { .horizontal = true, .childOffset = Clay_GetScrollOffset() }
        }) {
            // Section banner
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
                    .clip = { .horizontal = true, .vertical = true, .childOffset = Clay_GetScrollOffset() }
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
                            .sizing = { .width = CLAY_SIZING_FIXED(100) }
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
                        Clay_OnHover(HandleEventRowInteraction, (intptr_t)idx);

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

                        // Actions
                        CLAY(CLAY_IDI("EventActions", idx), {
                            .layout = {
                                .sizing = { .width = CLAY_SIZING_FIXED(100) }
                            }
                        }) {
                            RenderEventsActionsButtons();
                        }
                    }

                    idx = tournament->nxt;
                }
                }
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
        // Section banner
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
                .clip = { .horizontal = true, .vertical = true, .childOffset = Clay_GetScrollOffset() }
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
                        CLAY_TEXT(str8_to_clay_truncated(data.frameArena, player->name, MAX_DISPLAY_NAME_LEN), CLAY_TEXT_CONFIG({
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
                }

                idx = player->nxt;
            }
            }
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
    }

    Clay_RenderCommandArray renderCommands = Clay_EndLayout();

    for (int32_t i = 0; i < renderCommands.length; i++) {
        Clay_RenderCommandArray_Get(&renderCommands, i)->boundingBox.y += data.yOffset;
    }
    return renderCommands;
}
