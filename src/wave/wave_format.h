/* date = April 23rd 2021 3:39 pm */

#ifndef WAVE_FORMAT_H
#define WAVE_FORMAT_H

//requires: riff_format

// definitions:
//  sample  - one value in a sequence of values forming a signal
//  channel - one (of several) "parallel" signals
//            i.e. signals with the same number of samples
//  block   - a sample for each channel, at a single sequence position

////////////////////////////////
// NOTE(allen): Types defined for the "WAV" file format

enum{
    WAVE_ID_WAVE = AsciiID4('W', 'A', 'V', 'E'),
    WAVE_ID_fmt  = AsciiID4('f', 'm', 't', ' '),
    WAVE_ID_data = AsciiID4('d', 'a', 't', 'a'),
};

////////////////////////////////
// NOTE(allen): Speaker Spatial Flags

typedef U32 WAVE_SpeakerFlags;
enum{
    WAVE_SpeakerFlag_FRONT_LEFT            = (1 << 0),
    WAVE_SpeakerFlag_FRONT_RIGHT           = (1 << 1),
    WAVE_SpeakerFlag_FRONT_CENTER          = (1 << 2),
    WAVE_SpeakerFlag_LOW_FREQUENCY         = (1 << 3),
    WAVE_SpeakerFlag_BACK_LEFT             = (1 << 4),
    WAVE_SpeakerFlag_BACK_RIGHT            = (1 << 5),
    WAVE_SpeakerFlag_FRONT_LEFT_OF_CENTER  = (1 << 6),
    WAVE_SpeakerFlag_FRONT_RIGHT_OF_CENTER = (1 << 7),
    WAVE_SpeakerFlag_BACK_CENTER           = (1 << 8),
    WAVE_SpeakerFlag_SIDE_LEFT             = (1 << 9),
    WAVE_SpeakerFlag_SIDE_RIGHT            = (1 << 10),
    WAVE_SpeakerFlag_TOP_CENTER            = (1 << 11),
    WAVE_SpeakerFlag_TOP_FRONT_LEFT        = (1 << 12),
    WAVE_SpeakerFlag_TOP_FRONT_CENTER      = (1 << 13),
    WAVE_SpeakerFlag_TOP_FRONT_RIGHT       = (1 << 14),
    WAVE_SpeakerFlag_TOP_BACK_LEFT         = (1 << 15),
    WAVE_SpeakerFlag_TOP_BACK_CENTER       = (1 << 16),
    WAVE_SpeakerFlag_TOP_BACK_RIGHT        = (1 << 17),
};

////////////////////////////////
// NOTE(allen): "fmt " Sub Chunk

typedef U16 WAVE_FormatTag;
enum{
    WAVE_FormatTag_PCM = 0x0001,
    WAVE_FormatTag_IEEE_FLOAT = 0x0003,
    WAVE_FormatTag_ALAW = 0x0006,
    WAVE_FormatTag_MULAW = 0x0007,
    WAVE_FormatTag_EXTENSIBLE = 0xFFFE,
};

struct WAVE_ChunkFmt{
    WAVE_FormatTag format_tag;
    U16 channel_count;
    U32 blocks_per_second;
    U32 bytes_per_second;
    U16 bytes_per_block;
    U16 bits_per_sample;
};

struct WAVE_ChunkFmtExtSize{
    U16 extension_size;
};

#pragma pack(push, 1)
struct WAVE_ChunkFmtExt1{
    U16 valid_bits_per_sample;
    WAVE_SpeakerFlags channel_mask;
    U8  sub_format[16];
};
#pragma pack(pop)

#endif //WAVE_FORMAT_H
