# Tournament System Documentation

This document explains how the tournament bracket layout works and how tournament progress is tracked in the application.

## Overview

The tournament system implements a **single-elimination bracket** where players compete in matches, and winners advance to the next round until a final champion is determined.

---

## Data Structures

### Tournament State

Each tournament has a state that controls its behavior:

```c
typedef enum TournamentState {
    TOURNAMENT_REGISTRATION = 0,  // Players can register/unregister
    TOURNAMENT_IN_PROGRESS  = 1,  // Bracket is locked, matches can be played
    TOURNAMENT_FINISHED     = 2,  // Tournament completed
} TournamentState;
```

| State | Description |
|-------|-------------|
| `REGISTRATION` | Players can be added/removed. Bracket is reconstructed each frame. |
| `IN_PROGRESS` | Bracket is locked. Click to advance winners. Right-click to undo. |
| `FINISHED` | Tournament completed. Results are final. |

### Bracket Array

The bracket is stored as a **heap-style binary tree** in a flat array of 127 elements:

```c
u8 bracket[BRACKET_SIZE];  // BRACKET_SIZE = 127
```

Each element stores a **player index** (1-based) or `0` for empty/TBD slots.

---

## Binary Tree Structure

The bracket uses the standard heap indexing formula:

- **Root**: index `0` (tournament winner)
- **For any node at index `i`**:
  - Left child: `2*i + 1`
  - Right child: `2*i + 2`
  - Parent: `(i - 1) / 2`

### Tree Levels

The 127 elements support up to 64 players across 7 levels:

```
Level 0 (Root):     1 node   - Index 0         (Final Winner)
Level 1:            2 nodes  - Indices 1-2     (Finals)
Level 2:            4 nodes  - Indices 3-6     (Semi-finals for 4 players)
Level 3:            8 nodes  - Indices 7-14    (Quarter-finals for 8 players)
Level 4:           16 nodes  - Indices 15-30
Level 5:           32 nodes  - Indices 31-62
Level 6 (Leaves):  64 nodes  - Indices 63-126  (First round for 64 players)
```

**Formula**: First index at level `L` = `2^L - 1`

### Visual Representation

For a 4-player tournament:

```
                    [0] Winner
                   /   \
                [1]     [2]      <- Finals (Level 1)
               /   \   /   \
             [3]  [4] [5]  [6]   <- Semi-finals (Level 2) - Players start here
```

For a 3-player tournament (with 1 bye):

```
                    [0] Winner
                   /   \
                [1]     [2]      <- Finals
               (Bye)   /   \
                     [5]  [6]    <- First match (Level 2, right subtree)
```

---

## Bracket Construction

When `tournament_construct_bracket()` is called during the REGISTRATION phase:

### Algorithm

1. **Count registered players** (`N`) from the registrations bitmap
2. **Calculate bracket size** (`P`) = smallest power of 2 >= N
3. **Calculate byes** = `P - N`
4. **Determine leaf level** = `log2(P)`
5. **Place bye players** at parent level (they skip the first round)
6. **Place fighting players** at leaf level (they play in round 1)

### Placement Example

For **3 players** (indices 3, 6, 8):
- `P = 4`, `byes = 1`
- Leaf level = 2 (indices 3-6)
- Player at index 3 gets bye → placed at position `1`
- Players at indices 6, 8 fight first → placed at positions `5`, `6`

Result:
```
bracket[1] = 3   (bye player)
bracket[5] = 6   (fighter 1)
bracket[6] = 8   (fighter 2)
```

### Bye Placement Logic

```c
// Bye players go to parent level positions
for (u32 i = 0; i < byes; ++i)
{
    u32 leaf_pos = leaf_start + i * 2;
    u32 parent_pos = (leaf_pos - 1) / 2;
    bracket[parent_pos] = player_idx;
}

// Fighting players go to leaf positions
u32 fighting_start = leaf_start + byes * 2;
for remaining players:
    bracket[fighting_start++] = player_idx;
```

---

## Rendering the Bracket

The UI renders rounds from **left to right** (first round → finals):

### Round to Level Mapping

```
Visual Round 0 (leftmost)  → Deepest bracket level (leaves)
Visual Round 1             → Level above leaves
...
Visual Round N-1 (rightmost) → Level 1 (finals)
```

**Formula**: `bracket_level = num_rounds - visual_round`

### Match Position Calculation

For each match `M` in visual round `R`:

```c
u32 bracket_level = num_rounds - round;
u32 level_base = (1 << bracket_level) - 1;  // First index at this level
u32 pos1 = level_base + match * 2;          // Player 1 position
u32 pos2 = level_base + match * 2 + 1;      // Player 2 position
```

### Slot Rendering Rules

| Condition | Render |
|-----------|--------|
| `bracket[pos1] == 0 && bracket[pos2] == 0 && round == 0` | BYE slot |
| `bracket[pos] == 0` | "TBD" text |
| `bracket[pos] != 0` | Player name (clickable) |

---

## Advancing Winners

When a player name is clicked (left-click) during `IN_PROGRESS`:

### Data Encoding

Click handler receives encoded data:
```c
intptr_t userData = (bracket_pos << 8) | player_idx;
```

Decoding:
```c
u8 player_idx = userData & 0xFF;
u32 bracket_pos = userData >> 8;
```

### Advancement Logic

```c
u32 parent_pos = (bracket_pos - 1) / 2;
tournament->bracket[parent_pos] = player_idx;
```

**Example**: Player at position `5` wins
- Parent = `(5 - 1) / 2 = 2`
- `bracket[2] = player_idx`

---

## Undoing Advancement (Right-Click)

Right-clicking on a player cascades the reset **up the tree**:

```c
u32 pos = bracket_pos;
while (pos > 0)
{
    u32 parent = (pos - 1) / 2;
    if (tournament->bracket[parent] == player_idx)
    {
        tournament->bracket[parent] = 0;  // Reset to TBD
    }
    pos = parent;
}
```

This ensures that if a player was advanced multiple rounds by mistake, all their advancements are undone.

**Example**: Player A advanced from position 5 → 2 → 0
- Right-click on A at position 5
- Clears `bracket[2]` (if it contains A)
- Clears `bracket[0]` (if it contains A)

---

## Tournament Flow

```
┌─────────────────────────────────────────────────────────────┐
│                    REGISTRATION PHASE                        │
│                                                              │
│  • Players can register/unregister                           │
│  • Bracket is reconstructed every frame                      │
│  • "Start Tournament" button appears when >= 2 players       │
└─────────────────────────────────────────────────────────────┘
                            │
                            │ Click "Start Tournament"
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                    IN_PROGRESS PHASE                         │
│                                                              │
│  • Bracket is locked (no reconstruction)                     │
│  • Left-click player name → Advance to next round            │
│  • Right-click player name → Undo advancement (cascade)      │
│  • Registration lists are hidden                             │
└─────────────────────────────────────────────────────────────┘
                            │
                            │ (Future: detect tournament end)
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                    FINISHED PHASE                            │
│                                                              │
│  • Results are final                                         │
│  • Medals can be assigned                                    │
└─────────────────────────────────────────────────────────────┘
```

---

## Key Constants

```c
#define MAX_NUM_ENTITIES 64      // Max players/tournaments
#define BRACKET_SIZE 127         // 64+32+16+8+4+2+1 = 127 nodes

#define ENTITY_IDX_TO_BIT(idx)  ((idx) - 1)  // Entity index to bitmap bit
#define BIT_TO_ENTITY_IDX(bit)  ((bit) + 1)  // Bitmap bit to entity index
```

---

## Registrations Bitmap

Player-tournament relationships are tracked via a 64-bit bitmap:

```c
u64 registrations;  // In Entity struct
```

- For a **player**: bits represent which tournaments they're registered to
- For a **tournament**: bits represent which players are registered

**Check if player `p` is registered to tournament `t`**:
```c
bool registered = (player->registrations >> ENTITY_IDX_TO_BIT(tournament_idx)) & 1;
```

**Get all registered players**:
```c
s32 positions[64];
u32 count = find_all_filled_slots(tournament->registrations, positions);
// positions[] now contains bit positions (use BIT_TO_ENTITY_IDX to get player indices)
```

---

## Summary

| Concept | Implementation |
|---------|----------------|
| Bracket storage | Heap-style array (`bracket[127]`) |
| Player at position `i` | `bracket[i]` (0 = TBD, >0 = player index) |
| Advance winner | Write player index to parent: `bracket[(pos-1)/2] = player_idx` |
| Undo advancement | Cascade reset: clear player from all ancestors |
| Bye handling | Bye players placed at parent level, skip first round |
| State management | `REGISTRATION` → `IN_PROGRESS` → `FINISHED` |
