#ifndef MINIMP3_MIPS32R1_SOFTFLOAT_H
#define MINIMP3_MIPS32R1_SOFTFLOAT_H

/*
   Profile for the SF2000/GB300-style target:
   little-endian MIPS32r1, soft-float, size-oriented, no host file helpers.

   This profile strips minimp3's optional code paths and selects the integer
   fixed-point decoder core.
*/

#define MINIMP3_ONLY_MP3
#define MINIMP3_NO_SIMD
#define MINIMP3_NO_ARMV6
#define MINIMP3_NO_STDIO
#define MINIMP3_NONSTANDARD_BUT_LOGICAL
#define MINIMP3_FIXED_POINT

#endif /* MINIMP3_MIPS32R1_SOFTFLOAT_H */
