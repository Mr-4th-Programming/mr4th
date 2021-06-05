/* date = May 7th 2021 11:17 am */

#ifndef BASE_BUFFER_OPS_H
#define BASE_BUFFER_OPS_H

////////////////////////////////
// NOTE(allen): Interleaving Functions

function String8  bop_interleave(M_Arena *arena, void **in,
                                 U64 lane_count, U64 el_size, U64 el_count);

function String8* bop_uninterleave(M_Arena *arena, void *in,
                                   U64 lane_count, U64 el_size, U64 el_count);

////////////////////////////////
// NOTE(allen): Conversions

function String8 bop_f32_from_s24(M_Arena *arena, String8 in);

#endif //BASE_BUFFER_OPS_H
