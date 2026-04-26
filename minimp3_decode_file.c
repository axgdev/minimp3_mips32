#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MINIMP3_FIXED_POINT
#define MINIMP3_ONLY_MP3
#define MINIMP3_NO_SIMD
#define MINIMP3_NO_ARMV6
#define MINIMP3_NONSTANDARD_BUT_LOGICAL
#define MINIMP3_IMPLEMENTATION
#include "minimp3_ex.h"

static void write_u16le(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
}

static void write_u32le(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)(v >> 16);
    p[3] = (uint8_t)(v >> 24);
}

static int write_wav_header(FILE *f, int hz, int channels, uint32_t data_bytes)
{
    uint8_t h[44];
    memcpy(h + 0, "RIFF", 4);
    write_u32le(h + 4, 36u + data_bytes);
    memcpy(h + 8, "WAVEfmt ", 8);
    write_u32le(h + 16, 16);
    write_u16le(h + 20, 1);
    write_u16le(h + 22, (uint16_t)channels);
    write_u32le(h + 24, (uint32_t)hz);
    write_u32le(h + 28, (uint32_t)(hz * channels * 2));
    write_u16le(h + 32, (uint16_t)(channels * 2));
    write_u16le(h + 34, 16);
    memcpy(h + 36, "data", 4);
    write_u32le(h + 40, data_bytes);
    return fwrite(h, 1, sizeof(h), f) == sizeof(h) ? 0 : -1;
}

int main(int argc, char **argv)
{
    mp3dec_t mp3d;
    mp3dec_file_info_t info;
    FILE *out;
    uint32_t data_bytes;

    if (argc != 3)
    {
        fprintf(stderr, "usage: %s input.mp3 output.wav\n", argv[0]);
        return 2;
    }

    memset(&info, 0, sizeof(info));
    if (mp3dec_load(&mp3d, argv[1], &info, NULL, NULL))
    {
        fprintf(stderr, "error: failed to decode %s\n", argv[1]);
        return 1;
    }

    if (!info.buffer || info.samples <= 0 || info.hz <= 0 || info.channels <= 0)
    {
        free(info.buffer);
        fprintf(stderr, "error: no audio decoded from %s\n", argv[1]);
        return 1;
    }

    if (info.samples > UINT32_MAX / sizeof(mp3d_sample_t))
    {
        free(info.buffer);
        fprintf(stderr, "error: decoded audio is too large for wav output\n");
        return 1;
    }
    data_bytes = (uint32_t)(info.samples * sizeof(mp3d_sample_t));

    out = fopen(argv[2], "wb");
    if (!out)
    {
        free(info.buffer);
        fprintf(stderr, "error: failed to open %s\n", argv[2]);
        return 1;
    }

    if (write_wav_header(out, info.hz, info.channels, data_bytes) ||
        fwrite(info.buffer, sizeof(mp3d_sample_t), info.samples, out) != info.samples)
    {
        fclose(out);
        free(info.buffer);
        fprintf(stderr, "error: failed to write %s\n", argv[2]);
        return 1;
    }

    fclose(out);
    printf("rate=%d channels=%d samples=%zu wrote=%s\n", info.hz, info.channels, info.samples, argv[2]);
    free(info.buffer);
    return 0;
}
