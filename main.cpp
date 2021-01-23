#include "base.h"

#include <stdio.h>

#define EvalPrint(x) printf("%s = %d\n", #x, (int)(x))

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
    
    return(0);
}