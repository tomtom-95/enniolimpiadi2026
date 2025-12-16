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
str8_from_cstr(char *c)
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

String8
str8_strip_whitespace(String8 s)
{
    u64 start;
    u64 stop;

    // Handle empty string
    if (s.len == 0)
        return s;

    for (start = 0; start < s.len; ++start)
    {
        u8 ch = s.str[start];
        if (ch != ' ' && ch != '\t' && ch != '\n')
            break;
    }

    for (stop = s.len; stop > 0; --stop)
    {
        u8 ch = s.str[stop - 1];
        if (ch != ' ' && ch != '\t' && ch != '\n')
            break;
    }

    // If all whitespace, return empty string
    if (start >= stop)
        return str8(s.str, 0);

    return str8(s.str + start, stop - start);
}

String8
str8_from_u32(Arena *arena, u32 num)
{
    String8 result;

    // Handle 0 specially
    if (num == 0)
    {
        result.len = 1;
        result.str = push_array(arena, u8, 1);
        result.str[0] = '0';
        return result;
    }

    // Calculate number of digits
    u32 temp = num;
    u64 len = 0;
    while (temp > 0)
    {
        temp /= 10;
        len++;
    }

    // Allocate memory
    result.len = len;
    result.str = push_array(arena, u8, len);

    // Fill in digits from right to left
    for (u64 i = len; i > 0; i--)
    {
        result.str[i - 1] = '0' + (num % 10);
        num /= 10;
    }

    return result;
}
