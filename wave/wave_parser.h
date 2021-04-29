/* date = April 23rd 2021 3:48 pm */

#ifndef WAVE_PARSER_H
#define WAVE_PARSER_H

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
    U16 channel_count;
    U32 block_per_second;
    U32 bytes_per_second;
    U16 bytes_per_block;
    U16 bits_per_sample;
    U16 valid_bits_per_sample;
    U32 channel_mask;
    U8  sub_format[16];
};

////////////////////////////////
// NOTE(allen): Wave Render Types

enum WAVE_RenderKind{
    WAVE_RenderKind_Signed,
    WAVE_RenderKind_Float,
};

struct WAVE_RenderParams{
    WAVE_RenderKind kind;
    U32 channel_count;
    U32 block_count;
    U16 block_per_second;
    U16 bits_per_sample;
    void **channels;
};

////////////////////////////////
// NOTE(allen): Wave Parser Functions

function WAVE_SubChunkList wave_sub_chunks_from_data(M_Arena *arena, String8 data);
function WAVE_SubChunkNode* wave_chunk_from_id(WAVE_SubChunkList list, U32 id);

function WAVE_FormatData wave_format_data_from_fmt_chunk(WAVE_SubChunkNode *node, String8 data);

////////////////////////////////
// NOTE(allen): Wave Render Functions

function String8 wave_render(M_Arena *arena, WAVE_RenderParams *params);

#endif //WAVE_PARSER_H
