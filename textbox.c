///////////////////////////////////////////////////////////////////////////////
// Text Input Helper Functions

#include "raylib/raylib.h"
#include "clay.h"
#include "textbox.h"
#include "colors.c"

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

    // Handle escape to unfocus
    if (IsKeyPressed(KEY_ESCAPE)) {
        input->focused = false;
    }
}

void
TextInput_UpdateCursorFromClick(TextInput *input, float clickRelativeX, Font *fonts, int fontId)
{
    if (clickRelativeX < 0) clickRelativeX = 0;

    u32 newCursorPos = 0;
    float prevWidth = 0;
    for (u32 i = 0; i <= input->len; i++) {
        char tempBuffer[TEXT_INPUT_MAX_LEN];
        for (u32 j = 0; j < i; j++) {
            tempBuffer[j] = input->buffer[j];
        }
        tempBuffer[i] = '\0';

        Vector2 textSize = MeasureTextEx(fonts[fontId], tempBuffer, 16.0f, 0);

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

void
TextInput_HandleClick(TextInput *input, Clay_BoundingBox inputBox, Font *fonts, int fontId)
{
    Vector2 mousePos = GetMousePosition();
    bool clickedInside = mousePos.x >= inputBox.x && mousePos.x <= inputBox.x + inputBox.width &&
                         mousePos.y >= inputBox.y && mousePos.y <= inputBox.y + inputBox.height;

    if (clickedInside) {
        float inputPaddingLeft = 12.0f;
        float clickRelativeX = mousePos.x - inputBox.x - inputPaddingLeft;

        if (!input->focused) {
            input->focused = true;
        }
        TextInput_UpdateCursorFromClick(input, clickRelativeX, fonts, fontId);
    } else if (input->focused) {
        input->focused = false;
    }
}

void
TextInput_Render(TextInput *input, Clay_String elementId,
    Clay_String placeholder, Font *fonts, int fontId)
{
    Clay_Color inputBorderColor = input->focused ? headerButtonStringClickColor : textInputBorderColor;

    CLAY(Clay_GetElementId(elementId), {
        .layout = {
            .sizing = {.width = CLAY_SIZING_FIXED(300), .height = CLAY_SIZING_FIXED(40)},
            .padding = { 12, 12, 0, 0 },
            .childAlignment = { .x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER }
        },
        .backgroundColor = textInputBackgroundColor,
        .cornerRadius = CLAY_CORNER_RADIUS(4),
        .border = { .width = {1, 1, 1, 1}, .color = inputBorderColor }
    }) {
        // Calculate cursor X offset based on text before cursor
        float cursorOffsetX = 0;
        if (input->cursorPos > 0)
        {
            char beforeCursor[TEXT_INPUT_MAX_LEN];
            for (u32 i = 0; i < input->cursorPos; i++) {
                beforeCursor[i] = input->buffer[i];
            }
            beforeCursor[input->cursorPos] = '\0';
            Vector2 textSize = MeasureTextEx(fonts[fontId], beforeCursor, 16.0f, 0);
            cursorOffsetX = textSize.x;
        }

        // Container that stacks cursor and text
        CLAY_AUTO_ID({
            .layout = {
                .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
            }
        }) {
            // Render cursor with left padding to position it
            if (input->focused)
            {
                bool cursorVisible = input->blinkTimer < 0.5f;
                Clay_Color cursorColor = cursorVisible ? stringColor : (Clay_Color){0, 0, 0, 0};
                CLAY_AUTO_ID({
                    .layout = {
                        .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
                        .padding = { .left = (u16)cursorOffsetX },
                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                    },
                    .floating = { .attachTo = CLAY_ATTACH_TO_PARENT }
                }) {
                    CLAY_AUTO_ID({
                        .layout = {
                            .sizing = {.width = CLAY_SIZING_FIXED(2), .height = CLAY_SIZING_FIXED(18)}
                        },
                        .backgroundColor = cursorColor
                    }) {}
                }
            }

            // Render full text in a locally-scoped container to avoid ID collisions
            if (input->len > 0)
            {
                CLAY(CLAY_ID_LOCAL("TextContent"), {}) {
                    Clay_String inputText = {
                        .length = (int)input->len,
                        .chars = input->buffer,
                        .isStaticallyAllocated = false
                    };
                    CLAY_TEXT(inputText, CLAY_TEXT_CONFIG({
                        .fontId = fontId,
                        .fontSize = 16,
                        .textColor = stringColor
                    }));
                }
            }
            else if (!input->focused)
            {
                // Show placeholder when empty and not focused
                CLAY(CLAY_ID_LOCAL("Placeholder"), {}) {
                    CLAY_TEXT(placeholder, CLAY_TEXT_CONFIG({
                        .fontId = fontId,
                        .fontSize = 16,
                        .textColor = matchVsColor
                    }));
                }
            }
        }
    }
}
