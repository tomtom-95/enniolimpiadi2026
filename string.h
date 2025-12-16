#ifndef STRING_H
#define STRING_H

#include <stdbool.h>

#include "core.h"
#include "arena.h"

typedef struct String8 String8;
struct String8 {
    u8 *str;
    u64 len;
};

#define str8_lit(S) str8((u8 *)S, sizeof(S)-1)
#define str8_lit_comp(S) {(u8 *)S, sizeof(S)-1,}

String8 str8(u8 *str, u64 len);

String8 str8_cat(Arena *arena, String8 s1, String8 s2);
String8 str8_copy(Arena *arena, String8 s);
String8 str8_strip_whitespace(String8 s);

bool str8_cmp(String8 s1, String8 s2);

String8 str8_from_cstr(char *c);
String8 str8_from_u32(Arena *arena, u32 num);

u64 cstr8_len(u8 *c);

#endif // STRING_H
