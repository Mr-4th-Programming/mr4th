#include "base/base_inc.h"
#include "os/os_inc.h"


#include "base/base_inc.cpp"
#include "os/os_inc.cpp"

#include <stdio.h>
#include <stdlib.h>

#include "base/base_memory_malloc.cpp"

int main(int argc, char **argv){
    OS_ThreadContext tctx_memory = {};
    os_main_init(&tctx_memory, argc, argv);
    
    {
        String8List cmd_line = os_command_line_arguments();
        U64 counter = 0;
        for (String8Node *node = cmd_line.first;
             node != 0;
             node = node->next, counter += 1){
            printf("%2llu: '%.*s'\n", counter, str8_expand(node->string));
        }
    }
    
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
        
        DateTime time = date_time_from_dense_time(props.create_time);
        printf("%04d, %02d, %02d; %02d:%02d:%02d.%03d\n",
               time.year, time.mon, time.day,
               time.hour, time.min, time.sec, time.msec);
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
    
    // date time encode/decode test
    {
        DateTime date_time = {};
        date_time.msec = 501;
        date_time.sec = 1;
        date_time.min = 0;
        date_time.hour = 23;
        date_time.day = 0;
        date_time.mon = 10;
        date_time.year = -100;
        
        DenseTime dense = dense_time_from_date_time(&date_time);
        DateTime decoded = date_time_from_dense_time(dense);
        
        Assert(date_time.msec == decoded.msec);
        Assert(date_time.sec == decoded.sec);
        Assert(date_time.min == decoded.min);
        Assert(date_time.hour == decoded.hour);
        Assert(date_time.day == decoded.day);
        Assert(date_time.mon == decoded.mon);
        Assert(date_time.year == decoded.year);
    }
    
    // now date time; local time conversion
    {
        DateTime now = os_now_universal_time();
        printf("%04d, %02d, %02d; %02d:%02d:%02d.%03d\n",
               now.year, now.mon, now.day,
               now.hour, now.min, now.sec, now.msec);
        
        DateTime now_local = os_local_time_from_universal(&now);
        printf("%04d, %02d, %02d; %02d:%02d:%02d.%03d\n",
               now_local.year, now_local.mon, now_local.day,
               now_local.hour, now_local.min, now_local.sec, now_local.msec);
        
        DateTime round_trip = os_universal_time_from_local(&now_local);
        Assert(now.msec == round_trip.msec);
        Assert(now.sec == round_trip.sec);
        Assert(now.min == round_trip.min);
        Assert(now.hour == round_trip.hour);
        Assert(now.day == round_trip.day);
        Assert(now.mon == round_trip.mon);
        Assert(now.year == round_trip.year);
    }
    
    String8 cwd = os_file_path(scratch, OS_SystemPath_CurrentDirectory);
    printf("cwd: %.*s\n", str8_expand(cwd));
    
    String8 bin_path = os_file_path(scratch, OS_SystemPath_Binary);
    printf("bin_path: %.*s\n", str8_expand(bin_path));
    
    String8 user_path = os_file_path(scratch, OS_SystemPath_UserData);
    printf("user_path: %.*s\n", str8_expand(user_path));
    
    String8 temp_path = os_file_path(scratch, OS_SystemPath_TempData);
    printf("temp_path: %.*s\n", str8_expand(temp_path));
}

