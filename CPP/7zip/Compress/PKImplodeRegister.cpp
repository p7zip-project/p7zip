// PKImplodeRegister.cpp

#include "StdAfx.h"

#include "../Common/RegisterCodec.h"

#include "PKImplodeDecoder.h"
#include "PKImplodeEncoder.h"

namespace NCompress {

REGISTER_CODEC_E(
  PKImplode,
  NPKImplode::NDecoder::CDecoder(),
  NPKImplode::NEncoder::CEncoder(),
  0x40100+10, "PKImplode")

}
