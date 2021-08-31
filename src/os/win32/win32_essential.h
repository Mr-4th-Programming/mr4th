/* date = March 19th 2021 4:06 pm */

#ifndef WIN32_ESSENTIAL_H
#define WIN32_ESSENTIAL_H

////////////////////////////////
// NOTE(allen): Windows Includes

#undef function
#include <Windows.h>
#define function static

#include <userenv.h>

////////////////////////////////
// NOTE(allen): Win32 File Iter

struct W32_FileIter{
    HANDLE handle;
    WIN32_FIND_DATAW find_data;
    B32 done;
};
StaticAssert(sizeof(W32_FileIter) <= sizeof(OS_FileIter), w32_fileiter);

////////////////////////////////
// NOTE(allen): WinMain Specialized Init

function void w32_WinMain_init(OS_ThreadContext *tctx_memory,
                               HINSTANCE hInstance,
                               HINSTANCE hPrevInstance,
                               LPSTR     lpCmdLine,
                               int       nShowCmd);

////////////////////////////////
// NOTE(allen): Time Helpers

function DateTime   w32_date_time_from_system_time(SYSTEMTIME *in);
function SYSTEMTIME w32_system_time_from_date_time(DateTime *in);
function DenseTime  w32_dense_time_from_file_time(FILETIME *file_time);

////////////////////////////////
// NOTE(allen): File Helpers

function FilePropertyFlags w32_prop_flags_from_attribs(DWORD attribs);
function DataAccessFlags w32_access_from_attributes(DWORD attribs);

#endif //WIN32_ESSENTIAL_H
