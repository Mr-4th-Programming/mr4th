////////////////////////////////
// NOTE(allen): Interleaving Functions

function String8
bop_interleave(M_Arena *arena, void **in,
               U64 lane_count, U64 el_size, U64 el_count){
    // TODO(allen): look at disassembly for real speed work
    
    // setup buffer
    String8 result = {};
    result.size = lane_count*el_size*el_count;
    result.str = push_array(arena, U8, result.size);
    
    // fill loop
    U8 *out_ptr = result.str;
    U64 in_off = 0;
    for (U64 i = 0; i < el_count; i += 1, in_off += el_size){
        U8 **in_base_ptr = (U8**)in;
        for (U64 j = 0; j < lane_count; j += 1, in_base_ptr += 1){
            MemoryCopy(out_ptr, *in_base_ptr + in_off, el_size);
            out_ptr += el_size;
        }
    }
    
    return(result);
}

function String8*
bop_uninterleave(M_Arena *arena, void *in,
                 U64 lane_count, U64 el_size, U64 el_count){
    // TODO(allen): look at disassembly for real speed work
    
    // compute sizes
    U64 bytes_per_lane = el_size*el_count;
    U64 total_size = lane_count*bytes_per_lane;
    
    // allocate outs
    String8 *result = push_array(arena, String8, lane_count);
    for (U64 i = 0; i < lane_count; i += 1){
        result[i].str = push_array(arena, U8, bytes_per_lane);
        result[i].size = bytes_per_lane;
    }
    
    // fill loop
    U8 *in_ptr = (U8*)in;
    U64 out_off = 0;
    for (U64 i = 0; i < el_count; i += 1, out_off += el_size){
        String8 *out_buffer = result;
        for (U64 j = 0; j < lane_count; j += 1, out_buffer += 1){
            MemoryCopy(out_buffer->str + out_off, in_ptr, el_size);
            in_ptr += el_size;
        }
    }
    
    return(result);
}

////////////////////////////////
// NOTE(allen): Conversions

function String8
bop_f32_from_s24(M_Arena *arena, String8 in){
    // TODO(allen): look at disassembly for real speed work
    
    // round size
    U64 el_count = in.size/3;
    
    // allocate out
    F32 *out = push_array(arena, F32, el_count);
    
    // fill loop
    U8 *in_ptr = in.str;
    F32 *out_ptr = out;
    F32 *opl_ptr = out_ptr + el_count;
    for (; out_ptr < opl_ptr; out_ptr += 1){
        // read s24
        S32 x = (*in_ptr);
        in_ptr += 1;
        x |= (*in_ptr) << 8;
        in_ptr += 1;
        x |= (*in_ptr) << 16;
        in_ptr += 1;
        if ((x & (1 << 23)) != 0){
            x |= (0xFF << 24);
        }
        
        // divide to float
        F32 fx = 0.f;
        if (x >= 0){
            fx = ((F32)x)/(8388607.f);
        }
        else{
            fx = ((F32)x)/(8388608.f);
        }
        
        // write f32
        *out_ptr = fx;
    }
    
    // package output buffer
    String8 result = {};
    result.str = (U8*)out;
    result.size = el_count*sizeof(F32);
    return(result);
}
