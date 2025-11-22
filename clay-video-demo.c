#include <stdlib.h>
#include <assert.h>

#include "clay.h"
#include "core.h"
#include "layout.h"

#include "arena.h"


///////////////////////////////////////////////////////////////////////////////
// Globals

const int FONT_ID_BODY_16 = 0;

Clay_Color COLOR_WHITE     = { 255, 255, 255, 255};
Clay_Color COLOR_OFF_WHITE = { 245, 245, 245, 255};
Clay_Color COLOR_BLACK     = { 0, 0, 0, 255};

Clay_Color stringColor            = { 125, 125, 125, 255};
Clay_Color stringColorOnClick     = { 135, 206, 235, 255 };
Clay_Color contentBackgroundColor = { 90, 90, 90, 255 };
Clay_Color headerBackgroundColor  = { 255, 255, 255, 255 };


Clay_Color headerButtonColor      = { 255, 255, 255, 255};
Clay_Color headerbuttonHoverColor = { 240, 240, 240, 255 };
Clay_Color headerbuttonClickColor = { 135, 206, 235, 255 };

Clay_Sizing layoutExpand = {
    .width = CLAY_SIZING_GROW(0),
    .height = CLAY_SIZING_GROW(0)
};

ClayVideoDemo_Data data = {0};

///////////////////////////////////////////////////////////////////////////////

void
HandleHeaderButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    u8 buttonIndex = (u8)userData;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        data.selectedHeaderButton = buttonIndex;
    }
}

void
RenderHeaderButton(Clay_String text, u8 buttonIndex)
{
    bool isSelected = (data.selectedHeaderButton == buttonIndex);

    CLAY(CLAY_IDI("HeaderButton", buttonIndex), {
        .layout = { .padding = { 16, 16, 8, 8 }},
        .backgroundColor = isSelected ? headerbuttonClickColor : (Clay_Hovered() ? headerbuttonHoverColor : headerButtonColor),
        .cornerRadius = CLAY_CORNER_RADIUS(5)
    }) {
        Clay_OnHover(HandleHeaderButtonInteraction, (intptr_t)buttonIndex);
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 24,
            .textColor = stringColor
        }));
    }
}

Clay_RenderCommandArray 
ClayVideoDemo_CreateLayout(void)
{
    data.frameArena->pos = 0;

    Clay_BeginLayout();

    // Build UI here
    CLAY(CLAY_ID("OuterContainer"), {
        .backgroundColor = COLOR_OFF_WHITE,
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = layoutExpand,
            .padding = CLAY_PADDING_ALL(16),
            .childGap = 16
        }
    }) {
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
            RenderHeaderButton(CLAY_STRING("Dashboard"), 0);
            RenderHeaderButton(CLAY_STRING("Events"), 1);
            RenderHeaderButton(CLAY_STRING("Players"), 2);
            RenderHeaderButton(CLAY_STRING("Results"), 3);
        };
        CLAY(CLAY_ID("MainPageContent"), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = layoutExpand,
                .padding = { 16, 16, 0, 0 },
                .childGap = 16,
                .childAlignment = {
                    .y = CLAY_ALIGN_Y_CENTER
                }
            },
            .clip = { .horizontal = true, .childOffset = Clay_GetScrollOffset() }
        }) {
            // Put main content stuff
        }
    }

    Clay_RenderCommandArray renderCommands = Clay_EndLayout();
    for (int32_t i = 0; i < renderCommands.length; i++) {
        Clay_RenderCommandArray_Get(&renderCommands, i)->boundingBox.y += data.yOffset;
    }
    return renderCommands;
}
