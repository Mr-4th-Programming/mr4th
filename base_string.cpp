////////////////////////////////
// NOTE(allen): String Functions

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
