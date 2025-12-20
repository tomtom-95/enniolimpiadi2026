#include "arena.h"
#include "layout.h"
#include "players.h"
#include "raylib/raylib.h"
#define CLAY_IMPLEMENTATION

#include "core.h"
#include "arena.c"
#include "string.c"
#include "players.c"

#include "clay.h"
#include "layout.c"
#include "raylib/clay_renderer_raylib.c"

#include "resources/roboto_font.h"
#include "resources/press_start_2p_font.h"


void
HandleClayErrors(Clay_ErrorData errorData)
{
    printf("%s", errorData.errorText.chars);
}

int
main(void)
{
    // Initialize ctx for using scratch arenas
    ctx_init();

    Clay_Raylib_Initialize(1280, 720, "Enniolimpiadi 2026",
        FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);

    // Maximize window after creation to get proper dimensions
    MaximizeWindow();

    // Increased with respect to the default
    Clay_SetMaxElementCount(32768);

    uint64_t clayRequiredMemory = Clay_MinMemorySize();
    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(clayRequiredMemory, malloc(clayRequiredMemory));

    // Initialize Clay with actual window dimensions after maximizing
    Clay_Initialize(clayMemory, (Clay_Dimensions) {
       .width = GetScreenWidth(),
       .height = GetScreenHeight()
    }, (Clay_ErrorHandler) { HandleClayErrors });

    Font fonts[2];
    fonts[FONT_ID_BODY_16] = LoadFontFromMemory(".ttf", __Roboto_Regular_ttf, __Roboto_Regular_ttf_len, 48, 0, 400);
    fonts[FONT_ID_PRESS_START_2P] = LoadFontFromMemory(".ttf", __PressStart2P_Regular_ttf, __PressStart2P_Regular_ttf_len, 48, 0, 400);

    SetTextureFilter(fonts[FONT_ID_BODY_16].texture, TEXTURE_FILTER_BILINEAR);
    SetTextureFilter(fonts[FONT_ID_PRESS_START_2P].texture, TEXTURE_FILTER_POINT);

    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    Clay_SetDebugModeEnabled(false);

    Arena *arena = arena_alloc(MegaByte(10));

    PlayersList players_list = players_list_init(arena, 64);
    EventsList events_list = events_list_init(arena, 64);

    olympiad_load(arena, &players_list, &events_list);

    // Initialization of global data
    data.arena = arena;
    data.frameArena = arena_alloc(MegaByte(1));

    data.yOffset = 0;
    data.fonts = fonts;

    data.players     = players_list;
    data.tournaments = events_list;

    data.chartZoomLevel       = 1.0f;
    data.groupMatrixZoomLevel = 1.0f;

    data.groupsPanelVisible   = true;
    data.knockoutPanelVisible = true;

    f64 lastSaveTime = 0;
    while (!WindowShouldClose())
    {
        f64 currentTime = GetTime();
        if (currentTime - lastSaveTime > 30.0)
        {
            olympiad_save(&players_list, &events_list);
            lastSaveTime = currentTime;
        }

        Clay_SetLayoutDimensions((Clay_Dimensions) {
            .width = GetScreenWidth(),
            .height = GetScreenHeight()
        });

        // Reset cursor state at start of frame (handlers will update it)
        data.mouseCursor = MOUSE_CURSOR_DEFAULT;

        Vector2 mousePosition = GetMousePosition();
        Vector2 scrollDelta = GetMouseWheelMoveV();

        Clay_SetPointerState((Clay_Vector2) { mousePosition.x, mousePosition.y }, IsMouseButtonDown(0));
        Clay_UpdateScrollContainers(true, (Clay_Vector2) { scrollDelta.x, scrollDelta.y }, GetFrameTime());

        Clay_RenderCommandArray renderCommands = CreateLayout();

        BeginDrawing();
        ClearBackground(BLACK);
        Clay_Raylib_Render(renderCommands, fonts);

        // Render text input cursor (after Clay, so it doesn't interfere with scrolling)
        if (data.focusedTextbox != TEXTBOX_NULL)
        {
            const char *inputIdStr = TextBoxInputIds[data.focusedTextbox];
            const char *scrollIdStr = TextBoxScrollIds[data.focusedTextbox];

            Clay_BoundingBox bounding_box = Clay_GetElementData(
                Clay_GetElementId((Clay_String){ .length = strlen(inputIdStr), .chars = inputIdStr })
            ).boundingBox;

            Clay_ScrollContainerData scroll_data = Clay_GetScrollContainerData(
                Clay_GetElementId((Clay_String){ .length = strlen(scrollIdStr), .chars = scrollIdStr })
            );

            TextInput_RenderCursor(&data.textInputs[data.focusedTextbox], bounding_box, scroll_data);
        }

        EndDrawing();

        SetMouseCursor(data.mouseCursor);
    }

    // Save state before closing
    if (olympiad_save(&data.players, &data.tournaments))
    {
        printf("State saved\n");
    }
    else
    {
        printf("Failed to save state!\n");
    }

    Clay_Raylib_Close();
}
