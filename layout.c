#include <stdlib.h>
#include <assert.h>

#include "clay.h"
#include "core.h"
#include "layout.h"

#include "arena.h"
#include "players.h"
#include "raylib/raylib.h"
#include "string.h"

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
// Globals

const int FONT_ID_BODY_16 = 0;
const int FONT_ID_ORIENTAL_CHICKEN   = 1;

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

Clay_Sizing layoutExpand = {
    .width = CLAY_SIZING_GROW(0),
    .height = CLAY_SIZING_GROW(0)
};

ClayVideoDemo_Data data = {0};

///////////////////////////////////////////////////////////////////////////////

void
HandleHeaderButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    Page page = (Page)userData;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        data.selectedHeaderButton = page;
    }
}

void
HandleEventElementInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    u8 event_idx = (u8)userData;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        data.selectedTournamentIdx = event_idx;
    }
}

void
HandleGoBack(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        data.selectedTournamentIdx = 0;
    }
}

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
            .padding = { .top = 8, .bottom = 8 },
            .childGap = 8
        }
    }) {
        CLAY_TEXT(CLAY_STRING("Edit"), CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 18,
            .textColor = COLOR_BLUE
        }));
        CLAY_TEXT(CLAY_STRING("Delete"), CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 18,
            .textColor = COLOR_RED
        }));
    }
}

void
RenderEvents(void)
{
    if (data.selectedTournamentIdx != 0)
    {
        CLAY(CLAY_ID("Tournament"), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = layoutExpand,
                .padding = { 16, 16, 0, 0 },
                .childGap = 16,
                .childAlignment = {
                    .y = CLAY_ALIGN_Y_TOP
                }
            },
            .backgroundColor = COLOR_OFF_WHITE
        }) {

            CLAY(CLAY_ID("GoBackWrapper"), {

            }) {
                u32 dummy = 0;
                Clay_OnHover(HandleGoBack, (intptr_t)dummy);
                CLAY_TEXT(CLAY_STRING("<- Go back"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 18,
                    .textColor = stringColor
                }));
            }
        }
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
                .childGap = 1
            },
            .cornerRadius = CLAY_CORNER_RADIUS(8)
        }) {
            CLAY(CLAY_ID("EventsHeader"), {
                .layout = {
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                    .sizing = {.height = CLAY_SIZING_FIT(0) , .width = CLAY_SIZING_GROW(0)},
                    .padding = {.left = 8, .right = 8},
                },
            }) {
                RenderEventsHeaderElement(CLAY_STRING("Event Name"));
                RenderEventsHeaderElement(CLAY_STRING("Players"));
                RenderEventsHeaderElement(CLAY_STRING("Actions"));
            }

            u32 idx_tail = data.tournaments.len + 1;
            u32 idx = (data.tournaments.entities)->nxt;
            while (idx != idx_tail)
            {
                Clay_OnHover(HandleEventElementInteraction, (intptr_t)idx);

                Entity *tournament = data.tournaments.entities + idx;

                CLAY_AUTO_ID({
                    .layout = {
                        .layoutDirection = CLAY_LEFT_TO_RIGHT,
                        .sizing = {.height = CLAY_SIZING_FIT(0) , .width = CLAY_SIZING_GROW(0)},
                        .padding = {.left = 8, .right = 8}
                    },
                    .backgroundColor = Clay_Hovered() ? eventElementHoverColor : eventElementColor
                }) {
                    Clay_String event_string = str8_to_clay(tournament->name);
                
                    // Render event name
                    RenderEventElement(event_string);

                    // Render number of players registered to that event
                    s32 positions[64];
                    u32 count = find_all_filled_slots((data.tournaments.entities + idx)->registrations, positions);
                    RenderEventElement(str8_to_clay(str8_u32(data.frameArena, count)));

                    // Render Edit/Delete
                    RenderEventsActionsButtons();
                }

                idx = tournament->nxt;
            }
        }
    }
}

void
RenderPlayers(void)
{
    CLAY(CLAY_ID("Players"), {
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
ClayVideoDemo_CreateLayout(void)
{
    data.frameArena->pos = ARENA_HEADER_SIZE;
    // data.counterHeaderButton = 0;

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
