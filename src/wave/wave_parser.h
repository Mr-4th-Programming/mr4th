/* date = April 23rd 2021 3:48 pm */

#ifndef WAVE_PARSER_H
#define WAVE_PARSER_H

// TODO(allen): In its simplest form, it adds a <bext> chunk with
// additional metadata.  Full documentation is available on line from the EBU.

////////////////////////////////
// NOTE(allen): Wave Parser Types

struct WAVE_SubChunkNode{
    WAVE_SubChunkNode *next;
    U32 id;
    U32 size;
    U64 off;
};

struct WAVE_SubChunkList{
    WAVE_SubChunkNode *first;
    WAVE_SubChunkNode *last;
    U64 count;
};

struct WAVE_FormatData{
    WAVE_FormatTag format_tag;
    U32 channel_count;
    U32 bytes_per_block;
    U32 blocks_per_second;
    U32 bytes_stride_per_sample;
    U32 bits_per_sample;
    U32 channel_mask;
    U8  sub_format[16];
};

////////////////////////////////
// NOTE(allen): Wave Render Types

typedef U32 WAVE_RenderKind;
enum{
    // [0,0xFFFF] reserved for signed types
    // with bit_size(WAVE_RenderKind_Signed0 + n) = n
    WAVE_RenderKind_Signed0  = (0x0 << 16),
    
    // [0x10000,0x1FFFF] reserved for float types
    // with bit_size(WAVE_RenderKind_Float<n>) = n
    WAVE_RenderKind_Float16 = (0x1 << 16),
    WAVE_RenderKind_Float32,
    WAVE_RenderKind_Float64,
};

struct WAVE_RenderParams{
    WAVE_RenderKind kind;
    U32 channel_count;
    U16 block_per_second;
};

////////////////////////////////
// NOTE(allen): Wave Parser Functions

function WAVE_SubChunkList
wave_sub_chunks_from_data(M_Arena *arena, String8 data);

function WAVE_SubChunkNode*
wave_chunk_from_id(WAVE_SubChunkList list, U32 id);

function WAVE_FormatData
wave_format_data_from_fmt_chunk(WAVE_SubChunkNode *node, String8 data);

////////////////////////////////
// NOTE(allen): Wave Render Functions

function String8 wave_render(M_Arena *arena, WAVE_RenderParams *params,
                             String8 sample_data);

////////////////////////////////
// NOTE(allen): Wave Debug Functions

function void wave_dump(M_Arena *arena, String8List *out, String8 data);

#endif //WAVE_PARSER_H
