#ifndef TEXTBOX_H
#define TEXTBOX_H

#include "raylib/raylib.h"
#include "clay.h"
#include "core.h"

#define TEXT_INPUT_MAX_LEN 256

typedef struct TextInput TextInput;
struct TextInput {
    char buffer[TEXT_INPUT_MAX_LEN];
    u32 len;
    bool focused;
    u32 cursorPos;
    float blinkTimer;
};

void TextInput_ProcessKeyboard(TextInput *input);
void TextInput_UpdateCursorFromClick(TextInput *input, float clickRelativeX, Font *fonts, int fontId);
void TextInput_HandleClick(TextInput *input, Clay_BoundingBox inputBox, Font *fonts, int fontId);
void TextInput_Render(TextInput *input, Clay_String elementId, Clay_String placeholder, Font *fonts, int fontId);

#endif // TEXTBOX_H
