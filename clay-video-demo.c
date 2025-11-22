#include <stdlib.h>
#include "./clay.h"
#include "layout.h"

const int FONT_ID_BODY_16 = 0;
Clay_Color COLOR_WHITE = { 255, 255, 255, 255};

void
RenderHeaderButton(Clay_String text)
{
    CLAY_AUTO_ID({
        .layout = { .padding = { 16, 16, 8, 8 }},
        .backgroundColor = { 140, 140, 140, 255 },
        .cornerRadius = CLAY_CORNER_RADIUS(5)
    }) {
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 16,
            .textColor = { 255, 255, 255, 255 }
        }));
    }
}

void
RenderDropdownMenuItem(Clay_String text)
{
    CLAY_AUTO_ID({.layout = { .padding = CLAY_PADDING_ALL(16)}}) {
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 16,
            .textColor = { 255, 255, 255, 255 }
        }));
    }
}

void
HandleSidebarInteraction(Clay_ElementId elementId,
    Clay_PointerData pointerData, intptr_t userData)
{
    SidebarClickData *clickData = (SidebarClickData*)userData;
    // If this button was clicked
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME)
    {
        if (clickData->requestedDocumentIndex >= 0 && clickData->requestedDocumentIndex < documents.length)
        {
            // Select the corresponding document
            *clickData->selectedDocumentIndex = clickData->requestedDocumentIndex;
        }
    }
}

ClayVideoDemo_Data
ClayVideoDemo_Initialize()
{
    documents.documents[0] = (Document){ .title = CLAY_STRING("Alpine Skiing"), .contents = CLAY_STRING("Event: Alpine Skiing\n\nVenue: Cortina d'Ampezzo\nDate: February 6-22, 2026\n\nDisciplines:\n- Downhill (Men & Women)\n- Super-G (Men & Women)\n- Giant Slalom (Men & Women)\n- Slalom (Men & Women)\n- Combined (Men & Women)\n\nParticipants: 300 athletes from 70 countries\n\nSchedule:\nFeb 6-8: Men's & Women's Downhill\nFeb 9-10: Men's & Women's Super-G\nFeb 13-16: Men's & Women's Giant Slalom\nFeb 19-22: Men's & Women's Slalom\n\nMedal Count (Current):\n Gold: Austria (3), Switzerland (2), Norway (2)\n Silver: France (2), Italy (2), USA (1)\n Bronze: Germany (2), Canada (1), Sweden (1)\n\nTop Athletes:\n- Sofia Goggia (Italy) - 2 Gold\n- Marco Odermatt (Switzerland) - 1 Gold, 1 Silver\n- Mikaela Shiffrin (USA) - 1 Gold\n") };
    documents.documents[1] = (Document){ .title = CLAY_STRING("Ice Hockey"), .contents = CLAY_STRING("Event: Ice Hockey\n\nVenue: Milan Arena\nDate: February 5-28, 2026\n\nTournaments:\n- Men's Tournament (12 teams)\n- Women's Tournament (10 teams)\n\nParticipants: 440 athletes\n\nGroup Stage:\nGroup A: Canada, USA, Finland, Czech Republic\nGroup B: Sweden, Russia, Germany, Slovakia\nGroup C: Switzerland, Denmark, Norway, Latvia\n\nSchedule:\nFeb 5-15: Preliminary Round\nFeb 16-20: Quarter Finals\nFeb 23-24: Semi Finals\nFeb 28: Medal Games\n\nCurrent Standings (Preliminary):\nGroup A: Canada (3-0), USA (2-1)\nGroup B: Sweden (3-0), Russia (2-1)\nGroup C: Switzerland (2-1), Denmark (2-1)\n\nTop Scorers:\n- Connor McDavid (Canada) - 8 goals, 6 assists\n- Nathan MacKinnon (Canada) - 6 goals, 7 assists\n- Auston Matthews (USA) - 7 goals, 4 assists\n") };
    documents.documents[2] = (Document){ .title = CLAY_STRING("Figure Skating"), .contents = CLAY_STRING("Event: Figure Skating\n\nVenue: Milan Ice Palace\nDate: February 7-25, 2026\n\nCategories:\n- Men's Singles\n- Women's Singles\n- Pairs\n- Ice Dance\n- Team Event\n\nParticipants: 150 athletes from 35 countries\n\nSchedule:\nFeb 7-10: Team Event\nFeb 12-14: Men's Singles\nFeb 15-17: Women's Singles\nFeb 18-19: Pairs\nFeb 23-25: Ice Dance\n\nCurrent Medal Standings:\n Gold: Japan (2), USA (1), Russia (1)\n Silver: Russia (2), China (1)\n Bronze: Canada (1), France (1), South Korea (1)\n\nTop Performers:\n- Yuzuru Hanyu (Japan) - Men's Singles Champion\n- Kaori Sakamoto (Japan) - Women's Singles Champion\n- Madison Chock/Evan Bates (USA) - Ice Dance Leaders\n\nJudging Panel: 12 international judges\n") };
    documents.documents[3] = (Document){ .title = CLAY_STRING("Speed Skating"), .contents = CLAY_STRING("Event: Speed Skating\n\nVenue: Baselga di Pinè Oval\nDate: February 8-23, 2026\n\nDistances:\n- 500m, 1000m, 1500m (Men & Women)\n- 3000m, 5000m (Men & Women)\n- 10000m (Men), Mass Start (Men & Women)\n- Team Pursuit (Men & Women)\n\nParticipants: 200 athletes from 28 countries\n\nWorld Records Under Threat:\n- 500m: 33.61s (Men), 36.36s (Women)\n- 10000m: 12:33.86 (Men)\n\nCurrent Medal Leaders:\n Gold: Netherlands (5), Norway (3), China (2)\n Silver: Japan (3), Canada (2), USA (2)\n Bronze: South Korea (3), Russia (2)\n\nTop Athletes:\n- Nils van der Poel (Sweden) - 2 Gold\n- Irene Schouten (Netherlands) - 3 Gold\n- Joey Mantia (USA) - 1 Gold, 1 Silver\n") };
    documents.documents[4] = (Document){ .title = CLAY_STRING("Curling"), .contents = CLAY_STRING("Event: Curling\n\nVenue: Cortina Ice Stadium\nDate: February 2-24, 2026\n\nTournaments:\n- Men's Tournament (10 teams)\n- Women's Tournament (10 teams)\n- Mixed Doubles (16 teams)\n\nParticipants: 120 athletes\n\nRound Robin Standings:\nMen: Sweden (7-1), Canada (6-2), Switzerland (6-2)\nWomen: Switzerland (7-1), Canada (6-2), Japan (5-3)\nMixed: Italy (6-1), Canada (6-1), Norway (5-2)\n\nSchedule:\nFeb 2-18: Round Robin\nFeb 19-21: Semi-Finals\nFeb 23-24: Medal Games\n\nPast Champions:\n2022: Great Britain (Men), Great Britain (Women)\n2018: USA (Men), Sweden (Women)\n\nHost Advantage: Italy showing strong performance\nCrowd Favorite: Italian mixed doubles team\n") };

    ClayVideoDemo_Data data = {
        .frameArena = { .memory = (intptr_t)malloc(1024) }
    };
    return data;
}

Clay_RenderCommandArray 
ClayVideoDemo_CreateLayout(ClayVideoDemo_Data *data)
{
    data->frameArena.offset = 0;

    Clay_BeginLayout();

    // Build UI here
    CLAY(CLAY_ID("OuterContainer"), {
        .backgroundColor = {43, 41, 51, 255 },
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = layoutExpand,
            .padding = CLAY_PADDING_ALL(16),
            .childGap = 16
        }
    }) {
        // Child elements go inside braces
        CLAY(CLAY_ID("HeaderBar"), {
            .layout = {
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
            .backgroundColor = contentBackgroundColor,
            .cornerRadius = CLAY_CORNER_RADIUS(8)
        }) {
            // Olympiad header buttons
            CLAY(CLAY_ID("EventButton"), {
                .layout = { .padding = { 16, 16, 8, 8 }},
                .backgroundColor = {140, 140, 140, 255 },
                .cornerRadius = CLAY_CORNER_RADIUS(5)
            }) {
                CLAY_TEXT(CLAY_STRING("Add Event"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 16,
                    .textColor = { 255, 255, 255, 255 }
                }));

                bool eventMenuVisible =
                    Clay_PointerOver(Clay_GetElementId(CLAY_STRING("EventButton")))
                    ||
                    Clay_PointerOver(Clay_GetElementId(CLAY_STRING("EventMenu")));

                // Dropdown menu for event types
                if (eventMenuVisible) {
                    CLAY(CLAY_ID("EventMenu"), {
                        .floating = {
                            .attachTo = CLAY_ATTACH_TO_PARENT,
                            .attachPoints = {
                                .parent = CLAY_ATTACH_POINT_LEFT_BOTTOM
                            },
                        },
                        .layout = {
                            .padding = {0, 0, 8, 8 }
                        }
                    }) {
                        CLAY_AUTO_ID({
                            .layout = {
                                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                                .sizing = {
                                        .width = CLAY_SIZING_FIXED(200)
                                },
                            },
                            .backgroundColor = {40, 40, 40, 255 },
                            .cornerRadius = CLAY_CORNER_RADIUS(8)
                        }) {
                            // Olympic event categories
                            RenderDropdownMenuItem(CLAY_STRING("Winter Sport"));
                            RenderDropdownMenuItem(CLAY_STRING("Team Event"));
                            RenderDropdownMenuItem(CLAY_STRING("Individual Event"));
                        }
                    }
                }
            }
            RenderHeaderButton(CLAY_STRING("Add Athlete"));
            CLAY_AUTO_ID({ .layout = { .sizing = { CLAY_SIZING_GROW(0) }}}) {}
            RenderHeaderButton(CLAY_STRING("Schedule"));
            RenderHeaderButton(CLAY_STRING("Results"));
            RenderHeaderButton(CLAY_STRING("Reports"));
        }

        CLAY(CLAY_ID("LowerContent"), {
            .layout = { .sizing = layoutExpand, .childGap = 16 }
        }) {
            CLAY(CLAY_ID("Sidebar"), {
                .backgroundColor = contentBackgroundColor,
                .cornerRadius = CLAY_CORNER_RADIUS(8),
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .padding = CLAY_PADDING_ALL(16),
                    .childGap = 8,
                    .sizing = {
                        .width = CLAY_SIZING_FIXED(250),
                        .height = CLAY_SIZING_GROW(0)
                    }
                }
            }) {
                // Sidebar header
                CLAY_AUTO_ID({
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_GROW(0) },
                        .padding = { 8, 8, 8, 16 }
                    }
                }) {
                    CLAY_TEXT(CLAY_STRING("OLYMPIC EVENTS"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 18,
                        .textColor = { 180, 180, 180, 255 }
                    }));
                }

                // Event list
                for (int i = 0; i < documents.length; i++) {
                    Document document = documents.documents[i];
                    Clay_LayoutConfig sidebarButtonLayout = {
                        .sizing = { .width = CLAY_SIZING_GROW(0) },
                        .padding = CLAY_PADDING_ALL(16)
                    };

                    if (i == data->selectedDocumentIndex) {
                        CLAY_AUTO_ID({
                            .layout = sidebarButtonLayout,
                            .backgroundColor = {70, 130, 200, 255 },
                            .cornerRadius = CLAY_CORNER_RADIUS(8)
                        }) {
                            CLAY_TEXT(document.title, CLAY_TEXT_CONFIG({
                                .fontId = FONT_ID_BODY_16,
                                .fontSize = 20,
                                .textColor = { 255, 255, 255, 255 }
                            }));
                        }
                    } else {
                        SidebarClickData *clickData = (SidebarClickData *)(data->frameArena.memory + data->frameArena.offset);
                        *clickData = (SidebarClickData) { .requestedDocumentIndex = i, .selectedDocumentIndex = &data->selectedDocumentIndex };
                        data->frameArena.offset += sizeof(SidebarClickData);
                        CLAY_AUTO_ID({ .layout = sidebarButtonLayout, .backgroundColor = (Clay_Color) { 100, 100, 100, Clay_Hovered() ? 180 : 100 }, .cornerRadius = CLAY_CORNER_RADIUS(8) }) {
                            Clay_OnHover(HandleSidebarInteraction, (intptr_t)clickData);
                            CLAY_TEXT(document.title, CLAY_TEXT_CONFIG({
                                .fontId = FONT_ID_BODY_16,
                                .fontSize = 20,
                                .textColor = { 240, 240, 240, 255 }
                            }));
                        }
                    }
                }
            }

            CLAY(CLAY_ID("MainContent"), {
                .backgroundColor = contentBackgroundColor,
                .cornerRadius = CLAY_CORNER_RADIUS(8),
                .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() },
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .childGap = 20,
                    .padding = CLAY_PADDING_ALL(24),
                    .sizing = layoutExpand
                }
            }) {
                Document selectedDocument = documents.documents[data->selectedDocumentIndex];

                // Event title header
                CLAY_AUTO_ID({
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_GROW(0) },
                        .padding = { 0, 0, 0, 12 },
                        .childGap = 8
                    }
                }) {
                    CLAY_TEXT(selectedDocument.title, CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 32,
                        .textColor = {100, 180, 255, 255}
                    }));
                }

                // Divider line
                CLAY_AUTO_ID({
                    .layout = {
                        .sizing = {
                            .width = CLAY_SIZING_GROW(0),
                            .height = CLAY_SIZING_FIXED(2)
                        }
                    },
                    .backgroundColor = {80, 80, 80, 255}
                }) {}

                // Event details content
                CLAY_AUTO_ID({
                    .layout = {
                        .sizing = { .width = CLAY_SIZING_GROW(0) },
                        .padding = { 0, 0, 16, 0 }
                    }
                }) {
                    CLAY_TEXT(selectedDocument.contents, CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 18,
                        .textColor = {220, 220, 220, 255}
                    }));
                }
            }
        }
    }

    Clay_RenderCommandArray renderCommands = Clay_EndLayout();
    for (int32_t i = 0; i < renderCommands.length; i++) {
        Clay_RenderCommandArray_Get(&renderCommands, i)->boundingBox.y += data->yOffset;
    }
    return renderCommands;
}
