/* date = July 24th 2021 6:41 pm */

#ifndef BASE_PRNG_H
#define BASE_PRNG_H

////////////////////////////////
// TODO(allen): Notes, Unfinished Issues, Etc.
// [ ] Put in a better pseudorandom number generator.
//     This is just doing a lame pcg knock off, probably just get the real
//     deal up in here!
// [ ] Build out more high level features for a good prng layer.

////////////////////////////////
// NOTE(allen): PRNG Type(s)

struct PRNG{
    U64 s;
};

////////////////////////////////
// NOTE(allen): PRNG Functions

function PRNG prng_make_from_seed(U64 seed);
function U32  prng_next_u32(PRNG *prng);
function U64  prng_next_u64(PRNG *prng);

// [0,1]
function F32 prng_next_unital_f32(PRNG *prng);
// [-1,1]
function F32 prng_next_biunital_f32(PRNG *prng);

#endif //BASE_PRNG_H
