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
}
