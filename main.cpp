#include "base/base_inc.h"
#include "os/os_inc.h"

#include "wave/riff_format.h"
#include "wave/wave_format.h"
#include "wave/wave_parser.h"


#include "base/base_inc.cpp"
#include "os/os_inc.cpp"

#include "wave/wave_parser.cpp"

#include <stdio.h>

int main(int argc, char **argv){
    OS_ThreadContext tctx_memory = {};
    os_main_init(&tctx_memory, argc, argv);
    
    M_Scratch scratch;
    
    String8 data = os_file_read(scratch, str8_lit("Binary Main Frame.wav"));
    
    printf("sizeof 'Binary Main Frame.wav' = %llu\n", data.size);
    
    // print all chunks
    WAVE_SubChunkList wave_chunks = wave_sub_chunks_from_data(scratch, data);
    printf("wave chunk count: %llu\n", wave_chunks.count);
    
    U64 chunk_counter = 0;
    for (WAVE_SubChunkNode *node = wave_chunks.first;
         node != 0;
         node = node->next, chunk_counter += 1){
        printf("chunk #%llu:\n", chunk_counter);
        printf(" id:   %.*s\n", ExpandAsciiID(node->id));
        printf(" size: %u\n", node->size);
        printf(" off:  0x%llx\n", node->off);
    }
    
    // print fmt chunk specifics
    WAVE_FormatData fmt_data = {};
    WAVE_SubChunkNode *fmt_node = wave_chunk_from_id(wave_chunks, WAVE_ID_fmt);
    if (fmt_node != 0){
        fmt_data = wave_format_data_from_fmt_chunk(fmt_node, data);
    }
    
    printf("fmt data:\n");
    printf(" format_tag:        %-5u\n", fmt_data.format_tag);
    printf(" channel_count:     %-5u\n", fmt_data.channel_count);
    printf(" blocks_per_second: %-5u\n", fmt_data.blocks_per_second);
    //printf(" bytes_per_second:  %-5u\n", fmt_data.bytes_per_second);
    printf(" bytes_per_block:   %-5u\n", fmt_data.bytes_per_block);
    printf(" bytes_stride_per_sample: %u\n", fmt_data.bytes_stride_per_sample);
    printf(" bits_per_sample:   %-5u\n", fmt_data.bits_per_sample);
    printf(" channel_mask:      %08x\n", fmt_data.channel_mask);
    printf(" sub_format:        %u-%02x%02x%02x%02x%02x%02x"
           "%02x%02x%02x%02x%02x%02x%02x%02x\n",
           *(U16*)fmt_data.sub_format,
           fmt_data.sub_format[2], fmt_data.sub_format[3],
           fmt_data.sub_format[4], fmt_data.sub_format[5],
           fmt_data.sub_format[6], fmt_data.sub_format[7],
           fmt_data.sub_format[8], fmt_data.sub_format[9],
           fmt_data.sub_format[10], fmt_data.sub_format[11],
           fmt_data.sub_format[12], fmt_data.sub_format[13],
           fmt_data.sub_format[14], fmt_data.sub_format[15]);
    
    // print sample data chunk specifics
    String8 sample_data = {};
    WAVE_SubChunkNode *data_node = wave_chunk_from_id(wave_chunks, WAVE_ID_data);
    if (data_node != 0){
        sample_data = str8_substr_size(data, data_node->off, data_node->size);
    }
    
    printf("sample data:\n");
    printf(" size: %llu\n", sample_data.size);
    printf(" values: %02x%02x%02x%02x%02x%02x%02x%02x"
           "%02x%02x%02x%02x%02x%02x%02x%02x...\n",
           sample_data.str[0], sample_data.str[1],
           sample_data.str[2], sample_data.str[3],
           sample_data.str[4], sample_data.str[5],
           sample_data.str[6], sample_data.str[7],
           sample_data.str[8], sample_data.str[9],
           sample_data.str[10], sample_data.str[11],
           sample_data.str[12], sample_data.str[13],
           sample_data.str[14], sample_data.str[15]);
    
    // test render files
    if (fmt_data.channel_count <= 2){
        // rates and sizes
        U64 stride = fmt_data.bytes_per_block/fmt_data.channel_count;
        U64 block_count = sample_data.size/fmt_data.bytes_per_block;
        
        // unswizzled channels
        String8 *unswizzled_s24 =
            bop_uninterleave(scratch, sample_data.str, fmt_data.channel_count,
                             stride, block_count);
        
        // convert to float
        String8 unswizzled_f32[2];
        for (U64 i = 0; i < 2; i += 1){
            String8 samples = unswizzled_s24[i];
            unswizzled_f32[i] = bop_f32_from_s24(scratch, samples);
        }
        
        
        // setup default params
        WAVE_RenderParams wave_render_params = {};
        wave_render_params.kind = WAVE_RenderKind_Float32;
        wave_render_params.channel_count    = 2;
        wave_render_params.block_per_second = fmt_data.blocks_per_second;
        
        void *channels[4];
        for (U32 i = 0; i < 2; i += 1){
            channels[i]     = unswizzled_f32[i].str;
            channels[i + 2] = unswizzled_f32[i].str;
        }
        
        // render with default params
        {
            String8 swizzled = bop_interleave(scratch, channels, 2, 4, block_count);
            String8 rendered_data = wave_render(scratch, &wave_render_params, swizzled);
            os_file_write(str8_lit("wav_test1.wav"), rendered_data);
        }
        
        // render wtih truncated time
        {
            String8 swizzled = bop_interleave(scratch, channels, 2, 4,
                                              wave_render_params.block_per_second);
            String8 rendered_data = wave_render(scratch, &wave_render_params, swizzled);
            os_file_write(str8_lit("wav_test2.wav"), rendered_data);
        }
        
        // render with silent right channel
        {
            void *silent_channel = push_array_zero(scratch, U8, unswizzled_f32[1].size);
            channels[1] = silent_channel;
            
            String8 swizzled = bop_interleave(scratch, channels, 2, 4, block_count);
            String8 rendered_data = wave_render(scratch, &wave_render_params, swizzled);
            os_file_write(str8_lit("wav_test3.wav"), rendered_data);
            
            channels[1] = unswizzled_f32[1].str;
        }
    }
}

