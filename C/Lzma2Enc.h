/* Lzma2Enc.h -- LZMA2 Encoder
2013-01-18 : Igor Pavlov : Public domain */

#ifndef __LZMA2_ENC_H
#define __LZMA2_ENC_H

#include "LzmaEnc.h"

EXTERN_C_BEGIN

#define LZMA2_ENC_PROPS__BLOCK_SIZE__AUTO 0
#define LZMA2_ENC_PROPS__BLOCK_SIZE__SOLID ((UInt64)(Int64)-1)

typedef struct
{
  CLzmaEncProps lzmaProps;
  UInt64 blockSize;
  int numBlockThreads_Reduced;
  int numBlockThreads_Max;
  int numTotalThreads;
} CLzma2EncProps;

void Lzma2EncProps_Init(CLzma2EncProps *p);
void Lzma2EncProps_Normalize(CLzma2EncProps *p);

/* ---------- CLzmaEnc2Handle Interface ---------- */

/* Lzma2Enc_* functions can return the following exit codes:
Returns:
  SZ_OK           - OK
  SZ_ERROR_MEM    - Memory allocation error
  SZ_ERROR_PARAM  - Incorrect paramater in props
  SZ_ERROR_WRITE  - Write callback error
  SZ_ERROR_PROGRESS - some break from progress callback
  SZ_ERROR_THREAD - errors in multithreading functions (only for Mt version)
*/

typedef void * CLzma2EncHandle;

CLzma2EncHandle Lzma2Enc_Create(ISzAllocPtr alloc, ISzAllocPtr allocBig);
void Lzma2Enc_Destroy(CLzma2EncHandle p);
SRes Lzma2Enc_SetProps(CLzma2EncHandle p, const CLzma2EncProps *props);
void Lzma2Enc_SetDataSize(CLzma2EncHandle p, UInt64 expectedDataSiize);
Byte Lzma2Enc_WriteProperties(CLzma2EncHandle p);
// SRes Lzma2Enc_Encode(CLzma2EncHandle p,
//     ISeqOutStream *outStream, ISeqInStream *inStream, ICompressProgress *progress);
SRes Lzma2Enc_Encode2(CLzma2EncHandle p,
    ISeqOutStream *outStream,
    Byte *outBuf, size_t *outBufSize,
    ISeqInStream *inStream,
    const Byte *inData, size_t inDataSize,
    ICompressProgress *progress);
/* ---------- One Call Interface ---------- */

/* Lzma2Encode
Return code:
  SZ_OK               - OK
  SZ_ERROR_MEM        - Memory allocation error
  SZ_ERROR_PARAM      - Incorrect paramater
  SZ_ERROR_OUTPUT_EOF - output buffer overflow
  SZ_ERROR_THREAD     - errors in multithreading functions (only for Mt version)
*/

/*
SRes Lzma2Encode(Byte *dest, SizeT *destLen, const Byte *src, SizeT srcLen,
    const CLzmaEncProps *props, Byte *propsEncoded, int writeEndMark,
    ICompressProgress *progress, ISzAllocPtr alloc, ISzAllocPtr allocBig);
*/

EXTERN_C_END

#endif
