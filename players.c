#include <assert.h>

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

    // Allocate 2 nodes more than MAX_NUM_PLAYERS for the dummy head and tail nodes
    Entity *entities = push_array(arena, Entity, len + 2);

    u32 tail_idx = len + 1;

    // Link head and tail sentinel
    entities->nxt = len + 1;
    (entities + tail_idx)->prv = 0;

    EntityList entity_list = {.entities = entities, .first_free_idx = 1, .len = len};

    // Initialize the free list
    for (u32 i = 1; i < len + 1; ++i)
    {
        entity_list.entities[i].nxt = (i + 1) % tail_idx;
        entity_list.entities[i].phase = PHASE_REGISTRATION;
    }

    return entity_list;
}

u32
entity_list_find(EntityList *entity_list, String8 name)
{
    u32 idx_tail = entity_list->len + 1;

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
entity_list_count(EntityList *list)
{
    u32 idx_tail = list->len + 1;
    u32 idx = list->entities->nxt;

    u32 count = 0;
    while (idx != idx_tail)
    {
        idx = (list->entities + idx)->nxt;
        ++count;
    }

    return count;
}

u32
entity_list_add(EntityList *entity_list, String8 name)
{
    u32 idx_tail = entity_list->len + 1;

    // Make sure there is not already a player with this name
    assert(entity_list_find(entity_list, name) == idx_tail);

    u32 first_free_idx = entity_list->first_free_idx;

    // There are no elements in the free list
    // TODO: instead of hard crash must be implemented a mechanism for growing the entities array
    //       which is to move the backing array into a new allocation (2x bigger)
    assert(first_free_idx != 0);

    // Pointer to the node that will be used
    Entity *entity = entity_list->entities + first_free_idx;

    // Move the first free
    entity_list->first_free_idx = (entity_list->entities + first_free_idx)->nxt;

    // Add player on top of the list
    u32 idx = (entity_list->entities)->nxt;

    (entity_list->entities)->nxt = first_free_idx;

    entity->prv = 0;
    entity->nxt = idx;

    (entity_list->entities + idx)->prv = first_free_idx;

    // Fill the node with data
    entity->name.len = name.len;
    entity->name.str = name.str;

    // Player entity is not registered to anything
    entity->registrations = 0;

    // Default group size and advance count for tournaments
    entity->group_phase.group_size = 4;
    entity->group_phase.advance_per_group = 2;

    return first_free_idx;
}

void
entity_list_rename(EntityList *entity_list, u32 idx, String8 new_name)
{
    u32 idx_tail = entity_list->len + 1;

    // Check if another active entity already has this name
    u32 existing_idx = entity_list_find(entity_list, new_name);
    assert(existing_idx == idx_tail || existing_idx == idx);

    // No duplicate found, perform the rename
    Entity *entity = entity_list->entities + idx;
    entity->name = new_name;
}

void
entity_list_remove(EntityList *list1, EntityList *list2, String8 name)
{
    u32 idx_tail = list1->len + 1;

    u32 idx = entity_list_find(list1, name);
    assert(idx != idx_tail);

    Entity *entity = list1->entities + idx;

    s32 positions[64] = {0};
    u32 count = find_all_filled_slots((list1->entities + idx)->registrations, positions);
    for (u32 i = 0; i < count; ++i)
    {
        Entity *entity2 = list2->entities + positions[i] + 1;

        // Set a bit at position (idx - 1) to 0
        entity2->registrations &= ~(1ULL << ENTITY_IDX_TO_BIT(idx));

        // Update data relative of tournament distribution of players
        tournament_construct_groups(entity2);
        tournament_construct_bracket(entity2);
        tournament_populate_bracket_from_groups(entity2);
    }

    (list1->entities + entity->prv)->nxt = entity->nxt;
    (list1->entities + entity->nxt)->prv = entity->prv;

    entity->nxt = list1->first_free_idx;
    list1->first_free_idx = idx;
}

void
entity_list_register(EntityList *list1, EntityList *list2, String8 name1, String8 name2)
{
    u32 idx_tail1 = list1->len + 1;
    u32 idx_tail2 = list2->len + 1;

    u32 idx1 = entity_list_find(list1, name1);
    assert(idx1 != idx_tail1);

    u32 idx2 = entity_list_find(list2, name2);
    assert(idx2 != idx_tail2);

    Entity *entity1 = list1->entities + idx1;
    Entity *entity2 = list2->entities + idx2;

    // Set registration bits
    entity1->registrations |= (1ULL << ENTITY_IDX_TO_BIT(idx2));
    entity2->registrations |= (1ULL << ENTITY_IDX_TO_BIT(idx1));

    // Update data relative of tournament distribution of players
    tournament_construct_groups(entity2);
    tournament_construct_bracket(entity2);
    tournament_populate_bracket_from_groups(entity2);
}

void
entity_list_unregister(EntityList *list1, EntityList *list2, String8 name1, String8 name2)
{
    u32 idx_tail1 = list1->len + 1;
    u32 idx_tail2 = list2->len + 1;

    u32 idx1 = entity_list_find(list1, name1);
    assert(idx1 != idx_tail1);

    u32 idx2 = entity_list_find(list2, name2);
    assert(idx2 != idx_tail2);

    Entity *entity1 = list1->entities + idx1;
    Entity *entity2 = list2->entities + idx2;

    // Unset the registration bits
    entity1->registrations &= ~(1ULL << ENTITY_IDX_TO_BIT(idx2));
    entity2->registrations &= ~(1ULL << ENTITY_IDX_TO_BIT(idx1));

    // Update data relative of tournament distribution of players
    tournament_construct_groups(entity2);
    tournament_construct_bracket(entity2);
    tournament_populate_bracket_from_groups(entity2);
}

void
assign_medal(EntityList *players, EntityList *tournaments,
    String8 player_name, String8 tournament_name, MedalsEnum medal)
{
    u32 player_idx = entity_list_find(players, player_name);
    assert(player_idx != players->len + 1);

    u32 tournament_idx = entity_list_find(tournaments, tournament_name);
    assert(tournament_idx != tournaments->len + 1);

    (tournaments->entities + tournament_idx)->medals[medal] = player_idx;
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
        tournament->bracket[parent_pos] = BIT_TO_ENTITY_IDX(positions[player_idx]);
        player_idx++;
    }

    // Place remaining players at leaf positions (they fight in first round)
    u32 fighting_start = leaf_start + byes * 2;

    while (player_idx < num_players)
    {
        tournament->bracket[fighting_start] = BIT_TO_ENTITY_IDX(positions[player_idx]);
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
            u32 global_idx = BIT_TO_ENTITY_IDX(positions[player_i]);
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
 * @param tournament      The tournament entity
 * @param group_idx       The group index to calculate standings for
 * @param standings       Output array to store player indices sorted by rank
 * @param players_in_group Number of players in this group
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
