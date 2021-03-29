/* date = March 8th 2021 5:56 pm */

#ifndef OS_HELPERS_H
#define OS_HELPERS_H

////////////////////////////////
// NOTE(allen): Base Memory

function M_BaseMemory* os_base_memory(void);

function M_Arena os_make_arena_reserve(U64 reserve_size);
function M_Arena os_make_arena(void);

////////////////////////////////
// NOTE(allen): Thread Self Init

function void os_thread_init(OS_ThreadContext *tctx_memory);

////////////////////////////////
// NOTE(allen): Thread Context

function void os_tctx_init(OS_ThreadContext *tctx, M_BaseMemory *memory);
function M_Arena *os_tctx_get_scratch(M_Arena **conflict_array, U32 count);

////////////////////////////////
// NOTE(allen): Files

function B32 os_file_write(String8 file_name, String8 data);

////////////////////////////////
// NOTE(allen): Scratch Arena Wrapper

struct M_Scratch{
    M_Temp temp;
    
    M_Scratch(void);
    M_Scratch(M_Arena *a1);
    M_Scratch(M_Arena *a1, M_Arena *a2);
    ~M_Scratch(void);
    operator M_Arena*(void);
    void reset(void);
};

#endif //OS_HELPERS_H
