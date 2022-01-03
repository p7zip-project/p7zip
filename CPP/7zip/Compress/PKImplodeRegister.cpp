// PKImplodeRegister.cpp

#include "StdAfx.h"

#include "../Common/RegisterCodec.h"

#include "PKImplodeDecoder.h"

namespace NCompress {

REGISTER_CODEC_CREATE(CreateCodecPKImplode, NPKImplode::NDecoder::CDecoder())

REGISTER_CODECS_VAR
{
  { CreateCodecPKImplode, NULL, 0x4010a, "PKImplode", 1, false }
};

REGISTER_CODECS(PKImplode)

}
