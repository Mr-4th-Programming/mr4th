#include "base/base_inc.h"
#include "os/os_inc.h"

#include "wave/riff_format.h"
#include "wave/wave_format.h"
#include "wave/wave_parser.h"


#include "base/base_inc.cpp"
#include "os/os_inc.cpp"

#include "wave/wave_parser.cpp"

#include <stdio.h>

struct SignalParams{
    I1F32 mix;
    I1F32 bend;
};

struct SignalVars{
    F32 s;
};

enum{
    Voice_Sin,
    Voice_Sqr,
    Voice_Saw,
    Voice_Tri,
    Voice_COUNT,
};

F32
mul_from_decibel(F32 db){
    F32 div_db = db/10.f;
    F32 exp_db = pow_F32(10.f, div_db);
    return(exp_db);
}

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
    
    // generate 440hz sine wave
    {
        WAVE_RenderParams params = {};
        params.kind = WAVE_RenderKind_Float32;
        params.channel_count = 2;
        params.block_per_second = 44100;
        
        // universal constants
        F32 silent_db = -40.f;
        F32 half_step = pow_F32(2.f, 1.f/12.f);
        
        // top-level constants
        U64 seconds = 1;
        F32 boop_length = 0.2f;
        F32 chime_anticipation = 0.01f;
        F32 chime_length = 0.04f;
        
        U64 sample_count = params.block_per_second*seconds;
        
        F32 *samples_1 = push_array(scratch, F32, sample_count);
        {
            MemoryZero(samples_1, sizeof(*samples_1)*sample_count);
            
            F32 tone_min_t = 0.f;
            F32 tone_max_t = boop_length;
            
            F32 freq_min_s = 220.f;
            F32 freq_max_s = 440.f;
            
            F32 base_db    = -5.f;    // dB
            F32 attack_s   =  0.009f; // s
            F32 decay_s    =  0.100f; // s
            F32 sustain_db = -10.f;   // dB
            F32 release_s  =  0.100f; // s
            
            SignalParams par[Voice_COUNT];
            
            par[Voice_Sin].mix = i1f32(0.714f, 0.714f);
            par[Voice_Sqr].mix = i1f32(0.000f, 0.000f);
            par[Voice_Saw].mix = i1f32(0.048f, 0.048f);
            par[Voice_Tri].mix = i1f32(0.238f, 0.238f);
            
            par[Voice_Sin].bend = i1f32(1.f, 1.f);
            par[Voice_Sqr].bend = i1f32(1.f, 1.f);
            par[Voice_Saw].bend = i1f32(1.f/half_step, 1.f);
            par[Voice_Tri].bend = i1f32(1.f/half_step, 1.f);
            
            SignalVars var[Voice_COUNT];
            for (U32 i = 0; i < ArrayCount(var); i += 1){
                var[i].s = 0.f;
            }
            
            F32 *out = samples_1;
            F32 delta_t = 1.f/44100.f;
            F32 t = 0.f;
            for (U64 i = 0; i < sample_count; i += 1){
                // build shapes
                F32 v[Voice_COUNT];
                
                F32 sin_theta = var[Voice_Sin].s*tau_F32;
                v[Voice_Sin] = sin_F32(sin_theta);
                
                B32 sqr_up = (var[Voice_Sqr].s < 0.5f);
                v[Voice_Sqr] = (sqr_up?1.f:-1.f);
                
                v[Voice_Saw] = (1.f - 2.f*var[Voice_Saw].s);
                
                B32 tri_up = (var[Voice_Tri].s < 0.5f);
                v[Voice_Tri] = (tri_up?
                                (1.f - 4.f*var[Voice_Tri].s):
                                (4.f*var[Voice_Tri].s - 3));
                
                // tone s
                F32 tone_s = Clamp(0.f, unlerp(tone_min_t, t, tone_max_t), 1.f);
                
                // combine
                F32 mixed = 0;
                {
                    F32 *vptr = v;
                    SignalParams *parptr = par;
                    for (U64 i = 0; i < ArrayCount(par); i += 1, vptr += 1, parptr += 1){
                        F32 mix = lerp_range(parptr->mix, tone_s);
                        mixed += (*vptr)*mix;
                    }
                }
                
                // attack-decay-sustain-release
                F32 adsr_m = 0.f;
                if (tone_min_t <= t){
                    F32 adsr_t = t - tone_min_t;
                    F32 adsr_tail_t = t - tone_max_t;
                    F32 attack_m  = ClampTop(adsr_t/attack_s, 1.f);
                    F32 decay_m   = ClampTop(adsr_t/decay_s, 1.f);
                    F32 release_m = 1.f - Clamp(0.f, adsr_tail_t/release_s, 1.f);
                    F32 decay_db =   base_db + (sustain_db -   base_db)*decay_m;
                    F32 fxv_db   = silent_db + (  decay_db - silent_db)*attack_m*release_m;
                    adsr_m = mul_from_decibel(fxv_db);
                }
                
                // output
                *out = mixed*adsr_m;
                
                // center freq
                F32 center_freq_s = lerp(freq_min_s, tone_s, freq_max_s);
                
                // increment voices
                {
                    SignalVars *varptr = var;
                    SignalParams *parptr = par;
                    for (U64 i = 0; i < ArrayCount(var); i += 1, varptr += 1, parptr += 1){
                        F32 freq_mul = lerp_range(parptr->bend, tone_s);
                        F32 freq_s = freq_mul*center_freq_s;
                        F32 delta_s = freq_s/44100.f;
                        varptr->s += delta_s;
                        if (varptr->s > 1.f){
                            varptr->s -= 1.f;
                        }
                    }
                }
                
                // increment track
                t += delta_t;
                out += 1;
            }
        }
        
        F32 *samples_2 = push_array(scratch, F32, sample_count);
        MemoryZero(samples_2, sizeof(*samples_2)*sample_count);
#if 1
        {
            
            F32 tone_min_t = boop_length - chime_anticipation;
            F32 tone_max_t = tone_min_t + chime_length;
            
            F32 freq_min_s = 440.f*pow_F32(half_step, 0.5f);
            F32 freq_max_s = 440.f;
            
            F32 base_db    = -1.f;    //dB
            F32 attack_s   =  0.090f; //s
            F32 decay_s    =  0.100f; //s
            F32 sustain_db = -4.f;    //dB
            F32 release_s  =  0.060f; //s
            
            SignalParams par[Voice_COUNT];
            
#if 0            
            par[Voice_Sin].mix = i1f32(0.714f, 0.714f);
            par[Voice_Sqr].mix = i1f32(0.000f, 0.000f);
            par[Voice_Saw].mix = i1f32(0.048f, 0.048f);
            par[Voice_Tri].mix = i1f32(0.238f, 0.238f);
#endif
            
            par[Voice_Sin].mix = i1f32(0.800f, 0.800);
            par[Voice_Sqr].mix = i1f32(0.000f, 0.000f);
            par[Voice_Saw].mix = i1f32(0.000f, 0.000f);
            par[Voice_Tri].mix = i1f32(0.200f, 0.200f);
            
            par[Voice_Sin].bend = i1f32(1.f, 1.f);
            par[Voice_Sqr].bend = i1f32(1.f, 1.f);
            par[Voice_Saw].bend = i1f32(1.f, 1.f);
            par[Voice_Tri].bend = i1f32(1.f, 1.f);
            
            SignalVars var[Voice_COUNT];
            for (U32 i = 0; i < ArrayCount(var); i += 1){
                var[i].s = 0.f;
            }
            
            F32 *out = samples_2;
            F32 delta_t = 1.f/44100.f;
            F32 t = 0.f;
            for (U64 i = 0; i < sample_count; i += 1){
                // build shapes
                F32 v[Voice_COUNT];
                
                F32 sin_theta = var[Voice_Sin].s*tau_F32;
                v[Voice_Sin] = sin_F32(sin_theta);
                
                B32 sqr_up = (var[Voice_Sqr].s < 0.5f);
                v[Voice_Sqr] = (sqr_up?1.f:-1.f);
                
                v[Voice_Saw] = (1.f - 2.f*var[Voice_Saw].s);
                
                B32 tri_up = (var[Voice_Tri].s < 0.5f);
                v[Voice_Tri] = (tri_up?
                                (1.f - 4.f*var[Voice_Tri].s):
                                (4.f*var[Voice_Tri].s - 3));
                
                // tone s
                F32 tone_s = Clamp(0.f, unlerp(tone_min_t, t, tone_max_t), 1.f);
                
                // combine
                F32 mixed = 0;
                {
                    F32 *vptr = v;
                    SignalParams *parptr = par;
                    for (U64 i = 0; i < ArrayCount(par); i += 1, vptr += 1, parptr += 1){
                        F32 mix = lerp_range(parptr->mix, tone_s);
                        mixed += (*vptr)*mix;
                    }
                }
                
                // attack-decay-sustain-release
                F32 adsr_m = 0.f;
                if (tone_min_t <= t){
                    F32 adsr_t = t - tone_min_t;
                    F32 adsr_tail_t = t - tone_max_t;
                    F32 attack_m  = ClampTop(adsr_t/attack_s, 1.f);
                    F32 decay_m   = ClampTop(adsr_t/decay_s, 1.f);
                    F32 release_m = 1.f - Clamp(0.f, adsr_tail_t/release_s, 1.f);
                    F32 decay_db =   base_db + (sustain_db -   base_db)*decay_m;
                    F32 fxv_db   = silent_db + (  decay_db - silent_db)*attack_m*release_m;
                    adsr_m = mul_from_decibel(fxv_db);
                }
                
                // output
                *out = mixed*adsr_m;
                
                // center freq
                F32 center_freq_s = lerp(freq_min_s, tone_s, freq_max_s);
                
                // increment voices
                {
                    SignalVars *varptr = var;
                    SignalParams *parptr = par;
                    for (U64 i = 0; i < ArrayCount(var); i += 1, varptr += 1, parptr += 1){
                        F32 freq_mul = lerp_range(parptr->bend, tone_s);
                        F32 freq_s = freq_mul*center_freq_s;
                        F32 delta_s = freq_s/44100.f;
                        varptr->s += delta_s;
                        if (varptr->s > 1.f){
                            varptr->s -= 1.f;
                        }
                    }
                }
                
                // increment track
                t += delta_t;
                out += 1;
            }
        }
#endif
        
        F32 *samples = push_array(scratch, F32, sample_count);
        MemoryZero(samples, sizeof(*samples)*sample_count);
        mix(samples, samples_1, sample_count);
        mix(samples, samples_2, sample_count);
        compress(samples, sample_count, 2);
        
        void *ins[] = {samples, samples};
        String8 interleaved = bop_interleave(scratch, ins, 2, 4, sample_count);
        
        String8 wave_file_data = wave_render(scratch, &params, interleaved);
        String8 file_name =
            str8_lit("gen_cool_sound_5.wav");
        os_file_write(file_name, wave_file_data);
    }
    
}

