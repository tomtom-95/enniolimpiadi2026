#include "arena.h"
#include "core.h"
#include "raylib/raylib.h"
#include "string.c"
#include "arena.c"
#include "players.c"
#include "string.h"

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
    String8 lucia    = str8_lit("Lucia");
    String8 antonia  = str8_lit("Antonia");
    String8 tommaso  = str8_lit("Tommaso");
    String8 emilia   = str8_lit("Emilia");
    String8 maya     = str8_lit("Maya");
    String8 marco    = str8_lit("Marco");
    String8 sofia    = str8_lit("Sofia");
    String8 luca     = str8_lit("Luca");
    String8 giulia   = str8_lit("Giulia");
    String8 matteo   = str8_lit("Matteo");
    String8 chiara   = str8_lit("Chiara");
    String8 andrea   = str8_lit("Andrea");
    String8 francesca = str8_lit("Francesca"); 

    String8 pingpong    = str8_lit("Ping Pong");
    String8 machiavelli = str8_lit("Machiavelli");
    String8 freccette   = str8_lit("Freccette");

    entity_list_add(&players, aldo);
    entity_list_add(&players, giovanni);
    entity_list_add(&players, giacomo);
    entity_list_add(&players, lucia);
    entity_list_add(&players, antonia);
    entity_list_add(&players, tommaso);
    entity_list_add(&players, emilia);
    entity_list_add(&players, maya);
    entity_list_add(&players, marco);
    entity_list_add(&players, sofia);
    entity_list_add(&players, luca);
    entity_list_add(&players, giulia);
    entity_list_add(&players, matteo);
    entity_list_add(&players, chiara);
    entity_list_add(&players, andrea);
    entity_list_add(&players, francesca);

    entity_list_add(&tournaments, pingpong);
    entity_list_add(&tournaments, machiavelli);
    entity_list_add(&tournaments, freccette);

    entity_list_register(&players, &tournaments, aldo, pingpong);
    entity_list_register(&players, &tournaments, giovanni, pingpong);
    entity_list_register(&players, &tournaments, giacomo, pingpong);
    entity_list_register(&players, &tournaments, lucia, pingpong);
    entity_list_register(&players, &tournaments, antonia, pingpong);
    entity_list_register(&players, &tournaments, tommaso, pingpong);
    entity_list_register(&players, &tournaments, emilia, pingpong);
    entity_list_register(&players, &tournaments, maya, pingpong);
    entity_list_register(&players, &tournaments, marco, pingpong);
    entity_list_register(&players, &tournaments, sofia, pingpong);
    entity_list_register(&players, &tournaments, luca, pingpong);
    entity_list_register(&players, &tournaments, giulia, pingpong);
    entity_list_register(&players, &tournaments, matteo, pingpong);
    entity_list_register(&players, &tournaments, chiara, pingpong);
    entity_list_register(&players, &tournaments, andrea, pingpong);
    entity_list_register(&players, &tournaments, francesca, pingpong);


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

        SetMouseCursor(MOUSE_CURSOR_DEFAULT);

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
