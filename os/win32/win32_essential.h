/* date = March 19th 2021 4:06 pm */

#ifndef WIN32_ESSENTIAL_H
#define WIN32_ESSENTIAL_H

////////////////////////////////
// NOTE(allen): Windows Includes

#undef function
#include <Windows.h>
#define function static

////////////////////////////////
// NOTE(allen): Win32 File Iter

struct W32_FileIter{
    HANDLE handle;
    WIN32_FIND_DATAW find_data;
    B32 done;
};
StaticAssert(sizeof(W32_FileIter) <= sizeof(OS_FileIter), w32_fileiter);

#endif //WIN32_ESSENTIAL_H
