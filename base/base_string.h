/* date = February 26th 2021 1:30 pm */

#ifndef BASE_STRING_H
#define BASE_STRING_H

////////////////////////////////
// NOTE(allen): String Types

struct String8{
    U8 *str;
    U64 size;
};

struct String8Node{
    String8Node *next;
    String8 string;
};

struct String8List{
    String8Node *first;
    String8Node *last;
    U64 node_count;
    U64 total_size;
};

struct StringJoin{
    String8 pre;
    String8 mid;
    String8 post;
};

typedef U32 StringMatchFlags;
enum{
    StringMatchFlag_NoCase = 1 << 0,
};

struct String16{
    U16 *str;
    U64 size;
};

struct String32{
    U32 *str;
    U64 size;
};

////////////////////////////////
// NOTE(allen): Unicode Helper Types

struct StringDecode{
    U32 codepoint;
    U32 size;
};

////////////////////////////////
// NOTE(allen): Character Functions

function U8 str8_char_uppercase(U8 c);
function U8 str8_char_lowercase(U8 c);

////////////////////////////////
// NOTE(allen): String Constructor Functions

function String8 str8(U8 *str, U64 size);
function String8 str8_range(U8 *first, U8 *opl);
function String8 str8_cstring(U8 *cstr);

#define str8_lit(s) str8((U8*)(s), sizeof(s) - 1)

function String8 str8_prefix(String8 str, U64 size);
function String8 str8_chop(String8 str, U64 amount);
function String8 str8_postfix(String8 str, U64 size);
function String8 str8_skip(String8 str, U64 amount);
function String8 str8_substr(String8 str, U64 first, U64 opl);

#define str8_expand(s) (int)((s).size), ((s).str)

function void str8_list_push_explicit(String8List *list, String8 string,
                                      String8Node *node_memory);
function void str8_list_push(M_Arena *arena, String8List *list, String8 string);

function String8 str8_join(M_Arena *arena, String8List *list,
                           StringJoin *optional_join);

function String8List str8_split(M_Arena *arena, String8 string,
                                U8 *split_characters, U32 count);

function String8 str8_pushfv(M_Arena *arena, char *fmt, va_list args);
function String8 str8_pushf(M_Arena *arena, char *fmt, ...);
function void    str8_list_pushf(M_Arena *arena, String8List *list, char *fmt, ...);

function String16 str16_cstring(U16 *cstr);

////////////////////////////////
// NOTE(allen): String Comparison Functions

function B32 str8_match(String8 a, String8 b, StringMatchFlags flags);

////////////////////////////////
// NOTE(allen): Unicode Functions

function StringDecode str_decode_utf8(U8 *str, U32 cap);
function U32          str_encode_utf8(U8 *dst, U32 codepoint);
function StringDecode str_decode_utf16(U16 *str, U32 cap);
function U32          str_encode_utf16(U16 *dst, U32 codepoint);

function String32     str32_from_str8(M_Arena *arena, String8 string);
function String8      str8_from_str32(M_Arena *arena, String32 string);
function String16     str16_from_str8(M_Arena *arena, String8 string);
function String8      str8_from_str16(M_Arena *arena, String16 string);


#endif //BASE_STRING_H
