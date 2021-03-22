////////////////////////////////
// NOTE(allen): Character Functions

function U8
str8_char_uppercase(U8 c){
    if ('a' <= c && c <= 'z'){
        c += 'A' - 'a';
    }
    return(c);
}

function U8
str8_char_lowercase(U8 c){
    if ('A' <= c && c <= 'Z'){
        c += 'a' - 'A';
    }
    return(c);
}

////////////////////////////////
// NOTE(allen): String Constructor Functions

function String8
str8(U8 *str, U64 size){
    String8 result = {str, size};
    return(result);
}

function String8
str8_range(U8 *first, U8 *opl){
    String8 result = {first, (U64)(opl - first)};
    return(result);
}

function String8
str8_cstring(U8 *cstr){
    U8 *ptr = cstr;
    for (;*ptr != 0; ptr += 1);
    String8 result = str8_range(cstr, ptr);
    return(result);
}

function String8
str8_prefix(String8 str, U64 size){
    U64 size_clamped = ClampTop(size, str.size);
    String8 result = {str.str, size_clamped};
    return(result);
}

function String8
str8_chop(String8 str, U64 amount){
    U64 amount_clamped = ClampTop(amount, str.size);
    U64 remaining_size = str.size - amount_clamped;
    String8 result = {str.str, remaining_size};
    return(result);
}

function String8
str8_postfix(String8 str, U64 size){
    U64 size_clamped = ClampTop(size, str.size);
    U64 skip_to = str.size - size_clamped;
    String8 result = {str.str + skip_to, size_clamped};
    return(result);
}

function String8
str8_skip(String8 str, U64 amount){
    U64 amount_clamped = ClampTop(amount, str.size);
    U64 remaining_size = str.size - amount_clamped;
    String8 result = {str.str + amount_clamped, remaining_size};
    return(result);
}

function String8
str8_substr(String8 str, U64 first, U64 opl){
    U64 opl_clamped = ClampTop(opl, str.size);
    U64 first_clamped = ClampTop(first, opl_clamped);
    String8 result = {str.str + first_clamped, opl_clamped - first_clamped};
    return(result);
}

function void
str8_list_push_explicit(String8List *list, String8 string,
                        String8Node *node_memory){
    node_memory->string = string;
    SLLQueuePush(list->first, list->last, node_memory);
    list->node_count += 1;
    list->total_size += string.size;
}

function void
str8_list_push(M_Arena *arena, String8List *list, String8 string){
    String8Node *node = push_array(arena, String8Node, 1);
    str8_list_push_explicit(list, string, node);
}

function String8
str8_join(M_Arena *arena, String8List *list,
          StringJoin *join_optional){
    // setup join parameters
    local StringJoin dummy_join = {};
    StringJoin *join = join_optional;
    if (join == 0){
        join = &dummy_join;
    }
    
    // compute total size
    U64 size = (join->pre.size +
                join->post.size +
                join->mid.size*(list->node_count - 1) +
                list->total_size);
    
    // begin string build
    U8 *str = push_array(arena, U8, size + 1);
    U8 *ptr = str;
    
    // write pre
    MemoryCopy(ptr, join->pre.str, join->pre.size);
    ptr += join->pre.size;
    
    B32 is_mid = false;
    for (String8Node *node = list->first;
         node != 0;
         node = node->next){
        // write mid
        if (is_mid){
            MemoryCopy(ptr, join->mid.str, join->mid.size);
            ptr += join->mid.size;
        }
        
        // write node string
        MemoryCopy(ptr, node->string.str, node->string.size);
        ptr += node->string.size;
        
        is_mid = true;
    }
    
    // write post
    MemoryCopy(ptr, join->post.str, join->post.size);
    ptr += join->post.size;
    
    // write null
    *ptr = 0;
    
    String8 result = str8(str, size);
    return(result);
}

function String8List
str8_split(M_Arena *arena, String8 string, U8 *splits, U32 count){
    String8List result = {};
    
    U8 *ptr = string.str;
    U8 *word_first = ptr;
    U8 *opl = string.str + string.size;
    for (;ptr < opl; ptr += 1){
        // is this a split
        U8 byte = *ptr;
        B32 is_split_byte = false;
        for (U32 i = 0; i < count; i += 1){
            if (byte == splits[i]){
                is_split_byte = true;
                break;
            }
        }
        
        if (is_split_byte){
            // try to emit word, advance word first pointer
            if (word_first < ptr){
                str8_list_push(arena, &result, str8_range(word_first, ptr));
            }
            word_first = ptr + 1;
        }
    }
    
    // try to emit final word
    if (word_first < ptr){
        str8_list_push(arena, &result, str8_range(word_first, ptr));
    }
    
    return(result);
}

// TODO(allen): where do I want this to live??
#include <stdio.h>

function String8
str8_pushfv(M_Arena *arena, char *fmt, va_list args){
    // in case we need to try a second time
    va_list args2;
    va_copy(args2, args);
    
    // try to build the string in 1024 bytes
    U64 buffer_size = 1024;
    U8 *buffer = push_array(arena, U8, buffer_size);
    U64 actual_size = vsnprintf((char*)buffer, buffer_size, fmt, args);
    
    String8 result = {};
    if (actual_size < buffer_size){
        // if first try worked, put back what we didn't use and finish
        m_arena_pop_amount(arena, buffer_size - actual_size - 1);
        result = str8(buffer, actual_size);
    }
    else{
        // if first try failed, reset and try again with correct size
        m_arena_pop_amount(arena, buffer_size);
        U8 *fixed_buffer = push_array(arena, U8, actual_size + 1);
        U64 final_size = vsnprintf((char*)fixed_buffer, actual_size + 1, fmt, args2);
        result = str8(fixed_buffer, final_size);
    }
    
    // end args2
    va_end(args2);
    
    return(result);
}

function String8
str8_pushf(M_Arena *arena, char *fmt, ...){
    va_list args;
    va_start(args, fmt);
    String8 result = str8_pushfv(arena, fmt, args);
    va_end(args);
    return(result);
}

function void
str8_list_pushf(M_Arena *arena, String8List *list, char *fmt, ...){
    va_list args;
    va_start(args, fmt);
    String8 string = str8_pushfv(arena, fmt, args);
    va_end(args);
    str8_list_push(arena, list, string);
}

function String16
str16_cstring(U16 *cstr){
    U16 *ptr = cstr;
    for (;*ptr != 0; ptr += 1);
    String16 result = {cstr, (U64)(ptr - cstr)};
    return(result);
}

////////////////////////////////
// NOTE(allen): String Comparison Functions

function B32
str8_match(String8 a, String8 b, StringMatchFlags flags){
    B32 result = false;
    if (a.size == b.size){
        result = true;
        B32 no_case = ((flags & StringMatchFlag_NoCase) != 0);
        for (U64 i = 0; i < a.size; i += 1){
            U8 ac = a.str[i];
            U8 bc = b.str[i];
            if (no_case){
                ac = str8_char_uppercase(ac);
                bc = str8_char_uppercase(bc);
            }
            if (ac != bc){
                result = false;
                break;
            }
        }
    }
    return(result);
}

////////////////////////////////
// NOTE(allen): Unicode Functions

function StringDecode
str_decode_utf8(U8 *str, U32 cap){
    local U8 length[] = {
        1, 1, 1, 1, // 000xx
        1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1,
        0, 0, 0, 0, // 100xx
        0, 0, 0, 0,
        2, 2, 2, 2, // 110xx
        3, 3,       // 1110x
        4,          // 11110
        0,          // 11111
    };
    local U8 first_byte_mask[] = { 0, 0x7F, 0x1F, 0x0F, 0x07 };
    local U8 final_shift[] = { 0, 18, 12, 6, 0 };
    
    StringDecode result = {};
    if (cap > 0){
        result.codepoint = '#';
        result.size = 1;
        
        U8 byte = str[0];
        U8 l = length[byte >> 3];
        if (0 < l && l <= cap){
            U32 cp = (byte & first_byte_mask[l]) << 18;
            switch (l){
                case 3: cp |= ((str[3] & 0x3F) << 0);
                case 2: cp |= ((str[2] & 0x3F) << 6);
                case 1: cp |= ((str[1] & 0x3F) << 12);
                case 0: cp >>= final_shift[l];
            }
            
            result.codepoint = cp;
            result.size = l;
        }
    }
    
    return(result);
}

function U32
str_encode_utf8(U8 *dst, U32 codepoint){
    U32 size = 0;
    if (codepoint < (1 << 8)){
        dst[0] = codepoint;
        size = 1;
    }
    else if (codepoint < (1 << 11)){
        dst[0] = 0xC0 | (codepoint >> 6);
        dst[1] = 0x80 | (codepoint & 0x3F);
        size = 2;
    }
    else if (codepoint < (1 << 16)){
        dst[0] = 0xE0 | (codepoint >> 12);
        dst[1] = 0x80 | ((codepoint >> 6) & 0x3F);
        dst[2] = 0x80 | (codepoint & 0x3F);
        size = 3;
    }
    else if (codepoint < (1 << 21)){
        dst[0] = 0xF0 | (codepoint >> 18);
        dst[1] = 0x80 | ((codepoint >> 12) & 0x3F);
        dst[2] = 0x80 | ((codepoint >> 6) & 0x3F);
        dst[3] = 0x80 | (codepoint & 0x3F);
        size = 4;
    }
    else{
        dst[0] = '#';
        size = 1;
    }
    return(size);
}

function StringDecode
str_decode_utf16(U16 *str, U32 cap){
    StringDecode result = {'#', 1};
    U16 x = str[0];
    if (x < 0xD800 || 0xDFFF < x){
        result.codepoint = x;
    }
    else if (cap >= 2){
        U16 y = str[1];
        if (0xD800 <= x && x < 0xDC00 &&
            0xDC00 <= y && y < 0xE000){
            U16 xj = x - 0xD800;
            U16 yj = y - 0xDc00;
            U32 xy = (xj << 10) | yj;
            result.codepoint = xy + 0x10000;
            result.size = 2;
        }
    }
    return(result);
}

function U32
str_encode_utf16(U16 *dst, U32 codepoint){
    U32 size = 0;
    if (codepoint < 0x10000){
        dst[0] = codepoint;
        size = 1;
    }
    else{
        U32 cpj = codepoint - 0x10000;
        dst[0] = (cpj >> 10) + 0xD800;
        dst[1] = (cpj & 0x3FF) + 0xDC00;
        size = 2;
    }
    return(size);
}

function String32
str32_from_str8(M_Arena *arena, String8 string){
    U32 *memory = push_array(arena, U32, string.size + 1);
    
    U32 *dptr = memory;
    U8 *ptr = string.str;
    U8 *opl = string.str + string.size;
    for (; ptr < opl;){
        StringDecode decode = str_decode_utf8(ptr, (U64)(opl - ptr));
        *dptr = decode.codepoint;
        ptr += decode.size;
        dptr += 1;
    }
    
    *dptr = 0;
    
    U64 alloc_count = string.size + 1;
    U64 string_count = (U64)(dptr - memory);
    U64 unused_count = alloc_count - string_count - 1;
    m_arena_pop_amount(arena, unused_count*sizeof(*memory));
    
    String32 result = {memory, string_count};
    return(result);
}

function String8
str8_from_str32(M_Arena *arena, String32 string){
    U8 *memory = push_array(arena, U8, string.size*4 + 1);
    
    U8 *dptr = memory;
    U32 *ptr = string.str;
    U32 *opl = string.str + string.size;
    for (; ptr < opl;){
        U32 size = str_encode_utf8(dptr, *ptr);
        ptr += 1;
        dptr += size;
    }
    
    *dptr = 0;
    
    U64 alloc_count = string.size*4 + 1;
    U64 string_count = (U64)(dptr - memory);
    U64 unused_count = alloc_count - string_count - 1;
    m_arena_pop_amount(arena, unused_count*sizeof(*memory));
    
    String8 result = {memory, string_count};
    return(result);
}

function String16
str16_from_str8(M_Arena *arena, String8 string){
    U16 *memory = push_array(arena, U16, string.size*2 + 1);
    
    U16 *dptr = memory;
    U8 *ptr = string.str;
    U8 *opl = string.str + string.size;
    for (; ptr < opl;){
        StringDecode decode = str_decode_utf8(ptr, (U64)(opl - ptr));
        U32 enc_size = str_encode_utf16(dptr, decode.codepoint);
        ptr += decode.size;
        dptr += enc_size;
    }
    
    *dptr = 0;
    
    U64 alloc_count = string.size*2 + 1;
    U64 string_count = (U64)(dptr - memory);
    U64 unused_count = alloc_count - string_count - 1;
    m_arena_pop_amount(arena, unused_count*sizeof(*memory));
    
    String16 result = {memory, string_count};
    return(result);
}

function String8
str8_from_str16(M_Arena *arena, String16 string){
    U8 *memory = push_array(arena, U8, string.size*3 + 1);
    
    U8 *dptr = memory;
    U16 *ptr = string.str;
    U16 *opl = string.str + string.size;
    for (; ptr < opl;){
        StringDecode decode = str_decode_utf16(ptr, (U64)(opl - ptr));
        U16 enc_size = str_encode_utf8(dptr, decode.codepoint);
        ptr += decode.size;
        dptr += enc_size;
    }
    
    *dptr = 0;
    
    U64 alloc_count = string.size*3 + 1;
    U64 string_count = (U64)(dptr - memory);
    U64 unused_count = alloc_count - string_count - 1;
    m_arena_pop_amount(arena, unused_count*sizeof(*memory));
    
    String8 result = {memory, string_count};
    return(result);
}
