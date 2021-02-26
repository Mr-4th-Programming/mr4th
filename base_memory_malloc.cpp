////////////////////////////////
// NOTE(allen): Malloc Base Implementation of M_BaseMemory

function void*
m_malloc_reserve(void *ctx, U64 size){
    return(malloc(size));
}

function void
m_malloc_release(void *ctx, void *ptr, U64 size){
    free(ptr);
}

function M_BaseMemory*
m_malloc_base_memory(void){
    local M_BaseMemory memory = {};
    if (memory.reserve == 0){
        memory.reserve = m_malloc_reserve;
        memory.commit = m_change_memory_noop;
        memory.decommit = m_change_memory_noop;
        memory.release = m_malloc_release;
    }
    return(&memory);
}
