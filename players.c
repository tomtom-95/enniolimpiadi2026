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

EntityList
entity_list_init(Arena *arena, u32 len)
{
    assert(len <= MAX_NUM_ENTITIES);

    Entity *entities = push_array(arena, Entity, len);

    u32 idx_tail = len - 1;

    // Link head and tail sentinel
    Entity *head = entities;
    Entity *tail = entities + idx_tail;

    head->nxt = idx_tail;
    tail->prv = 0;

    // Initialize the free list
    EntityList entity_list = { .entities = entities, .first_free_idx = 1, .len = len };

    for (u32 i = 1; i < idx_tail; ++i)
    {
        entity_list.entities[i].nxt = i + 1;
        entity_list.entities[i].phase = PHASE_REGISTRATION;
    }

    return entity_list;
}

u32
entity_list_find(EntityList *entity_list, String8 name)
{
    u32 idx_tail = entity_list->len - 1;

    u32 idx = entity_list->entities->nxt;
    while (idx != idx_tail)
    {
        Entity *entity = entity_list->entities + idx;
        if (str8_cmp(name, entity->name))
        {
            return idx;
        }

        idx = entity->nxt;
    }

    return idx;
}

u32
entity_list_count(EntityList *entity_list)
{
    u32 idx_tail = entity_list->len - 1;

    u32 idx = entity_list->entities->nxt;
    u32 count = 0;
    while (idx != idx_tail)
    {
        idx = (entity_list->entities + idx)->nxt;
        ++count;
    }

    return count;
}

u32
entity_list_add(EntityList *entity_list, String8 name)
{
    u32 idx_tail = entity_list->len - 1;

    // Make sure there is not already a player with this name
    assert(entity_list_find(entity_list, name) == idx_tail);

    u32 idx_entity = entity_list->first_free_idx;
    assert(idx_entity != idx_tail);

    Entity *head = entity_list->entities;

    u32 idx_next = head->nxt;

    Entity *entity = head + idx_entity;
    Entity *next   = head + idx_next;

    // Move the first free
    entity_list->first_free_idx = entity->nxt;

    head->nxt   = idx_entity;
    entity->prv = 0;
    entity->nxt = idx_next;
    next->prv   = idx_entity;

    // Fill the node with data
    entity->name.len = name.len;
    entity->name.str = name.str;

    // Player entity is not registered to anything
    entity->registrations = 0;

    // Default group size and advance count for tournaments
    entity->group_phase.group_size = 4;
    entity->group_phase.advance_per_group = 2;

    return idx_entity;
}

void
entity_list_rename(EntityList *entity_list, u32 idx, String8 name)
{
    u32 idx_tail = entity_list->len - 1;

    // Check if another active entity already has this name
    u32 existing_idx = entity_list_find(entity_list, name);
    assert(existing_idx == idx_tail || existing_idx == idx);

    // No duplicate found, the entity can be renamed
    Entity *entity = entity_list->entities + idx;
    entity->name = name;
}

void
entity_list_remove(EntityList *list1, EntityList *list2, String8 name)
{
    u32 idx_tail = list1->len - 1;

    u32 idx = entity_list_find(list1, name);
    assert(idx != idx_tail);

    Entity *head1 = list1->entities;
    Entity *entity = head1 + idx;

    u32 idx_prv = entity->prv;
    u32 idx_nxt = entity->nxt;

    Entity *prev = head1 + idx_prv;
    Entity *next = head1 + idx_nxt;

    s32 positions[64] = {0};
    u32 count = find_all_filled_slots((list1->entities + idx)->registrations, positions);
    for (u32 i = 0; i < count; ++i)
    {
        Entity *entity2 = list2->entities + positions[i];

        // Set a bit at position idx to 0
        entity2->registrations &= ~(1ULL << idx);

        // Update data relative of tournament distribution of players
        tournament_construct_groups(entity2);
        tournament_construct_bracket(entity2);
        tournament_populate_bracket_from_groups(entity2);
    }

    prev->nxt = idx_nxt;
    next->prv = idx_prv;

    entity->nxt = list1->first_free_idx;
    list1->first_free_idx = idx;
}

void
entity_list_register(EntityList *list1, EntityList *list2, String8 name1, String8 name2)
{
    u32 idx_tail1 = list1->len - 1;
    u32 idx_tail2 = list2->len - 1;

    u32 idx1 = entity_list_find(list1, name1);
    assert(idx1 != idx_tail1);

    u32 idx2 = entity_list_find(list2, name2);
    assert(idx2 != idx_tail2);

    Entity *entity1 = list1->entities + idx1;
    Entity *entity2 = list2->entities + idx2;

    // Set registration bits
    entity1->registrations |= (1ULL << idx2);
    entity2->registrations |= (1ULL << idx1);

    // Update data relative of tournament distribution of players
    tournament_construct_groups(entity2);
    tournament_construct_bracket(entity2);
    tournament_populate_bracket_from_groups(entity2);
}

void
entity_list_unregister(EntityList *list1, EntityList *list2, String8 name1, String8 name2)
{
    u32 idx_tail1 = list1->len - 1;
    u32 idx_tail2 = list2->len - 1;

    u32 idx1 = entity_list_find(list1, name1);
    assert(idx1 != idx_tail1);

    u32 idx2 = entity_list_find(list2, name2);
    assert(idx2 != idx_tail2);

    Entity *entity1 = list1->entities + idx1;
    Entity *entity2 = list2->entities + idx2;

    // Unset the registration bits
    entity1->registrations &= ~(1ULL << idx2);
    entity2->registrations &= ~(1ULL << idx1);

    // Update data relative of tournament distribution of players
    tournament_construct_groups(entity2);
    tournament_construct_bracket(entity2);
    tournament_populate_bracket_from_groups(entity2);
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
 * @param tournament The tournament entity to construct bracket for
 */
void
tournament_construct_bracket(Entity *tournament)
{
    // Clear the bracket
    MemoryZeroArray(tournament->bracket);

    // Get all registered players
    s32 positions[64];
    u32 num_players = find_all_filled_slots(tournament->registrations, positions);

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
        tournament->bracket[parent_pos] = positions[player_idx];
        player_idx++;
    }

    // Place remaining players at leaf positions (they fight in first round)
    u32 fighting_start = leaf_start + byes * 2;

    while (player_idx < num_players)
    {
        tournament->bracket[fighting_start] = positions[player_idx];
        fighting_start++;
        player_idx++;
    }
}

void
tournament_construct_groups(Entity *tournament)
{
    s32 positions[64];
    u32 num_players = find_all_filled_slots(tournament->registrations, positions);

    if (num_players == 0)
    {
        return;
    }

    u32 group_size = tournament->group_phase.group_size;
    u32 advance_per_group = tournament->group_phase.advance_per_group;

    MemoryZeroArray(tournament->group_phase.groups);
    MemorySet(tournament->group_phase.player_group, GROUP_NONE, MAX_NUM_ENTITIES + 1);
    MemoryZeroArray(tournament->group_phase.player_slot);

    MemoryZeroArray(tournament->group_phase.scores);
    MemoryZeroArray(tournament->group_phase.results);

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

    tournament->group_phase.num_groups = num_groups;

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
            tournament->group_phase.groups[g][s] = global_idx;
            tournament->group_phase.player_group[global_idx] = g;  // 0-based group index
            tournament->group_phase.player_slot[global_idx] = s;
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
 * @param tournament        The tournament entity
 * @param group_idx         The group index to calculate standings for
 * @param standings         Output array to store player indices sorted by rank
 * @param players_in_group  Number of players in this group
 */
static void
calculate_group_standings(Entity *tournament, u32 group_idx, u8 *standings, u32 players_in_group)
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
        u8 player_idx = tournament->group_phase.groups[group_idx][slot];
        stats[slot].player_idx = player_idx;

        // Calculate points and goals from match results
        for (u32 opponent = 0; opponent < players_in_group; opponent++)
        {
            if (slot == opponent) continue;

            MatchScore score = tournament->group_phase.scores[group_idx][slot][opponent];

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
 * @param tournament The tournament entity with completed group phase
 */
void
tournament_populate_bracket_from_groups(Entity *tournament)
{
    // Clear the bracket
    MemoryZeroArray(tournament->group_phase.bracket);

    u32 num_groups = tournament->group_phase.num_groups;
    u32 advance_per_group = tournament->group_phase.advance_per_group;

    // Collect all qualifiers from each group
    u8 qualifiers[MAX_GROUPS * MAX_GROUP_SIZE];
    u32 num_qualifiers = 0;

    for (u32 g = 0; g < num_groups; g++)
    {
        // Count players in this group
        u32 players_in_group = 0;
        for (u32 slot = 0; slot < MAX_GROUP_SIZE; slot++)
        {
            if (tournament->group_phase.groups[g][slot] != 0)
            {
                players_in_group++;
            }
        }

        // Get standings for this group
        u8 standings[MAX_GROUP_SIZE];
        calculate_group_standings(tournament, g, standings, players_in_group);

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
        tournament->group_phase.bracket[parent_pos] = qualifiers[qualifier_idx];
        qualifier_idx++;
    }

    // Place remaining players at leaf positions (they fight in first round)
    u32 fighting_start = leaf_start + byes * 2;

    while (qualifier_idx < num_qualifiers)
    {
        tournament->group_phase.bracket[fighting_start] = qualifiers[qualifier_idx];
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
olympiad_save(Arena *arena, EntityList *players, EntityList *tournaments)
{
    // Calculate max buffer size needed
    // Header + (len+2) entities for both lists
    // Each entity: nxt(4) + prv(4) + name_len(4) + name(MAX_STRING_SIZE) + registrations(8)
    //              + medals(3) + phase(1) + format(1) + bracket(BRACKET_SIZE) + group_phase
    u64 entity_max_size = 4 + 4 + 4 + MAX_STRING_SIZE + 8 + 3 + 1 + 1 + BRACKET_SIZE + sizeof(GroupPhase);
    u64 players_total = players->len;
    u64 tournaments_total = tournaments->len;
    u64 max_size = sizeof(SaveHeader) + (players_total + tournaments_total) * entity_max_size;

    u8 *buffer = push_array(arena, u8, max_size);
    u64 offset = 0;

    // Write header
    SaveHeader header = {
        .magic = SAVE_MAGIC,
        .version = SAVE_VERSION,
        .players_len = players->len,
        .players_first_free_idx = players->first_free_idx,
        .tournaments_len = tournaments->len,
        .tournaments_first_free_idx = tournaments->first_free_idx,
    };
    MemoryCopy(buffer + offset, &header, sizeof(SaveHeader));
    offset += sizeof(SaveHeader);

    // Write players entities
    for (u32 i = 0; i < players_total; ++i)
    {
        Entity *e = &players->entities[i];

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

        // medals
        MemoryCopy(buffer + offset, e->medals, 3); offset += 3;

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

    // Write tournaments entities
    for (u32 i = 0; i < tournaments_total; ++i)
    {
        Entity *e = &tournaments->entities[i];

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

        // medals
        MemoryCopy(buffer + offset, e->medals, 3); offset += 3;

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

    // Single fwrite
    FILE *f = fopen("olympiad.sav", "wb");
    if (!f)
    {
        printf("Failed to open olympiad.sav for writing\n");
        return false;
    }

    if (fwrite(buffer, 1, offset, f) != offset)
    {
        printf("Failed to write save file\n");
        fclose(f);
        return false;
    }

    fclose(f);
    printf("Saved to olympiad.sav (%llu bytes)\n", offset);
    return true;
}

/**
 * Load the olympiad state from a binary file.
 *
 * Reads entire file with one fread, then parses the buffer.
 */
b32
olympiad_load(Arena *arena, EntityList *players, EntityList *tournaments)
{
    FILE *f = fopen("olympiad.sav", "rb");
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

    // Single fread into buffer
    u8 *buffer = push_array(arena, u8, file_size);
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

    if (header.players_len != players->len || header.tournaments_len != tournaments->len)
    {
        printf("EntityList length mismatch\n");
        return false;
    }

    // Load players
    players->first_free_idx = header.players_first_free_idx;
    u32 players_total = players->len;

    for (u32 i = 0; i < players_total; ++i)
    {
        Entity *e = &players->entities[i];

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

        // medals
        MemoryCopy(e->medals, buffer + offset, 3); offset += 3;

        // phase, format
        e->phase = (TournamentPhase)buffer[offset++];
        e->format = (TournamentFormat)buffer[offset++];

        // bracket
        MemoryCopy(e->bracket, buffer + offset, BRACKET_SIZE); offset += BRACKET_SIZE;

        // group_phase
        MemoryCopy(&e->group_phase, buffer + offset, sizeof(GroupPhase)); offset += sizeof(GroupPhase);
    }

    // Load tournaments
    tournaments->first_free_idx = header.tournaments_first_free_idx;
    u32 tournaments_total = tournaments->len;

    for (u32 i = 0; i < tournaments_total; ++i)
    {
        Entity *e = &tournaments->entities[i];

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

        // medals
        MemoryCopy(e->medals, buffer + offset, 3); offset += 3;

        // phase, format
        e->phase = (TournamentPhase)buffer[offset++];
        e->format = (TournamentFormat)buffer[offset++];

        // bracket
        MemoryCopy(e->bracket, buffer + offset, BRACKET_SIZE); offset += BRACKET_SIZE;

        // group_phase
        MemoryCopy(&e->group_phase, buffer + offset, sizeof(GroupPhase)); offset += sizeof(GroupPhase);
    }

    printf("Loaded from olympiad.sav (%llu bytes)\n", offset);
    return true;
}
