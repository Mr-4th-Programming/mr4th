////////////////////////////////
// NOTE(allen): Variables

global DWORD win32_thread_context_index = 0;


////////////////////////////////
// NOTE(allen): Init

function void
os_init(void){
    win32_thread_context_index = TlsAlloc();
}

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

////////////////////////////////
// NOTE(allen): Thread Context

function void
os_thread_context_set(void *ptr){
    TlsSetValue(win32_thread_context_index, ptr);
}

function void*
os_thread_context_get(void){
    void *result = TlsGetValue(win32_thread_context_index);
    return(result);
}

////////////////////////////////
// NOTE(allen): Time

function DenseTime
w32_dense_time_from_file_time(FILETIME *file_time){
    DenseTime result = 0;
    // TODO(allen): 
    return(result);
}

////////////////////////////////
// NOTE(allen): File Handling

function FilePropertyFlags
w32_prop_flags_from_attribs(DWORD attribs){
    FilePropertyFlags result = 0;
    if (attribs & FILE_ATTRIBUTE_DIRECTORY){
        result |= FilePropertyFlag_IsFolder;
    }
    return(result);
}

function DataAccessFlags
w32_access_from_attributes(DWORD attribs){
    DataAccessFlags result = DataAccessFlag_Read|DataAccessFlag_Execute;
    if (!(attribs & FILE_ATTRIBUTE_READONLY)){
        result |= DataAccessFlag_Write;
    }
    return(result);
}

function String8
os_file_read(M_Arena *arena, String8 file_name){
    // get handle
    M_Scratch scratch(arena);
    String16 file_name16 = str16_from_str8(scratch, file_name);
    HANDLE file = CreateFileW((WCHAR*)file_name16.str,
                              GENERIC_READ, 0, 0,
                              OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
                              0);
    
    String8 result = {};
    if (file != INVALID_HANDLE_VALUE){
        // get size
        DWORD hi_size = 0;
        DWORD lo_size = GetFileSize(file, &hi_size);
        U64 total_size = (((U64)hi_size) << 32) | (U64)lo_size;
        
        // allocate buffer
        M_Temp restore_point = m_begin_temp(arena);
        U8 *buffer = push_array(arena, U8, total_size);
        
        // read
        U8 *ptr = buffer;
        U8 *opl = buffer + total_size;
        B32 success = true;
        for (;ptr < opl;){
            U64 total_to_read = (U64)(opl - ptr);
            DWORD to_read = (DWORD)total_to_read;
            if (total_to_read > max_U32){
                to_read = max_U32;
            }
            DWORD actual_read = 0;
            if (!ReadFile(file, ptr, to_read, &actual_read, 0)){
                success = false;
                break;
            }
            ptr += actual_read;
        }
        
        // set result or reset memory
        if (success){
            result.str = buffer;
            result.size = total_size;
        }
        else{
            m_end_temp(restore_point);
        }
        
        CloseHandle(file);
    }
    
    return(result);
}

function B32
os_file_write(String8 file_name, String8List data){
    // get handle
    M_Scratch scratch;
    String16 file_name16 = str16_from_str8(scratch, file_name);
    HANDLE file = CreateFileW((WCHAR*)file_name16.str,
                              GENERIC_WRITE, 0, 0,
                              CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
                              0);
    
    B32 result = false;
    if (file != INVALID_HANDLE_VALUE){
        result = true;
        
        for (String8Node *node = data.first;
             node != 0;
             node = node->next){
            U8 *ptr = node->string.str;
            U8 *opl = ptr + node->string.size;
            for (;ptr < opl;){
                U64 total_to_write = (U64)(opl - ptr);
                DWORD to_write = total_to_write;
                if (total_to_write > max_U32){
                    to_write = max_U32;
                }
                DWORD actual_write = 0;
                if (!WriteFile(file, ptr, to_write, &actual_write, 0)){
                    result = false;
                    goto dblbreak;
                }
                ptr += actual_write;
            }
        }
        dblbreak:;
        
        CloseHandle(file);
    }
    
    return(result);
}

function FileProperties
os_file_properties(String8 file_name){
    // convert name
    M_Scratch scratch;
    String16 file_name16 = str16_from_str8(scratch, file_name);
    
    // get attribs and convert to properties
    FileProperties result = {};
    WIN32_FILE_ATTRIBUTE_DATA attribs = {};
    if (GetFileAttributesExW((WCHAR*)file_name16.str, GetFileExInfoStandard,
                             &attribs)){
        result.size = ((U64)attribs.nFileSizeHigh << 32) | (U64)attribs.nFileSizeLow;
        result.flags = w32_prop_flags_from_attribs(attribs.dwFileAttributes);
        result.create_time = w32_dense_time_from_file_time(&attribs.ftCreationTime);
        result.modify_time = w32_dense_time_from_file_time(&attribs.ftLastWriteTime);
        result.access = w32_access_from_attributes(attribs.dwFileAttributes);
    }
    
    return(result);
}

function B32
os_file_delete(String8 file_name){
    // convert name
    M_Scratch scratch;
    String16 file_name16 = str16_from_str8(scratch, file_name);
    // delete file
    B32 result = DeleteFileW((WCHAR*)file_name16.str);
    return(result);
}

function B32
os_file_rename(String8 og_name, String8 new_name){
    // convert name
    M_Scratch scratch;
    String16 og_name16 = str16_from_str8(scratch, og_name);
    String16 new_name16 = str16_from_str8(scratch, new_name);
    // rename file
    B32 result = MoveFileW((WCHAR*)og_name16.str, (WCHAR*)new_name16.str);
    return(result);
}

function B32
os_file_make_directory(String8 path){
    // convert name
    M_Scratch scratch;
    String16 path16 = str16_from_str8(scratch, path);
    // make directory
    B32 result = CreateDirectoryW((WCHAR*)path16.str, 0);
    return(result);
}

function B32
os_file_delete_directory(String8 path){
    // convert name
    M_Scratch scratch;
    String16 path16 = str16_from_str8(scratch, path);
    // make directory
    B32 result = RemoveDirectoryW((WCHAR*)path16.str);
    return(result);
}


function OS_FileIter
os_file_iter_init(String8 path){
    // convert name
    String8Node nodes[2];
    String8List list = {};
    str8_list_push_explicit(&list, path, nodes + 0);
    str8_list_push_explicit(&list, str8_lit("\\*"), nodes + 1);
    M_Scratch scratch;
    String8 path_star = str8_join(scratch, &list, 0);
    // TODO(allen): make unicode conversions take string lists.
    String16 path16 = str16_from_str8(scratch, path_star);
    
    // store into iter
    OS_FileIter result = {};
    W32_FileIter *w32_iter = (W32_FileIter*)&result;
    w32_iter->handle = FindFirstFileW((WCHAR*)path16.str, &w32_iter->find_data);
    return(result);
}

function B32
os_file_iter_next(M_Arena *arena, OS_FileIter *iter,
                  String8 *name, FileProperties *props){
    B32 result = false;
    
    W32_FileIter *w32_iter = (W32_FileIter*)iter;
    if (w32_iter->handle != 0 &&
        w32_iter->handle != INVALID_HANDLE_VALUE){
        for (;!w32_iter->done;){
            // check for . and ..
            WCHAR *file_name = w32_iter->find_data.cFileName;
            B32 is_dot = (file_name[0] == '.' && file_name[1] == 0);
            B32 is_dotdot = (file_name[0] == '.' && file_name[1] == '.' &&
                             file_name[2] == 0);
            
            // setup to emit
            B32 emit = (!is_dot && !is_dotdot);
            WIN32_FIND_DATAW data = {};
            if (emit){
                MemoryCopyStruct(&data, &w32_iter->find_data);
            }
            
            // increment the iterator
            if (!FindNextFileW(w32_iter->handle, &w32_iter->find_data)){
                w32_iter->done = true;
            }
            
            // do the emit if we saved one earlier
            if (emit){
                *name = str8_from_str16(arena, str16_cstring((U16*)data.cFileName));
                props->size = ((U64)data.nFileSizeHigh << 32) | (U64)data.nFileSizeLow;
                props->flags = w32_prop_flags_from_attribs(data.dwFileAttributes);
                props->create_time = w32_dense_time_from_file_time(&data.ftCreationTime);
                props->modify_time = w32_dense_time_from_file_time(&data.ftLastWriteTime);
                props->access = w32_access_from_attributes(data.dwFileAttributes);
                result = true;
                break;
            }
        }
    }
    
    return(result);
}

function void
os_file_iter_end(OS_FileIter *iter){
    W32_FileIter *w32_iter = (W32_FileIter*)iter;
    if (w32_iter->handle != 0 &&
        w32_iter->handle != INVALID_HANDLE_VALUE){
        FindClose(w32_iter->handle);
    }
}

