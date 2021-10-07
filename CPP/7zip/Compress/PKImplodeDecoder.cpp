// ImplodeDecoder.cpp

#include <algorithm>
#include "StdAfx.h"

#include "../../Common/Defs.h"

#include "PKImplodeDecoder.h"
#include "../../Common/StreamUtils.h"

extern "C" {
#include "../../../C/blast/blast.h"
}

namespace NCompress {
namespace NPKImplode {
namespace NDecoder {

static int BLAST_put(void *out_desc, unsigned char *buf, unsigned int len){
  CCoder *w = (CCoder*)out_desc;
  UInt32 rlen=0;
  HRESULT res = w->outS->Write(buf, len, &rlen);
  w->processedOut += rlen;
  if(w->progr)
	  w->progr->SetRatioInfo(&w->processedIn, &w->processedOut);
  return res != S_OK || rlen != len;
}

static unsigned int BLAST_get(void *in_desc, unsigned char **buf){
  CCoder *r = (CCoder*)in_desc;
  *buf = r->buf;
  size_t readsize = kS;
  HRESULT res = ReadStream(r->inS,r->buf,&readsize);
  r->processedIn += readsize;
  return res != S_OK ? 0 : readsize;
}

CCoder::CCoder(){}

HRESULT CCoder::CodeReal(ISequentialInStream *inStream, ISequentialOutStream *outStream,
    const UInt64 *inSize, const UInt64 *outSize, ICompressProgressInfo *progress)
{
  inS = inStream;
  outS = outStream;
  progr = progress;
  processedIn = processedOut = 0;
  int x = blast(BLAST_get, this, BLAST_put, this, NULL, NULL);
  if(x<0)return E_FAIL;
  if(x==2)return E_ABORT;
  if(x==1)return E_OUTOFMEMORY;
  return x==0 ? S_OK : E_FAIL;
}


STDMETHODIMP CCoder::Code(ISequentialInStream *inStream, ISequentialOutStream *outStream,
    const UInt64 *inSize, const UInt64 *outSize, ICompressProgressInfo *progress)
{
  try { return CodeReal(inStream, outStream, inSize, outSize, progress);  }
  catch(const CInBufferException &e)  { return e.ErrorCode; }
  catch(const CSystemException &e) { return e.ErrorCode; }
  catch(...) { return S_FALSE; }
}

STDMETHODIMP CCoder::GetInStreamProcessedSize(UInt64 *value)
{
  *value = processedIn;
  return S_OK;
}

}}}

