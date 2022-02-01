////////////////////////////////
// NOTE(allen): Base Memory Helper Functions

function void
m_change_memory_noop(void *ctx, void *ptr, U64 size){}


////////////////////////////////
// NOTE(allen): Arena Functions

#define M_INITIAL_COMMIT KB(4)
StaticAssert(sizeof(M_Arena) <= M_INITIAL_COMMIT, check_arena_size);

function M_Arena*
m_alloc_arena_reserve(U64 reserve_size){
    M_Arena *result = 0;
    if (reserve_size >= M_INITIAL_COMMIT){
        void *memory = m_reserve(reserve_size);
        if (m_commit(memory, M_INITIAL_COMMIT)){
            result = (M_Arena*)memory;
            result->cap = reserve_size;
            result->pos = AlignUpPow2(sizeof(M_Arena), 64);
            result->commit_pos = M_INITIAL_COMMIT;
        }
    }
    Assert(result != 0);
    return(result);
}

function M_Arena*
m_alloc_arena(void){
    M_Arena *result = m_alloc_arena_reserve(M_DEFAULT_RESERVE_SIZE);
    return(result);
}

function void
m_arena_release(M_Arena *arena){
    m_release(arena, arena->cap);
}

function void*
m_arena_push(M_Arena *arena, U64 size){
    void *result = 0;
    if (arena->pos + size <= arena->cap){
        
        // save pointer we should return
        void *result_on_success = ((U8*)arena) + arena->pos;
        
        // grow the committed range to include the last returned byte
        U64 p = arena->pos + size;
        U64 commit_p = arena->commit_pos;
        if (p > commit_p){
            U64 p_aligned = AlignUpPow2(p, M_COMMIT_BLOCK_SIZE);
            U64 next_commit_p = ClampTop(p_aligned, arena->cap);
            U64 commit_size = next_commit_p - commit_p;
            if (m_commit((U8*)arena + commit_p, commit_size)){
                commit_p = next_commit_p;
                arena->commit_pos = commit_p;
            }
        }
        
        // if the commit range contains the allocation range, save the allocation
        if (p <= commit_p){
            result = result_on_success;
            arena->pos = p;
        }
    }
    Assert(result != 0);
    return(result);
}

function void
m_arena_pop_to(M_Arena *arena, U64 pos){
    if (pos < arena->pos){
        arena->pos = pos;
        
        U64 p = arena->pos;
        U64 p_aligned = AlignUpPow2(p, M_COMMIT_BLOCK_SIZE);
        U64 next_commit_p = ClampTop(p_aligned, arena->cap);
        
        U64 commit_p = arena->commit_pos;
        if (next_commit_p < commit_p){
            U64 decommit_size = commit_p - next_commit_p;
            m_decommit((U8*)arena + next_commit_p, decommit_size);
            arena->commit_pos = next_commit_p;
        }
    }
}

function void*
m_arena_push_zero(M_Arena *arena, U64 size){
    void *result = m_arena_push(arena, size);
    MemoryZero(result, size);
    return(result);
}

function void
m_arena_align(M_Arena *arena, U64 pow2_align){
    U64 p = arena->pos;
    U64 p_aligned = AlignUpPow2(p, pow2_align);
    U64 z = p_aligned - p;
    if (z > 0){
        m_arena_push(arena, z);
    }
}

function void
m_arena_align_zero(M_Arena *arena, U64 pow2_align){
    U64 p = arena->pos;
    U64 p_aligned = AlignUpPow2(p, pow2_align);
    U64 z = p_aligned - p;
    if (z > 0){
        m_arena_push_zero(arena, z);
    }
}

function void
m_arena_pop_amount(M_Arena *arena, U64 amount){
    if (amount <= arena->pos){
        m_arena_pop_to(arena, arena->pos - amount);
    }
}

////////////////////////////////
// NOTE(allen): Temp Helper Functions

function M_ArenaTemp
m_begin_temp(M_Arena *arena){
    M_ArenaTemp temp = {arena, arena->pos};
    return(temp);
}

function void
m_end_temp(M_ArenaTemp temp){
    m_arena_pop_to(temp.arena, temp.pos);
}

////////////////////////////////
// NOTE(allen): Scratch

threadvar M_Arena *m__scratch_pool[M_SCRATCH_POOL_COUNT] = {};

function M_ArenaTemp
m_get_scratch(M_Arena **conflict_array, U32 count){
    // init on first time
    if (m__scratch_pool[0] == 0){
        M_Arena **scratch_slot = m__scratch_pool;
        for (U64 i = 0;
             i < M_SCRATCH_POOL_COUNT;
             i += 1, scratch_slot += 1){
            *scratch_slot = m_alloc_arena();
        }
    }
    
    // get non-conflicting arena
    M_ArenaTemp result = {};
    M_Arena **scratch_slot = m__scratch_pool;
    for (U64 i = 0;
         i < M_SCRATCH_POOL_COUNT;
         i += 1, scratch_slot += 1){
        B32 is_non_conflict = true;
        M_Arena **conflict_ptr = conflict_array;
        for (U32 j = 0; j < count; j += 1, conflict_ptr += 1){
            if (*scratch_slot == *conflict_ptr){
                is_non_conflict = false;
                break;
            }
        }
        if (is_non_conflict){
            result = m_begin_temp(*scratch_slot);
            break;
        }
    }
    
    return(result);
}