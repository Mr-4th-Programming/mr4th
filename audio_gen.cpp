////////////////////////////////
// NOTE(allen): Audio Generation Decibel Math

function F32
audgen_mul_from_decibel(F32 db){
    F32 div_db = db/10.f;
    F32 exp_db = pow_F32(10.f, div_db);
    return(exp_db);
}

////////////////////////////////
// NOTE(allen): Audio Generation Rate

function AUDGEN_Rate
audgen_rate(F32 sample_per_second){
    AUDGEN_Rate result = {};
    result.sample_per_second = sample_per_second;
    result.delta_t = 1.f/sample_per_second;
    return(result);
}

function U64
audgen_i_from_t(AUDGEN_Rate rate, F32 t){
    U64 result = (U64)(rate.sample_per_second*t);
    return(result);
}

function I1U64
audgen_i_from_t(AUDGEN_Rate rate, I1F32 t){
    I1U64 result = {};
    result.min = (U64)(rate.sample_per_second*t.min);
    result.max = (U64)(rate.sample_per_second*t.max);
    return(result);
}

////////////////////////////////
// NOTE(allen): Audio Generation Mix

function void
audgen_mix(F32 *dst, F32 *src, U64 sample_count){
    F32 *opl = src + sample_count;
    for (; src < opl; src += 1, dst += 1){
        *dst += *src;
    }
}

////////////////////////////////
// NOTE(allen): Audio Generation Compressors

function void
audgen_compress_lin(F32 *buf, U64 sample_count, F32 mix_count){
    F32 mul = (1.f/mix_count);
    F32 *ptr = buf;
    F32 *opl = buf + sample_count;
    for (; ptr < opl; ptr += 1){
        F32 x = *ptr;
        *ptr = Clamp(-1.f, x*mul, 1.f);
    }
}

////////////////////////////////
// NOTE(allen): Audio Generation Signal Shapes

function void
audgen_mix_shape_sin(AUDGEN_Buffer buf, I1F32 range_t,
                     AUDGEN_SignalParams *params){
    // setup
    I1U64 range_i_unclamped = audgen_i_from_t(buf.rate, range_t);
    I1U64 range_i = intr_clamp_top(range_i_unclamped, buf.count);
    F32 max_t = range_t.max - range_t.min;
    I1F32 param_mix  = params->mix;
    I1F32 param_freq = params->freq;
    F32 s = 0.f;
    F32 t = 0.f;
    F32 *out = buf.buf + range_i.min;
    for (U64 i = range_i.min; i < range_i.max; i += 1){
        
        // THE SHAPE
        // vvvv
        F32 sin_theta = s*tau_F32;
        F32 v = sin_F32(sin_theta);
        // ^^^^
        
        // signal volume control
        F32 tone_s = ClampTop(t/max_t, 1.f);
        F32 mix = lerp_range(param_mix, tone_s);
        // mix in sample
        *out += v*mix;
        // increment shape
        F32 freq_s  = lerp_range(param_freq, tone_s);
        F32 delta_s = freq_s*buf.rate.delta_t;
        s += delta_s;
        if (s > 1.f){
            s -= 1.f;
        }
        // increment track
        t += buf.rate.delta_t;
        out += 1;
    }
}

function void
audgen_mix_shape_sqr(AUDGEN_Buffer buf, I1F32 range_t,
                     AUDGEN_SignalParams *params){
    // setup
    I1U64 range_i_unclamped = audgen_i_from_t(buf.rate, range_t);
    I1U64 range_i = intr_clamp_top(range_i_unclamped, buf.count);
    F32 max_t = range_t.max - range_t.min;
    I1F32 param_mix  = params->mix;
    I1F32 param_freq = params->freq;
    F32 s = 0.f;
    F32 t = 0.f;
    F32 *out = buf.buf + range_i.min;
    for (U64 i = range_i.min; i < range_i.max; i += 1){
        
        // THE SHAPE
        // vvvv
        B32 sqr_up = (s < 0.5f);
        F32 v = (sqr_up?1.f:-1.f);
        // ^^^^
        
        // signal volume control
        F32 tone_s = ClampTop(t/max_t, 1.f);
        F32 mix = lerp_range(param_mix, tone_s);
        // mix in sample
        *out += v*mix;
        // increment shape
        F32 freq_s  = lerp_range(param_freq, tone_s);
        F32 delta_s = freq_s*buf.rate.delta_t;
        s += delta_s;
        if (s > 1.f){
            s -= 1.f;
        }
        // increment track
        t += buf.rate.delta_t;
        out += 1;
    }
}

function void
audgen_mix_shape_saw(AUDGEN_Buffer buf, I1F32 range_t,
                     AUDGEN_SignalParams *params){
    // setup
    I1U64 range_i_unclamped = audgen_i_from_t(buf.rate, range_t);
    I1U64 range_i = intr_clamp_top(range_i_unclamped, buf.count);
    F32 max_t = range_t.max - range_t.min;
    I1F32 param_mix  = params->mix;
    I1F32 param_freq = params->freq;
    F32 s = 0.f;
    F32 t = 0.f;
    F32 *out = buf.buf + range_i.min;
    for (U64 i = range_i.min; i < range_i.max; i += 1){
        
        // THE SHAPE
        // vvvv
        F32 v = (1.f - 2.f*s);
        // ^^^^
        
        // signal volume control
        F32 tone_s = ClampTop(t/max_t, 1.f);
        F32 mix = lerp_range(param_mix, tone_s);
        // mix in sample
        *out += v*mix;
        // increment shape
        F32 freq_s  = lerp_range(param_freq, tone_s);
        F32 delta_s = freq_s*buf.rate.delta_t;
        s += delta_s;
        if (s > 1.f){
            s -= 1.f;
        }
        // increment track
        t += buf.rate.delta_t;
        out += 1;
    }
}

function void
audgen_mix_shape_tri(AUDGEN_Buffer buf, I1F32 range_t,
                     AUDGEN_SignalParams *params){
    // setup
    I1U64 range_i_unclamped = audgen_i_from_t(buf.rate, range_t);
    I1U64 range_i = intr_clamp_top(range_i_unclamped, buf.count);
    F32 max_t = range_t.max - range_t.min;
    I1F32 param_mix  = params->mix;
    I1F32 param_freq = params->freq;
    F32 s = 0.f;
    F32 t = 0.f;
    F32 *out = buf.buf + range_i.min;
    for (U64 i = range_i.min; i < range_i.max; i += 1){
        
        // THE SHAPE
        // vvvv
        B32 tri_up = (s < 0.5f);
        F32 v = (tri_up?
                 (1.f - 4.f*s):
                 (4.f*s - 3));
        // ^^^^
        
        // signal volume control
        F32 tone_s = ClampTop(t/max_t, 1.f);
        F32 mix = lerp_range(param_mix, tone_s);
        // mix in sample
        *out += v*mix;
        // increment shape
        F32 freq_s  = lerp_range(param_freq, tone_s);
        F32 delta_s = freq_s*buf.rate.delta_t;
        s += delta_s;
        if (s > 1.f){
            s -= 1.f;
        }
        // increment track
        t += buf.rate.delta_t;
        out += 1;
    }
}

////////////////////////////////
// NOTE(allen): Audio Generation ADSR Functions

function F32
audgen_sample_adsr(AUDGEN_ADSRCurve *adsr, F32 t, F32 max_t){
    F32 result = 0.f;
    if (0 <= t){
        F32 decay_m   = ClampTop(t/adsr->decay_s, 1.f);
        
        F32 tail_t = t - max_t;
        F32 attack_m  = ClampTop(t/adsr->attack_s, 1.f);
        F32 release_m = 1.f - Clamp(0.f, tail_t/adsr->release_s, 1.f);
        F32 full_m   = attack_m*release_m;
        
        F32 db1 = adsr->base_db;
        F32 db2 = adsr->sustain_db;
        F32 db3 = audgen_silent_db;
        
        F32 decay_db = db1 + (db2 - db1)*decay_m;
        F32 fxv_db   = db3 + (decay_db - db3)*full_m;
        
        result = audgen_mul_from_decibel(fxv_db);
    }
    return(result);
}

function void
audgen_mul_adsr(AUDGEN_Buffer buf, AUDGEN_ADSRCurve *adsr,
                F32 attack_t, F32 release_t, I1F32 range_t){
    I1U64 range_i_unclamped = audgen_i_from_t(buf.rate, range_t);
    I1U64 range_i = intr_clamp_top(range_i_unclamped, buf.count);
    F32 max_t = release_t - attack_t;
    F32 t = range_t.min;
    F32 *out = buf.buf;
    for (U64 i = range_i.min; i < range_i.max; i += 1){
        F32 adsr_m = audgen_sample_adsr(adsr, t - attack_t, max_t);
        *out *= adsr_m;
        t += buf.rate.delta_t;
        out += 1;
    }
}

////////////////////////////////
// NOTE(allen): Audio Generation Timeline

function void
audgen_timeline_push(M_Arena *arena, AUDGEN_Timeline *l,
                     F32 *buf, U64 count, U64 first_i){
    AUDGEN_TimelineSound *sound = push_array(arena, AUDGEN_TimelineSound, 1);
    sound->buf = buf;
    sound->count = count;
    sound->first = first_i;
    
    U64 opl_sample_i = first_i + count;
    
    SLLQueuePush(l->first, l->last, sound);
    l->sound_count += 1;
    l->sample_count = Max(l->sample_count, opl_sample_i);
}

function F32*
audgen_timeline_render(M_Arena *arena, AUDGEN_Timeline *l){
    U64 sample_count = l->sample_count;
    F32 *result = push_array_zero(arena, F32, sample_count);
    for (AUDGEN_TimelineSound *node = l->first;
         node != 0;
         node = node->next){
        U64 first = node->first;
        U64 opl_unclamped = first + node->count;
        U64 opl = ClampTop(opl_unclamped, sample_count);
        F32 *dst = result + first;
        F32 *dst_opl = result + opl;
        F32 *src = node->buf;
        for (;dst < dst_opl; dst += 1, src += 1){
            *dst += *src;
        }
    }
    return(result);
}
