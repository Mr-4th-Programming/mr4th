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
