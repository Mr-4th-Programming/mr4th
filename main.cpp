#include "base/base_inc.h"
#include "os/os_inc.h"

#include "wave/wave_format.h"
#include "wave/wave_parser.h"


#include "base/base_inc.cpp"
#include "os/os_inc.cpp"

#include "wave/wave_parser.cpp"

#include <stdio.h>

int main(int argc, char **argv){
    OS_ThreadContext tctx_memory = {};
    os_main_init(&tctx_memory, argc, argv);
    
    M_Scratch scratch;
    
    String8 data = os_file_read(scratch, str8_lit("Binary Main Frame.wav"));
    
    printf("sizeof 'Binary Main Frame.wav' = %llu\n", data.size);
    
    WAVE_SubChunkList wave_chunks = wave_sub_chunks_from_data(scratch, data);
    printf("wave chunk count: %llu\n", wave_chunks.count);
    
    U64 chunk_counter = 0;
    for (WAVE_SubChunkNode *node = wave_chunks.first;
         node != 0;
         node = node->next, chunk_counter += 1){
        printf("chunk #%llu:\n", chunk_counter);
        printf(" id:   %.*s\n", ExpandAsciiID(node->id));
        printf(" size: %u\n", node->size);
        printf(" off:  0x%llx\n", node->off);
    }
}

