// (C) 2016 Tino Reichardt

#include "StdAfx.h"

#include "../Common/RegisterCodec.h"

#include "ZstdDecoder.h"

#ifndef EXTRACT_ONLY
#include "ZstdEncoder.h"
#endif

REGISTER_CODEC_E(
  ZSTD,
  NCompress::NZSTD::CDecoder(),
  NCompress::NZSTD::CEncoder(),
  0x4F71101, "ZSTD")


  /*
    CreateDec函数返回CDecoder类指针
    CreateEnc函数返回CEncoder类指针
    声明 CCodecInfo 类型静态全局变量并将其指针加入到全局数组 g_Codecs 中，此数组最大容量 64

    将ZSTD算法注册进g_Codecs数组中
  */
/* g_Codecs 的指针类型
struct CCodecInfo
{
  CreateCodecP CreateDecoder;  函数指针返回解压类的指针
  CreateCodecP CreateEncoder;  函数指针返回压缩类的指针
  CMethodId Id;                算法的 ID 0x4F71101
  const char *Name;            算法的名字 “ZSTD”
  UInt32 NumStreams;           算法输入的数据流个数，通常为 1
  bool IsFilter;               是否为过滤器 False
};
*/