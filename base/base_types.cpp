////////////////////////////////
// NOTE(allen): Symbolic Constant Functions

function OperatingSystem
operating_system_from_context(void){
    OperatingSystem result = OperatingSystem_Null;
#if OS_WINDOWS
    result = OperatingSystem_Windows;
#elif OS_LINUX
    result = OperatingSystem_Linux;
#elif OS_MAC
    result = OperatingSystem_Mac;
#endif
    return(result);
}

function Architecture
architecture_from_context(void){
    Architecture result = Architecture_Null;
#if ARCH_X64
    result = Architecture_X64;
#elif ARCH_X86
    result = Architecture_X86;
#elif ARCH_ARM
    result = Architecture_Arm;
#elif ARCH_ARM64
    result = Architecture_Arm64;
#endif
    return(result);
}

function char*
string_from_operating_system(OperatingSystem os){
    char *result = "(null)";
    switch (os){
        case OperatingSystem_Windows:
        {
            result = "windows";
        }break;
        case OperatingSystem_Linux:
        {
            result = "linux";
        }break;
        case OperatingSystem_Mac:
        {
            result = "mac";
        }break;
    }
    return(result);
}

function char*
string_from_architecture(Architecture arch){
    char *result = "(null)";
    switch (arch){
        case Architecture_X64:
        {
            result = "x64";
        }break;
        case Architecture_X86:
        {
            result = "x86";
        }break;
        case Architecture_Arm:
        {
            result = "arm";
        }break;
        case Architecture_Arm64:
        {
            result = "armm64";
        }break;
    }
    return(result);
}

function char*
string_from_month(Month month){
    char *result = "(null)";
    switch (month){
        case Month_Jan:
        {
            result = "jan";
        }break;
        case Month_Feb:
        {
            result = "feb";
        }break;
        case Month_Mar:
        {
            result = "mar";
        }break;
        case Month_Apr:
        {
            result = "apr";
        }break;
        case Month_May:
        {
            result = "may";
        }break;
        case Month_Jun:
        {
            result = "jun";
        }break;
        case Month_Jul:
        {
            result = "jul";
        }break;
        case Month_Aug:
        {
            result = "aug";
        }break;
        case Month_Sep:
        {
            result = "sep";
        }break;
        case Month_Oct:
        {
            result = "oct";
        }break;
        case Month_Nov:
        {
            result = "nov";
        }break;
        case Month_Dec:
        {
            result = "dec";
        }break;
    }
    return(result);
}

function char*
string_from_day_of_week(DayOfWeek day_of_week){
    char *result = "(null)";
    switch (day_of_week){
        case DayOfWeek_Sunday:
        {
            result = "sunday";
        }break;
        case DayOfWeek_Monday:
        {
            result = "monday";
        }break;
        case DayOfWeek_Tuesday:
        {
            result = "tuesday";
        }break;
        case DayOfWeek_Wednesday:
        {
            result = "wednesday";
        }break;
        case DayOfWeek_Thursday:
        {
            result = "thursday";
        }break;
        case DayOfWeek_Friday:
        {
            result = "friday";
        }break;
        case DayOfWeek_Saturday:
        {
            result = "saturday";
        }break;
    }
    return(result);
}


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

function F32
sign_F32(F32 x){
    union{ F32 f; U32 u; } r;
    r.f = x;
    F32 result = (r.u&0x80000000)?-1.f:1.f;
    return(result);
}

function F64
sign_F64(F64 x){
    union{ F64 f; U32 u; } r;
    r.f = x;
    F64 result = (r.u&0x8000000000000000)?-1.:1.;
    return(result);
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
atan_F32(F32 x){
    return(atanf(x));
}

function F32
ln_F32(F32 x){
    return(logf(x));
}

function F32
pow_F32(F32 base, F32 x){
    return(powf(base, x));
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
atan_F64(F64 x){
    return(atanf(x));
}

function F64
ln_F64(F64 x){
    return(log(x));
}

function F64
pow_F64(F64 base, F64 x){
    return(powf(base, x));
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

function F32
lerp_range(I1F32 r, F32 t){
    F32 x = r.min + (r.max - r.min)*t;
    return(x);
}

function F32
trunc_F32(F32 x){
    // TODO(allen): does this always work?
    //              can we do better?
    return((F32)(S32)x);
}

function F32
floor_F32(F32 x){
    F32 r;
    if (x >= 0.f){
        r = (F32)(S32)x;
    }
    else{
        r = (F32)(((S32)x) - 1);
    }
    return(r);
}

function F32
ceil_F32(F32 x){
    F32 r;
    if (x >= 0.f){
        r = (F32)(((S32)x) + 1);
    }
    else{
        r = (F32)(S32)x;
    }
    return(r);
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

function I1F32
intr_clamp_top(I1F32 r, F32 top){
    I1F32 result = { ClampTop(r.min, top), ClampTop(r.max, top) };
    return(r);
}

function I1U64
intr_clamp_top(I1U64 r, U64 top){
    I1U64 result = { ClampTop(r.min, top), ClampTop(r.max, top) };
    return(r);
}

////////////////////////////////
// NOTE(allen): Time Functions

function DenseTime
dense_time_from_date_time(DateTime *in){
    U32 year_encoded = (U32)((S32)in->year + 0x8000);
    DenseTime result = 0;
    result += year_encoded;
    result *= 12;
    result += (in->mon - 1);
    result *= 31;
    result += (in->day - 1);
    result *= 24;
    result += in->hour;
    result *= 60;
    result += in->min;
    result *= 61;
    result += in->sec;
    result *= 1000;
    result += in->msec;
    return(result);
}

function DateTime
date_time_from_dense_time(DenseTime in){
    DateTime result = {};
    result.msec = in%1000;
    in /= 1000;
    result.sec = in%61;
    in /= 61;
    result.min = in%60;
    in /= 60;
    result.hour = in%24;
    in /= 24;
    result.day = (in%31) + 1;
    in /= 31;
    result.mon = (in%12) + 1;
    in /= 12;
    S32 year_encoded = (S32)in;
    result.year = (year_encoded - 0x8000);
    return(result);
}


