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

struct Array_U8{
  U8 *v;
  U64 count;
};

struct EulerData{
  U8 *v;
  U64 count;
  U64 line_count;
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

struct U64x3{
  U64 v[3];
};

struct U64x3_DivR{
  U64x3 q;
  U64 r;
};

typedef U64 U64xN;

////////////////////////////////
// NOTE(allen): Assembly Function Declarations

c_linkage U64 triangle_number(U64 n);
c_linkage U64 sum_of_squares(U64 n);

c_linkage void fibonacci_stepper_mul(void *dst, void *src);
c_linkage void fibonacci_stepper_sqr(void *dst);
c_linkage U64 fibonacci_number(U64 n);
c_linkage U64 fibonacci_sigma(U64 n);

c_linkage U64 choose__asm(U64 *buffer, U64 n, U64 m);

c_linkage void fill_factorial_table(U64 *table, U64 count);

c_linkage U32 prime_sieve__asm(B8 *table_memory,
                               U32 *primes_list,
                               U32 first, U32 opl,
                               ListNode_U32 *node);

c_linkage void factorization_table__asm(U32 *table_memory, U32 count);

c_linkage U32  get_prime_factor(U32 *fact_tbl, U32 count, U32 n);
c_linkage Pair_U32 max_divide(U32 n, U32 f);
c_linkage U32  divisor_count(U32 *fact_tbl, U32 count, U32 n);
c_linkage U64  divisor_sum(U32 *fact_tbl, U32 count, U32 n);

c_linkage Pair_U64 find_bounded_factors(U64 min, U64 max, U64 target_product);

c_linkage U64 gcd_euclidean(U64 a, U64 b);
c_linkage U64 lcm_euclidean(U64 a, U64 b);

c_linkage EulerData euler_data_from_text__asm(String8 text, U8 *memory);
c_linkage EulerData euler_data_from_text_2dig__asm(String8 text, U8 *memory);

c_linkage U64x3      add_u64x3(U64x3 a, U64x3 b);
c_linkage U64x3      add_small_u64x3(U64x3 a, U64 b);
c_linkage U64x3      mul_small_u64x3(U64x3 a, U64 b);
c_linkage U64x3_DivR div_small_u64x3(U64x3 n, U64 d);

c_linkage U64x3      u64x3_from_dec(U8 *digits, U64 count);
c_linkage U64        dec_from_u64x3__asm(U8 *out, U64x3 x);

c_linkage void add_small_in_place_u64xn(U64xN *a, U64 b);
c_linkage void mul_small_in_place_u64xn(U64xN *a, U64 b);
c_linkage U64  div_small_in_place_u64xn(U64xN *n, U64 d);

c_linkage U64 dec_from_u64xn__asm(U8 *out, U64xN *x); // x is "destroyed" by this call

////////////////////////////////
// NOTE(allen): Helpers/Wrappers with C/C++ Niceness

function U64
choose(U64 n, U64 m){
  U64 result = 0;
  if (m <= n){
    M_ArenaTemp scratch = m_get_scratch(0, 0);
    U64 *buffer = push_array(scratch.arena, U64, n + 1);
    result = choose__asm(buffer, n, m);
    m_end_temp(scratch);
  }
  return(result);
}

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
  m_arena_align(arena, 8);
  
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

function EulerData
euler_data_from_text(M_Arena *arena, String8 text, U64 num_digits){
  Assert(num_digits == 1 || num_digits == 2);
  
  U8 *memory = push_array(arena, U8, text.size);
  EulerData result = {};
  if (num_digits == 1){
    result = euler_data_from_text__asm(text, memory);
  }
  else if (num_digits == 2){
    result = euler_data_from_text_2dig__asm(text, memory);
  }
  m_arena_pop_amount(arena, text.size - result.count);
  m_arena_align(arena, 8);
  
  return(result);
}

function Array_U8
dec_from_u64x3(M_Arena *arena, U64x3 x){
  Array_U8 result = {};
  
  if (x.v[0] == 0 && x.v[1] == 0 && x.v[2] == 0){
    local U8 zbuffer[1] = {0};
    result.v = zbuffer;
    result.count = 1;
  }
  else{
    U64 cap = 60;
    
    U8 *buffer = push_array(arena, U8, cap);
    U64 count = dec_from_u64x3__asm(buffer, x);
    m_arena_pop_amount(arena, cap - count);
    m_arena_align(arena, 8);
    
    result.v = buffer;
    result.count = count;
  }
  
  return(result);
}

function U64xN*
copy_u64xn(M_Arena *arena, U64xN *x){
  U64 size = (*x) + 1;
  U64xN *result = push_array(arena, U64, size);
  MemoryCopy(result, x, sizeof(*x)*size);
  return(result);
}

function Array_U8
dec_from_u64xn(M_Arena *arena, U64xN *x){
  Array_U8 result = {};
  
  if (*x == 0){
    local U8 zbuffer[1] = {0};
    result.v = zbuffer;
    result.count = 1;
  }
  else{
    M_ArenaTemp scratch = m_get_scratch(&arena, 1);
    
    U64 cap = (*x)*20;
    
    U64xN *x_copy = copy_u64xn(scratch.arena, x);
    
    U8 *buffer = push_array(arena, U8, cap);
    U64 count = dec_from_u64xn__asm(buffer, x_copy);
    m_arena_pop_amount(arena, cap - count);
    m_arena_align(arena, 8);
    
    result.v = buffer;
    result.count = count;
    
    m_release_scratch(scratch);
  }
  
  return(result);
}

function U64
dec_repeating_cycle_score(U64 d){
  U64 result = 0;
  
  if (d > 0){
    // divide out 2s
    for (;(d & 1) == 0;){
      d >>= 1;
    }
    
    // divide out 5s
    for (;(d % 5) == 0;){
      d /= 5;
    }
    
    // check 9s
    if (d > 1){
      U64 score = 1;
      U64 buf1[60] = {1, 9};
      U64 buf2[60] = {};
      for (;;){
        memcpy(buf2, buf1, sizeof(buf1));
        U64 remainder = div_small_in_place_u64xn(buf1, d);
        if (remainder == 0){
          break;
        }
        score += 1;
        memcpy(buf1, buf2, sizeof(buf1));
        mul_small_in_place_u64xn(buf1, 10);
        add_small_in_place_u64xn(buf1, 9);
      }
      
      result = score;
    }
  }
  
  return(result);
}

int
main(void){
  M_Arena *arena = m_alloc_arena();
  
  U64 max_score = 0;
  for (U64 d = 1; d < 1000; d += 1){
    U64 score = dec_repeating_cycle_score(d);
    max_score = Max(score, max_score);
    printf("score(%llu) = %llu\n", d, score);
  }
  
  
  return(0);
}
