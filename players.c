#include <assert.h>
#include <stdio.h>

#include "arena.h"
#include "core.h"
#include "string.h"
#include "players.h"

/**
 * Find all filled slots in a bitmap and store their positions.
 *
 * Iterates through a 64-bit bitmap and extracts the position of each bit
 * that is set to 1, storing them in the positions array.
 *
 * @param bitmap    The 64-bit bitmap to scan for set bits
 * @param positions Output array to store the positions of set bits (0-63)
 * @return          The number of filled slots found
 */
u32
find_all_filled_slots(u64 bitmap, s32 positions[64])
{
    u32 count = 0;

    while (bitmap != 0)
    {
        s32 pos = __builtin_ctzll(bitmap);
        positions[count++] = pos;
        bitmap &= bitmap - 1;
    }

    // Return number of filled slots
    return count;
}

PlayersList
players_list_init(Arena *arena, u32 len)
{
    assert(len <= MAX_NUM_ENTITIES);

    Player *players = push_array(arena, Player, len);

    u32 idx_tail = len - 1;

    // Link head and tail sentinel
    Player *head = players;
    Player *tail = players + idx_tail;

    head->nxt = idx_tail;
    tail->prv = 0;

    // Initialize the free list
    PlayersList players_list = { .players = players, .first_free_idx = 1, .len = len };

    for (u32 i = 1; i < idx_tail; ++i)
    {
        players_list.players[i].nxt = i + 1;
    }

    return players_list;
}

EventsList
events_list_init(Arena *arena, u32 len)
{
    assert(len <= MAX_NUM_ENTITIES);

    Event *events = push_array(arena, Event, len);

    u32 idx_tail = len - 1;

    // Link head and tail sentinel
    Event *head = events;
    Event *tail = events + idx_tail;

    head->nxt = idx_tail;
    tail->prv = 0;

    // Initialize the free list
    EventsList events_list = { .events = events, .first_free_idx = 1, .len = len };

    for (u32 i = 1; i < idx_tail; ++i)
    {
        events_list.events[i].nxt = i + 1;
        events_list.events[i].phase = PHASE_REGISTRATION;
    }

    return events_list;
}

u32
players_list_find(PlayersList *list, String8 name)
{
    u32 idx_tail = list->len - 1;

    u32 idx = list->players->nxt;
    while (idx != idx_tail)
    {
        Player *player = list->players + idx;
        if (str8_cmp(name, player->name))
        {
            return idx;
        }

        idx = player->nxt;
    }

    return idx;
}

u32
events_list_find(EventsList *list, String8 name)
{
    u32 idx_tail = list->len - 1;

    u32 idx = list->events->nxt;
    while (idx != idx_tail)
    {
        Event *event = list->events + idx;
        if (str8_cmp(name, event->name))
        {
            return idx;
        }

        idx = event->nxt;
    }

    return idx;
}

u32
players_list_count(PlayersList *list)
{
    u32 idx_tail = list->len - 1;

    u32 idx = list->players->nxt;
    u32 count = 0;
    while (idx != idx_tail)
    {
        idx = (list->players + idx)->nxt;
        ++count;
    }

    return count;
}

u32
events_list_count(EventsList *list)
{
    u32 idx_tail = list->len - 1;

    u32 idx = list->events->nxt;
    u32 count = 0;
    while (idx != idx_tail)
    {
        idx = (list->events + idx)->nxt;
        ++count;
    }

    return count;
}

u32
players_list_add(PlayersList *list, String8 name)
{
    u32 idx_tail = list->len - 1;

    // Make sure there is not already a player with this name
    assert(players_list_find(list, name) == idx_tail);

    u32 idx_player = list->first_free_idx;
    assert(idx_player != idx_tail);

    Player *head = list->players;

    u32 idx_next = head->nxt;

    Player *player = head + idx_player;
    Player *next   = head + idx_next;

    // Move the first free
    list->first_free_idx = player->nxt;

    head->nxt   = idx_player;
    player->prv = 0;
    player->nxt = idx_next;
    next->prv   = idx_player;

    // Fill the node with data
    player->name.len = name.len;
    player->name.str = name.str;

    // Player is not registered to anything
    player->registrations = 0;

    return idx_player;
}

u32
events_list_add(EventsList *list, String8 name)
{
    u32 idx_tail = list->len - 1;

    // Make sure there is not already an event with this name
    assert(events_list_find(list, name) == idx_tail);

    u32 idx_event = list->first_free_idx;
    assert(idx_event != idx_tail);

    Event *head = list->events;

    u32 idx_next = head->nxt;

    Event *event = head + idx_event;
    Event *next  = head + idx_next;

    // Move the first free
    list->first_free_idx = event->nxt;

    head->nxt  = idx_event;
    event->prv = 0;
    event->nxt = idx_next;
    next->prv  = idx_event;

    // Fill the node with data
    event->name.len = name.len;
    event->name.str = name.str;

    // Event has no registered players
    event->registrations = 0;

    // Default group size and advance count for tournaments
    event->group_phase.group_size = 4;
    event->group_phase.advance_per_group = 2;

    return idx_event;
}

void
players_list_rename(PlayersList *list, u32 idx, String8 name)
{
    u32 idx_tail = list->len - 1;

    // Check if another active player already has this name
    u32 existing_idx = players_list_find(list, name);
    assert(existing_idx == idx_tail || existing_idx == idx);

    // No duplicate found, the player can be renamed
    Player *player = list->players + idx;
    player->name = name;
}

void
events_list_rename(EventsList *list, u32 idx, String8 name)
{
    u32 idx_tail = list->len - 1;

    // Check if another active event already has this name
    u32 existing_idx = events_list_find(list, name);
    assert(existing_idx == idx_tail || existing_idx == idx);

    // No duplicate found, the event can be renamed
    Event *event = list->events + idx;
    event->name = name;
}

void
players_list_remove(PlayersList *players, EventsList *events, String8 name)
{
    u32 idx_tail = players->len - 1;

    u32 idx = players_list_find(players, name);
    assert(idx != idx_tail);

    Player *head = players->players;
    Player *player = head + idx;

    u32 idx_prv = player->prv;
    u32 idx_nxt = player->nxt;

    Player *prev = head + idx_prv;
    Player *next = head + idx_nxt;

    // Unregister this player from all events they were registered to
    s32 positions[64] = {0};
    u32 count = find_all_filled_slots(player->registrations, positions);
    for (u32 i = 0; i < count; ++i)
    {
        Event *event = events->events + positions[i];

        // Clear the bit for this player
        event->registrations &= ~(1ULL << idx);

        // Update tournament bracket/groups
        tournament_construct_groups(event);
        tournament_construct_bracket(event);
        tournament_populate_bracket_from_groups(event);
    }

    prev->nxt = idx_nxt;
    next->prv = idx_prv;

    player->nxt = players->first_free_idx;
    players->first_free_idx = idx;
}

void
events_list_remove(EventsList *events, PlayersList *players, String8 name)
{
    u32 idx_tail = events->len - 1;

    u32 idx = events_list_find(events, name);
    assert(idx != idx_tail);

    Event *head = events->events;
    Event *event = head + idx;

    u32 idx_prv = event->prv;
    u32 idx_nxt = event->nxt;

    Event *prev = head + idx_prv;
    Event *next = head + idx_nxt;

    // Unregister all players from this event
    s32 positions[64] = {0};
    u32 count = find_all_filled_slots(event->registrations, positions);
    for (u32 i = 0; i < count; ++i)
    {
        Player *player = players->players + positions[i];

        // Clear the bit for this event
        player->registrations &= ~(1ULL << idx);
    }

    prev->nxt = idx_nxt;
    next->prv = idx_prv;

    event->nxt = events->first_free_idx;
    events->first_free_idx = idx;
}

void
register_player_to_event(PlayersList *players, EventsList *events, String8 player_name, String8 event_name)
{
    u32 idx_tail_players = players->len - 1;
    u32 idx_tail_events = events->len - 1;

    u32 player_idx = players_list_find(players, player_name);
    assert(player_idx != idx_tail_players);

    u32 event_idx = events_list_find(events, event_name);
    assert(event_idx != idx_tail_events);

    Player *player = players->players + player_idx;
    Event *event = events->events + event_idx;

    // Set registration bits
    player->registrations |= (1ULL << event_idx);
    event->registrations |= (1ULL << player_idx);

    // Update tournament bracket/groups
    tournament_construct_groups(event);
    tournament_construct_bracket(event);
    tournament_populate_bracket_from_groups(event);
}

void
unregister_player_from_event(PlayersList *players, EventsList *events, String8 player_name, String8 event_name)
{
    u32 idx_tail_players = players->len - 1;
    u32 idx_tail_events = events->len - 1;

    u32 player_idx = players_list_find(players, player_name);
    assert(player_idx != idx_tail_players);

    u32 event_idx = events_list_find(events, event_name);
    assert(event_idx != idx_tail_events);

    Player *player = players->players + player_idx;
    Event *event = events->events + event_idx;

    // Unset the registration bits
    player->registrations &= ~(1ULL << event_idx);
    event->registrations &= ~(1ULL << player_idx);

    // Update tournament bracket/groups
    tournament_construct_groups(event);
    tournament_construct_bracket(event);
    tournament_populate_bracket_from_groups(event);
}

/**
 * Construct the tournament bracket from registered players.
 *
 * Places player indices into a binary tree array (heap-style layout).
 * For non-power-of-2 player counts, some players receive byes.
 *
 * Example: 3 players (indices 3, 6, 8)
 *   - Player 3 at position 1 (bye to finals)
 *   - Player 6 at position 5, Player 8 at position 6 (fight first)
 *   - Winner of 5 vs 6 goes to position 2, fights position 1 for position 0
 *
 * @param event The event/tournament to construct bracket for
 */
void
tournament_construct_bracket(Event *event)
{
    // Clear the bracket
    MemoryZeroArray(event->bracket);

    // Get all registered players
    s32 positions[64];
    u32 num_players = find_all_filled_slots(event->registrations, positions);

    if (num_players == 0)
    {
        return;
    }

    // Find smallest power of 2 >= num_players
    u32 bracket_size = 1;
    while (bracket_size < num_players)
    {
        bracket_size *= 2;
    }

    u32 byes = bracket_size - num_players;

    // Determine leaf level for this bracket size
    // P=2: level 1, P=4: level 2, P=8: level 3, etc.
    u32 leaf_level = 0;
    u32 tmp = bracket_size;
    while (tmp > 1)
    {
        leaf_level++;
        tmp /= 2;
    }

    u32 leaf_start = (1 << leaf_level) - 1;  // 2^level - 1

    u32 player_idx = 0;

    // Place bye players at parent level (they skip the first round)
    for (u32 i = 0; i < byes && player_idx < num_players; ++i)
    {
        u32 leaf_pos = leaf_start + i * 2;
        u32 parent_pos = (leaf_pos - 1) / 2;
        event->bracket[parent_pos] = positions[player_idx];
        player_idx++;
    }

    // Place remaining players at leaf positions (they fight in first round)
    u32 fighting_start = leaf_start + byes * 2;

    while (player_idx < num_players)
    {
        event->bracket[fighting_start] = positions[player_idx];
        fighting_start++;
        player_idx++;
    }
}

void
tournament_construct_groups(Event *event)
{
    s32 positions[64];
    u32 num_players = find_all_filled_slots(event->registrations, positions);

    if (num_players == 0)
    {
        return;
    }

    u32 group_size = event->group_phase.group_size;
    u32 advance_per_group = event->group_phase.advance_per_group;

    MemoryZeroArray(event->group_phase.groups);
    MemorySet(event->group_phase.player_group, GROUP_NONE, MAX_NUM_ENTITIES + 1);
    MemoryZeroArray(event->group_phase.player_slot);

    MemoryZeroArray(event->group_phase.scores);
    MemoryZeroArray(event->group_phase.results);

    // Form groups, distributing players evenly when leftover is too small
    // Example: 14 players with group_size=4 -> (4, 4, 4, 2)
    // Example: 13 players with group_size=4 -> (5, 4, 4) instead of (4, 4, 4, 1)
    u32 num_full_groups = num_players / group_size;
    u32 leftover = num_players % group_size;

    u32 num_groups;
    b32 distribute_leftover = false;

    if (num_players < group_size)
    {
        // Not enough players for one full group - put everyone in one group
        num_groups = 1;
    }
    else if (leftover > 0 && leftover < 2)
    {
        // Leftover too small for meaningful group (need at least 2 for matches)
        // Distribute leftover players among existing groups
        num_groups = num_full_groups;
        distribute_leftover = true;
    }
    else
    {
        // Full groups + one extra group for leftovers (if any)
        num_groups = num_full_groups + (leftover > 0 ? 1 : 0);
    }

    event->group_phase.num_groups = num_groups;

    u32 player_i = 0;
    for (u32 g = 0; g < num_groups; g++)
    {
        u32 players_in_this_group;
        if (num_players < group_size)
        {
            // All players in one group
            players_in_this_group = num_players;
        }
        else if (distribute_leftover)
        {
            // Distribute leftover players among groups (first 'leftover' groups get +1)
            players_in_this_group = group_size + (g < leftover ? 1 : 0);
        }
        else if (g < num_full_groups)
        {
            // Full groups get exactly group_size players
            players_in_this_group = group_size;
        }
        else
        {
            // Last group gets the leftover players
            players_in_this_group = leftover;
        }

        for (u32 s = 0; s < players_in_this_group; s++)
        {
            u32 global_idx = positions[player_i];
            event->group_phase.groups[g][s] = global_idx;
            event->group_phase.player_group[global_idx] = g;  // 0-based group index
            event->group_phase.player_slot[global_idx] = s;
            player_i++;
        }
    }
}

/**
 * Calculate standings for a single group based on match results.
 *
 * Rankings are determined by:
 * 1. Points (win = 3, draw = 1, loss = 0)
 * 2. Goal difference
 * 3. Goals scored
 *
 * @param event             The event/tournament
 * @param group_idx         The group index to calculate standings for
 * @param standings         Output array to store player indices sorted by rank
 * @param players_in_group  Number of players in this group
 */
void
calculate_group_standings(Event *event, u32 group_idx, u8 *standings, u32 players_in_group)
{
    // Structure to hold player stats for sorting
    typedef struct {
        u8 player_idx;
        s32 points;
        s32 goal_diff;
        s32 goals_for;
    } PlayerStats;

    PlayerStats stats[MAX_GROUP_SIZE] = {0};

    // Calculate stats for each player in the group
    for (u32 slot = 0; slot < players_in_group; slot++)
    {
        u8 player_idx = event->group_phase.groups[group_idx][slot];
        stats[slot].player_idx = player_idx;

        // Calculate points and goals from match results
        for (u32 opponent = 0; opponent < players_in_group; opponent++)
        {
            if (slot == opponent) continue;

            MatchScore score = event->group_phase.scores[group_idx][slot][opponent];

            // Add goals scored by this player
            stats[slot].goals_for += score.row_score;
            stats[slot].goal_diff += score.row_score - score.col_score;

            // Calculate points from result
            if (score.row_score > score.col_score)
            {
                stats[slot].points += 3;  // Win
            }
            else if (score.row_score == score.col_score && (score.row_score > 0 || score.col_score > 0))
            {
                stats[slot].points += 1;  // Draw (only if match was played)
            }
        }
    }

    // Simple bubble sort by points, then goal diff, then goals scored
    for (u32 i = 0; i < players_in_group - 1; i++)
    {
        for (u32 j = 0; j < players_in_group - i - 1; j++)
        {
            bool swap = false;
            if (stats[j].points < stats[j + 1].points)
            {
                swap = true;
            }
            else if (stats[j].points == stats[j + 1].points)
            {
                if (stats[j].goal_diff < stats[j + 1].goal_diff)
                {
                    swap = true;
                }
                else if (stats[j].goal_diff == stats[j + 1].goal_diff)
                {
                    if (stats[j].goals_for < stats[j + 1].goals_for)
                    {
                        swap = true;
                    }
                }
            }

            if (swap)
            {
                PlayerStats temp = stats[j];
                stats[j] = stats[j + 1];
                stats[j + 1] = temp;
            }
        }
    }

    // Copy sorted player indices to output
    for (u32 i = 0; i < players_in_group; i++)
    {
        standings[i] = stats[i].player_idx;
    }
}

/**
 * Populate the tournament bracket from group phase qualifiers.
 *
 * Takes the top N players from each group (where N = advance_per_group)
 * and places them into the elimination bracket. Seeding is done to avoid
 * players from the same group meeting in early rounds when possible.
 *
 * @param event The event/tournament with completed group phase
 */
void
tournament_populate_bracket_from_groups(Event *event)
{
    // Clear the bracket
    MemoryZeroArray(event->group_phase.bracket);

    u32 num_groups = event->group_phase.num_groups;
    u32 advance_per_group = event->group_phase.advance_per_group;

    // Collect all qualifiers from each group
    u8 qualifiers[MAX_GROUPS * MAX_GROUP_SIZE];
    u32 num_qualifiers = 0;

    for (u32 g = 0; g < num_groups; g++)
    {
        // Count players in this group
        u32 players_in_group = 0;
        for (u32 slot = 0; slot < MAX_GROUP_SIZE; slot++)
        {
            if (event->group_phase.groups[g][slot] != 0)
            {
                players_in_group++;
            }
        }

        // Get standings for this group
        u8 standings[MAX_GROUP_SIZE];
        calculate_group_standings(event, g, standings, players_in_group);

        // Take top N players from this group
        u32 to_advance = advance_per_group;
        if (to_advance > players_in_group)
        {
            to_advance = players_in_group;
        }

        for (u32 i = 0; i < to_advance; i++)
        {
            qualifiers[num_qualifiers++] = standings[i];
        }
    }

    if (num_qualifiers == 0)
    {
        return;
    }

    // Find smallest power of 2 >= num_qualifiers
    u32 bracket_size = 1;
    while (bracket_size < num_qualifiers)
    {
        bracket_size *= 2;
    }

    u32 byes = bracket_size - num_qualifiers;

    // Determine leaf level for this bracket size
    u32 leaf_level = 0;
    u32 tmp = bracket_size;
    while (tmp > 1)
    {
        leaf_level++;
        tmp /= 2;
    }

    u32 leaf_start = (1 << leaf_level) - 1;

    u32 qualifier_idx = 0;

    // Place bye players at parent level (they skip the first round)
    for (u32 i = 0; i < byes && qualifier_idx < num_qualifiers; ++i)
    {
        u32 leaf_pos = leaf_start + i * 2;
        u32 parent_pos = (leaf_pos - 1) / 2;
        event->group_phase.bracket[parent_pos] = qualifiers[qualifier_idx];
        qualifier_idx++;
    }

    // Place remaining players at leaf positions (they fight in first round)
    u32 fighting_start = leaf_start + byes * 2;

    while (qualifier_idx < num_qualifiers)
    {
        event->group_phase.bracket[fighting_start] = qualifiers[qualifier_idx];
        fighting_start++;
        qualifier_idx++;
    }
}

// ============================================================================
// Save/Load Implementation
// ============================================================================

#define SAVE_VERSION 2
#define SAVE_MAGIC 0x454E4E49  // "ENNI"

typedef struct SaveHeader {
    u32 magic;
    u32 version;
    u32 players_len;
    u32 players_first_free_idx;
    u32 tournaments_len;
    u32 tournaments_first_free_idx;
} SaveHeader;

/**
 * Save the olympiad state to a binary file.
 *
 * Writes everything to a buffer first, then fwrite once.
 */
b32
olympiad_save(PlayersList *players, EventsList *events)
{
    // Calculate max buffer size needed
    // Player: nxt(4) + prv(4) + name_len(4) + name(MAX_STRING_SIZE) + registrations(8)
    // Event: nxt(4) + prv(4) + name_len(4) + name(MAX_STRING_SIZE) + registrations(8)
    //        + phase(1) + format(1) + bracket(BRACKET_SIZE) + group_phase
    u64 player_max_size = 4 + 4 + 4 + MAX_STRING_SIZE + 8;
    u64 event_max_size = 4 + 4 + 4 + MAX_STRING_SIZE + 8 + 1 + 1 + BRACKET_SIZE + sizeof(GroupPhase);
    u64 players_total = players->len;
    u64 events_total = events->len;
    u64 max_size = sizeof(SaveHeader) + players_total * player_max_size + events_total * event_max_size;

    Temp temp = scratch_get(0, 0);

    u8 *buffer = push_array(temp.arena, u8, max_size);
    u64 offset = 0;

    // Write header
    SaveHeader header = {
        .magic = SAVE_MAGIC,
        .version = SAVE_VERSION,
        .players_len = players->len,
        .players_first_free_idx = players->first_free_idx,
        .tournaments_len = events->len,
        .tournaments_first_free_idx = events->first_free_idx,
    };
    MemoryCopy(buffer + offset, &header, sizeof(SaveHeader));
    offset += sizeof(SaveHeader);

    // Write players
    for (u32 i = 0; i < players_total; ++i)
    {
        Player *p = &players->players[i];

        // nxt, prv
        MemoryCopy(buffer + offset, &p->nxt, sizeof(u32)); offset += sizeof(u32);
        MemoryCopy(buffer + offset, &p->prv, sizeof(u32)); offset += sizeof(u32);

        // name_len + name
        u32 name_len = (u32)p->name.len;
        MemoryCopy(buffer + offset, &name_len, sizeof(u32)); offset += sizeof(u32);
        if (name_len > 0)
        {
            MemoryCopy(buffer + offset, p->name.str, name_len);
            offset += name_len;
        }

        // registrations
        MemoryCopy(buffer + offset, &p->registrations, sizeof(u64)); offset += sizeof(u64);
    }

    // Write events
    for (u32 i = 0; i < events_total; ++i)
    {
        Event *e = &events->events[i];

        // nxt, prv
        MemoryCopy(buffer + offset, &e->nxt, sizeof(u32)); offset += sizeof(u32);
        MemoryCopy(buffer + offset, &e->prv, sizeof(u32)); offset += sizeof(u32);

        // name_len + name
        u32 name_len = (u32)e->name.len;
        MemoryCopy(buffer + offset, &name_len, sizeof(u32)); offset += sizeof(u32);
        if (name_len > 0)
        {
            MemoryCopy(buffer + offset, e->name.str, name_len);
            offset += name_len;
        }

        // registrations
        MemoryCopy(buffer + offset, &e->registrations, sizeof(u64)); offset += sizeof(u64);

        // phase, format
        u8 phase = (u8)e->phase;
        u8 format = (u8)e->format;
        buffer[offset++] = phase;
        buffer[offset++] = format;

        // bracket
        MemoryCopy(buffer + offset, e->bracket, BRACKET_SIZE); offset += BRACKET_SIZE;

        // group_phase
        MemoryCopy(buffer + offset, &e->group_phase, sizeof(GroupPhase)); offset += sizeof(GroupPhase);
    }

    // Write to temp file first (atomic write pattern)
    FILE *f = fopen(olympiad_temp_file, "wb");
    if (!f)
    {
        printf("Failed to open temp file for writing\n");
        return false;
    }

    if (fwrite(buffer, 1, offset, f) != offset)
    {
        printf("Failed to write temp file\n");
        fclose(f);
        return false;
    }

    scratch_release(temp);
    fclose(f);

    // Atomic rename: remove old file (required on Windows), then rename temp to final
    remove(olympiad_save_file);
    if (rename(olympiad_temp_file, olympiad_save_file) != 0)
    {
        printf("Failed to rename temp file to save file\n");
        return false;
    }

    printf("Saved to olympiad.sav (%llu bytes)\n", offset);

    return true;
}

/**
 * Load the olympiad state from a binary file.
 *
 * Reads entire file with one fread, then parses the buffer.
 */
b32
olympiad_load(Arena *arena, PlayersList *players, EventsList *events)
{
    FILE *f = fopen(olympiad_save_file, "rb");
    if (!f)
    {
        printf("Cannot open olympiad.sav\n");
        return false;
    }

    // Get file size
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (file_size <= 0)
    {
        printf("Invalid file size\n");
        fclose(f);
        return false;
    }

    Temp temp = scratch_get(0, 0);

    // Single fread into buffer
    u8 *buffer = push_array(temp.arena, u8, file_size);
    if (fread(buffer, 1, file_size, f) != (size_t)file_size)
    {
        printf("Failed to read save file\n");
        fclose(f);
        return false;
    }
    fclose(f);

    u64 offset = 0;

    // Read header
    SaveHeader header;
    MemoryCopy(&header, buffer + offset, sizeof(SaveHeader));
    offset += sizeof(SaveHeader);

    if (header.magic != SAVE_MAGIC)
    {
        printf("Invalid save file (bad magic)\n");
        return false;
    }

    if (header.version != SAVE_VERSION)
    {
        printf("Version mismatch: file=%u, expected=%u\n", header.version, SAVE_VERSION);
        return false;
    }

    if (header.players_len != players->len || header.tournaments_len != events->len)
    {
        printf("List length mismatch\n");
        return false;
    }

    // Load players
    players->first_free_idx = header.players_first_free_idx;
    u32 players_total = players->len;

    for (u32 i = 0; i < players_total; ++i)
    {
        Player *p = &players->players[i];

        // nxt, prv
        MemoryCopy(&p->nxt, buffer + offset, sizeof(u32)); offset += sizeof(u32);
        MemoryCopy(&p->prv, buffer + offset, sizeof(u32)); offset += sizeof(u32);

        // name_len + name
        u32 name_len = 0;
        MemoryCopy(&name_len, buffer + offset, sizeof(u32)); offset += sizeof(u32);
        if (name_len > 0)
        {
            u8 *name_str = push_array(arena, u8, name_len);
            MemoryCopy(name_str, buffer + offset, name_len);
            offset += name_len;
            p->name.str = name_str;
            p->name.len = name_len;
        }
        else
        {
            p->name.str = NULL;
            p->name.len = 0;
        }

        // registrations
        MemoryCopy(&p->registrations, buffer + offset, sizeof(u64)); offset += sizeof(u64);
    }

    // Load events
    events->first_free_idx = header.tournaments_first_free_idx;
    u32 events_total = events->len;

    for (u32 i = 0; i < events_total; ++i)
    {
        Event *e = &events->events[i];

        // nxt, prv
        MemoryCopy(&e->nxt, buffer + offset, sizeof(u32)); offset += sizeof(u32);
        MemoryCopy(&e->prv, buffer + offset, sizeof(u32)); offset += sizeof(u32);

        // name_len + name
        u32 name_len = 0;
        MemoryCopy(&name_len, buffer + offset, sizeof(u32)); offset += sizeof(u32);
        if (name_len > 0)
        {
            u8 *name_str = push_array(arena, u8, name_len);
            MemoryCopy(name_str, buffer + offset, name_len);
            offset += name_len;
            e->name.str = name_str;
            e->name.len = name_len;
        }
        else
        {
            e->name.str = NULL;
            e->name.len = 0;
        }

        // registrations
        MemoryCopy(&e->registrations, buffer + offset, sizeof(u64)); offset += sizeof(u64);

        // phase, format
        e->phase = (TournamentPhase)buffer[offset++];
        e->format = (TournamentFormat)buffer[offset++];

        // bracket
        MemoryCopy(e->bracket, buffer + offset, BRACKET_SIZE); offset += BRACKET_SIZE;

        // group_phase
        MemoryCopy(&e->group_phase, buffer + offset, sizeof(GroupPhase)); offset += sizeof(GroupPhase);
    }

    scratch_release(temp);

    printf("Loaded from olympiad.sav (%llu bytes)\n", offset);

    return true;
}
