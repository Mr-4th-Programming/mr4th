////////////////////////////////
// NOTE(allen): PRNG Functions

function PRNG
prng_make_from_seed(U64 seed){
    PRNG prng = {seed};
    return(prng);
}

function U32
prng_next_u32(PRNG *prng){
    local const U64 M = 4093;
    local const U64 N = 8191;
    
    U64 s = prng->s;
    s = s*M + N;
    prng->s = s;
    
    U32 rot = (s >> 60);
    U32 x = (U32)s;
    U32 lrot = (32 - rot);
    if (lrot >= 32){
        lrot = 0;
    }
    U32 result = (x >> rot) | (x << lrot);
    return(result);
}

function U64
prng_next_u64(PRNG *prng){
    U64 x0 = prng_next_u32(prng);
    U64 x1 = prng_next_u32(prng);
    U64 result = (x1 << 32) | x0;
    return(result);
}

// [0,1]
function F32
prng_next_unital_f32(PRNG *prng){
    U32 x = prng_next_u32(prng);
    U32 x_masked = (x & 0x1FFFFF); // 21-bit mask
    F32 result = (F32)x_masked/2097151.f;
    return(result);
}

// [-1,1]
function F32
prng_next_biunital_f32(PRNG *prng){
    U32 x = prng_next_u32(prng);
    U32 x_masked = (x & 0x3FFFFF); // 22-bit mask
    F32 result = ((F32)x_masked/2097151.f) - 1.f;
    return(result);
}
