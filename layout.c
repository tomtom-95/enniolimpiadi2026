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

Clay_Color goBackButtonColor      = { 230, 240, 250, 255};
Clay_Color goBackButtonHoverColor = { 200, 220, 245, 255};
Clay_Color goBackTextColor        = { 60, 100, 160, 255};
Clay_Color tournamentTitleColor   = { 40, 40, 60, 255};

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
const int FONT_ID_ORIENTAL_CHICKEN   = 1;

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

///////////////////////////////////////////////////////////////////////////////
// Event Handlers

void
HandleHeaderButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    Page page = (Page)userData;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        data.selectedHeaderButton = page;
    }
}

void
HandleEventRowInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    u8 event_idx = (u8)userData;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        data.selectedTournamentIdx = event_idx;
    }
}

void
HandleGoBackInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
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
    SetMouseCursor(MOUSE_CURSOR_IBEAM);

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
    SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
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
            .childGap = 16,
            .childAlignment = {
                .y = CLAY_ALIGN_Y_TOP
            }
        },
        .clip = { .horizontal = true, .childOffset = Clay_GetScrollOffset() },
        .backgroundColor = COLOR_OFF_WHITE
    }) {
        CLAY(CLAY_ID("TotalEventsCount"), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {.height = CLAY_SIZING_FIT(0) , .width = CLAY_SIZING_GROW(0)},
                .padding = { 16, 16, 8, 8 },
                .childGap = 8
            },
            .backgroundColor = headerButtonColor,
            .cornerRadius = CLAY_CORNER_RADIUS(5)
        }) {
            CLAY_TEXT(CLAY_STRING("Total Events"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 18,
                .textColor = stringColor
            }));
            u32 tournaments_count = entity_list_count(&data.tournaments);
            String8 tournaments_count_str8 = str8_u32(data.frameArena, tournaments_count); 
            Clay_String tournaments_count_clay = str8_to_clay(tournaments_count_str8);

            CLAY_TEXT(tournaments_count_clay, CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_ORIENTAL_CHICKEN,
                .fontSize = 22,
                .textColor = stringColor
            }));
        }
        CLAY(CLAY_ID("TotalPlayersCount"), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {.height = CLAY_SIZING_FIT(0) , .width = CLAY_SIZING_GROW(0)},
                .padding = { 16, 16, 8, 8 },
                .childGap = 8
            },
            .backgroundColor = headerButtonColor,
            .cornerRadius = CLAY_CORNER_RADIUS(5)
        }) {
            CLAY_TEXT(CLAY_STRING("Total Players"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 18,
                .textColor = stringColor
            }));

            u32 players_count = entity_list_count(&data.players);
            String8 players_count_str8 = str8_u32(data.frameArena, players_count); 
            Clay_String players_count_clay = str8_to_clay(players_count_str8);

            CLAY_TEXT(players_count_clay, CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_ORIENTAL_CHICKEN,
                .fontSize = 22,
                .textColor = stringColor
            }));
        }
        CLAY(CLAY_ID("TotalEventsFinished"), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {.height = CLAY_SIZING_FIT(0) , .width = CLAY_SIZING_GROW(0)},
                .padding = { 16, 16, 8, 8 },
                .childGap = 8
            },
            .backgroundColor = headerButtonColor,
            .cornerRadius = CLAY_CORNER_RADIUS(5)
        }) {
            CLAY_TEXT(CLAY_STRING("Events Finished"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 18,
                .textColor = stringColor
            }));
            CLAY_TEXT(CLAY_STRING("TODO: modify the data structure to track finished events"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 18,
                .textColor = stringColor
            }));
        }
        CLAY(CLAY_ID("Medagliere"), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {.height = CLAY_SIZING_FIT(0) , .width = CLAY_SIZING_GROW(0)},
                .padding = { 16, 16, 8, 8 },
                .childGap = 8
            },
            .backgroundColor = headerButtonColor,
            .cornerRadius = CLAY_CORNER_RADIUS(5)
        }) {
            CLAY_TEXT(CLAY_STRING("Medagliere"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 18,
                .textColor = stringColor
            }));
            CLAY_TEXT(CLAY_STRING("Oro: "), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 18,
                .textColor = stringColor
            }));
            CLAY_TEXT(CLAY_STRING("Argento: "), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 18,
                .textColor = stringColor
            }));
            CLAY_TEXT(CLAY_STRING("Bronzo: "), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 18,
                .textColor = stringColor
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
            .sizing = {.height = CLAY_SIZING_FIT(0) , .width = CLAY_SIZING_GROW(0)},
            .childGap = 8
        }
    }) {
        CLAY_TEXT(CLAY_STRING("Edit"), CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 16,
            .textColor = COLOR_BLUE
        }));
        CLAY_TEXT(CLAY_STRING("Delete"), CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 16,
            .textColor = COLOR_RED
        }));
    }
}

void
RenderMatchSlot(Clay_String player1_name, Clay_String player2_name, bool player1_is_tbd, bool player2_is_tbd, u32 match_id)
{
    Clay_Color name1Color = player1_is_tbd ? matchVsColor : stringColor;
    Clay_Color name2Color = player2_is_tbd ? matchVsColor : stringColor;

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
            // Player 1
            CLAY_TEXT(player1_name, CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 16,
                .textColor = name1Color
            }));

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

            // Player 2
            CLAY_TEXT(player2_name, CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 16,
                .textColor = name2Color
            }));
        }
    }
}

void
RenderByeSlot(u32 match_id)
{
    // Render an empty/minimal bye slot
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
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER }
            },
            .backgroundColor = COLOR_WHITE,
            .cornerRadius = CLAY_CORNER_RADIUS(6)
        }) {
            CLAY_TEXT(CLAY_STRING("BYE"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 16,
                .textColor = matchVsColor
            }));
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
                    .sizing = {.width = CLAY_SIZING_FIXED(180), .height = CLAY_SIZING_GROW(0)},
                    .padding = { 12, 12, 12, 12 },
                    .childGap = 12
                },
                .backgroundColor = COLOR_WHITE,
                .cornerRadius = CLAY_CORNER_RADIUS(6)
            }) {
                // Registered players section
                CLAY_TEXT(CLAY_STRING("Registered Players:"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 16,
                    .textColor = matchVsColor
                }));

                // List of registered players (click to unregister)
                CLAY(CLAY_ID("RegisteredPlayersList"), {
                    .layout = {
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
                        .childGap = 4
                    },
                    .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() }
                }) {
                    s32 positions[64] = {0};
                    u32 count = find_all_filled_slots((data.tournaments.entities + tournament_idx)->registrations, positions);

                    for (u32 i = 0; i < count; i++)
                    {
                        u32 player_idx = BIT_TO_ENTITY_IDX(positions[i]);
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
                            CLAY_TEXT(str8_to_clay(player->name), CLAY_TEXT_CONFIG({
                                .fontId = FONT_ID_BODY_16,
                                .fontSize = 16,
                                .textColor = stringColor
                            }));
                        }
                    }

                    if (count == 0)
                    {
                        CLAY_TEXT(CLAY_STRING("No players registered"), CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_BODY_16,
                            .fontSize = 16,
                            .textColor = matchVsColor
                        }));
                    }
                }

                // Not registered players section
                CLAY_TEXT(CLAY_STRING("Not Registered:"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 16,
                    .textColor = matchVsColor
                }));

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
                                CLAY_TEXT(str8_to_clay(player->name), CLAY_TEXT_CONFIG({
                                    .fontId = FONT_ID_BODY_16,
                                    .fontSize = 16,
                                    .textColor = stringColor
                                }));
                            }
                        }

                        idx = player->nxt;
                    }

                    if (not_registered_count == 0)
                    {
                        CLAY_TEXT(CLAY_STRING("All players registered"), CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_BODY_16,
                            .fontSize = 16,
                            .textColor = matchVsColor
                        }));
                    }
                }
            }

            // Right panel - Tournament chart
            CLAY(CLAY_ID("TournamentChart"), {
                .layout = {
                    .sizing = layoutExpand,
                    .childAlignment = { .x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER }
                },
                .clip = { .horizontal = true, .vertical = true, .childOffset = Clay_GetScrollOffset() },
                .backgroundColor = COLOR_OFF_WHITE
            }) {
                // Get the number of players in the tournament
                s32 _positions[64] = {0};
                u32 num_players = find_all_filled_slots((data.tournaments.entities + tournament_idx)->registrations, _positions);

                // Calculate bracket size (next power of 2 >= num_players)
                u32 bracket_size = 1;
                while (bracket_size < num_players) {
                    bracket_size <<= 1;
                }

                // Calculate byes
                u32 num_byes = bracket_size - num_players;
                u32 matches_in_r1 = bracket_size / 2;
                u32 actual_matches_r1 = matches_in_r1 - num_byes;

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
                    for (u32 round = 0; round < num_rounds; round++) {
                        u32 matches_in_round = bracket_size >> (round + 1);
                        // Each slot grows by 2^round so matches align with parent matches
                        u32 grow_factor = 1 << round;

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

                                // Slot container that grows proportionally and centers the match
                                CLAY(CLAY_IDI("MatchSlot", match_id), {
                                    .layout = {
                                        .sizing = { .width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_GROW(grow_factor) },
                                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                                    }
                                }) {
                                    if (round == 0)
                                    {
                                        // First round
                                        if (match < actual_matches_r1)
                                        {
                                            // Real match with two players
                                            u32 player1_idx = _positions[match * 2];
                                            u32 player2_idx = _positions[match * 2 + 1];

                                            Entity *player1 = data.players.entities + BIT_TO_ENTITY_IDX(player1_idx);
                                            Entity *player2 = data.players.entities + BIT_TO_ENTITY_IDX(player2_idx);

                                            RenderMatchSlot(str8_to_clay(player1->name), str8_to_clay(player2->name), false, false, match_id);
                                        }
                                        else
                                        {
                                            // Bye slot
                                            RenderByeSlot(match_id);
                                        }
                                    }
                                    else if (round == 1 && num_byes > 0)
                                    {
                                        // Second round - check if this slot has a bye player
                                        u32 r1_first = match * 2;
                                        u32 r1_second = match * 2 + 1;

                                        bool first_is_bye = (r1_first >= actual_matches_r1);
                                        bool second_is_bye = (r1_second >= actual_matches_r1);

                                        if (first_is_bye && !second_is_bye)
                                        {
                                            // First parent was bye, second was real match
                                            u32 bye_idx = r1_first - actual_matches_r1;
                                            u32 bye_player_pos = 2 * actual_matches_r1 + bye_idx;
                                            u32 bye_player_idx = _positions[bye_player_pos];
                                            Entity *bye_player = data.players.entities + BIT_TO_ENTITY_IDX(bye_player_idx);

                                            RenderMatchSlot(str8_to_clay(bye_player->name), CLAY_STRING("TBD"), false, true, match_id);
                                        }
                                        else if (!first_is_bye && second_is_bye)
                                        {
                                            // Second parent was bye, first was real match
                                            u32 bye_idx = r1_second - actual_matches_r1;
                                            u32 bye_player_pos = 2 * actual_matches_r1 + bye_idx;
                                            u32 bye_player_idx = _positions[bye_player_pos];
                                            Entity *bye_player = data.players.entities + BIT_TO_ENTITY_IDX(bye_player_idx);

                                            RenderMatchSlot(CLAY_STRING("TBD"), str8_to_clay(bye_player->name), true, false, match_id);
                                        }
                                        else if (first_is_bye && second_is_bye)
                                        {
                                            // Both parents were byes (rare, but handle it)
                                            u32 bye_idx1 = r1_first - actual_matches_r1;
                                            u32 bye_idx2 = r1_second - actual_matches_r1;
                                            u32 bye_player_pos1 = 2 * actual_matches_r1 + bye_idx1;
                                            u32 bye_player_pos2 = 2 * actual_matches_r1 + bye_idx2;
                                            u32 bye_player_idx1 = _positions[bye_player_pos1];
                                            u32 bye_player_idx2 = _positions[bye_player_pos2];
                                            Entity *bye_player1 = data.players.entities + BIT_TO_ENTITY_IDX(bye_player_idx1);
                                            Entity *bye_player2 = data.players.entities + BIT_TO_ENTITY_IDX(bye_player_idx2);

                                            RenderMatchSlot(str8_to_clay(bye_player1->name), str8_to_clay(bye_player2->name), false, false, match_id);
                                        }
                                        else
                                        {
                                            // Neither parent was bye - TBD vs TBD
                                            RenderMatchSlot(CLAY_STRING("TBD"), CLAY_STRING("TBD"), true, true, match_id);
                                        }
                                    }
                                    else
                                    {
                                        // Later rounds or round 1 without byes
                                        RenderMatchSlot(CLAY_STRING("TBD"), CLAY_STRING("TBD"), true, true, match_id);
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
                .childGap = 16
            },
            .backgroundColor = COLOR_OFF_WHITE
        }) {
            // Header with input field and add button
            CLAY(CLAY_ID("EventsInputHeader"), {
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                    .padding = { 16, 16, 12, 12 },
                    .childGap = 12,
                    .childAlignment = { .x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = COLOR_WHITE,
                .cornerRadius = CLAY_CORNER_RADIUS(8)
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
                        .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIXED(40)},
                        .padding = { 20, 20, 0, 0 },
                        .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .backgroundColor = Clay_Hovered() ? addButtonHoverColor : addButtonColor,
                    .cornerRadius = CLAY_CORNER_RADIUS(4)
                }) {
                    Clay_OnHover(HandleAddEventButtonInteraction, 0);
                    CLAY_TEXT(CLAY_STRING("Add Event"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 16,
                        .textColor = addButtonTextColor
                    }));
                }
            }

            // Events list container
            CLAY(CLAY_ID("EventsList"), {
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = layoutExpand,
                    .childGap = 1,
                },
                .cornerRadius = CLAY_CORNER_RADIUS(8),
                .clip = { .horizontal = true, .vertical = true, .childOffset = Clay_GetScrollOffset() }
            }) {
                // List header
                CLAY(CLAY_ID("EventsListHeader"), {
                    .layout = {
                        .layoutDirection = CLAY_LEFT_TO_RIGHT,
                        .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                        .padding = { 16, 16, 10, 10 }
                    },
                    .backgroundColor = COLOR_WHITE
                }) {
                    CLAY(CLAY_ID("EventNameHeader"), {
                        .layout = {
                            .sizing = { .width = CLAY_SIZING_GROW(0) }
                        }
                    }) {
                        CLAY_TEXT(CLAY_STRING("Event Name"), CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_BODY_16,
                            .fontSize = 16,
                            .textColor = matchVsColor
                        }));
                    }
                    CLAY(CLAY_ID("EventPlayersHeader"), {
                        .layout = {
                            .sizing = { .width = CLAY_SIZING_FIXED(150) }
                        }
                    }) {
                        CLAY_TEXT(CLAY_STRING("Players"), CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_BODY_16,
                            .fontSize = 16,
                            .textColor = matchVsColor
                        }));
                    }
                    CLAY(CLAY_ID("EventActionsHeader"), {
                        .layout = {
                            .sizing = { .width = CLAY_SIZING_FIXED(100) }
                        }
                    }) {
                        CLAY_TEXT(CLAY_STRING("Actions"), CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_BODY_16,
                            .fontSize = 16,
                            .textColor = matchVsColor
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
                            .padding = { 16, 16, 6, 6 }
                        },
                        .backgroundColor = Clay_Hovered() ? eventElementHoverColor : eventElementColor
                    }) {
                        Clay_OnHover(HandleEventRowInteraction, (intptr_t)idx);

                        // Event name
                        CLAY(CLAY_IDI("EventName", idx), {
                            .layout = {
                                .sizing = { .width = CLAY_SIZING_GROW(0) },
                            }
                        }) {
                            CLAY_TEXT(str8_to_clay(tournament->name), CLAY_TEXT_CONFIG({
                                .fontId = FONT_ID_BODY_16,
                                .fontSize = 16,
                                .textColor = stringColor
                            }));
                        }

                        // Number of players registered
                        CLAY(CLAY_IDI("EventPlayers", idx), {
                            .layout = {
                                .sizing = { .width = CLAY_SIZING_FIXED(150) }
                            }
                        }) {
                            s32 positions[64];
                            u32 count = find_all_filled_slots(tournament->registrations, positions);
                            CLAY_TEXT(str8_to_clay(str8_u32(data.frameArena, count)), CLAY_TEXT_CONFIG({
                                .fontId = FONT_ID_BODY_16,
                                .fontSize = 16,
                                .textColor = stringColor
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
            .childGap = 16
        },
        .backgroundColor = COLOR_OFF_WHITE
    }) {
        // Header with input field and add button
        CLAY(CLAY_ID("PlayersHeader"), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                .padding = { 16, 16, 12, 12 },
                .childGap = 12,
                .childAlignment = { .x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = COLOR_WHITE,
            .cornerRadius = CLAY_CORNER_RADIUS(8)
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
                    .sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIXED(40)},
                    .padding = { 20, 20, 0, 0 },
                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = Clay_Hovered() ? addButtonHoverColor : addButtonColor,
                .cornerRadius = CLAY_CORNER_RADIUS(4)
            }) {
                Clay_OnHover(HandleAddPlayerButtonInteraction, 0);
                CLAY_TEXT(CLAY_STRING("Add Player"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 16,
                    .textColor = addButtonTextColor
                }));
            }
        }

        // Players list container
        CLAY(CLAY_ID("PlayersList"), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = layoutExpand,
                .childGap = 1
            },
            .cornerRadius = CLAY_CORNER_RADIUS(8),
            .clip = { .horizontal = true, .vertical = true, .childOffset = Clay_GetScrollOffset() }
        }) {
            // List header
            CLAY(CLAY_ID("PlayersListHeader"), {
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = {.height = CLAY_SIZING_FIT(0), .width = CLAY_SIZING_GROW(0)},
                    .padding = { 16, 16, 10, 10 }
                },
                .backgroundColor = COLOR_WHITE
            }) {
                CLAY(CLAY_ID("PlayerNameHeader"), {
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_GROW(0) }
                    }
                }) {
                    CLAY_TEXT(CLAY_STRING("Player Name"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 16,
                        .textColor = matchVsColor
                    }));
                }
                CLAY(CLAY_ID("PlayerRegistrationsHeader"), {
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_FIXED(150) }
                    }
                }) {
                    CLAY_TEXT(CLAY_STRING("Registrations"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 16,
                        .textColor = matchVsColor
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
                        .padding = { 16, 16, 6, 6 }
                    },
                    .backgroundColor = Clay_Hovered() ? playerRowHoverColor : playerRowColor
                }) {
                    // Player name
                    CLAY(CLAY_IDI("PlayerName", idx), {
                        .layout = {
                            .sizing = { .width = CLAY_SIZING_GROW(0) }
                        }
                    }) {
                        CLAY_TEXT(str8_to_clay(player->name), CLAY_TEXT_CONFIG({
                            .fontId = FONT_ID_BODY_16,
                            .fontSize = 16,
                            .textColor = stringColor
                        }));
                    }

                    // Number of registrations
                    CLAY(CLAY_IDI("PlayerRegistrations", idx), {
                        .layout = {
                            .sizing = { .width = CLAY_SIZING_FIXED(150) }
                        }
                    }) {
                        s32 positions[64];
                        u32 count = find_all_filled_slots(player->registrations, positions);
                        CLAY_TEXT(str8_to_clay(str8_u32(data.frameArena, count)), CLAY_TEXT_CONFIG({
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
