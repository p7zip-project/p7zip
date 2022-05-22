/*
* Copyright (c) 2018, Conor McCarthy
* All rights reserved.
*
* This source code is licensed under both the BSD-style license (found in the
* LICENSE file in the root directory of this source tree) and the GPLv2 (found
* in the COPYING file in the root directory of this source tree).
* You may select, at your option, one of the above-listed licenses.
*/

#include <stdio.h>
#include <stdlib.h>
#include <fast-lzma2.h> /* Assumes that libfast-lzma2 was installed using 'make install' */

static FILE *fin;
static FILE *fout;
static char out_name[4096];
static FL2_CStream *fcs;
static FL2_DStream *fds;

static void exit_fail(const char *msg)
{
    fputs(msg, stderr);
    exit(1);
}

static int compress_file(FL2_CStream *fcs)
{
    unsigned char in_buffer[8 * 1024];
    unsigned char out_buffer[4 * 1024];
    FL2_inBuffer in_buf = { in_buffer, sizeof(in_buffer), sizeof(in_buffer) };
    FL2_outBuffer out_buf = { out_buffer, sizeof(out_buffer), 0 };
    size_t res = 0;

    do {
        if (in_buf.pos == in_buf.size) {
            in_buf.size = fread(in_buffer, 1, sizeof(in_buffer), fin);
            in_buf.pos = 0;
        }
        res = FL2_compressStream(fcs, &out_buf, &in_buf);
        if (FL2_isError(res))
            goto error_out;

        fwrite(out_buf.dst, 1, out_buf.pos, fout);
        out_buf.pos = 0;

    } while (in_buf.size == sizeof(in_buffer));
    do {
        res = FL2_endStream(fcs, &out_buf);
        if (FL2_isError(res))
            goto error_out;

        fwrite(out_buf.dst, 1, out_buf.pos, fout);
        out_buf.pos = 0;
    } while (res);

    return 0;

error_out:
    fprintf(stderr, "Error: %s\n", FL2_getErrorName(res));
    return 1;
}

static int decompress_file(FL2_DStream *fds)
{
    unsigned char in_buffer[4 * 1024];
    unsigned char out_buffer[8 * 1024];
    FL2_inBuffer in_buf = { in_buffer, sizeof(in_buffer), sizeof(in_buffer) };
    FL2_outBuffer out_buf = { out_buffer, sizeof(out_buffer), 0 };
    size_t res;

    do {
        if (in_buf.pos == in_buf.size) {
            in_buf.size = fread(in_buffer, 1, sizeof(in_buffer), fout);
            in_buf.pos = 0;
        }
        res = FL2_decompressStream(fds, &out_buf, &in_buf);
        if (FL2_isError(res))
            goto error_out;
        /* Discard the output. XXhash will verify the integrity. */
        out_buf.pos = 0;
    } while (res && in_buf.size);
    return 0;

error_out:
    fprintf(stderr, "Error: %s\n", FL2_getErrorName(res));
    return 1;
}

static void open_files(const char *name)
{
    fin = fopen(name, "rb");
    if (fin == NULL)
        exit_fail("Cannot open input file.\n");

    snprintf(out_name, sizeof(out_name), "%s.lzma2", name);
    out_name[sizeof(out_name) - 1] = '\0';

    fout = fopen(out_name, "w+b");
    if (fout == NULL)
        exit_fail("Cannot open output file.\n");
}

static void create_init_fl2_streams(int preset)
{
    fcs = FL2_createCStreamMt(2, 0);
    if (fcs == NULL)
        exit_fail("Cannot allocate compression context.\n");

    fds = FL2_createDStreamMt(2);
    if (fds == NULL)
        exit_fail("Cannot allocate decompression context.\n");

    size_t res = FL2_initCStream(fcs, preset);
    if(!res)
        res = FL2_initDStream(fds);
    if (FL2_isError(res)) {
        fprintf(stderr, "Error: %s\n", FL2_getErrorName(res));
        exit(1);
    }
}

int main(int argc, char **argv)
{
    int ret = 1;
    static const char *usage = "Usage: file_test [-1..10] FILE\n";

    if (argc < 2)
        exit_fail(usage);

    int preset = 6;
    for (int i = 1; i < argc; ++i)
        if (argv[i][0] == '-')
            preset = atoi(argv[i] + 1);

    const char *name = NULL;
    for (int i = 1; i < argc; ++i)
        if (argv[i][0] != '-') {
            name = argv[i];
            break;
        }
    if (name == NULL)
        exit_fail(usage);

    create_init_fl2_streams(preset);
    open_files(name);

    if (compress_file(fcs))
        goto cleanup;

    fseek(fout, 0, SEEK_SET);
    ret = decompress_file(fds);

cleanup:
    fclose(fout);
    fclose(fin);
    remove(out_name);
    FL2_freeCStream(fcs);
    FL2_freeDStream(fds);
    return ret;
}