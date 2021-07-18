/* date = July 17th 2021 7:52 pm */

#ifndef AUDIO_GEN_H
#define AUDIO_GEN_H

////////////////////////////////
// NOTE(allen): Audio Generation Constants

global F32 audgen_silent_db = -40.f;
global F32 audgen_half_step = pow_F32(2.f, 1.f/12.f);

////////////////////////////////
// NOTE(allen): Audio Generation Types

struct AUDGEN_Rate{
    F32 sample_per_second;
    F32 delta_t;
};

struct AUDGEN_Buffer{
    F32 *buf;
    U64 count;
    AUDGEN_Rate rate;
};

struct AUDGEN_ADSRCurve{
    F32 base_db;
    F32 attack_s;
    F32 decay_s;
    F32 sustain_db;
    F32 release_s;
};

struct AUDGEN_SignalParams{
    I1F32 mix;
    I1F32 freq;
};

////////////////////////////////
// NOTE(allen): Audio Generation Timeline

struct AUDGEN_TimelineSound{
    AUDGEN_TimelineSound *next;
    F32 *buf;
    U64 count;
    U64 first;
};

struct AUDGEN_Timeline{
    AUDGEN_TimelineSound *first;
    AUDGEN_TimelineSound *last;
    U64 sound_count;
    U64 sample_count;
};

////////////////////////////////
// NOTE(allen): Audio Generation Decibel Math

function F32  audgen_mul_from_decibel(F32 db);

////////////////////////////////
// NOTE(allen): Audio Generation Rate

function AUDGEN_Rate   audgen_rate(F32 sample_per_second);

function U64   audgen_i_from_t(AUDGEN_Rate rate, F32 t);
function I1U64 audgen_i_from_t(AUDGEN_Rate rate, I1F32 t);

////////////////////////////////
// NOTE(allen): Audio Generation Mix

function void audgen_mix(F32 *dst, F32 *src, U64 sample_count);

////////////////////////////////
// NOTE(allen): Audio Generation Compressors

function void audgen_compress_lin(F32 *buf, U64 sample_count, F32 mix_count);

////////////////////////////////
// NOTE(allen): Audio Generation Signal Shapes

function void audgen_mix_shape_sin(AUDGEN_Buffer buf,
                                   I1F32 range_t, AUDGEN_SignalParams *params);
function void audgen_mix_shape_sqr(AUDGEN_Buffer buf,
                                   I1F32 range_t, AUDGEN_SignalParams *params);
function void audgen_mix_shape_saw(AUDGEN_Buffer buf,
                                   I1F32 range_t, AUDGEN_SignalParams *params);
function void audgen_mix_shape_tri(AUDGEN_Buffer buf,
                                   I1F32 range_t, AUDGEN_SignalParams *params);

////////////////////////////////
// NOTE(allen): Audio Generation ADSR Functions

function F32  audgen_sample_adsr(AUDGEN_ADSRCurve *adsr, F32 t, F32 max_t);
function void audgen_mul_adsr(AUDGEN_Buffer buf, AUDGEN_ADSRCurve *adsr,
                              F32 attack_t, F32 release_t, I1F32 range_t);

////////////////////////////////
// NOTE(allen): Audio Generation Timeline

function void audgen_timeline_push(M_Arena *arena, AUDGEN_Timeline *l,
                                   F32 *buf, U64 count, U64 first_i);
function F32* audgen_timeline_render(M_Arena *arena, AUDGEN_Timeline *l);

#endif //AUDIO_GEN_H
