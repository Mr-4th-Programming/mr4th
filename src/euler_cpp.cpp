#include "base/base_inc.h"
#include "os/os_inc.h"
#include "base/base_inc.cpp"
#include "os/os_inc.cpp"

#include <stdio.h>

////////////////////////////////
// NOTE(allen): Assembly Function Declarations

c_linkage U64 triangle_number(U64 n);
c_linkage U64 sum_of_squares(U64 n);

c_linkage void fibonacci_stepper_mul(void *dst, void *src);
c_linkage void fibonacci_stepper_sqr(void *dst);
c_linkage U64 fibonacci_number(U64 n);
c_linkage U64 fibonacci_sigma(U64 n);

c_linkage U32 prime_sieve__asm(B8 *table_memory,
                               U32 *primes_list,
                               U32 max_value);

struct Pair_U64{
  U64 a;
  U64 b;
};

c_linkage Pair_U64 find_bounded_factors(U64 min, U64 max, U64 traget_product);

c_linkage U64 gcd_euclidean(U64 a, U64 b);
c_linkage U64 lcm_euclidean(U64 a, U64 b);

////////////////////////////////
// NOTE(allen): Helpers/Wrappers with C/C++ Niceness

struct Array_U32{
  U32 *v;
  U64 count;
};

function Array_U32
prime_sieve(M_Arena *arena, U32 max_value){
  Assert(max_value > 2);
  
  // allocate memory
  M_ArenaTemp scratch = m_get_scratch(&arena, 1);
  U32 *primes_list = push_array(arena, U32, max_value);
  B8  *sieve_table = push_array_zero(scratch.arena, B8, max_value);
  
  // run sieve
  U32 count = prime_sieve__asm(sieve_table, primes_list, max_value);
  
  m_end_temp(scratch);
  
  // fill the result
  Array_U32 result = {};
  result.v = primes_list;
  result.count = count;
  
  return(result);
}

int
main(void){
  M_Arena *arena = m_alloc_arena();
  
  U64 tri = triangle_number(100);
  U64 answer = tri*tri - sum_of_squares(100);
  printf("%llu\n", answer);
  
  return(0);
}
