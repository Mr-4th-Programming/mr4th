/* date = January 22nd 2021 3:51 pm */

#ifndef BASE_H
#define BASE_H

////////////////////////////////
// NOTE(allen): Context Cracking

#if defined(__clang__)
# define COMPILER_CLANG 1

# if defined(_WIN32)
#  define OS_WINDOWS 1
# elif defined(__gnu_linux__)
#  define OS_LINUX 1
# elif defined(__APPLE__) && defined(__MACH__)
#  define OS_MAC 1
# else
#  error missing OS detection
# endif

# if defined(__amd64__)
#  define ARCH_X64 1
// TODO(allen): verify this works on clang
# elif defined(__i386__)
#  define ARCH_X86 1
// TODO(allen): verify this works on clang
# elif defined(__arm__)
#  define ARCH_ARM 1
// TODO(allen): verify this works on clang
# elif defined(__aarch64__)
#  define ARCH_ARM64 1
# else
#  error missing ARCH detection
# endif

#elif defined(_MSC_VER)
# define COMPILER_CL 1

# if defined(_WIN32)
#  define OS_WINDOWS 1
# else
#  error missing OS detection
# endif

# if defined(_M_AMD64)
#  define ARCH_X64 1
# elif defined(_M_I86)
#  define ARCH_X86 1
# elif defined(_M_ARM)
#  define ARCH_ARM 1
// TODO(allen): ARM64?
# else
#  error missing ARCH detection
# endif

#elif defined(__GNUC__)
# define COMPILER_GCC 1

# if defined(_WIN32)
#  define OS_WINDOWS 1
# elif defined(__gnu_linux__)
#  define OS_LINUX 1
# elif defined(__APPLE__) && defined(__MACH__)
#  define OS_MAC 1
# else
#  error missing OS detection
# endif

# if defined(__amd64__)
#  define ARCH_X64 1
# elif defined(__i386__)
#  define ARCH_X86 1
# elif defined(__arm__)
#  define ARCH_ARM 1
# elif defined(__aarch64__)
#  define ARCH_ARM64 1
# else
#  error missing ARCH detection
# endif

#else
# error no context cracking for this compiler
#endif

// NOTE(allen): Zero fill missing context macros
#if !defined(COMPILER_CL)
# define COMPILER_CL 0
#endif
#if !defined(COMPILER_CLANG)
# define COMPILER_CLANG 0
#endif
#if !defined(COMPILER_GCC)
# define COMPILER_GCC 0
#endif
#if !defined(OS_WINDOWS)
# define OS_WINDOWS 0
#endif
#if !defined(OS_LINUX)
# define OS_LINUX 0
#endif
#if !defined(OS_MAC)
# define OS_MAC 0
#endif
#if !defined(ARCH_X64)
# define ARCH_X64 0
#endif
#if !defined(ARCH_X86)
# define ARCH_X86 0
#endif
#if !defined(ARCH_ARM)
# define ARCH_ARM 0
#endif
#if !defined(ARCH_ARM64)
# define ARCH_ARM64 0
#endif

#if !defined(ENABLE_ASSERT)
# define ENABLE_ASSERT
#endif

////////////////////////////////
// NOTE(allen): Helper Macros

#define Stmnt(S) do{ S }while(0)

#if !defined(AssertBreak)
# define AssertBreak() (*(int*)0 = 0)
#endif

#if ENABLE_ASSERT
# define Assert(c) Stmnt( if (!(c)){ AssertBreak(); } )
#else
# define Assert(c)
#endif

#define Stringify_(S) #S
#define Stringify(S) Stringify_(S)
#define Glue_(A,B) A##B
#define Glue(A,B) Glue_(A,B)

#define ArrayCount(a) (sizeof(a)/sizeof(*(a)))

#define IntFromPtr(p) (U64)((U8*)p - (U8*)0)
#define PtrFromInt(n) (void*)((U8*)0 + (n))

#define Member(T,m) (((T*)0)->m)
#define OffsetOfMember(T,m) IntFromPtr(&Member(T,m))

#define Min(a,b) (((a)<(b))?(a):(b))
#define Max(a,b) (((a)>(b))?(a):(b))
#define Clamp(a,x,b) (((x)<(a))?(a):\
((b)<(x))?(b):(x))
#define ClampTop(a,b) Min(a,b)
#define ClampBot(a,b) Max(a,b)

#define global static
#define local  static
#define function static

#define c_linkage_begin extern "C"{
#define c_linkage_end   }
#define c_linkage extern "C"

#include <string.h>
#define MemoryZero(p,z) memset((p), 0, (z))
#define MemoryZeroStruct(p) MemoryZero((p), sizeof(*(p)))
#define MemoryZeroArray(p)  MemoryZero((p), sizeof(p))
#define MemoryZeroTyped(p,c) MemoryZero((p), sizeof(*(p))*(c))

#define MemoryMatch(a,b,z) (memcmp((a),(b),(z)) == 0)

#define MemoryCopy(d,s,z) memmove((d), (s), (z))
#define MemoryCopyStruct(d,s) MemoryCopy((d),(s),\
Min(sizeof(*(d)),sizeof(*(s))))
#define MemoryCopyArray(d,s)  MemoryCopy((d),(s),Min(sizeof(s),sizeof(d)))
#define MemoryCopyTyped(d,s,c) MemoryCopy((d),(s),\
Min(sizeof(*(d)),sizeof(*(s)))*(c))

////////////////////////////////
// NOTE(allen): Basic Types

#include <stdint.h>
typedef int8_t  S8;
typedef int16_t S16;
typedef int32_t S32;
typedef int64_t S64;
typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef S8 B8;
typedef S16 B16;
typedef S32 B32;
typedef S64 B64;
typedef float F32;
typedef double F64;

typedef void VoidFunc(void);

////////////////////////////////
// NOTE(allen): Basic Constants

global S8  min_S8  = (S8) 0x80;
global S16 min_S16 = (S16)0x8000;
global S32 min_S32 = (S32)0x80000000;
global S64 min_S64 = (S64)0x8000000000000000llu;

global S8  max_S8  = (S8) 0x7f;
global S16 max_S16 = (S16)0x7fff;
global S32 max_S32 = (S32)0x7fffffff;
global S64 max_S64 = (S64)0x7fffffffffffffffllu;

global U8  max_U8  = 0xff;
global U16 max_U16 = 0xffff;
global U32 max_U32 = 0xffffffff;
global U64 max_U64 = 0xffffffffffffffffllu;

global F32 machine_epsilon_F32 = 1.1920929e-7f;
global F32 pi_F32  = 3.14159265359f;
global F32 tau_F32 = 6.28318530718f;
global F32 e_F32 = 2.71828182846f;
global F32 gold_big_F32 = 1.61803398875f;
global F32 gold_small_F32 = 0.61803398875f;

global F64 machine_epsilon_F64 = 2.220446e-16;
global F64 pi_F64  = 3.14159265359;
global F64 tau_F64 = 6.28318530718;
global F64 e_F64 = 2.71828182846;
global F64 gold_big_F64 = 1.61803398875;
global F64 gold_small_F64 = 0.61803398875;

////////////////////////////////
// NOTE(allen): Symbolic Constants

enum Axis{
    Axis_X,
    Axis_Y,
    Axis_Z,
    Axis_W,
};

////////////////////////////////
// NOTE(allen): Compound Types

union V2S32{
    struct{
        S32 x;
        S32 y;
    };
    S32 v[2];
};

union V2F32{
    struct{
        F32 x;
        F32 y;
    };
    F32 v[2];
};

union V3F32{
    struct{
        F32 x;
        F32 y;
        F32 z;
    };
    F32 v[3];
};

union V4F32{
    struct{
        F32 x;
        F32 y;
        F32 z;
        F32 w;
    };
    F32 v[4];
};

union I1F32{
    struct{
        F32 min;
        F32 max;
    };
    F32 v[2];
};

union I1U64{
    struct{
        U64 min;
        U64 max;
    };
    struct{
        U64 first;
        U64 opl;
    };
    U64 v[2];
};

union I2S32{
    struct{
        V2S32 min;
        V2S32 max;
    };
    struct{
        V2S32 p0;
        V2S32 p1;
    };
    struct{
        S32 x0;
        S32 y0;
        S32 x1;
        S32 y1;
    };
    V2S32 p[2];
    S32 v[4];
};

union I2F32{
    struct{
        V2F32 min;
        V2F32 max;
    };
    struct{
        V2F32 p0;
        V2F32 p1;
    };
    struct{
        F32 x0;
        F32 y0;
        F32 x1;
        F32 y1;
    };
    V2F32 p[2];
    F32 v[4];
};

////////////////////////////////
// NOTE(allen): Float Constant Functions

function F32 inf_F32(void);
function F32 neg_inf_F32(void);
function F64 inf_F64(void);
function F64 neg_inf_F64(void);

////////////////////////////////
// NOTE(allen): Math Functions

function F32 abs_F32(F32 x);
function F64 abs_F64(F64 x);

function F32 sqrt_F32(F32 x);
function F32 sin_F32(F32 x);
function F32 cos_F32(F32 x);
function F32 tan_F32(F32 x);
function F32 ln_F32(F32 x);

function F64 sqrt_F64(F64 x);
function F64 sin_F64(F64 x);
function F64 cos_F64(F64 x);
function F64 tan_F64(F64 x);
function F64 ln_F64(F64 x);

function F32 lerp(F32 a, F32 t, F32 b);
function F32 unlerp(F32 a, F32 x, F32 b);

////////////////////////////////
// NOTE(allen): Compound Type Functions

function V2S32 v2s32(S32 x, S32 y);

function V2F32 v2f32(F32 x, F32 y);
function V3F32 v3f32(F32 x, F32 y, F32 z);
function V4F32 v4f32(F32 x, F32 y, F32 z, F32 w);

function I1F32 i1f32(F32 min, F32 max);
function I1U64 i1u64(U64 min, U64 max);

function I2S32 i2s32(S32 x0, S32 y0, S32 x1, S32 y1);
function I2S32 i2s32_vec(V2S32 min, V2S32 max);

function I2F32 i2f32(F32 x0, F32 y0, F32 x1, F32 y1);
function I2F32 i2f32_vec(V2F32 min, V2F32 max);
function I2F32 i2f32_range(I1F32 x, I1F32 y);

function V2S32 operator+(const V2S32 &a, const V2S32 &b);
function V2F32 operator+(const V2F32 &a, const V2F32 &b);
function V3F32 operator+(const V3F32 &a, const V3F32 &b);
function V4F32 operator+(const V4F32 &a, const V4F32 &b);

function V2S32 operator-(const V2S32 &a, const V2S32 &b);
function V2F32 operator-(const V2F32 &a, const V2F32 &b);
function V3F32 operator-(const V3F32 &a, const V3F32 &b);
function V4F32 operator-(const V4F32 &a, const V4F32 &b);

function V2S32 operator*(const V2S32 &v, const S32 &s);
function V2F32 operator*(const V2F32 &v, const F32 &s);
function V3F32 operator*(const V3F32 &v, const F32 &s);
function V4F32 operator*(const V4F32 &v, const F32 &s);

function V2S32 operator*(const S32 &s, const V2S32 &v);
function V2F32 operator*(const F32 &s, const V2F32 &v);
function V3F32 operator*(const F32 &s, const V3F32 &v);
function V4F32 operator*(const F32 &s, const V4F32 &v);

function V2F32 vec_hadamard(V2F32 a, V2F32 b);
function V3F32 vec_hadamard(V3F32 a, V3F32 b);
function V4F32 vec_hadamard(V4F32 a, V4F32 b);

function F32 vec_dot(V2F32 a, V2F32 b);
function F32 vec_dot(V3F32 a, V3F32 b);
function F32 vec_dot(V4F32 a, V4F32 b);

function B32 intr_overlaps(I1F32 a, I1F32 b);
function B32 intr_overlaps(I2F32 a, I2F32 b);
function B32 intr_overlaps(I2S32 a, I2S32 b);

function B32 intr_contains(I1F32 r, F32 x);
function B32 intr_contains(I2F32 r, V2F32 p);
function B32 intr_contains(I2S32 r, V2S32 p);

function F32 intr_dim(I1F32 r);
function U64 intr_dim(I1U64 r);
function V2F32 intr_dim(I2F32 r);
function V2S32 intr_dim(I2S32 r);

function F32 intr_center(I1F32 r);
function V2F32 intr_center(I2F32 r);

function I1F32 intr_axis(I2F32 r, Axis axis);

#endif //BASE_H
