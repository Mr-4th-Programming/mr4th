////////////////////////////////
// NOTE(allen): Play Wave From Temp Path

#include "base/base_inc.h"
#include "os/os_inc.h"

#include "temp_wave.h"

#include "base/base_inc.cpp"
#include "os/os_inc.cpp"

#include "temp_wave.cpp"

int main(int argc, char **argv){
    os_main_init(argc, argv);
    M_ArenaTemp scratch = m_get_scratch(0, 0);
    String8 temp_file = temp_wave_file_name(scratch.arena);
    PlaySoundA((char*)temp_file.str, 0, SND_FILENAME|SND_NODEFAULT);
}
