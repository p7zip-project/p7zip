// ImplodeDecoder.cpp

#include <algorithm>
#include "StdAfx.h"

#include "../../Common/Defs.h"

#include "PKImplodeDecoder.h"
#include "../Common/StreamUtils.h"

namespace NCompress {
namespace NPKImplode {
namespace NDecoder {

static void C_put(char *buf, unsigned int *size, void *param){
  CDecoder *w = (CDecoder*)param;
  UInt32 rlen=0;
  /*HRESULT res =*/ w->outS->Write(buf, *size, &rlen);
  //*size = rlen;
  w->processedOut += rlen;
  if(w->progr)
	  w->progr->SetRatioInfo(&w->processedIn, &w->processedOut);
  //return res != S_OK || rlen != len;
}

static unsigned int C_get(char *buf, unsigned int *size, void *param){
  CDecoder *r = (CDecoder*)param;
  size_t readsize = *size;
  HRESULT res = ReadStream(r->inS,buf,&readsize);
  //*size = readsize;
  r->processedIn += readsize;
  return res != S_OK ? 0 : readsize;
}

CDecoder::CDecoder(){
  memset(&DcmpStruct, 0, sizeof(TDcmpStruct));
}

HRESULT CDecoder::CodeReal(ISequentialInStream *inStream, ISequentialOutStream *outStream,
    const UInt64* /* inSize */, const UInt64* /* outSize */, ICompressProgressInfo *progress)
{
  inS = inStream;
  outS = outStream;
  progr = progress;
  processedIn = processedOut = 0;
  int x = explode(C_get, C_put, (char*)&DcmpStruct, this);
  if(x<0)return E_FAIL;
  if(x==2)return E_ABORT;
  if(x==1)return E_OUTOFMEMORY;
  return x==0 ? S_OK : E_FAIL;
}


STDMETHODIMP CDecoder::Code(ISequentialInStream *inStream, ISequentialOutStream *outStream,
    const UInt64 *inSize, const UInt64 *outSize, ICompressProgressInfo *progress)
{
  try { return CodeReal(inStream, outStream, inSize, outSize, progress);  }
  catch(const CInBufferException &e)  { return e.ErrorCode; }
  catch(const CSystemException &e) { return e.ErrorCode; }
  catch(...) { return S_FALSE; }
}

STDMETHODIMP CDecoder::GetInStreamProcessedSize(UInt64 *value)
{
  *value = processedIn;
  return S_OK;
}

}}}
