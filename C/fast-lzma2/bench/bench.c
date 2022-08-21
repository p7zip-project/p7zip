// bench.cpp : Defines the entry point for the console application.
//

#include <stdlib.h>
#include <stdio.h>
#include "../fast-lzma2.h"
#include "../fuzzer/datagen.h"
#include "../mem.h"
#include "../util.h"

#define TIMELOOP_MICROSEC     1*1000000ULL /* 1 second */
#define ACTIVEPERIOD_MICROSEC 70*1000000ULL /* 70 seconds */
#define COOLPERIOD_SEC        5

#define KB *(1 <<10)
#define MB *(1 <<20)
#define GB *(1U<<30)

static U32 g_nbSeconds = 0;
static unsigned g_iterations = 2;

static void benchmark(FL2_CCtx* fcs, FL2_DCtx* dctx, char* srcBuffer, size_t srcSize, char* compressedBuffer, size_t maxCompressedSize,
    char* resultBuffer)
{

//    RDG_genBuffer(compressedBuffer, maxCompressedSize, 0.10, 0.50, 1);

    U64 fastestC = (U64)(-1LL), fastestD = (U64)(-1LL);
    UTIL_time_t coolTime;
    U64 const maxTime = (g_nbSeconds * TIMELOOP_MICROSEC) + 1;
    U64 totalCTime = 0, totalDTime = 0;
    U32 cCompleted = 0, dCompleted = 0;
    unsigned totalCLoops = 0, totalDLoops = 0;
#       define NB_MARKS 4
    const char* const marks[NB_MARKS] = { " |", " /", " =",  "\\" };
    U32 markNb = 0;

    coolTime = UTIL_getTime();
    printf( "\r%79s\r", "");
    size_t cSize = 0;
    while (!cCompleted || !dCompleted) {

        /* overheat protection */
        if (UTIL_clockSpanMicro(coolTime) > ACTIVEPERIOD_MICROSEC) {
            printf( "\rcooling down ...    \r");
            UTIL_sleep(COOLPERIOD_SEC);
            printf("\r                    \r");
            coolTime = UTIL_getTime();
        }

        UTIL_time_t clockStart;
        /* Compression */
        if (!cCompleted) memset(compressedBuffer, 0xE5, maxCompressedSize);  /* warm up and erase result buffer */

        UTIL_waitForNextTick();
        clockStart = UTIL_getTime();

        if (!cCompleted) {   /* still some time to do compression tests */
            U64 const clockLoop = g_nbSeconds ? TIMELOOP_MICROSEC : 1;
            U32 nbLoops = 0;
            do {
                cSize = FL2_compressCCtx(fcs, compressedBuffer, maxCompressedSize, srcBuffer, srcSize, 0);
                if (FL2_isError(cSize)) {
                    printf("FL2_compressCCtx() error : %s  \r\n", FL2_getErrorName(cSize));
                    return;
                }
                nbLoops++;
            } while (UTIL_clockSpanMicro(clockStart) < clockLoop);
            U64 const loopDuration = UTIL_clockSpanMicro(clockStart);
            if (loopDuration < fastestC*nbLoops)
                fastestC = loopDuration / nbLoops;
            totalCTime += loopDuration;
            totalCLoops += nbLoops;
            cCompleted = (totalCTime >= maxTime && totalCLoops >= g_iterations);  /* end compression tests */
        }

#if 0       /* disable decompression test */
        dCompleted = 1;
        (void)totalDTime; (void)fastestD;   /* unused when decompression disabled */
#else
        /* Decompression */
        if (!dCompleted) memset(resultBuffer, 0xD6, srcSize);  /* warm result buffer */

        UTIL_waitForNextTick();

        if (!dCompleted) {
            U64 clockLoop = g_nbSeconds ? TIMELOOP_MICROSEC : 1;
            U32 nbLoops = 0;
            UTIL_time_t const clockStart = UTIL_getTime();
            do {
                size_t const regenSize = FL2_decompressDCtx(dctx,
                    resultBuffer, srcSize,
                    compressedBuffer, cSize);
                if (FL2_isError(regenSize)) {
                    printf("FL2_decompressDCtx() failed on size %u : %s  \r\n",
                        (unsigned)cSize, FL2_getErrorName(regenSize));
                    return;
                }
                nbLoops++;
            } while (UTIL_clockSpanMicro(clockStart) < clockLoop);
        {   U64 const loopDuration = UTIL_clockSpanMicro(clockStart);
            if (loopDuration < fastestD*nbLoops)
                fastestD = loopDuration / nbLoops;
            totalDTime += loopDuration;
            totalDLoops += nbLoops;
            dCompleted = (totalDTime >= maxTime && totalDLoops >= g_iterations);
        }
            if (memcmp(resultBuffer, srcBuffer, srcSize) != 0)
                printf("Corruption on dSize %u cSize %u\r\n", (unsigned)srcSize, (unsigned)cSize);
        }

#endif
        double ratio = (double)srcSize / (double)cSize;
        markNb = (markNb + 1) % NB_MARKS;
        {   int const ratioAccuracy = (ratio < 10.) ? 3 : 2;
        double const compressionSpeed = (double)srcSize / fastestC;
        int const cSpeedAccuracy = 2;// (compressionSpeed < 10.) ? 2 : 1;
        double const decompressionSpeed = (double)srcSize / fastestD;
        printf("%2s-%-17.17s :%10u ->%10u (%5.*f),%6.*f MB/s ,%6.1f MB/s\r",
            marks[markNb], "", (U32)srcSize, (U32)cSize,
            ratioAccuracy, ratio,
            cSpeedAccuracy, compressionSpeed,
            decompressionSpeed);
        }
    }
}

static int parse_params(FL2_CCtx* fcs, int argc, char** argv)
{
    for (int i = 2; i < argc; ++i) {
        if (argv[i][0] == '-' && argv[i][1] >= '0' && argv[i][1] <= '9')
            FL2_CCtx_setParameter(fcs, FL2_p_compressionLevel, atoi(argv[i] + 1));
    }
    int end_level = 0;
    for (int i = 2; i < argc; ++i) {
        if (argv[i][0] != '-')
            continue;
        char param[4];
        int j = 1;
        for (; j < 4 && argv[i][j] && (argv[i][j] < '0' || argv[i][j] > '9'); ++j) {
            param[j-1] = argv[i][j];
        }
        param[j-1] = 0;
        unsigned value = atoi(argv[i] + j);
        if (strcmp(param, "t") == 0) {
            g_nbSeconds = value;
        }
        else if (strcmp(param, "i") == 0) {
            g_iterations = value;
        }
        else if(strcmp(param, "d") == 0) {
            FL2_CCtx_setParameter(fcs, FL2_p_dictionaryLog, value);
        }
        else if (strcmp(param, "o") == 0) {
            FL2_CCtx_setParameter(fcs, FL2_p_overlapFraction, value);
        }
        else if (strcmp(param, "ds") == 0) {
            FL2_CCtx_setParameter(fcs, FL2_p_hybridChainLog, value);
        }
        else if (strcmp(param, "mc") == 0) {
            FL2_CCtx_setParameter(fcs, FL2_p_hybridCycles, value);
        }
        else if (strcmp(param, "sd") == 0) {
            FL2_CCtx_setParameter(fcs, FL2_p_searchDepth, value);
        }
        else if (strcmp(param, "fb") == 0) {
            FL2_CCtx_setParameter(fcs, FL2_p_fastLength, value);
        }
        else if (strcmp(param, "q") == 0) {
            FL2_CCtx_setParameter(fcs, FL2_p_divideAndConquer, value);
        }
        else if (strcmp(param, "bm") == 0) {
            FL2_CCtx_setParameter(fcs, FL2_p_resetInterval, value);
        }
        else if (strcmp(param, "b") == 0) {
            FL2_CCtx_setParameter(fcs, FL2_p_bufferResize, value);
        }
        else if (strcmp(param, "a") == 0) {
            FL2_CCtx_setParameter(fcs, FL2_p_strategy, value);
        }
        else if (strcmp(param, "h") == 0) {
            FL2_CCtx_setParameter(fcs, FL2_p_doXXHash, value);
        }
        else if (strcmp(param, "x") == 0) {
            FL2_CCtx_setParameter(fcs, FL2_p_highCompression, value);
        }
        else if (strcmp(param, "e") == 0) {
            end_level = value;
        }
#ifdef RMF_REFERENCE
        else if (strcmp(param, "r") == 0) {
            FL2_CCtx_setParameter(fcs, FL2_p_useReferenceMF, value);
        }
#endif
    }
    return end_level;
}

int FL2LIB_CALL main(int argc, char** argv)
{
    if (argc < 2)
        return 1;
    size_t size;
    char* src;
    if (1) {
        FILE* f = fopen(argv[1], "rb");
        if (f == NULL)
            return 1;
        fseek(f, 0, 2);
        size = ftell(f);
        if (size > 3UL << 29) size = 3UL << 29;
        fseek(f, 0, 0);
        src = malloc(size);
        size = fread(src, 1, size, f);
        fclose(f);
    }
    else {
        size = 1UL << 30;
        src = malloc(size);
        RDG_genBuffer(src, size, 0.6, 0.02, 1);
    }
    unsigned threads = 1;
    unsigned dthreads = ~0U;
    for (int i = 2; i < argc; ++i) {
        if (argv[i][0] == '-' && argv[i][1] == 'T')
            threads = atoi(argv[i] + 2);
        if (argv[i][0] == '-' && argv[i][1] == 'D')
            dthreads = atoi(argv[i] + 2);
    }
    if (dthreads == ~0U)
        dthreads = threads;
    FL2_CCtx* fcs = FL2_createCCtxMt(threads);
    FL2_DCtx* dctx = FL2_createDCtxMt(dthreads);
    if (fcs == NULL)
        return 1;
    int end_level = parse_params(fcs, argc, argv);
    int level = (int)FL2_CCtx_getParameter(fcs, FL2_p_compressionLevel);
    size_t maxCompressedSize = FL2_compressBound(size);
    char* compressedBuffer = malloc(maxCompressedSize);
    char* resultBuffer = malloc(size);
    if (!end_level)
        end_level = level;
    else if (end_level > FL2_maxCLevel())
        end_level = FL2_maxCLevel();
    for (; level <= end_level; ++level) {
        benchmark(fcs, dctx, src, size, compressedBuffer, maxCompressedSize, resultBuffer);
        FL2_CCtx_setParameter(fcs, FL2_p_compressionLevel, level + 1);
        printf("%u\r\n", level);
    }
    FL2_freeDCtx(dctx);
    FL2_freeCCtx(fcs);
    free(resultBuffer);
    free(compressedBuffer);
    free(src);
    return 0;
}

