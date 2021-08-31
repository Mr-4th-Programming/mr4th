////////////////////////////////
// NOTE(allen): Base Memory

function void*
os_memory_reserve_wrapped(void *ctx, U64 size){
    return(os_memory_reserve(size));
}

function void
os_memory_commit_wrapped(void *ctx, void *ptr, U64 size){
    return(os_memory_commit(ptr, size));
}

function void
os_memory_decommit_wrapped(void *ctx, void *ptr, U64 size){
    return(os_memory_decommit(ptr, size));
}

function void
os_memory_release_wrapped(void *ctx, void *ptr, U64 size){
    return(os_memory_release(ptr, size));
}

function M_BaseMemory*
os_base_memory(void){
    local M_BaseMemory memory = {};
    if (memory.reserve == 0){
        memory.reserve = os_memory_reserve_wrapped;
        memory.commit = os_memory_commit_wrapped;
        memory.decommit = os_memory_decommit_wrapped;
        memory.release = os_memory_release_wrapped;
    }
    return(&memory);
}

function M_Arena
os_make_arena_reserve(U64 size){
    M_Arena result = m_make_arena_reserve(os_base_memory(), size);
    return(result);
}

function M_Arena
os_make_arena(void){
    M_Arena result = os_make_arena_reserve(M_DEFAULT_RESERVE_SIZE);
    return(result);
}

////////////////////////////////
// NOTE(allen): Thread Setup

function void
os_thread_init(OS_ThreadContext *tctx_memory){
    M_BaseMemory *os_base = os_base_memory();
    os_tctx_init(tctx_memory, os_base);
    os_thread_context_set(tctx_memory);
}

////////////////////////////////
// NOTE(allen): Thread Context

function void
os_tctx_init(OS_ThreadContext *tctx, M_BaseMemory *memory){
    tctx->memory = memory;
    M_Arena *scratch = tctx->scratch_pool;
    for (U32 i = 0;
         i < ArrayCount(tctx->scratch_pool);
         i += 1, scratch += 1){
        *scratch = m_make_arena(memory);
    }
}

function M_Arena*
os_tctx_get_scratch(M_Arena **conflict_array, U32 count){
    OS_ThreadContext *tctx = (OS_ThreadContext*)os_thread_context_get();
    M_Arena *result = 0;
    M_Arena *scratch = tctx->scratch_pool;
    for (U64 i = 0;
         i < ArrayCount(tctx->scratch_pool);
         i += 1, scratch += 1){
        B32 is_non_conflict = true;
        M_Arena **conflict_ptr = conflict_array;
        for (U32 j = 0; j < count; j += 1, conflict_ptr += 1){
            if (scratch == *conflict_ptr){
                is_non_conflict = false;
                break;
            }
        }
        if (is_non_conflict){
            result = scratch;
            break;
        }
    }
    return(result);
}

////////////////////////////////
// NOTE(allen): Files

function B32
os_file_write(String8 file_name, String8 data){
    String8Node node = {};
    String8List list = {};
    str8_list_push_explicit(&list, data, &node);
    B32 result = os_file_write(file_name, list);
    return(result);
}

////////////////////////////////
// NOTE(allen): Scratch Arena Wrapper

M_Scratch::M_Scratch(void){
    this->temp = m_begin_temp(os_tctx_get_scratch(0, 0));
}

M_Scratch::M_Scratch(M_Arena *a1){
    this->temp = m_begin_temp(os_tctx_get_scratch(&a1, 1));
}

M_Scratch::M_Scratch(M_Arena *a1, M_Arena *a2){
    M_Arena *conflicts[] = {a1, a2};
    this->temp = m_begin_temp(os_tctx_get_scratch(conflicts, 2));
}

M_Scratch::~M_Scratch(void){
    m_end_temp(this->temp);
}

M_Scratch::operator M_Arena*(void){
    return(this->temp.arena);
}

void
M_Scratch::reset(void){
    m_end_temp(this->temp);
}

