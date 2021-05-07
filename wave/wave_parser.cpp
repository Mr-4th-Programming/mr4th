////////////////////////////////
// NOTE(allen): Wave Parser Functions

function WAVE_SubChunkList
wave_sub_chunks_from_data(M_Arena *arena, String8 data){
    // read master header
    RIFF_MasterChunkHeader header = {};
    str8_read_typed(data, 0, &header);
    
    // check master header
    B32 looks_like_wave_file = (header.riff_id == RIFF_ID_RIFF &&
                                header.sub_id == WAVE_ID_WAVE);
    
    // get clamped size
    U64 clamped_size = 0;
    if (looks_like_wave_file){
        // NOTE(allen): 8 = 4 bytes for riff_id + 4 bytes for size
        U64 size_according_to_header = header.size + 8;
        clamped_size = ClampTop(data.size, size_according_to_header);
    }
    
    // start of main data
    U64 main_data_off = sizeof(header);
    
    // scan chunks
    WAVE_SubChunkList result = {};
    for (U64 off = main_data_off;
         off < clamped_size;){
        // read sub header
        RIFF_SubChunkHeader sub_header = {};
        B32 got_sub_header = str8_read_typed(data, off, &sub_header);
        if (!got_sub_header){
            break;
        }
        
        // get range of chunk data
        U64 data_off = off + sizeof(sub_header);
        U64 end_of_data_off = data_off + sub_header.size;
        U64 end_of_data_off_clamped = ClampTop(end_of_data_off, clamped_size);
        U64 clamped_size = end_of_data_off - data_off;
        
        // emit chunk node
        WAVE_SubChunkNode *new_node = push_array(arena, WAVE_SubChunkNode, 1);
        SLLQueuePush(result.first, result.last, new_node);
        result.count += 1;
        new_node->id = sub_header.id;
        new_node->size = clamped_size;
        new_node->off = data_off;
        
        // increment cursor
        U64 aligned_next_off = AlignUpPow2(end_of_data_off, riff_chunk_align);
        off = aligned_next_off;
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

function WAVE_FormatData
wave_format_data_from_fmt_chunk(WAVE_SubChunkNode *node, String8 data){
    WAVE_FormatData result = {};
    
    // try to read fmt info
    String8 node_data = str8_substr_size(data, node->off, node->size);
    U64 fmt_off = 0;
    WAVE_ChunkFmt fmt = {};
    if (str8_read_typed(node_data, fmt_off, &fmt)){
        
        // try to read extension info
        WAVE_ChunkFmtExt1 fmt_ext1 = {};
        fmt_ext1.valid_bits_per_sample = max_U16;
        {
            U64 extheader_off = fmt_off + sizeof(fmt);
            WAVE_ChunkFmtExtSize fmt_extheader = {};
            if (str8_read_typed(node_data, extheader_off, &fmt_extheader)){
                U64 ext_size = fmt_extheader.extension_size;
                U64 ext_size_clamped = ClampTop(ext_size, sizeof(fmt_ext1));
                U64 ext_off = extheader_off + sizeof(U16);
                str8_read_typed(node_data, ext_off, &fmt_ext1);
            }
        }
        
        // NOTE(allen): Expecting:
        //  (fmt.bytes_per_second == fmt.bytes_per_block*fmt.blocks_per_second)
        //  ignoring  bytes_per_second in parser right now.
        
        // sample size calculations
        U32 bits_stride = AlignUpPow2(fmt.bits_per_sample, 8);
        U32 bytes_stride = bits_stride/8;
        U32 bits = Min(fmt.bits_per_sample, fmt_ext1.valid_bits_per_sample);
        
        // fill output
        result.format_tag = fmt.format_tag;
        result.channel_count = fmt.channel_count;
        result.blocks_per_second = fmt.blocks_per_second;
        result.bytes_per_block = fmt.bytes_per_block;
        result.bytes_stride_per_sample = bytes_stride;
        result.bits_per_sample = bits;
        result.channel_mask = fmt_ext1.channel_mask;
        MemoryCopyArray(result.sub_format, fmt_ext1.sub_format);
    }
    
    return(result);
}

////////////////////////////////
// NOTE(allen): Wave Render Functions

function String8
wave_render(M_Arena *arena, WAVE_RenderParams *params){
    // TODO(allen): incorporate
    // NOTE(allen): According to the reference (need to test)
    //  In PCM bits_per_sample may be a non-multiple of 8
    //   to indicate that less than all of the bits are significant.
    //   Still stride by a multiple of 8 bits.
    //  In Non-PCM bits_per_sample must be a multiple of 8,
    //   and valid_bits_per_sample (if included) can be used to
    //   indicate the number of significant bits.
    
    // compute additional rates from params
    U32 bits_per_sample_rounded = AlignUpPow2(params->bits_per_sample, 8);
    U32 bytes_per_sample = bits_per_sample_rounded >> 3;
    U32 bytes_per_block  = bytes_per_sample*params->channel_count;
    U32 bytes_per_second = bytes_per_block*params->block_per_second;
    
    M_Scratch scratch(arena);
    String8List list = {};
    
    // master header
    RIFF_MasterChunkHeader *master = push_array(scratch, RIFF_MasterChunkHeader, 1);
    str8_list_push(scratch, &list, str8_typed(master));
    master->riff_id = RIFF_ID_RIFF;
    master->sub_id = WAVE_ID_WAVE;
    
    // fmt chunk
    {
        RIFF_SubChunkHeader *fmt_header = push_array(scratch, RIFF_SubChunkHeader, 1);
        str8_list_push(scratch, &list, str8_typed(fmt_header));
        fmt_header->id = WAVE_ID_fmt;
        
        // save position of fmt chunk
        U64 fmt_chunk_off = list.total_size;
        
        // determine format tag
        WAVE_FormatTag tag = 0;
        switch (params->kind){
            case WAVE_RenderKind_Signed:
            {
                tag = WAVE_FormatTag_PCM;
            }break;
            
            case WAVE_RenderKind_Float:
            {
                tag = WAVE_FormatTag_IEEE_FLOAT;
            }break;
        }
        
        // TODO(allen): this could be weird, we are pushing
        // the full sized fmt struct, but we're not actually
        // using the extension at all. Perhaps we need to the
        // fmt struct into smaller parts.
        
        // fill fmt data
        WAVE_ChunkFmt *fmt_data = push_array_zero(scratch, WAVE_ChunkFmt, 1);
        str8_list_push(scratch, &list, str8_typed(fmt_data));
        
        fmt_data->format_tag = tag;
        fmt_data->channel_count = params->channel_count;
        fmt_data->blocks_per_second = params->block_per_second;
        fmt_data->bytes_per_second = bytes_per_second;
        fmt_data->bits_per_sample  = params->bits_per_sample;
        
        // determine if extension is required
        B32 required_extension = false;
        if (params->channel_count > 2){
            required_extension = true;
        }
        
        if (required_extension){
            // fill fmt ext header data
            WAVE_ChunkFmtExtSize *fmt_extheader = push_array_zero(scratch, WAVE_ChunkFmtExtSize, 1);
            str8_list_push(scratch, &list, str8_typed(fmt_extheader));
            fmt_extheader->extension_size = sizeof(WAVE_ChunkFmtExt1);
            
            // determine channel mask (dummy path)
            WAVE_SpeakerFlags channel_mask = 0;
            if (params->channel_count > 0){
                channel_mask |= WAVE_SpeakerFlag_FRONT_LEFT;
            }
            if (params->channel_count > 1){
                channel_mask |= WAVE_SpeakerFlag_FRONT_RIGHT;
            }
            if (params->channel_count > 2){
                channel_mask |= WAVE_SpeakerFlag_BACK_LEFT;
            }
            if (params->channel_count > 3){
                channel_mask |= WAVE_SpeakerFlag_BACK_RIGHT;
            }
            
            // fill fmt ext data
            WAVE_ChunkFmtExt1 *fmt_ext = push_array_zero(scratch, WAVE_ChunkFmtExt1, 1);
            str8_list_push(scratch, &list, str8_typed(fmt_ext));
            fmt_ext->valid_bits_per_sample = params->bits_per_sample;
            fmt_ext->channel_mask = channel_mask;
            // TODO(allen): ??? fmt_ext->sub_format
        }
        
        // fill fmt header size
        fmt_header->size = list.total_size - fmt_chunk_off;
    }
    
    // data chunk
    {
        RIFF_SubChunkHeader *data_header = push_array(scratch, RIFF_SubChunkHeader, 1);
        str8_list_push(scratch, &list, str8_typed(data_header));
        data_header->id = WAVE_ID_data;
        
        // save position of data chunk
        U64 data_chunk_off = list.total_size;
        
        // swizzle the channels together
        String8 swizzled_data =
            bop_interleave(scratch, params->channels, params->channel_count,
                           bytes_per_sample, params->block_count);
        str8_list_push(scratch, &list, swizzled_data);
        
        // fill data header size
        data_header->size = list.total_size - data_chunk_off;
    }
    
    // fill master size
    master->size = list.total_size - 8;
    
    String8 result = str8_join(arena, &list, 0);
    return(result);
}

