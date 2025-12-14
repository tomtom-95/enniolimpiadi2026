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


void
HandleClayErrors(Clay_ErrorData errorData)
{
    printf("%s", errorData.errorText.chars);
}

int
main(void)
{
    Clay_Raylib_Initialize(1280, 720, "Enniolimpiadi 2026",
        FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);

    // Maximize window after creation to get proper dimensions
    MaximizeWindow();

    // Increased with respect to the default
    Clay_SetMaxElementCount(16384);

    uint64_t clayRequiredMemory = Clay_MinMemorySize();
    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(clayRequiredMemory, malloc(clayRequiredMemory));

    // Initialize Clay with actual window dimensions after maximizing
    Clay_Initialize(clayMemory, (Clay_Dimensions) {
       .width = GetScreenWidth(),
       .height = GetScreenHeight()
    }, (Clay_ErrorHandler) { HandleClayErrors });

    Font fonts[2];
    fonts[FONT_ID_BODY_16] = LoadFontEx("resources/Roboto-Regular.ttf", 48, 0, 400);
    fonts[FONT_ID_PRESS_START_2P] = LoadFontEx("resources/Comic_Neue_Press_Start_2P/Press_Start_2P/PressStart2P-Regular.ttf", 48, 0, 400);
    SetTextureFilter(fonts[FONT_ID_BODY_16].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    Clay_SetDebugModeEnabled(true);

    Arena *arena = arena_alloc(MegaByte(10));

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
    String8 longname = str8_lit("sjdkfjslfjlkdsjfudskfjdskfjlskdjfkdlsjflkdjflkdj");

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
    entity_list_add(&players, longname);

    entity_list_add(&tournaments, pingpong);
    entity_list_add(&tournaments, machiavelli);
    entity_list_add(&tournaments, freccette);

    entity_list_register(&players, &tournaments, aldo,     pingpong);
    entity_list_register(&players, &tournaments, giovanni, pingpong);
    entity_list_register(&players, &tournaments, giacomo,  pingpong);
    entity_list_register(&players, &tournaments, lucia,    pingpong);
    entity_list_register(&players, &tournaments, antonia,  pingpong);
    entity_list_register(&players, &tournaments, tommaso,  pingpong);
    entity_list_register(&players, &tournaments, emilia,   pingpong);
    entity_list_register(&players, &tournaments, maya,     pingpong);

    entity_list_register(&players, &tournaments, marco,    pingpong);
    entity_list_register(&players, &tournaments, sofia,    pingpong);
    entity_list_register(&players, &tournaments, luca,     pingpong);
    entity_list_register(&players, &tournaments, giulia,   pingpong);
    entity_list_register(&players, &tournaments, matteo,   pingpong);
    entity_list_register(&players, &tournaments, chiara,   pingpong);
    entity_list_register(&players, &tournaments, longname, pingpong);
    // entity_list_register(&players, &tournaments, andrea, pingpong);
    // entity_list_register(&players, &tournaments, francesca, pingpong);


    // Initialization of global data
    data.arena = arena;
    data.frameArena = arena_alloc(MegaByte(1));
    data.selectedHeaderButton = 0;
    data.yOffset = 0;
    data.fonts = fonts;
    data.players = players;
    data.tournaments = tournaments;
    data.chartZoomLevel = 1.0f;

    while (!WindowShouldClose())
    {
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

    Clay_Raylib_Close();
}
