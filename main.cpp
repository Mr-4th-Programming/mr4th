#include "base/base_inc.h"
#include "os/os_inc.h"

#include "wave/riff_format.h"
#include "wave/wave_format.h"
#include "wave/wave_parser.h"


#include "base/base_inc.cpp"
#include "os/os_inc.cpp"

#include "wave/wave_parser.cpp"

#include <stdio.h>

void
mix(F32 *dst, F32 *src, U64 sample_count){
    F32 *opl = src + sample_count;
    for (; src < opl; src += 1, dst += 1){
        *dst += *src;
    }
}

void
compress(F32 *buf, U64 sample_count, F32 mix_count){
    F32 mul = (1.f/mix_count);
    F32 *ptr = buf;
    F32 *opl = buf + sample_count;
    for (; ptr < opl; ptr += 1){
        F32 x = *ptr;
        *ptr = Clamp(-1.f, x*mul, 1.f);
    }
}

int main(int argc, char **argv){
    OS_ThreadContext tctx_memory = {};
    os_main_init(&tctx_memory, argc, argv);
    
    M_Scratch scratch;
    
#if 0
    // sin wave
    F32 theta = s*tau_F32;
    *out = sin_F32(theta);
#endif
    
#if 0
    // square wave
    B32 up = (s < 0.5f);
    *out = (up?1.f:-1.f);
#endif
    
#if 0
    // saw wave
    *out = (1.f - 2.f*s);
#endif
    
#if 0
    // triangle wave
    B32 up = (s < 0.5f);
    *out = (up?(1.f - 4.f*s):(4.f*s - 3));
#endif
    
#if 0
    // squared sine wave
    F32 theta = s*tau_F32;
    F32 x = sin_F32(theta);
    F32 abs_x = abs_F32(x);
    *out = x*abs_x;
#endif
    
    // generate move 440hz sine wave
    {
        WAVE_RenderParams params = {};
        params.kind = WAVE_RenderKind_Float32;
        params.channel_count = 2;
        params.block_per_second = 44100;
        
        U64 seconds = 4;
        U64 sample_count = params.block_per_second*seconds;
        
        F32 *samples_1 = push_array(scratch, F32, sample_count);
        {
            F32 *out = samples_1;
            F32 s = 0.f;
            F32 freq_s = 440.f;
            F32 delta_s = freq_s/44100.f;
            
            F32 t = 0.f;
            F32 delta_t = 1.f/44100.f;
            for (U64 i = 0; i < sample_count; i += 1){
                F32 theta = s*tau_F32;
                *out = sin_F32(theta);
                
                out += 1;
                s += delta_s;
                if (s > 1.f){
                    s -= 1.f;
                }
                t += delta_t;
            }
        }
        
        F32 *samples_2 = push_array(scratch, F32, sample_count);
        {
            F32 *out = samples_2;
            F32 s = 0.f;
            F32 freq_s = 587.33f;
            F32 delta_s = freq_s/44100.f;
            
            F32 t = 0.f;
            F32 delta_t = 1.f/44100.f;
            for (U64 i = 0; i < sample_count; i += 1){
                F32 theta = s*tau_F32;
                *out = sin_F32(theta);
                
                out += 1;
                s += delta_s;
                if (s > 1.f){
                    s -= 1.f;
                }
                t += delta_t;
            }
        }
        
        F32 *samples_3 = push_array(scratch, F32, sample_count);
        {
            F32 *out = samples_3;
            F32 s = 0.f;
            F32 freq_s = 880.f;
            F32 delta_s = freq_s/44100.f;
            
            F32 t = 0.f;
            F32 delta_t = 1.f/44100.f;
            for (U64 i = 0; i < sample_count; i += 1){
                F32 theta = s*tau_F32;
                *out = sin_F32(theta);
                
                out += 1;
                s += delta_s;
                if (s > 1.f){
                    s -= 1.f;
                }
                t += delta_t;
            }
        }
        
        F32 *samples = push_array_zero(scratch, F32, sample_count);
        mix(samples, samples_1, sample_count);
        mix(samples, samples_2, sample_count);
        mix(samples, samples_3, sample_count);
        compress(samples, sample_count, 3.f);
        
        void *ins[] = {samples, samples};
        String8 interleaved = bop_interleave(scratch, ins, 2, 4, sample_count);
        
        String8 wave_file_data = wave_render(scratch, &params, interleaved);
        os_file_write(str8_lit("gen_4s_harm_440hz_587hz_880hz_sine.wav"), wave_file_data);
    }
    
}

