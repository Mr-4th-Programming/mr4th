#include "base/base_inc.h"
#include "os/os_inc.h"

#include "wave/riff_format.h"
#include "wave/wave_format.h"
#include "wave/wave_parser.h"

#include "temp_wave.h"
#include "audio_gen.h"

#include "base/base_inc.cpp"
#include "os/os_inc.cpp"

#include "wave/wave_parser.cpp"

#include "temp_wave.cpp"
#include "audio_gen.cpp"

#include <stdio.h>

//$ bar baz bar bar baz foo bar foo baz foofoobar //

////////////////////////////////
// NOTE(allen): Main

int main(int argc, char **argv){
    OS_ThreadContext tctx_memory = {};
    os_main_init(&tctx_memory, argc, argv);
    
    M_Scratch scratch;
}

