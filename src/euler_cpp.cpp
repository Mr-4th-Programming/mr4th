#include "base/base_inc.h"
#include "os/os_inc.h"
#include "base/base_inc.cpp"
#include "os/os_inc.cpp"

#include <stdio.h>

////////////////////////////////
// NOTE(allen): Types

struct Array_U32{
  U32 *v;
  U64 count;
};

struct ListNode_U32{
  ListNode_U32 *next;
  Array_U32 array;
};

struct List_U32{
  ListNode_U32 *first;
  ListNode_U32 *last;
  U64 node_count;
  U64 total_count;
};

struct Pair_U64{
  U64 a;
  U64 b;
};

struct Pair_U32{
  U32 a;
  U32 b;
};

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
                               U32 first, U32 opl,
                               ListNode_U32 *node);

c_linkage void factorization_table__asm(U32 *table_memory, U32 count);

c_linkage U32  get_prime_factor(U32 *fact_tbl, U32 count, U32 n);
c_linkage Pair_U32 max_divide(U32 n, U32 f);
c_linkage U32  divisor_count(U32 *fact_tbl, U32 count, U32 n);

c_linkage Pair_U64 find_bounded_factors(U64 min, U64 max, U64 traget_product);

c_linkage U64 gcd_euclidean(U64 a, U64 b);
c_linkage U64 lcm_euclidean(U64 a, U64 b);

////////////////////////////////
// NOTE(allen): Helpers/Wrappers with C/C++ Niceness

function void
prime_sieve(M_Arena *arena, List_U32 *primes, U32 first, U32 opl){
  Assert(Implies(primes->node_count == 0, first == 3));
  Assert(first < opl);
  
  // allocate memory
  M_ArenaTemp scratch = m_get_scratch(&arena, 1);
  U32 max_count = opl - first;
  U32 *primes_list = push_array(arena, U32, max_count);
  B8  *sieve_table = push_array_zero(scratch.arena, B8, max_count);
  
  // run sieve
  U32 count = prime_sieve__asm(sieve_table, primes_list, first, opl,
                               primes->first);
  
  // put back memory we didn't use in the primes list
  m_arena_pop_amount(arena, sizeof(*primes_list)*(max_count - count));
  
  // insert new primes block onto the list
  Array_U32 array = {};
  array.v = primes_list;
  array.count = count;
  
  ListNode_U32 *node = push_array(arena, ListNode_U32, 1);
  node->array = array;
  SLLQueuePush(primes->first, primes->last, node);
  primes->node_count += 1;
  primes->total_count += array.count;
  
  m_end_temp(scratch);
}

function Array_U32
factorization_table(M_Arena *arena, U32 count){
  U32 *tbl = push_array_zero(arena, U32, count);
  factorization_table__asm(tbl, count);
  
  Array_U32 result = {};
  result.v = tbl;
  result.count = count;
  
  return(result);
}

int
main(void){
  M_Arena *arena = m_alloc_arena();
  
  U32 answer = 0;
  U32 n = 1;
  
  {
    Array_U32 fact_table = factorization_table(arena, Thousand(10));
    
    for (; n < 2*Thousand(10); n += 1){
      // tri(n) = n*(n + 1)/2
      
      U32 m0 = n;
      U32 m1 = n + 1;
      
      if ((n % 2) == 0){
        m0 /= 2;
      }
      else{
        m1 /= 2;
      }
      
      U32 d0 = divisor_count(fact_table.v, fact_table.count, m0);
      U32 d1 = divisor_count(fact_table.v, fact_table.count, m1);
      
      U32 d = d0*d1;
      if (d >= 500){
        answer = n;
        break;
      }
    }
  }
  
  fprintf(stdout, "%u (%llu)\n", answer, triangle_number(n));
  
  return(0);
}
