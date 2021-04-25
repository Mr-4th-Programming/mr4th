/* date = April 23rd 2021 3:39 pm */

#ifndef WAVE_FORMAT_H
#define WAVE_FORMAT_H

////////////////////////////////
// NOTE(allen): Types defined for the "WAV" file format

// NOTE(allen): I am not clear on whether I want to name this
// "wave_format" and namespace it with "wave" or "riff_format"
// and namespace it with "riff". It doesn't matter a whole lot
// but I would certainly like it if my names reflected the
// correct level of reusability for these definitions. For now
// I went with "wave" because that implies less reusability,
// which seems like the right default assumptions.
// TODO(allen): study the specs more for clarification.

enum{
    WAVE_ID_RIFF = AsciiID4('R', 'I', 'F', 'F'),
    WAVE_ID_WAVE = AsciiID4('W', 'A', 'V', 'E'),
    WAVE_ID_fmt  = AsciiID4('f', 'm', 't', ' '),
    WAVE_ID_data = AsciiID4('d', 'a', 't', 'a'),
};

struct WAVE_RiffMasterChunkHeader{
    U32 riff_id;
    U32 size;
    U32 wave_id;
};

struct WAVE_RiffSubChunkHeader{
    U32 id;
    U32 size;
};

#endif //WAVE_FORMAT_H
