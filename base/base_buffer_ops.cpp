////////////////////////////////
// NOTE(allen): Interleaving Functions

function String8
bop_interleave(M_Arena *arena, void **in,
               U64 lane_count, U64 el_size, U64 el_count){
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

