/* date = March 8th 2021 5:48 pm */

#ifndef OS_ESSENTIAL_H
#define OS_ESSENTIAL_H

////////////////////////////////
// NOTE(allen): Setup

function void os_init(void);

////////////////////////////////
// NOTE(allen): Memory Functions

function void* os_memory_reserve(U64 size);
function void  os_memory_commit(void *ptr, U64 size);
function void  os_memory_decommit(void *ptr, U64 size);
function void  os_memory_release(void *ptr, U64 size);

////////////////////////////////
// NOTE(allen): Thread Context

function void  os_thread_context_set(void *ptr);
function void* os_thread_context_get(void);

#endif //OS_ESSENTIAL_H
