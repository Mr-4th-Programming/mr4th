#include "base/base_inc.h"
#include "os/os_inc.h"


#include "base/base_inc.cpp"
#include "os/os_inc.cpp"

#include <stdio.h>
#include <stdlib.h>

#include "base/base_memory_malloc.cpp"

int main(){
    os_init();
    
    OS_ThreadContext tctx_memory = {};
    os_thread_init(&tctx_memory);
    
    M_Scratch scratch;
    int *foo = push_array(scratch, int, 1000);
    foo[999] = 0;
    printf("scratch: %llu %llu %llu\n",
           scratch.temp.arena->pos, scratch.temp.arena->commit_pos,
           scratch.temp.arena->cap);
    
    M_Scratch other_scratch((M_Arena*)scratch);
    printf("scratch:       %p\nother_scratch: %p\n",
           scratch.temp.arena,
           other_scratch.temp.arena);
    
    // read/write
    printf("read/write:\n");
    os_file_write(str8_lit("foo.txt"), str8_lit("Bar text\n"));
    
    String8 my_file_data = os_file_read(scratch, str8_lit("foo.txt"));
    printf("%.*s", str8_expand(my_file_data));
    
    // properties
    printf("properties:\n");
    {
        FileProperties props = os_file_properties(str8_lit("foo.txt"));
        printf("size: %llu\nis folder? %s\n", props.size, (props.flags & FilePropertyFlag_IsFolder)?"yes":"no");
    }
    
    {
        FileProperties props = os_file_properties(str8_lit("foo"));
        printf("size: %llu\nis folder? %s\n", props.size, (props.flags & FilePropertyFlag_IsFolder)?"yes":"no");
    }
    
    // delete
    printf("delete:\n");
    os_file_write(str8_lit("bar.txt"), str8_lit("Foo text\n"));
    os_file_delete(str8_lit("bar.txt"));
    
    
    
    {
        FileProperties props = os_file_properties(str8_lit("bar.txt"));
        printf("size: %llu\nis folder? %s\n", props.size, (props.flags & FilePropertyFlag_IsFolder)?"yes":"no");
    }
    
    // rename
    printf("rename:\n");
    os_file_write(str8_lit("bar.txt"), str8_lit("Foo text\n"));
    os_file_delete(str8_lit("foo.bar"));
    os_file_rename(str8_lit("bar.txt"), str8_lit("foo.bar"));
    
    {
        FileProperties props = os_file_properties(str8_lit("bar.txt"));
        printf("size: %llu\nis folder? %s\n", props.size, (props.flags & FilePropertyFlag_IsFolder)?"yes":"no");
    }
    
    {
        FileProperties props = os_file_properties(str8_lit("foo.bar"));
        printf("size: %llu\nis folder? %s\n", props.size, (props.flags & FilePropertyFlag_IsFolder)?"yes":"no");
    }
    
    // make directory
    printf("make directory:\n");
    os_file_make_directory(str8_lit("bar"));
    
    {
        FileProperties props = os_file_properties(str8_lit("bar"));
        printf("size: %llu\nis folder? %s\n", props.size, (props.flags & FilePropertyFlag_IsFolder)?"yes":"no");
    }
    
    // delete directory
    printf("delete directory:\n");
    os_file_delete_directory(str8_lit("bar"));
    
    {
        FileProperties props = os_file_properties(str8_lit("bar"));
        printf("size: %llu\nis folder? %s\n", props.size, (props.flags & FilePropertyFlag_IsFolder)?"yes":"no");
    }
    
    // file iterator
    printf("file iterator:\n");
    os_file_make_directory(str8_lit("bar"));
    os_file_write(str8_lit("bar/foo.txt"), str8_lit("Bar Foo text\n"));
    os_file_write(str8_lit("bar/bar.txt"), str8_lit("Bar Bar text\n"));
    os_file_write(str8_lit("bar/foo.bar"), str8_lit("Bar Foo Bar\n"));
    
    OS_FileIter iter = os_file_iter_init(str8_lit("bar"));
    String8 name = {};
    FileProperties props = {}; 
    for (;os_file_iter_next(scratch, &iter, &name, &props);){
        printf(" %.*s\n", str8_expand(name));
    }
    os_file_iter_end(&iter);
}

