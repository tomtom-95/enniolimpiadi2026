#!/usr/bin/env python3
"""Generate a binary olympiad.sav file matching the C save format."""

import struct

# Constants matching players.h
SAVE_VERSION = 2
SAVE_MAGIC = 0x454E4E49  # "ENNI"

MAX_NUM_ENTITIES = 64
BRACKET_SIZE = 127
MAX_GROUPS = 16
MAX_GROUP_SIZE = 8

# Entity list length (same as C code)
ENTITY_LIST_LEN = 64

# Test data
players = ["Alice", "Bob", "Charlie"]
events = ["Tennis", "Chess"]

# Registration bitmaps (bit N = entity index N+1)
# Alice (idx 1) -> bit 0, Bob (idx 2) -> bit 1, Charlie (idx 3) -> bit 2
player_registrations = {
    "Alice": 0b11,    # registered to Tennis (bit 0) and Chess (bit 1)
    "Bob": 0b01,      # registered to Tennis only
    "Charlie": 0b10,  # registered to Chess only
}

event_registrations = {
    "Tennis": 0b011,  # Alice (bit 0) and Bob (bit 1)
    "Chess": 0b101,   # Alice (bit 0) and Charlie (bit 2)
}


def calc_group_phase_size():
    """Calculate sizeof(GroupPhase) with padding."""
    # u8 num_groups, group_size, advance_per_group
    size = 3
    # u8 groups[16][8]
    size += MAX_GROUPS * MAX_GROUP_SIZE  # 128
    # u8 player_group[65], player_slot[65]
    size += (MAX_NUM_ENTITIES + 1) * 2  # 130
    # Total so far: 261 bytes

    # MatchScore scores[16][8][8] - needs 2-byte alignment (contains u16)
    if size % 2 != 0:
        size += 1  # padding
    size += MAX_GROUPS * MAX_GROUP_SIZE * MAX_GROUP_SIZE * 4  # 4096

    # MatchResult results[16][8][8] - enum typically 4 bytes, needs 4-byte alignment
    if size % 4 != 0:
        size += 4 - (size % 4)  # padding
    size += MAX_GROUPS * MAX_GROUP_SIZE * MAX_GROUP_SIZE * 4  # 4096

    # u8 bracket[127]
    size += BRACKET_SIZE  # 127

    # Struct alignment (max member alignment = 4)
    if size % 4 != 0:
        size += 4 - (size % 4)

    return size


def write_group_phase(buf):
    """Write a zeroed GroupPhase struct with default values."""
    # num_groups, group_size (default 4), advance_per_group (default 2)
    buf.extend([0, 4, 2])

    # groups[16][8]
    buf.extend([0] * (MAX_GROUPS * MAX_GROUP_SIZE))

    # player_group[65] - all 0xFF (GROUP_NONE)
    buf.extend([0xFF] * (MAX_NUM_ENTITIES + 1))

    # player_slot[65]
    buf.extend([0] * (MAX_NUM_ENTITIES + 1))

    # Current size: 261 bytes, need padding for scores (2-byte aligned)
    current = 3 + 128 + 65 + 65  # 261
    if current % 2 != 0:
        buf.append(0)  # 1 byte padding
        current += 1

    # scores[16][8][8] - MatchScore is {u16, u16} = 4 bytes
    buf.extend([0] * (MAX_GROUPS * MAX_GROUP_SIZE * MAX_GROUP_SIZE * 4))
    current += 4096

    # Padding for results (4-byte aligned enum)
    if current % 4 != 0:
        padding = 4 - (current % 4)
        buf.extend([0] * padding)
        current += padding

    # results[16][8][8] - MatchResult enum = 4 bytes
    buf.extend([0] * (MAX_GROUPS * MAX_GROUP_SIZE * MAX_GROUP_SIZE * 4))
    current += 4096

    # bracket[127]
    buf.extend([0] * BRACKET_SIZE)
    current += 127

    # Final struct padding
    if current % 4 != 0:
        padding = 4 - (current % 4)
        buf.extend([0] * padding)


def write_entity(buf, nxt, prv, name, registrations, medals=(0,0,0), phase=0, fmt=0):
    """Write a single entity to the buffer."""
    # nxt (u32), prv (u32)
    buf.extend(struct.pack('<I', nxt))
    buf.extend(struct.pack('<I', prv))

    # name_len (u32) + name bytes
    name_bytes = name.encode('utf-8') if name else b''
    buf.extend(struct.pack('<I', len(name_bytes)))
    buf.extend(name_bytes)

    # registrations (u64)
    buf.extend(struct.pack('<Q', registrations))

    # medals[3]
    buf.extend(bytes(medals))

    # phase (u8), format (u8)
    buf.append(phase)
    buf.append(fmt)

    # bracket[127]
    buf.extend([0] * BRACKET_SIZE)

    # group_phase
    write_group_phase(buf)


def generate_save():
    buf = bytearray()

    num_players = len(players)
    num_events = len(events)

    # After adding entities, first_free_idx points to next available slot
    players_first_free = num_players + 1
    tournaments_first_free = num_events + 1

    # === Header ===
    buf.extend(struct.pack('<I', SAVE_MAGIC))
    buf.extend(struct.pack('<I', SAVE_VERSION))
    buf.extend(struct.pack('<I', ENTITY_LIST_LEN))  # players_len
    buf.extend(struct.pack('<I', players_first_free))
    buf.extend(struct.pack('<I', ENTITY_LIST_LEN))  # tournaments_len
    buf.extend(struct.pack('<I', tournaments_first_free))

    tail_idx = ENTITY_LIST_LEN + 1  # 65

    # === Players entities (66 total: head + 64 slots + tail) ===

    # Entity 0: Head sentinel
    first_active = 1 if num_players > 0 else tail_idx
    write_entity(buf, nxt=first_active, prv=0, name="", registrations=0)

    # Active player entities (indices 1 to num_players)
    for i, name in enumerate(players):
        idx = i + 1
        nxt = idx + 1 if i < num_players - 1 else tail_idx
        prv = idx - 1 if i > 0 else 0
        reg = player_registrations.get(name, 0)
        write_entity(buf, nxt=nxt, prv=prv, name=name, registrations=reg)

    # Free list entities (indices num_players+1 to ENTITY_LIST_LEN)
    for i in range(num_players + 1, ENTITY_LIST_LEN + 1):
        # Free list: each points to next, last points to 0
        nxt = i + 1 if i < ENTITY_LIST_LEN else 0
        write_entity(buf, nxt=nxt, prv=0, name="", registrations=0)

    # Entity tail_idx (65): Tail sentinel
    last_active = num_players if num_players > 0 else 0
    write_entity(buf, nxt=tail_idx, prv=last_active, name="", registrations=0)

    # === Tournaments entities (66 total) ===

    # Entity 0: Head sentinel
    first_active = 1 if num_events > 0 else tail_idx
    write_entity(buf, nxt=first_active, prv=0, name="", registrations=0)

    # Active tournament entities
    for i, name in enumerate(events):
        idx = i + 1
        nxt = idx + 1 if i < num_events - 1 else tail_idx
        prv = idx - 1 if i > 0 else 0
        reg = event_registrations.get(name, 0)
        write_entity(buf, nxt=nxt, prv=prv, name=name, registrations=reg)

    # Free list entities
    for i in range(num_events + 1, ENTITY_LIST_LEN + 1):
        nxt = i + 1 if i < ENTITY_LIST_LEN else 0
        write_entity(buf, nxt=nxt, prv=0, name="", registrations=0)

    # Entity tail_idx: Tail sentinel
    last_active = num_events if num_events > 0 else 0
    write_entity(buf, nxt=tail_idx, prv=last_active, name="", registrations=0)

    return bytes(buf)


if __name__ == "__main__":
    data = generate_save()

    with open('olympiad.sav', 'wb') as f:
        f.write(data)

    print(f"Generated olympiad.sav ({len(data)} bytes)")
    print(f"  Magic: 0x{SAVE_MAGIC:08X}")
    print(f"  Version: {SAVE_VERSION}")
    print(f"  Players: {players}")
    print(f"  Events: {events}")
    print(f"  GroupPhase size: {calc_group_phase_size()} bytes")
