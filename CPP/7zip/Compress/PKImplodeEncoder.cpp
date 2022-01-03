// ImplodeDecoder.cpp

#include <algorithm>
#include "StdAfx.h"

#include "../../Common/Defs.h"

#include "PKImplodeEncoder.h"
#include "../../Common/StreamUtils.h"

namespace NCompress {
namespace NPKImplode {
namespace NEncoder {

static void C_put(char *buf, unsigned int *size, void *param){
  CEncoder *w = (CEncoder*)param;
  UInt32 rlen=0;
  HRESULT res = w->outS->Write(buf, *size, &rlen);
  //*size = rlen;
  w->processedOut += rlen;
  if(w->progr)
	  w->progr->SetRatioInfo(&w->processedIn, &w->processedOut);
  //return res != S_OK || rlen != len;
}

static unsigned int C_get(char *buf, unsigned int *size, void *param){
  CEncoder *r = (CEncoder*)param;
  size_t readsize = *size;
  HRESULT res = ReadStream(r->inS,buf,&readsize);
  //*size = readsize;
  r->processedIn += readsize;
  return res != S_OK ? 0 : readsize;
}

CEncoder::CEncoder(): typ(0), dsize(4096){
  memset(&CmpStruct, 0, sizeof(TCmpStruct));
}

STDMETHODIMP CEncoder::SetCoderProperties(const PROPID * propIDs, const PROPVARIANT * coderProps, UInt32 numProps)
{
  typ = 0;
  dsize = 4096;
  for (UInt32 i = 0; i < numProps; i++){
	const PROPVARIANT & prop = coderProps[i];
	PROPID propID = propIDs[i];
	UInt32 v = (UInt32)prop.ulVal;
	switch (propID)
	{
	case NCoderPropID::kLevel:
	  {
		typ = v/10;
		int dsize_ = v%10;
		if(typ<0)typ=0;
		if(typ>1)typ=1;
  	    if(dsize<1)dsize=1;
		if(dsize>3)dsize=3;
		dsize = 1<<(9+dsize_);
	  }
	}
  }
  return S_OK;
}

HRESULT CEncoder::CodeReal(ISequentialInStream *inStream, ISequentialOutStream *outStream,
    const UInt64 *inSize, const UInt64 *outSize, ICompressProgressInfo *progress)
{
  inS = inStream;
  outS = outStream;
  progr = progress;
  processedIn = processedOut = 0;
  int x = implode(C_get, C_put, (char*)&CmpStruct, this, &typ, &dsize);
  if(x<0)return E_FAIL;
  if(x==2)return E_ABORT;
  if(x==1)return E_OUTOFMEMORY;
  return x==0 ? S_OK : E_FAIL;
}


STDMETHODIMP CEncoder::Code(ISequentialInStream *inStream, ISequentialOutStream *outStream,
    const UInt64 *inSize, const UInt64 *outSize, ICompressProgressInfo *progress)
{
  try { return CodeReal(inStream, outStream, inSize, outSize, progress);  }
  catch(const CInBufferException &e)  { return e.ErrorCode; }
  catch(const CSystemException &e) { return e.ErrorCode; }
  catch(...) { return S_FALSE; }
}

STDMETHODIMP CEncoder::GetInStreamProcessedSize(UInt64 *value)
{
  *value = processedIn;
  return S_OK;
}

}}}

