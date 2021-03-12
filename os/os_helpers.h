/* date = March 8th 2021 5:56 pm */

#ifndef OS_HELPERS_H
#define OS_HELPERS_H

////////////////////////////////
// NOTE(allen): Thread Context

#if !defined(OS_SCRATCH_POOL_CAP)
# define OS_SCRATCH_POOL_CAP 4
#endif

struct OS_ThreadContext{
    M_BaseMemory *memory;
    M_Arena scratch_pool[OS_SCRATCH_POOL_CAP];
};

////////////////////////////////
// NOTE(allen): Base Memory

function M_BaseMemory* os_base_memory(void);

////////////////////////////////
// NOTE(allen): Thread Self Init

function void os_thread_init(OS_ThreadContext *tctx_memory);

////////////////////////////////
// NOTE(allen): Thread Context

function void os_tctx_init(OS_ThreadContext *tctx, M_BaseMemory *memory);
function M_Arena *os_tctx_get_scratch(M_Arena **conflict_array, U32 count);

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
