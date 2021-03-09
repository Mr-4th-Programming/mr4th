#include "base/base_inc.h"
#include "os/os_inc.h"


#include "base/base_inc.cpp"
#include "os/os_inc.cpp"

#include <stdio.h>
#include <stdlib.h>

#include "base/base_memory_malloc.cpp"

int main(){
    M_BaseMemory *os_base = os_base_memory();
    M_Arena arena = m_make_arena(os_base);
    
    int *foo = push_array(&arena, int, 1000);
    foo[999] = 0;
    
    printf("arena: %llu %llu %llu\n", arena.pos, arena.commit_pos, arena.cap);
}
