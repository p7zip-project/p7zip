// (C) 2016 Tino Reichardt

#include "StdAfx.h"

#include "../Common/RegisterCodec.h"

#include "ZstdDecoder.h"

#ifndef EXTRACT_ONLY
#include "ZstdEncoder.h"
#endif

namespace NCompress {

namespace NZstdPK {
REGISTER_CODEC_E(
  ZSTD_PK,
  NCompress::NZSTD::CDecoder(),
  NCompress::NZSTD::CEncoder(),
  0x40100+20, "ZSTD-PK")
}

namespace NZstdWZ {
REGISTER_CODEC_E(
  ZSTD_WZ,
  NCompress::NZSTD::CDecoder(),
  NCompress::NZSTD::CEncoder(),
  0x40100+93, "ZSTD-WZ")
}

}
