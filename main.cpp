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

////////////////////////////////
// NOTE(allen): Main

int main(int argc, char **argv){
    OS_ThreadContext tctx_memory = {};
    os_main_init(&tctx_memory, argc, argv);
    
    M_Scratch scratch;
    
    enum{
        Voice_Sin,
        Voice_Sqr,
        Voice_Saw,
        Voice_Tri,
        Voice_COUNT,
    };
    
    // generate 440hz sine wave
    {
        WAVE_RenderParams params = {};
        params.kind = WAVE_RenderKind_Signed0 + 16;
        params.channel_count = 2;
        params.block_per_second = 44100;
        
        AUDGEN_Rate rate = audgen_rate(params.block_per_second);
        
        // top-level constants
        U64 seconds = 1;
        F32 boop_length = 0.2f;
        F32 chime_anticipation = 0.01f;
        F32 chime_length = 0.04f;
        
        U64 sample_count = params.block_per_second*seconds;
        
        // timeline
        AUDGEN_Timeline timeline = {};
        
        // first buffer
        F32 *samples_1 = push_array(scratch, F32, sample_count);
        AUDGEN_Buffer buf1 = {samples_1, sample_count, rate};
        audgen_timeline_push(scratch, &timeline, samples_1, sample_count, 0);
        
        {
            MemoryZero(samples_1, sizeof(*samples_1)*sample_count);
            
            F32 tone_min_t = 0.f;
            F32 tone_max_t = boop_length;
            I1F32 tone_range_t = i1f32(tone_min_t, tone_max_t);
            
            F32 freq_min_s = 220.f;
            F32 freq_max_s = 440.f;
            
            AUDGEN_ADSRCurve adsr = {};
            adsr.base_db    = -5.f;    // dB
            adsr.attack_s   =  0.009f; // s
            adsr.decay_s    =  0.100f; // s
            adsr.sustain_db = -10.f;   // dB
            adsr.release_s  =  0.100f; // s
            
            AUDGEN_SignalParams par[Voice_COUNT];
            
            par[Voice_Sin].mix = i1f32(0.714f, 0.714f);
            par[Voice_Saw].mix = i1f32(0.048f, 0.048f);
            par[Voice_Tri].mix = i1f32(0.238f, 0.238f);
            
            par[Voice_Sin].freq = i1f32(freq_min_s, freq_max_s);
            par[Voice_Saw].freq = i1f32(freq_min_s/audgen_half_step, freq_max_s);
            par[Voice_Tri].freq = i1f32(freq_min_s/audgen_half_step, freq_max_s);
            
            audgen_mix_shape_sin(buf1, tone_range_t, &par[Voice_Sin]);
            audgen_mix_shape_saw(buf1, tone_range_t, &par[Voice_Saw]);
            audgen_mix_shape_tri(buf1, tone_range_t, &par[Voice_Tri]);
            
            audgen_mul_adsr(buf1, &adsr, tone_range_t.min, tone_range_t.max,
                            i1f32(0.f, 1.f));
        }
        
        // second buffer
        U64 samples_2_count = audgen_i_from_t(rate, chime_length + 0.15f);
        F32 *samples_2 = push_array(scratch, F32, samples_2_count);
        AUDGEN_Buffer buf2 = {samples_2, samples_2_count, rate};
        
        F32 samples_2_min_t = boop_length - chime_anticipation;
        U64 samples_2_min_i = audgen_i_from_t(rate, samples_2_min_t);
        audgen_timeline_push(scratch, &timeline, samples_2, samples_2_count,
                             samples_2_min_i);
        
        MemoryZero(samples_2, sizeof(*samples_2)*sample_count);
        
        {
            F32 tone_min_t = 0;
            F32 tone_max_t = chime_length;
            I1F32 tone_range_t = i1f32(tone_min_t, tone_max_t);
            
            F32 freq_min_s = 440.f*pow_F32(audgen_half_step, 0.5f);
            F32 freq_max_s = 440.f;
            
            AUDGEN_ADSRCurve adsr = {};
            adsr.base_db    = -1.f;    //dB
            adsr.attack_s   =  0.090f; //s
            adsr.decay_s    =  0.100f; //s
            adsr.sustain_db = -4.f;    //dB
            adsr.release_s  =  0.060f; //s
            
            AUDGEN_SignalParams par[Voice_COUNT];
            
            par[Voice_Sin].mix = i1f32(0.800f, 0.800f);
            par[Voice_Tri].mix = i1f32(0.200f, 0.200f);
            
            par[Voice_Sin].freq = i1f32(freq_min_s, freq_max_s);
            par[Voice_Tri].freq = i1f32(freq_min_s, freq_max_s);
            
            audgen_mix_shape_sin(buf2, tone_range_t, &par[Voice_Sin]);
            audgen_mix_shape_tri(buf2, tone_range_t, &par[Voice_Tri]);
            
            audgen_mul_adsr(buf2, &adsr, tone_range_t.min, tone_range_t.max,
                            i1f32(0.f, 1.f));
        }
        
        // render timeline
        F32 *samples = audgen_timeline_render(scratch, &timeline);
        
        {
            // render s16 buffer
            String8 samples_f32 = str8((U8*)samples, 4*sample_count);
            String8 samples_s16 = bop_s16_from_f32(scratch, samples_f32);
            
            // render stereo interleaved buffer
            void *ins[] = {samples_s16.str, samples_s16.str};
            String8 interleaved = bop_interleave(scratch, ins, 2, 2, sample_count);
            
            // render wave file
            String8 wave_file_data = wave_render(scratch, &params, interleaved);
            
            // save wave file
            String8 file_name = str8_lit("gen_cool_sound_5.wav");
            os_file_write(file_name, wave_file_data);
        }
        
        {
            // window the samples
            I1F32 range_t = i1f32(0.1f, 0.16f);
            I1U64 range_i = audgen_i_from_t(rate, range_t);
            range_i = intr_clamp_top(range_i, sample_count);
            F32 *window_samples = samples + range_i.min;
            U64 window_sample_count = range_i.max - range_i.min;
            
            // render s16 buffer
            String8 samples_f32 = str8((U8*)window_samples, 4*window_sample_count);
            String8 samples_s16 = bop_s16_from_f32(scratch, samples_f32);
            
            // render stereo interleaved buffer
            void *ins[] = {samples_s16.str, samples_s16.str};
            String8 interleaved = bop_interleave(scratch, ins, 2, 2,
                                                 window_sample_count);
            
            // render wave file
            String8 wave_file_data = wave_render(scratch, &params, interleaved);
            
            // save wave file
            String8 temp_file_name = temp_wave_file_name(scratch);
            os_file_write(temp_file_name, wave_file_data);
        }
    }
}

