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

////////////////////////////////
// NOTE(allen): Wave Parser Functions

function WAVE_SubChunkList wave_sub_chunks_from_data(M_Arena *arena, String8 data);

#endif //WAVE_PARSER_H
