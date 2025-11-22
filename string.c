#include <stdbool.h>

#include "core.h"
#include "arena.h"
#include "string.h"

String8
str8(u8 *str, u64 len)
{
    return (String8){str, len};
}

u64
cstr8_len(u8 *cstr)
{
    u8 *start = cstr;
    while (*cstr)
    {
        ++cstr;
    }
    return (u64)(cstr - start);
}

String8
str8_cat(Arena *arena, String8 s1, String8 s2)
{
    String8 result;

    result.len = s1.len + s2.len;
    result.str = push_array(arena, u8, result.len);

    MemoryCopy(result.str, s1.str, s1.len);
    MemoryCopy(result.str + s1.len, s2.str, s2.len);

    return result;
}

String8
str8_cstr(char *c)
{
    return (String8){(u8*)c, cstr8_len((u8*)c)};
}

String8
str8_copy(Arena *arena, String8 s)
{
    String8 result;

    result.len = s.len;
    result.str = push_array(arena, u8, s.len);

    MemoryCopy(result.str, s.str, s.len);

    return result;
}

bool
str8_cmp(String8 s1, String8 s2)
{
    return s1.len == s2.len && MemoryMatch(s1.str, s2.str, s1.len);
}
