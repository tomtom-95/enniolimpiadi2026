#include "arena.h"
#include "core.h"
#include "string.c"
#include "arena.c"
#include "players.c"

#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "raylib/clay_renderer_raylib.c"
#include "layout.c"


// This function is new since the video was published
void HandleClayErrors(Clay_ErrorData errorData)
{
    printf("%s", errorData.errorText.chars);
}

int main(void)
{
    Clay_Raylib_Initialize(1024, 768, "Introducing Clay Demo",
        FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);

    uint64_t clayRequiredMemory = Clay_MinMemorySize();
    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(
        clayRequiredMemory, malloc(clayRequiredMemory));

    Clay_Initialize(clayMemory, (Clay_Dimensions) {
       .width = GetScreenWidth(),
       .height = GetScreenHeight()
    }, (Clay_ErrorHandler) { HandleClayErrors });

    Font fonts[2];
    fonts[FONT_ID_BODY_16] = LoadFontEx("resources/Roboto-Regular.ttf", 48, 0, 400);
    fonts[FONT_ID_ORIENTAL_CHICKEN] = LoadFontEx("resources/Oriental-Chicken.ttf", 48, 0, 400);
    SetTextureFilter(fonts[FONT_ID_BODY_16].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    Clay_SetDebugModeEnabled(true);

    Arena *arena = arena_alloc(MegaByte(1));

    EntityList players = entity_list_init(arena, 64);
    EntityList tournaments = entity_list_init(arena, 64);

    String8 aldo     = str8_lit("Aldo");
    String8 giovanni = str8_lit("Giovanni");
    String8 giacomo  = str8_lit("Giacomo");

    String8 pingpong    = str8_lit("Ping Pong");
    String8 machiavelli = str8_lit("Machiavelli");
    String8 freccette   = str8_lit("Freccette");

    entity_list_add(&players, aldo);
    entity_list_add(&players, giovanni);
    entity_list_add(&players, giacomo);

    entity_list_add(&tournaments, pingpong);
    entity_list_add(&tournaments, machiavelli);
    entity_list_add(&tournaments, freccette);

    entity_list_register(&players, &tournaments, aldo, pingpong);


    // Initialization of global data
    data.frameArena = arena_alloc(MegaByte(1));
    data.selectedHeaderButton = 0;
    data.yOffset = 0;
    data.players = players;
    data.tournaments = tournaments;

    // ClayVideoDemo_Data data = ClayVideoDemo_Initialize();

    while (!WindowShouldClose())
    {
        Clay_SetLayoutDimensions((Clay_Dimensions) {
            .width = GetScreenWidth(),
            .height = GetScreenHeight()
        });

        Vector2 mousePosition = GetMousePosition();
        Vector2 scrollDelta = GetMouseWheelMoveV();
        Clay_SetPointerState(
            (Clay_Vector2) { mousePosition.x, mousePosition.y },
            IsMouseButtonDown(0)
        );
        Clay_UpdateScrollContainers(
            true,
            (Clay_Vector2) { scrollDelta.x, scrollDelta.y },
            GetFrameTime()
        );

        Clay_RenderCommandArray renderCommands = ClayVideoDemo_CreateLayout();

        BeginDrawing();
        ClearBackground(BLACK);
        Clay_Raylib_Render(renderCommands, fonts);
        EndDrawing();
    }

    Clay_Raylib_Close();
}
