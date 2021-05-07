/* date = May 7th 2021 0:25 pm */

#ifndef RIFF_FORMAT_H
#define RIFF_FORMAT_H

////////////////////////////////
// NOTE(allen): Types defined for the "RIFF" file format

enum{
    RIFF_ID_RIFF = AsciiID4('R', 'I', 'F', 'F'),
};

struct RIFF_MasterChunkHeader{
    U32 riff_id;
    U32 size;
    U32 sub_id;
};

struct RIFF_SubChunkHeader{
    U32 id;
    U32 size;
};

global const U64 riff_chunk_align = 2;

#endif //RIFF_FORMAT_H
