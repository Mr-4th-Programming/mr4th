/* date = February 26th 2021 0:01 pm */

#ifndef BASE_MEMORY_H
#define BASE_MEMORY_H

////////////////////////////////
// NOTE(allen): Base Memory Pre-Requisites

#if !defined(m_reserve)
# error missing definition for 'm_reserve' type: (U64)->void* 
#endif
#if !defined(m_commit)
# error missing definition for 'm_commit' type: (void*,U64)->B32
#endif
#if !defined(m_decommit)
# error missing definition for 'm_decommit' type: (void*,U64)->void
#endif
#if !defined(m_release)
# error missing definition for 'm_release' type: (void*,U64)->void
#endif

////////////////////////////////
// NOTE(allen): Arena Types

struct M_Arena{
    U64 cap;
    U64 pos;
    U64 commit_pos;
};

struct M_ArenaTemp{
    M_Arena *arena;
    U64 pos;
};

////////////////////////////////
// NOTE(allen): Base Memory Helper Functions

function void m_change_memory_noop(void *ctx, void *ptr, U64 size);

////////////////////////////////
// NOTE(allen): Arena Functions

#define M_DEFAULT_RESERVE_SIZE GB(1)
#define M_COMMIT_BLOCK_SIZE MB(64)

#define M_SCRATCH_POOL_COUNT 2

function M_Arena* m_alloc_arena_reserve(U64 reserve_size);
function M_Arena* m_alloc_arena(void);
function void     m_arena_release(M_Arena *arena);
function void*    m_arena_push(M_Arena *arena, U64 size);
function void     m_arena_pop_to(M_Arena *arena, U64 pos);

function void*    m_arena_push_zero(M_Arena *arena, U64 size);
function void     m_arena_align(M_Arena *arena, U64 pow2_align);
function void     m_arena_align_zero(M_Arena *arena, U64 pow2_align);
function void     m_arena_pop_amount(M_Arena *arena, U64 amount);

#define push_array(a,T,c) (T*)m_arena_push((a), sizeof(T)*(c))
#define push_array_zero(a,T,c) (T*)m_arena_push_zero((a), sizeof(T)*(c))

function M_ArenaTemp m_begin_temp(M_Arena *arena);
function void        m_end_temp(M_ArenaTemp temp);

function M_ArenaTemp m_get_scratch(M_Arena **conflict_array, U32 count);
#define              m_release_scratch(temp) m_end_temp(temp)

#endif //BASE_MEMORY_H
