#include "base/base_inc.h"
#include "os/os_inc.h"


#include "base/base_inc.cpp"
#include "os/os_inc.cpp"

#include <stdio.h>

int main(int argc, char **argv){
    OS_ThreadContext tctx_memory = {};
    os_main_init(&tctx_memory, argc, argv);
    
}

