// File: stream_common.h
// See Copyright Notice and license at the end of include/lzham.h
#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <stdarg.h>
#include <string>
#include <vector>

#define LZHAM_EX_MAX(a,b) (((a) > (b)) ? (a) : (b))
#define LZHAM_EX_MIN(a,b) (((a) < (b)) ? (a) : (b))

#if __BIG_ENDIAN__
   #define LZHAM_EX_LITTLE_ENDIAN_CPU 0
#else
   #define LZHAM_EX_LITTLE_ENDIAN_CPU 1
#endif

namespace lzham_ex
{
   typedef signed char int8;
   typedef unsigned char uint8;
   typedef signed short int16;
   typedef unsigned short uint16;
   typedef unsigned int uint;
   typedef signed int int32;
   typedef unsigned int uint32;
   typedef signed __int64 int64;
   typedef unsigned __int64 uint64;

   const uint16 UINT16_MIN = 0;
   const uint16 UINT16_MAX = 0xFFFFU;
   const uint64 UINT64_MIN = 0;
   const uint64 UINT64_MAX = 0xFFFFFFFFFFFFFFFFULL;    //0xFFFFFFFFFFFFFFFFui64;
   const int64 INT64_MIN = (int64)0x8000000000000000ULL; //(-9223372036854775807i64 - 1);
   const int64 INT64_MAX = (int64)0x7FFFFFFFFFFFFFFFULL; //9223372036854775807i64;

   template<typename T> inline void zero_object(T& obj) { memset(&obj, 0, sizeof(obj)); }
} // namespace lzham_ex
