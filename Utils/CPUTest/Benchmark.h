// Benchmark.h: Benchmark functions for CPU tests

#ifndef __BENCHMARK_H
#define __BENCHMARK_H

#if (defined _WIN32) || (defined _WIN32_WCE)
#define ITS_WINDOWS
#endif

#ifdef ITS_WINDOWS
#include <windows.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(_M_X64) || defined(_M_AMD64) || defined(__x86_64__)
#define MY_CPU_AMD64
#endif

#if defined(_M_IX86) || defined(__i386__)
#define MY_CPU_X86
#endif

#if defined(MY_CPU_X86) || defined(MY_CPU_AMD64)
#define MY_CPU_X86_OR_AMD64
#endif

#if defined(__sparc_v9__) || (defined(__sparc__) && defined(__arch64__))
#define MY_CPU_SPARCV9
#endif

typedef unsigned UInt32;
#ifdef __GNUC__
typedef long long int Int64;
typedef unsigned long long int UInt64;
#else
typedef __int64 Int64;
typedef unsigned __int64 UInt64;
#endif

UInt64 GetTimeCount2()
{
  #ifdef ITS_WINDOWS
  LARGE_INTEGER value; if (::QueryPerformanceCounter(&value)) return value.QuadPart;
  return GetTickCount();
  #else
  return clock();
  #endif
}

UInt64 GetFreq2()
{
  #ifdef ITS_WINDOWS
  LARGE_INTEGER value; if (::QueryPerformanceFrequency(&value)) return value.QuadPart;
  return 1000;
  #else
  return CLOCKS_PER_SEC;
  #endif
}

#if defined(MY_CPU_X86_OR_AMD64) || defined(MY_CPU_SPARCV9)
#define CYCLES_MODE
#define TIMER_IS_FAST
#endif

inline UInt64 GetTimeCount()
{
  #if defined(_WIN32) && defined(MY_CPU_X86_OR_AMD64)
    #ifdef _WIN64
      return __rdtsc();
    #else
      UInt32 valueLow, valueHigh;
      __asm RDTSC;
      __asm mov valueLow, EAX;
      __asm mov valueHigh, EDX;
      return ((UInt64)(valueHigh) << 32) | valueLow;
    #endif
  #elif defined(__GNUC__) && defined (MY_CPU_X86_OR_AMD64)
    UInt64 value;
    __asm__ __volatile__("rdtsc" : "=A" (value));
    return value;
  #elif defined(__GNUC__) && defined (_ARCH_PPC)
    UInt32 valueLow, valueHigh, valueHigh2;
    do
    {
      __asm__ __volatile__("mftbu %0" : "=r" (valueHigh));
      __asm__ __volatile__("mftb %0"  : "=r" (valueLow));
      __asm__ __volatile__("mftbu %0" : "=r" (valueHigh2));
    }
    while (valueHigh2 != valueHigh)
    return ((UInt64)(valueHigh) << 32) | valueLow;
  #elif defined(__GNUC__) && defined(MY_CPU_SPARCV9)
    UInt64 value;
    #ifdef __arch64__
    __asm__ __volatile__("rd %%tick,%0" : "=r" (value));
    #else
    __asm__ __volatile__("rd %%tick,%%g1\n\t"
                         "stx %%g1,%0"
                         : "=m" (value) :: "g1");
    #endif
  #else
  return GetTimeCount2();
  #endif
}

#if defined(ITS_WINDOWS) && !defined(UNDER_CE)
#define MY_SET_AFFINITY_NUMBER(n) SetThreadAffinityMask(GetCurrentThread(), 1 << (n))
#else
#define MY_SET_AFFINITY_NUMBER(n)
#endif

#define MY_SET_AFFINITY MY_SET_AFFINITY_NUMBER(0)

class CRandomGenerator
{
  UInt32 A1;
  UInt32 A2;
public:
  CRandomGenerator() { Init(); }
  void Init() { A1 = 362436069; A2 = 521288629;}
  UInt32 GetRnd()
  {
    return
      ((A1 = 36969 * (A1 & 0xffff) + (A1 >> 16)) << 16) +
      ((A2 = 18000 * (A2 & 0xffff) + (A2 >> 16)) );
  }
  UInt64 GetRnd64()
  {
    UInt64 res = ((UInt64)GetRnd()) << 32;
    return res | GetRnd();
  }
  UInt32 GetRnd32(UInt32 size)
  {
    return (UInt32)(GetRnd64() % size);
  }
};

struct CPrinter
{
  char *buf;
  size_t pos;
  size_t size;
  
  CPrinter(int size)
  {
    buf = new char[size];
    this->size = size;
    Init();
  }
  void Init()
  {
    pos = 0;
    *buf = 0;
  }
  void Print(const char *s);
  void NewLine();
};

void CPrinter::Print(const char *s)
{
  size_t len = strlen(s);
  size_t rem = size - pos;
  if (len >= rem)
    len = rem - 1;
  memcpy(buf + pos, s, len);
  pos += len;
  buf[pos] = 0;
}

void CPrinter::NewLine()
{
  Print("\n");
}

#endif
