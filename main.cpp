#include "base.h"

#include "base.cpp"

#include <stdio.h>

#define EvalPrint(x) printf("%s = %d\n", #x, (S32)(x))
#define EvalPrintLL(x) printf("%s = %lld\n", #x, (S64)(x))
#define EvalPrintU(x) printf("%s = %u\n", #x, (U32)(x))
#define EvalPrintULL(x) printf("%s = %llu\n", #x, (U64)(x))
#define EvalPrintF(x) printf("%s = %e [%f]\n", #x, (F64)(x), (F64)(x))
#define EvalPrintB(x) printf("%s = %s\n", #x, (char*)((x)?"true":"false"))

struct TestStruct{
    int a;
    int b;
    int c;
    int d;
};

int main(){
    int foo[100];
    for (int i = 0; i < ArrayCount(foo); i += 1){
        foo[i] = i;
    }
    
    EvalPrint(ArrayCount(foo));
    
    int bar[100];
    MemoryCopyArray(bar, foo);
    EvalPrint(bar[50]);
    EvalPrint(MemoryMatch(foo, bar, sizeof(foo)));
    MemoryZeroArray(bar);
    EvalPrint(bar[50]);
    EvalPrint(MemoryMatch(foo, bar, sizeof(foo)));
    
    EvalPrint(OffsetOfMember(TestStruct, a));
    EvalPrint(OffsetOfMember(TestStruct, b));
    EvalPrint(OffsetOfMember(TestStruct, c));
    EvalPrint(OffsetOfMember(TestStruct, d));
    
    TestStruct t = {1, 2, 3, 4};
    EvalPrint(t.a);
    EvalPrint(t.d);
    MemoryZeroStruct(&t);
    EvalPrint(t.a);
    EvalPrint(t.d);
    
    EvalPrint(Min(1, 10));
    EvalPrint(Min(100, 10));
    EvalPrint(Max(1, 10));
    EvalPrint(Max(100, 10));
    EvalPrint(Clamp(1, 10, 100));
    EvalPrint(Clamp(1, 0, 100));
    EvalPrint(Clamp(1, 500, 100));
    
    EvalPrint(min_S8);
    EvalPrint(min_S16);
    EvalPrint(min_S32);
    EvalPrintLL(min_S64);
    
    EvalPrint(max_S8);
    EvalPrint(max_S16);
    EvalPrint(max_S32);
    EvalPrintLL(max_S64);
    
    EvalPrintU(max_U8);
    EvalPrintU(max_U16);
    EvalPrintU(max_U32);
    EvalPrintULL(max_U64);
    
    EvalPrintF(machine_epsilon_F32);
    EvalPrintF(machine_epsilon_F64);
    
    EvalPrintF(inf_F32());
    EvalPrintF(neg_inf_F32());
    
    EvalPrintF(inf_F64());
    EvalPrintF(neg_inf_F64());
    
    EvalPrintF(abs_F32(-100.f));
    EvalPrintF(abs_F32(100.f));
    EvalPrintF(abs_F32(0));
    EvalPrintF(abs_F32(neg_inf_F32()));
    
    EvalPrintF(abs_F64(-100.));
    EvalPrintF(abs_F64(100.));
    EvalPrintF(abs_F64(0));
    EvalPrintF(abs_F64(neg_inf_F64()));
    
    EvalPrintF(sqrt_F32(100.f));
    EvalPrintF(sin_F32(tau_F32*.3f));
    EvalPrintF(cos_F32(tau_F32*.3f));
    EvalPrintF(tan_F32(0.5f));
    EvalPrintF(ln_F32(e_F32));
    
    EvalPrintF(sqrt_F64(100.));
    EvalPrintF(sin_F64(tau_F64*.3));
    EvalPrintF(cos_F64(tau_F64*.3));
    EvalPrintF(tan_F64(0.5));
    EvalPrintF(ln_F64(e_F64));
    
    EvalPrintF(lerp(0, 0.3f, 1.f));
    EvalPrintF(lerp(10.f, 0.5f, 100.f));
    EvalPrintF(unlerp(0, 0.3f, 1.f));
    EvalPrintF(unlerp(10.f, lerp(10.f, 0.5f, 100.f), 100.f));
    
    EvalPrintB(intr_contains(i2f32(20, 20, 200, 200), v2f32(100, 199)));
    EvalPrintB(intr_contains(i2f32(20, 20, 200, 200), v2f32(100, 201)));
    EvalPrintB(intr_contains(i2f32(20, 20, 200, 200), v2f32(19, 199)));
    
    return(0);
}