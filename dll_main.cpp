#include "base/base_inc.h"

#include "base/base_inc.cpp"

shared_function U32
u32_sum(U32 *vals, U64 count){
    U32 sum = 0;
    for (U64 i = 0; i < count; i += 1){
        sum += vals[i];
    }
    return(sum);
}

