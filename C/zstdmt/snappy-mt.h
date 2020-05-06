/* ***************************************
 * Defines
 ****************************************/

#ifndef SNAPPYMT_H
#define SNAPPYMT_H

#if defined (__cplusplus)
extern "C" {
#endif

#include <stddef.h>   /* size_t */


#define SNAPPYMT_THREAD_MAX 128
#define SNAPPYMT_MAGICNUMBER 0x5053 // SP
#define SNAPPYMT_MAGIC_SKIPPABLE 0x184D2A50U  // MT magic number

/* **************************************
 * Error Handling
 ****************************************/

typedef enum {
  SNAPPYMT_error_no_error,
  SNAPPYMT_error_memory_allocation,
  SNAPPYMT_error_read_fail,
  SNAPPYMT_error_write_fail,
  SNAPPYMT_error_data_error,
  SNAPPYMT_error_frame_compress,
  SNAPPYMT_error_frame_decompress,
  SNAPPYMT_error_compressionParameter_unsupported,
  SNAPPYMT_error_compression_library,
  SNAPPYMT_error_canceled,
  SNAPPYMT_error_maxCode
} SNAPPYMT_ErrorCode;

#define PREFIX(name) SNAPPYMT_error_##name
#define MT_ERROR(name)  ((size_t)-PREFIX(name))
extern unsigned SNAPPYMT_isError(size_t code);
extern const char* SNAPPYMT_getErrorString(size_t code);

/* **************************************
 * Structures
 ****************************************/

typedef struct {
	void *buf;		/* ptr to data */
	size_t size;		/* current filled in buf */
	size_t allocated;	/* length of buf */
} SNAPPYMT_Buffer;

/**
 * reading and writing functions
 * - you can use stdio functions or plain read/write
 * - just write some wrapper on your own
 * - a sample is given in 7-Zip ZS or bromt.c
 * - the function should return -1 on error and zero on success
 * - the read or written bytes will go to in->size or out->size
 */
typedef int (fnRead) (void *args, SNAPPYMT_Buffer * in);
typedef int (fnWrite) (void *args, SNAPPYMT_Buffer * out);

typedef struct {
	fnRead *fn_read;
	void *arg_read;
	fnWrite *fn_write;
	void *arg_write;
} SNAPPYMT_RdWr_t;

/* **************************************
 * Compression
 ****************************************/

typedef struct SNAPPYMT_CCtx_s SNAPPYMT_CCtx;

/**
 * 1) allocate new cctx
 * - return cctx or zero on error
 *
 * @level   - 1 .. 9
 * @threads - 1 .. BROTLIMT_THREAD_MAX
 * @inputsize - if zero, becomes some optimal value for the level
 *            - if nonzero, the given value is taken
 */
SNAPPYMT_CCtx *SNAPPYMT_createCCtx(int threads, int level,/*Not use*/ 
                                   int inputsize);

/**
 * 2) threaded compression
 * - errorcheck via 
 */
size_t SNAPPYMT_compressCCtx(SNAPPYMT_CCtx * ctx, SNAPPYMT_RdWr_t * rdwr);

/**
 * 3) get some statistic
 */
size_t SNAPPYMT_GetFramesCCtx(SNAPPYMT_CCtx * ctx);
size_t SNAPPYMT_GetInsizeCCtx(SNAPPYMT_CCtx * ctx);
size_t SNAPPYMT_GetOutsizeCCtx(SNAPPYMT_CCtx * ctx);

/**
 * 4) free cctx
 * - no special return value
 */
void SNAPPYMT_freeCCtx(SNAPPYMT_CCtx * ctx);

/* **************************************
 * Decompression
 ****************************************/

typedef struct SNAPPYMT_DCtx_s SNAPPYMT_DCtx;

/**
 * 1) allocate new cctx
 * - return cctx or zero on error
 *
 * @threads - 1 .. BROTLIMT_THREAD_MAX
 * @ inputsize - used for single threaded standard bro format without 
 *   skippable frames
 */
SNAPPYMT_DCtx *SNAPPYMT_createDCtx(int threads, int inputsize);

/**
 * 2) threaded compression
 * - return -1 on error
 */
size_t SNAPPYMT_decompressDCtx(SNAPPYMT_DCtx * ctx, SNAPPYMT_RdWr_t * rdwr);

/**
 * 3) get some statistic
 */
size_t SNAPPYMT_GetFramesDCtx(SNAPPYMT_DCtx * ctx);
size_t SNAPPYMT_GetInsizeDCtx(SNAPPYMT_DCtx * ctx);
size_t SNAPPYMT_GetOutsizeDCtx(SNAPPYMT_DCtx * ctx);

/**
 * 4) free cctx
 * - no special return value
 */
void SNAPPYMT_freeDCtx(SNAPPYMT_DCtx * ctx);

#if defined (__cplusplus)
}
#endif

#endif