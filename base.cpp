////////////////////////////////
// NOTE(allen): Float Constant Functions

function F32
inf_F32(void){
    union{ F32 f; U32 u; } r;
    r.u = 0x7f800000;
    return(r.f);
}

function F32
neg_inf_F32(void){
    union{ F32 f; U32 u; } r;
    r.u = 0xff800000;
    return(r.f);
}

function F64
inf_F64(void){
    union{ F64 f; U64 u; } r;
    r.u = 0x7ff0000000000000;
    return(r.f);
}

function F64
neg_inf_F64(void){
    union{ F64 f; U64 u; } r;
    r.u = 0xfff0000000000000;
    return(r.f);
}

////////////////////////////////
// NOTE(allen): Math Functions

function F32
abs_F32(F32 x){
    union{ F32 f; U32 u; } r;
    r.f = x;
    r.u &= 0x7fffffff;
    return(r.f);
}

function F64
abs_F64(F64 x){
    union{ F64 f; U64 u; } r;
    r.f = x;
    r.u &= 0x7fffffffffffffff;
    return(r.f);
}

#include <math.h>

function F32
sqrt_F32(F32 x){
    return(sqrtf(x));
}

function F32
sin_F32(F32 x){
    return(sinf(x));
}

function F32
cos_F32(F32 x){
    return(cosf(x));
}

function F32
tan_F32(F32 x){
    return(tanf(x));
}

function F32
ln_F32(F32 x){
    return(logf(x));
}

function F64
sqrt_F64(F64 x){
    return(sqrt(x));
}

function F64
sin_F64(F64 x){
    return(sin(x));
}

function F64
cos_F64(F64 x){
    return(cos(x));
}

function F64
tan_F64(F64 x){
    return(tan(x));
}

function F64
ln_F64(F64 x){
    return(log(x));
}

function F32
lerp(F32 a, F32 t, F32 b){
    F32 x = a + (b - a)*t;
    return(x);
}

function F32
unlerp(F32 a, F32 x, F32 b){
    F32 t = 0.f;
    if (a != b){
        t = (x - a)/(b - a);
    }
    return(t);
}

////////////////////////////////
// NOTE(allen): Compound Type Functions

function V2S32
v2s32(S32 x, S32 y){
    V2S32 r = {x, y};
    return(r);
}

function V2F32
v2f32(F32 x, F32 y){
    V2F32 r = {x, y};
    return(r);
}

function V3F32
v3f32(F32 x, F32 y, F32 z){
    V3F32 r = {x, y, z};
    return(r);
}

function V4F32
v4f32(F32 x, F32 y, F32 z, F32 w){
    V4F32 r = {x, y, z, w};
    return(r);
}

function I1F32
i1f32(F32 min, F32 max){
    I1F32 r = {min, max};
    if (max < min){
        r.min = max;
        r.max = min;
    }
    return(r);
}

function I1U64
i1u64(U64 min, U64 max){
    I1U64 r = {min, max};
    if (max < min){
        r.min = max;
        r.max = min;
    }
    return(r);
}

function I2S32
i2s32(S32 x0, S32 y0, S32 x1, S32 y1){
    I2S32 r = {x0, y0, x1, y1};
    if (x1 < x0){
        r.x0 = x1;
        r.x1 = x0;
    }
    if (y1 < y0){
        r.y0 = y1;
        r.y1 = y0;
    }
    return(r);
}

function I2S32
i2s32_vec(V2S32 min, V2S32 max){
    I2S32 r = i2s32(min.x, min.y, max.x, max.y);
    return(r);
}

function I2F32
i2f32(F32 x0, F32 y0, F32 x1, F32 y1){
    I2F32 r = {x0, y0, x1, y1};
    if (x1 < x0){
        r.x0 = x1;
        r.x1 = x0;
    }
    if (y1 < y0){
        r.y0 = y1;
        r.y1 = y0;
    }
    return(r);
}

function I2F32
i2f32_vec(V2F32 min, V2F32 max){
    I2F32 r = i2f32(min.x, min.y, max.x, max.y);
    return(r);
}

function I2F32
i2f32_range(I1F32 x, I1F32 y){
    I2F32 r = i2f32(x.min, y.min, x.max, y.max);
    return(r);
}

function V2S32
operator+(const V2S32 &a, const V2S32 &b){
    V2S32 r = {a.x + b.x, a.y + b.y};
    return(r);
}

function V2F32
operator+(const V2F32 &a, const V2F32 &b){
    V2F32 r = {a.x + b.x, a.y + b.y};
    return(r);
}

function V3F32
operator+(const V3F32 &a, const V3F32 &b){
    V3F32 r = {a.x + b.x, a.y + b.y, a.z + b.z};
    return(r);
}

function V4F32
operator+(const V4F32 &a, const V4F32 &b){
    V4F32 r = {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
    return(r);
}

function V2S32
operator-(const V2S32 &a, const V2S32 &b){
    V2S32 r = {a.x - b.x, a.y - b.y};
    return(r);
}

function V2F32
operator-(const V2F32 &a, const V2F32 &b){
    V2F32 r = {a.x - b.x, a.y - b.y};
    return(r);
}

function V3F32
operator-(const V3F32 &a, const V3F32 &b){
    V3F32 r = {a.x - b.x, a.y - b.y, a.z - b.z};
    return(r);
}

function V4F32
operator-(const V4F32 &a, const V4F32 &b){
    V4F32 r = {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
    return(r);
}

function V2S32
operator*(const V2S32 &v, const S32 &s){
    V2S32 r = {v.x*s, v.y*s};
    return(r);
}

function V2F32
operator*(const V2F32 &v, const F32 &s){
    V2F32 r = {v.x*s, v.y*s};
    return(r);
}

function V3F32
operator*(const V3F32 &v, const F32 &s){
    V3F32 r = {v.x*s, v.y*s, v.z*s};
    return(r);
}

function V4F32
operator*(const V4F32 &v, const F32 &s){
    V4F32 r = {v.x*s, v.y*s, v.z*s, v.w*s};
    return(r);
}

function V2S32
operator*(const S32 &s, const V2S32 &v){
    V2S32 r = {v.x*s, v.y*s};
    return(r);
}

function V2F32
operator*(const F32 &s, const V2F32 &v){
    V2F32 r = {v.x*s, v.y*s};
    return(r);
}

function V3F32
operator*(const F32 &s, const V3F32 &v){
    V3F32 r = {v.x*s, v.y*s, v.z*s};
    return(r);
}

function V4F32
operator*(const F32 &s, const V4F32 &v){
    V4F32 r = {v.x*s, v.y*s, v.z*s, v.w*s};
    return(r);
}

function V2F32
vec_hadamard(V2F32 a, V2F32 b){
    V2F32 r = {a.x*b.x, a.y*b.y};
    return(r);
}

function V3F32
vec_hadamard(V3F32 a, V3F32 b){
    V3F32 r = {a.x*b.x, a.y*b.y, a.z*b.z};
    return(r);
}

function V4F32
vec_hadamard(V4F32 a, V4F32 b){
    V4F32 r = {a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w};
    return(r);
}

function F32
vec_dot(V2F32 a, V2F32 b){
    F32 r = a.x*b.x + a.y*b.y;
    return(r);
}

function F32
vec_dot(V3F32 a, V3F32 b){
    F32 r = a.x*b.x + a.y*b.y + a.z*b.z;
    return(r);
}

function F32
vec_dot(V4F32 a, V4F32 b){
    F32 r = a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
    return(r);
}

function B32
intr_overlaps(I1F32 a, I1F32 b){
    B32 result = (b.min < a.max && a.min < b.max);
    return(result);
}

function B32
intr_overlaps(I2F32 a, I2F32 b){
    B32 result = (b.x0 < a.x1 && a.x0 < b.x1 &&
                  b.y0 < a.y1 && a.y0 < b.y1);
    return(result);
}

function B32
intr_overlaps(I2S32 a, I2S32 b){
    B32 result = (b.x0 < a.x1 && a.x0 < b.x1 &&
                  b.y0 < a.y1 && a.y0 < b.y1);
    return(result);
}

function B32
intr_contains(I1F32 r, F32 x){
    B32 result = (r.min <= x && x < r.max);
    return(result);
}

function B32
intr_contains(I2F32 r, V2F32 p){
    B32 result = (r.x0 <= p.x && p.x < r.x1 &&
                  r.y0 <= p.y && p.y < r.y1);
    return(result);
}

function B32
intr_contains(I2S32 r, V2S32 p){
    B32 result = (r.x0 <= p.x && p.x < r.x1 &&
                  r.y0 <= p.y && p.y < r.y1);
    return(result);
}

function F32
intr_dim(I1F32 r){
    F32 result = r.max - r.min;
    return(result);
}

function U64
intr_dim(I1U64 r){
    U64 result = r.max - r.min;
    return(result);
}

function V2F32
intr_dim(I2F32 r){
    V2F32 result = v2f32(r.x1 - r.x0, r.y1 - r.y0);
    return(result);
}

function V2S32
intr_dim(I2S32 r){
    V2S32 result = v2s32(r.x1 - r.x0, r.y1 - r.y0);
    return(result);
}

function F32
intr_center(I1F32 r){
    F32 result = (r.min + r.max)*0.5f;
    return(result);
}

function V2F32
intr_center(I2F32 r){
    V2F32 result = (r.min + r.max)*0.5f;
    return(result);
}

function I1F32
intr_axis(I2F32 r, Axis axis){
    I1F32 result = { r.p[0].v[axis], r.p[1].v[axis] };
    return(result);
}

