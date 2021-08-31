/* date = July 17th 2021 7:52 pm */

#ifndef AUDIO_GEN_H
#define AUDIO_GEN_H

////////////////////////////////
// TODO(allen): Notes, Unfinished Issues, Etc.
// [ ] Do we want a 12tet half step as a constant?
//     Seems like it might make more sense to bundle this
//     with some kind of notion of "scale information"
// [ ] Same idea as above but with all those hand written
//     scale constants (kind of a different problem, but it still
//     hasn't been deeply thought through).
//     [ ] If we do want to keep these, what's a good naming scheme?
// [ ] Could the timeline automate a dynamic compression parameter?
//     Right now we can only apply compression to ranges of samples
//     manually, with one parameter per range. But the timeline
//     system knows more about how many active voices there are at
//     each given point, maybe we want it to be involved.
// [ ] Inconsistencies in the APIs are still all over the place.
//     The compressor takes in (F32 *buf, U64 sample_count) but the
//     filters take in (AUDGEN_Buffer buf, I1F32 time_range), etc.
// [ ] The signal shapes generators have a lot of duplicate code
//     that is annoying to maintain, but deduplicating it has a lot
//     of problems too. This seems like an interesting place to mess
//     with codegen perhaps?
//     [ ] The linear blend vs exponential blend option is a good
//         example of the bigger problem. I want to mix and match
//         small pieces of the formulas at different times.
//         Sometimes do pitch bend fast, sometimes do it accurate,
//         sometimes not at all. But I don't want to involve an
//         abstraction/branch inside the loop. Not just because of
//         performance but because it would create a big complicated
//         knotted-up "super function" trying to do everything.
// [ ] It is unclear if the timeline/buffer/rate relationship has
//     been fully stream-lined.
// [ ] Resampling (change of rate)
// [ ] There might be upgrades for pitch tables involving better
//     numerical results, better speed, and narrowing the table
//     size to match the demand.
// [ ] Timeline invariants are not quite "complete" - upgrading this
//     is probably blocked on getting a little more completion on the
//     timeline itself

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
// NOTE(allen): Scale Types

// lingo: tet - "tone equal temperament"
// Should be combined with a number like: "12 tone equal temperament".

enum{
    AUDGEN_Pitch12TET_A ,
    AUDGEN_Pitch12TET_As, AUDGEN_Pitch12TET_Bf  = AUDGEN_Pitch12TET_As,
    AUDGEN_Pitch12TET_B ,
    AUDGEN_Pitch12TET_C ,
    AUDGEN_Pitch12TET_Cs, AUDGEN_Pitch12TET_Df  = AUDGEN_Pitch12TET_Cs,
    AUDGEN_Pitch12TET_D ,
    AUDGEN_Pitch12TET_Ds, AUDGEN_Pitch12TET_Ef  = AUDGEN_Pitch12TET_Ds,
    AUDGEN_Pitch12TET_E ,
    AUDGEN_Pitch12TET_F ,
    AUDGEN_Pitch12TET_Fs, AUDGEN_Pitch12TET_Gf  = AUDGEN_Pitch12TET_Fs,
    AUDGEN_Pitch12TET_G ,
    AUDGEN_Pitch12TET_Gs, AUDGEN_Pitch12TET_Af2 = AUDGEN_Pitch12TET_Gs,
};

enum{
    AUDGEN_Pitch10TET_A ,
    AUDGEN_Pitch10TET_As, AUDGEN_Pitch10TET_Bf = AUDGEN_Pitch10TET_As,
    AUDGEN_Pitch10TET_B ,
    AUDGEN_Pitch10TET_Bs, AUDGEN_Pitch10TET_Cf = AUDGEN_Pitch10TET_Bs,
    AUDGEN_Pitch10TET_C ,
    AUDGEN_Pitch10TET_Cs, AUDGEN_Pitch10TET_Df = AUDGEN_Pitch10TET_Cs,
    AUDGEN_Pitch10TET_D ,
    AUDGEN_Pitch10TET_Ds, AUDGEN_Pitch10TET_Ef = AUDGEN_Pitch10TET_Ds,
    AUDGEN_Pitch10TET_E ,
    AUDGEN_Pitch10TET_Es, AUDGEN_Pitch10TET_Af = AUDGEN_Pitch10TET_Es,
};

struct AUDGEN_NTETParams{
    U32 n;
    F32 mid_freq;
    U32 mid_tone;
    U32 opl_tone;
};

struct AUDGEN_PitchTable{
    F32 *pitch;
};

////////////////////////////////
// NOTE(allen): Audio Generation Decibel Math

function F32 audgen_mul_from_decibel(F32 db);

////////////////////////////////
// NOTE(allen): Audio Generation Rate

function AUDGEN_Rate audgen_rate(F32 sample_per_second);

function U64   audgen_i_from_t(AUDGEN_Rate rate, F32 t);
function I1U64 audgen_i_from_t(AUDGEN_Rate rate, I1F32 t);

////////////////////////////////
// NOTE(allen): Audio Generation Mix

function void audgen_mix(F32 *dst, F32 *src, U64 sample_count);

////////////////////////////////
// NOTE(allen): Audio Generation Compressors

function void audgen_compress_lin(F32 *buf, U64 sample_count, F32 mix_count);

////////////////////////////////
// NOTE(allen): Audio Generation Filters

function void audgen_low_pass_in_place(AUDGEN_Buffer buf, I1F32 range_t,
                                       F32 cutoff_freq);
function void audgen_high_pass_in_place(AUDGEN_Buffer buf, I1F32 range_t,
                                        F32 cutoff_freq);

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
function void audgen_mix_shape_white_noise(AUDGEN_Buffer buf, I1F32 range_t,
                                           U64 seed, F32 volume_linear);

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

function void audgen_assert_timeline_invariants(AUDGEN_Timeline *timeline);

////////////////////////////////
// NOTE(allen): Scale Functions

function AUDGEN_PitchTable
audgen_get_ntet_pitch_table(M_Arena *arena, AUDGEN_NTETParams *params);

#endif //AUDIO_GEN_H
