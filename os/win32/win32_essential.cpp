////////////////////////////////
// NOTE(allen): OS Includes

#undef function
#include <Windows.h>
#define function static

////////////////////////////////
// NOTE(allen): Memory Functions

function void*
os_memory_reserve(U64 size){
    void *result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
    return(result);
}

function void
os_memory_commit(void *ptr, U64 size){
    VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
}

function void
os_memory_decommit(void *ptr, U64 size){
    VirtualFree(ptr, size, MEM_DECOMMIT);
}

function void
os_memory_release(void *ptr, U64 size){
    VirtualFree(ptr, 0, MEM_RELEASE);
}
