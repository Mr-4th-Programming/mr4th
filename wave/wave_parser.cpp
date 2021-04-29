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
    U64 clamped_size = 0;
    if (looks_like_wave_file){
        // NOTE(allen): 8 = 4 bytes for riff_id + 4 bytes for size
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

function WAVE_SubChunkNode*
wave_chunk_from_id(WAVE_SubChunkList list, U32 id){
    WAVE_SubChunkNode *result = 0;
    for (WAVE_SubChunkNode *node = list.first;
         node != 0;
         node = node->next){
        if (node->id == id){
            result = node;
            break;
        }
    }
    return(result);
}

function WAVE_RiffSubChunk_fmt
wave_fmt_data_from_fmt_chunk(WAVE_SubChunkNode *node, String8 data){
    WAVE_RiffSubChunk_fmt result = {};
    U64 read_size = ClampTop(node->size, sizeof(result));
    str8_read(data, node->off, &result, read_size);
    return(result);
}

// TODO(allen): Real home for these
// TODO(allen): Tidy up the names, interfaces, etc

function void
unswizzle(M_Arena *arena, String8 in, String8 *outs,
          U64 stride, U64 out_count){
    // for now I am saying the input must be divisible by
    // both of these factors, there are ways this could be
    // relaxed if it would be convenient to do so.
    Assert((in.size%(out_count*stride)) == 0);
    
    // compute sizes
    U64 bytes_per_channel = in.size/out_count;
    U64 blocks_per_channel = bytes_per_channel/stride;
    
    // allocate outs
    for (U64 i = 0; i < out_count; i += 1){
        outs[i].str = push_array(arena, U8, bytes_per_channel);
        outs[i].size = bytes_per_channel;
    }
    
    // fill outs from input
    U8 *in_ptr = in.str;
    U64 out_off = 0;
    for (U64 i = 0; i < blocks_per_channel; i += 1, out_off += stride){
        String8 *o = outs;
        for (U64 j = 0; j < out_count; j += 1, o += 1){
            MemoryCopy(o->str + out_off, in_ptr, stride);
            in_ptr += stride;
        }
    }
}

function void
swizzle(M_Arena *arena, void **ins, String8 *out, U64 stride, U64 out_count, U64 blocks_per_channel){
    // setup out buffer
    U64 total_size = stride*out_count*blocks_per_channel;
    U8 *out_buffer = push_array(arena, U8, total_size);
    
    U8 *out_ptr = out_buffer;
    U64 in_off = 0;
    for (U64 i = 0; i < blocks_per_channel; i += 1, in_off += stride){
        U8 **p = (U8**)ins;
        for (U64 j = 0; j < out_count; j += 1, p += 1){
            MemoryCopy(out_ptr, *p + in_off, stride);
            out_ptr += stride;
        }
    }
    
    // fill out
    out->size = total_size;
    out->str = out_buffer;
}

////////////////////////////////
// NOTE(allen): Wave Render Functions

function String8
wave_render(M_Arena *arena, WAVE_RenderParams *params){
    // compute additional rates from params
    U32 bits_per_sample_rounded = AlignUpPow2(params->bits_per_sample, 8);
    U32 bytes_per_sample = bits_per_sample_rounded >> 3;
    U32 bytes_per_block  = bytes_per_sample*params->channel_count;
    U32 bytes_per_second = bytes_per_block*params->block_per_second;
    
    
    M_Scratch scratch(arena);
    String8List list = {};
    
    // master header
    WAVE_RiffMasterChunkHeader *master = push_array(scratch, WAVE_RiffMasterChunkHeader, 1);
    str8_list_push(scratch, &list, str8_typed(master));
    master->riff_id = WAVE_ID_RIFF;
    master->wave_id = WAVE_ID_WAVE;
    
    // fmt chunk
    {
        WAVE_RiffSubChunkHeader *fmt_header = push_array(scratch, WAVE_RiffSubChunkHeader, 1);
        str8_list_push(scratch, &list, str8_typed(fmt_header));
        fmt_header->id = WAVE_ID_fmt;
        
        // save position of fmt chunk
        U64 fmt_chunk_off = list.total_size;
        
        // TODO(allen): this could be weird, we are pushing
        // the full sized fmt struct, but we're not actually
        // using the extension at all. Perhaps we need to the
        // fmt struct into smaller parts.
        
        // fill fmt data
        WAVE_RiffSubChunk_fmt *fmt_data = push_array_zero(scratch, WAVE_RiffSubChunk_fmt, 1);
        str8_list_push(scratch, &list, str8_typed(fmt_data));
        
        fmt_data->format_tag = WAVE_FormatTag_PCM;
        fmt_data->channel_count = params->channel_count;
        fmt_data->block_per_second = params->block_per_second;
        fmt_data->bytes_per_second = bytes_per_second;
        fmt_data->bits_per_sample  = params->bits_per_sample;
        
        // fill fmt header size
        fmt_header->size = list.total_size - fmt_chunk_off;
    }
    
    // data chunk
    {
        WAVE_RiffSubChunkHeader *data_header = push_array(scratch, WAVE_RiffSubChunkHeader, 1);
        str8_list_push(scratch, &list, str8_typed(data_header));
        data_header->id = WAVE_ID_data;
        
        // save position of data chunk
        U64 data_chunk_off = list.total_size;
        
        // swizzle the channels together
        String8 swizzled_data = {};
        swizzle(scratch, params->channels, &swizzled_data,
                bytes_per_sample, params->channel_count, params->block_count);
        str8_list_push(scratch, &list, swizzled_data);
        
        // fill data header size
        data_header->size = list.total_size - data_chunk_off;
    }
    
    // fill master size
    master->size = list.total_size - 8;
    
    String8 result = str8_join(arena, &list, 0);
    return(result);
}

