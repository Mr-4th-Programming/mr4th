////////////////////////////////
// NOTE(allen): Wave Parser Functions

function WAVE_SubChunkList
wave_sub_chunks_from_data(M_Arena *arena, String8 data){
    // read master header
    WAVE_RiffMasterChunkHeader header = {};
    str8_read_typed(data, 0, &header);
    
    // check master header
    B32 looks_like_wave_file = (header.riff_id == WAVE_ID_RIFF &&
                                header.wave_id == WAVE_ID_WAVE);
    
    // get clamped size
    // NOTE(allen): extra 4 bytes for riff_id, 4 bytes for size
    U64 clamped_size = 0;
    if (looks_like_wave_file){
        U64 size_according_to_header = header.size + 8;
        clamped_size = ClampTop(data.size, size_according_to_header);
    }
    
    // start of main data
    U64 main_data_off = sizeof(header);
    
    // chunk alignment (only supports Pow2 right now)
    // TODO(allen): Is this the correct alignment? It doesn't say
    // in the overview I read, not explicitly anyways.
    local const U64 wave_chunk_align = 2;
    
    // scan chunks
    WAVE_SubChunkList result = {};
    for (U64 off = main_data_off;
         off < clamped_size;){
        // read sub header
        WAVE_RiffSubChunkHeader sub_header = {};
        B32 got_sub_header = str8_read_typed(data, off, &sub_header);
        
        // get range of chunk data
        U64 data_off = off + sizeof(sub_header);
        U64 end_of_data_off = data_off + sub_header.size;
        
        // emit chunk node
        if (got_sub_header){
            WAVE_SubChunkNode *new_node = push_array(arena, WAVE_SubChunkNode, 1);
            SLLQueuePush(result.first, result.last, new_node);
            result.count += 1;
            new_node->id = sub_header.id;
            new_node->size = sub_header.size;
            new_node->off = data_off;
        }
        
        // align offset
        U64 aligned_off = AlignUpPow2(end_of_data_off, wave_chunk_align);
        
        // increment cursor
        off = aligned_off;
    }
    
    return(result);
}
