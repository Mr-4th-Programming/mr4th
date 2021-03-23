/* date = March 8th 2021 5:48 pm */

#ifndef OS_ESSENTIAL_H
#define OS_ESSENTIAL_H

////////////////////////////////
// NOTE(allen): File Iterator

struct OS_FileIter{
    U8 v[640];
};

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

////////////////////////////////
// NOTE(allen): File Handling

function String8 os_file_read(M_Arena *arena, String8 file_name);
function B32     os_file_write(String8 file_name, String8List data);

function FileProperties os_file_properties(String8 file_name);

function B32 os_file_delete(String8 file_name);
function B32 os_file_rename(String8 og_name, String8 new_name);
function B32 os_file_make_directory(String8 path);
function B32 os_file_delete_directory(String8 path);

function OS_FileIter os_file_iter_init(String8 path);
function B32  os_file_iter_next(M_Arena *arena, OS_FileIter *iter,
                                String8 *name_out, FileProperties *prop_out);
function void os_file_iter_end(OS_FileIter *iter);

////////////////////////////////
// NOTE(allen): Time

function DateTime os_now_universal_time(void);
function DateTime os_local_time_from_universal(DateTime *date_time);
function DateTime os_universal_time_from_local(DateTime *date_time);

function U64  os_now_microseconds(void);
function void os_sleep_milliseconds(U32 t);

#endif //OS_ESSENTIAL_H
