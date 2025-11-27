#include <stdlib.h>
#include <assert.h>

#include "clay.h"
#include "core.h"
#include "layout.h"

#include "arena.h"
#include "players.h"
#include "raylib/raylib.h"
#include "string.h"
#include "textbox.h"
#include "colors.c"

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

Clay_Sizing layoutExpand = {
    .width = CLAY_SIZING_GROW(0),
    .height = CLAY_SIZING_GROW(0)
};

ClayVideoDemo_Data data = {0};

///////////////////////////////////////////////////////////////////////////////
// Event Handlers

void
HandleHeaderButtonInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    Page page = (Page)userData;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        data.selectedHeaderButton = page;
    }
}

void
HandleEventElementInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    u8 event_idx = (u8)userData;
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        data.selectedTournamentIdx = event_idx;
    }
}

void
HandleGoBack(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData)
{
    SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        data.selectedTournamentIdx = 0;
    }
}


///////////////////////////////////////////////////////////////////////////////
// Layout function

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
        .clip = { .horizontal = true, .vertical = true, .childOffset = Clay_GetScrollOffset() },
        .backgroundColor = COLOR_OFF_WHITE
    }) {
        CLAY(CLAY_ID("TournamentHeader"), {
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
                .padding = { 12, 12, 8, 8 },
                .childGap = 8,
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
                u32 dummy = 0;
                Clay_OnHover(HandleGoBack, (intptr_t)dummy);
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
        CLAY(CLAY_ID("TournamentChart"), {
            .layout = {
                .sizing = layoutExpand,
                .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
            },
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
                if (data.eventNameInput.focused)
                {
                    TextInput_ProcessKeyboard(&data.eventNameInput);
                }

                // Render text input
                TextInput_Render(&data.eventNameInput, CLAY_STRING("EventNameInput"),
                    CLAY_STRING("Enter event name..."), data.fonts, FONT_ID_BODY_16);

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
                        Clay_OnHover(HandleEventElementInteraction, (intptr_t)idx);

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
            if (data.playerNameInput.focused) {
                TextInput_ProcessKeyboard(&data.playerNameInput);
            }

            // Render text input
            TextInput_Render(&data.playerNameInput, CLAY_STRING("PlayerNameInput"),
                CLAY_STRING("Enter player name..."), data.fonts, FONT_ID_BODY_16);

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

    // Handle text input clicks
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        // Handle event name input
        Clay_BoundingBox eventInputBox = Clay_GetElementData(Clay_GetElementId(CLAY_STRING("EventNameInput"))).boundingBox;
        TextInput_HandleClick(&data.eventNameInput, eventInputBox, data.fonts, FONT_ID_BODY_16);

        // Handle player name input
        Clay_BoundingBox playerInputBox = Clay_GetElementData(Clay_GetElementId(CLAY_STRING("PlayerNameInput"))).boundingBox;
        TextInput_HandleClick(&data.playerNameInput, playerInputBox, data.fonts, FONT_ID_BODY_16);
    }

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
