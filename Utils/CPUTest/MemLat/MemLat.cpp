/* MemLat.cpp -- Memory Latency
2011-05-12 : Igor Pavlov : Public domain */

// #define _WIN32_WINNT 0x0500

#include <stdio.h>

#ifdef _MSC_VER
// "was declared deprecated" disabling
#pragma warning(disable : 4996 )
// #define LARGE_PAGES
#endif

#include "../Benchmark.h"
#include "../MyVersion.h"

#include "Walk.h"

#ifdef ITS_WINDOWS
#ifdef LARGE_PAGES
#include "..\..\..\Windows\Security.h"
#include "..\..\..\Windows\MemoryLock.h"
#endif
#endif

// #define _TEST_64

#ifdef _TEST_64
typedef UInt64 TestType;
const int kNumMoveBits = 3;
#else
typedef UInt32 TestType;
const int kNumMoveBits = 2;
#endif

const int kNumTestsMax = 22;

const int kBasePowerMax = 33;
const size_t kBufferSize3 = ((size_t)1 << (kBasePowerMax - kNumMoveBits));

#ifndef NUM_DIRECT_TESTS
    #define NUM_DIRECT_TESTS 10
#endif

#ifndef NUM_PAR_TESTS
    #define NUM_PAR_TESTS 12
#endif

static unsigned GetHex(const char *s)
{
  UInt32 value;
  if (1 != sscanf(s, "%x", &value))
    throw "incorrect value";
  return value;
}

#ifndef MY_CPU_AMD64
#define SUPPORT_DIRECT
#endif

#ifdef UNDER_CE
#define USE_GUI
#endif

#ifdef USE_GUI
HWND g_Hwnd;
BOOL g_bAbort;

#define MY_PROGRESS(id, nnn) \
  { MSG msg; wchar_t s2[64]; _itow(nnn, s2, 10); \
    SetWindowTextW(GetDlgItem(g_Hwnd, id), s2); \
    while (!g_bAbort && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) \
      if (!IsDialogMessage(g_Hwnd, &msg)) \
      { TranslateMessage(&msg); DispatchMessage(&msg); } \
      if (g_bAbort) return 1; }
#else
#define MY_PROGRESS(id, nnn)
#endif

int main2(CPrinter &f, int numArgs, const char *args[])
{
  f.Print("MemLat " MY_VERSION_COPYRIGHT_DATE "\n");
  if (numArgs < 2 || numArgs > 100)
  {
    f.Print(
      "use: MemLat.exe Size[K|M] [n] [l] [b] [c] [p[N]]\n"
      "  Size[K|M] - upper memory limit\n"
      "  n - show results in ns\n"
      "  l - use large pages\n"
      #ifdef SUPPORT_DIRECT
      "  a - unAligned read\n"
      #endif
      "  b - check Bandwidth using sum operator\n"
      "  c - check Latency and Bandwidth using sequential Chain\n"
      #ifdef SUPPORT_DIRECT
      "  d - Direct pointers\n"
      #endif
      "  p - check different page sizes\n"
      "  p[N] - check page performance for pageSize = 2^N\n"
      "  r - check different page sizes with NOP-40 mode\n"
      "  x[N] - align test array on 2^N range\n"
      "  z[N] - the number of random interations = 2^N\n"
      "  w    - write mode\n"
      );
    return 1;
  }
  const char *arg = args[1];
  int len = (int)strlen(arg);
  if (len < 1)
    return 2;
  size_t bufferSizeMax = atoi(arg);
  if (arg[len-1] == 'K' || arg[len-1] == 'k')
    bufferSizeMax <<= 10;
  else
    bufferSizeMax <<= 20;
  bufferSizeMax >>= kNumMoveBits;

  #ifdef _WIN32
  #ifdef LARGE_PAGES
  NWindows::NSecurity::AddLockMemoryPrivilege();
  NWindows::NSecurity::EnableLockMemoryPrivilege();
  #endif
  #endif

  bool nsMode = false;
  bool largePages = false;
  bool noCache = false;

  bool bwMode = false;
  bool seqMode = false;
  #ifdef SUPPORT_DIRECT
  bool alignMode = false;
  bool directMode = false;
  #endif

  bool writeMode = false;
  bool pageMode = false;
  int pageBits = 2;

  int zBits =
    #ifdef UNDER_CE
    16;
    #else
    21;
    #endif

  int seqStep = 0;

  int xBits =
  #ifdef ITS_WINDOWS
  #ifdef UNDER_CE
  20;
  #else
  24;
  #endif
  #else
  12;
  #endif

  int affinity = 0;

  bool nopMode = false;
  
  for (int ii = 2; ii < numArgs; ii++)
  {
    const char *str = args[ii];
    switch(str[0])
    {
      case 'p':
      {
        str++;
        int len = (int)strlen(str);
        if (len < 1)
          pageMode = true;
        else
        {
          pageBits = atoi(str);
          if (pageBits > 30 || pageBits < 2)
            return 1;
        }
        break;
      }
      case 'r':
      {
        pageMode = true;
        nopMode = true;
        break;
      }

      case 'b': bwMode = true; break;
      case 'c':
      {
        seqMode = true;
        str++;
        int len = (int)strlen(str);
        if (len >= 1)
        {
          if (str[len - 1] == 'h')
            seqStep = GetHex(str);
          else
          {
            return 1;
            // seqStep = atoi(str);
          }
          if (seqStep < 4 || seqStep > (1 << 30))
            return 1;
          seqStep >>= kNumMoveBits;
        }
        break;
      }
      #ifdef SUPPORT_DIRECT
      case 'a': alignMode = true; break;
      case 'd': directMode = true; break;
      #endif

      case 'n': nsMode = true; break;
      case 'l': largePages = true; break;
      case 'u': noCache = true; break;
      case 'w': writeMode = true; break;
      case 't':
      {
        str++;
        int len = (int)strlen(str);
        if (len < 1)
          return 1;
        else
        {
          affinity = atoi(str);
          if (affinity > 31 || affinity < 0)
            return 1;
        }
        break;
      }
      case 'x':
      {
        str++;
        int len = (int)strlen(str);
        if (len < 1)
          return 1;
        else
        {
          xBits = atoi(str);
          if (xBits > 30 || xBits < 3)
            return 1;
        }
        break;
      }
      case 'z':
      {
        str++;
        int len = (int)strlen(str);
        if (len < 1)
          return 1;
        else
        {
          zBits = atoi(str);
          if (zBits > 30 || zBits < 3)
            return 1;
        }
        break;
      }
      default: return 1;
    }
  }
  MY_SET_AFFINITY_NUMBER(affinity);

  bool latMode = !(seqMode || bwMode);

  size_t kAlignSize = (size_t)1 << xBits;
  size_t allocSize = ((size_t)bufferSizeMax * sizeof(TestType) + kAlignSize + kAlignSize - 1) & ~((size_t)kAlignSize - 1);
  TestType *randAddressesBase = (TestType *)
  #ifdef _WIN32
  ::VirtualAlloc(0, allocSize,
      MEM_COMMIT
      #ifndef UNDER_CE
      | (largePages ? MEM_LARGE_PAGES : 0)
      #endif
	    , PAGE_READWRITE | (noCache ? PAGE_NOCACHE : 0));
  #else
  malloc(allocSize);
  #endif

  if (randAddressesBase == 0)
  {
    f.Print( "Not enough memory\n");
    return 1;
  }

  TestType *randAddresses = randAddressesBase;
  
  while((((size_t)randAddresses) & (kAlignSize - 1)) != 0)
    randAddresses++;
 
  CRandomGenerator rg;

  const int kNumSubItemsBits = 2;
  const int kNumSubItems = 1 << kNumSubItemsBits;
  const int kNumMaxItems = (kBasePowerMax + 1) * kNumSubItems;
  const unsigned kNumTestCycles = 4;
  int kStartPower = 12;

  int numTests = kNumTestsMax;
  
  if (bwMode)
    numTests = 9;
  else if (seqMode)
    numTests = (seqStep == 0) ? 13 : 1;
  #ifdef SUPPORT_DIRECT
  else if (alignMode)
    numTests = 13;
  #endif
  else
  {
    if (pageMode)
      numTests = 21;
    else
    {
      if (writeMode)
      {
        numTests = 6;
        #ifdef SUPPORT_DIRECT
        numTests = directMode ? 3 : 2;
        #endif
      }
      else
      {
        numTests =
          #ifdef SUPPORT_DIRECT
          directMode ? NUM_DIRECT_TESTS :
          #endif
          NUM_PAR_TESTS;
      }
    }
  }

  if (numTests > kNumTestsMax)
    numTests = kNumTestsMax;


  unsigned sizes[kNumMaxItems];
  double ticksResults[kNumTestsMax][kNumMaxItems];
  TestType sum = 0;

  {
    for (int k = 0; k <
      #ifdef UNDER_CE
      20000000
      #else
      600000000
      #endif
      ; k++)
      sum += (k >> 3);
  }

  UInt64 startCount = GetTimeCount();
  UInt64 startCount2 = GetTimeCount2();

  {
    for (int k = 0; k <
      #ifdef UNDER_CE
      10000000
      #else
      100000000
      #endif
      ; k++)
      sum += (k >> 3);
  }



  int sizeIndexMax = 0;
  for (int testIndex = 0; testIndex < numTests; testIndex++)
  {
    MY_PROGRESS(102, testIndex);

    int numThreads = 1;
    if (!seqMode && !bwMode
        #ifdef SUPPORT_DIRECT
        && !alignMode
        #endif
      )
    {
      if (!pageMode)
        numThreads = testIndex + 1;
    }
    int sizeIndex = 0;
    // printf("\ntestIndex=%d\n", testIndex);
    for (int k = kStartPower; k < 64; k++)
    {
      MY_PROGRESS(103, (1 << k) >> 10);
      // printf("k=%d\n", k);
      int t;
      for (t = 0; t < kNumSubItems; t++, sizeIndex++)
      {
        unsigned a[kNumTestsMax];
        ticksResults[testIndex][sizeIndex] = 0;
        // printf("t=%d\n", t);
        unsigned bufferSize = ((unsigned)1 << (k - kNumMoveBits)) +
            (t << (k - kNumMoveBits - kNumSubItemsBits));
        if (bufferSize > kBufferSize3 || bufferSize > bufferSizeMax)
          break;
        sizes[sizeIndex] = bufferSize;
        
        UInt32 numCycles;

        unsigned i;
        const UInt32 kEmpty = 0xFFFFFFFF;
        for (i = 0; i < bufferSize; i++)
          randAddresses[i] = kEmpty;

        // printf("numCycles=%d\n", numCycles);

        #ifdef SUPPORT_DIRECT
        if (alignMode)
        {
          UInt32 step = 1 << testIndex;
          if (step > bufferSize)
            continue;
          numCycles = bufferSize / step;
          UInt32 stepInBytes = step << kNumMoveBits;
          char *pAlignStart = (char *)(randAddresses) + (stepInBytes - (1 << kNumMoveBits)) / 2;
          a[0] = (UInt32)pAlignStart;
          char *p = pAlignStart;
          for (i = 0; i + step <= bufferSize; i += step, p += stepInBytes)
            *(UInt32 *)p = (UInt32)(p + stepInBytes);
          *(UInt32 *)(p - stepInBytes) = (UInt32)pAlignStart;
        }
        else
        #endif

        if (bwMode || seqMode)
        {
          UInt32 step = (testIndex == 0) ? 1 : (1 << (testIndex - 1));
          if (seqStep != 0)
            step = seqStep;
          if (step > bufferSize)
            continue;
          numCycles = bufferSize / step;
          if (bwMode)
          {
            if (numCycles < 4)
              continue;
          }
          if (seqMode)
          {
            if (testIndex == 0 && seqStep == 0)
            {
              for (i = 0; i < bufferSize; i++)
                randAddresses[i] = 1;
              randAddresses[i - 1] = 0 - (i - 1);
            }
            else
            {
              for (i = 0; i < bufferSize; i += step)
                randAddresses[i] = i + step;
              randAddresses[i - step] = 0;
            }
          }
        }
        else
        {
          int curPageBits = pageBits;
          if (pageMode)
            curPageBits = pageBits + testIndex;
          int kPageIntsBits = curPageBits - 2;
          numCycles = bufferSize >> kPageIntsBits;
          numCycles /= numThreads;
          
          // printf(" kNumCycles = %5d", numCycles);
          if (numCycles > (1 << 10))
          {
            const UInt32 kDiv = 8;
            numCycles = numCycles / kDiv * (kDiv - 1);
            const UInt32 kNumCyclesMax = (1 << zBits);
            if (numCycles > kNumCyclesMax)
              numCycles = kNumCyclesMax;
          }
          if (numCycles < 1)
            continue;
          
          for (int s = 0; s < numThreads; s++)
          {
            UInt32 prev = 0xFFFFFFFF;
            for (i = 0; i < numCycles; i++)
            {
              for (;;)
              {
                UInt32 pageIndex = rg.GetRnd32(bufferSize >> kPageIntsBits);
                unsigned randValueBase = pageIndex << kPageIntsBits;
                unsigned randValue = randValueBase + rg.GetRnd32(1 << kPageIntsBits);
                
                // unsigned randValue = (aaa + 1)  % bufferSize;
                // printf("randValue=%d\n", randValue);
                
                if (randAddresses[randValueBase] == kEmpty)
                {
                  UInt32 val = randValue;
                  #ifdef SUPPORT_DIRECT
                  if (directMode)
                    val = (UInt32)(randAddresses + randValue);
                  #endif

                  if (i == 0)
                    a[s] = val;
                  else
                    randAddresses[prev] = val;
                  randAddresses[randValueBase] = 0;
                  prev = randValue;
                  // printf(" i = %d r=%d\n", i, randValue);
                  break;
                }
              }
            }
            randAddresses[prev] = a[s];
          }
        }

        double minValue = 1 << 30;

        UInt32 mult2 = (k <= 16) ? 2047000 : 127000;
        mult2 = (UInt32)((UInt64)mult2 / GetFreq2()) + 1;
        unsigned numSpecCycles = (unsigned)(((UInt64)1 << 12) * mult2 / numCycles) + 1;

        // printf("numSpecCycles = %5d\n", numSpecCycles);
        for (i = 0; i < kNumTestCycles; i++)
        {
          UInt32 res = 0;
          UInt64 startTime = GetTimeCount();
          if (latMode)
          {
            UInt32 numCycles2 = numCycles * numSpecCycles;
            // printf("  numCycles2 = %5d \n", numCycles2);

            #ifdef SUPPORT_DIRECT
            if (alignMode ||
                (pageMode || testIndex == 0) && directMode)
              if (writeMode)
                res = Walk1wd(numCycles2, (void **)a[0]);
              else
                res = Walk1d(numCycles2, (void **)a[0]);
            else
            #endif
            if (pageMode || testIndex == 0)
                if (nopMode)
                  res = Walk1Nop(randAddresses, numCycles2, a[0]);
                else if (writeMode)
                  res = Walk1w(randAddresses, numCycles2, a[0]);
                else
                  res = Walk1(randAddresses, numCycles2, a[0]);
            else
              #ifdef SUPPORT_DIRECT
              if (directMode)
                if (writeMode)
              switch (testIndex)
              {
                case 1: res = Walk2wd(numCycles2, (void **)a[0], (void **)a[1]); break;
                case 2: res = Walk3wd(numCycles2, (void **)a[0], (void **)a[1], (void **)a[2]); break;
                default: f.Print( "\nInternal error\n"); return 1;
              }
              else
              switch (testIndex)
              {
                case 1: res = Walk2d(numCycles2, (void **)a[0], (void **)a[1]); break;
                case 2: res = Walk3d(numCycles2, (void **)a[0], (void **)a[1], (void **)a[2]); break;
                case 3: res = Walk4d(numCycles2, (void **)a[0], (void **)a[1], (void **)a[2], (void **)a[3]); break;
                case 4: res = Walk5d(numCycles2, (void **)a[0], (void **)a[1], (void **)a[2], (void **)a[3], (void **)a[4]); break;
                case 5: res = Walk6d(numCycles2, (void **)a[0], (void **)a[1], (void **)a[2], (void **)a[3], (void **)a[4], (void **)a[5]); break;
                #if NUM_DIRECT_TESTS > 6
                case 6: res = Walk7d(numCycles2, (void **)a[0], (void **)a[1], (void **)a[2], (void **)a[3], (void **)a[4], (void **)a[5], (void **)a[6]); break;
                #if NUM_DIRECT_TESTS > 7
                case 7: res = Walk8d(numCycles2, (void **)a[0], (void **)a[1], (void **)a[2], (void **)a[3], (void **)a[4], (void **)a[5], (void **)a[6], (void **)a[7]); break;
                #if NUM_DIRECT_TESTS > 8
                case 8: res = Walk9d(numCycles2, (void **)a[0], (void **)a[1], (void **)a[2], (void **)a[3], (void **)a[4], (void **)a[5], (void **)a[6], (void **)a[7], (void **)a[8]); break;
                #if NUM_DIRECT_TESTS > 9
                case 9: res = Walk10d(numCycles2, (void **)a[0], (void **)a[1], (void **)a[2], (void **)a[3], (void **)a[4], (void **)a[5], (void **)a[6], (void **)a[7], (void **)a[8], (void **)a[9]); break;
                #endif
                #endif
                #endif
                #endif
                default: f.Print("\nInternal error\n"); return 1;
              }
              else
              #endif

            if (writeMode)
            {
              switch (testIndex)
              {
                case 1: res = Walk2w(randAddresses, numCycles2, a[0], a[1]); break;
                #ifdef _WIN64
                case 2: res = Walk3w(randAddresses, numCycles2, a[0], a[1], a[2]); break;
                case 3: res = Walk4w(randAddresses, numCycles2, a[0], a[1], a[2], a[3]); break;
                case 4: res = Walk5w(randAddresses, numCycles2, a[0], a[1], a[2], a[3], a[4]); break;
                case 5: res = Walk6w(randAddresses, numCycles2, a[0], a[1], a[2], a[3], a[4], a[5]); break;
                #endif
                default: f.Print("\nInternal error\n"); return 1;
              }
            }
            else
            switch (testIndex)
            {
              case 1: res = Walk2(randAddresses, numCycles2, a[0], a[1]); break;
              case 2: res = Walk3(randAddresses, numCycles2, a[0], a[1], a[2]); break;
              case 3: res = Walk4(randAddresses, numCycles2, a[0], a[1], a[2], a[3]); break;
              case 4: res = Walk5(randAddresses, numCycles2, a[0], a[1], a[2], a[3], a[4]); break;
              #if NUM_PAR_TESTS > 5
              case 5: res = Walk6(randAddresses, numCycles2, a[0], a[1], a[2], a[3], a[4], a[5]); break;
              #if NUM_PAR_TESTS > 6
              case 6: res = Walk7(randAddresses, numCycles2, a[0], a[1], a[2], a[3], a[4], a[5], a[6]); break;
              #if NUM_PAR_TESTS > 7
              case 7: res = Walk8(randAddresses, numCycles2, a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]); break;
              #if NUM_PAR_TESTS > 8
              case 8: res = Walk9(randAddresses, numCycles2, a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8]); break;
              #if NUM_PAR_TESTS > 9
              case 9: res = Walk10(randAddresses, numCycles2, a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9]); break;
              #if NUM_PAR_TESTS > 10
              case 10: res = Walk11(randAddresses, numCycles2, a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10]); break;
              #if NUM_PAR_TESTS > 11
              case 11: res = Walk12(randAddresses, numCycles2, a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9], a[10], a[11]); break;
              #endif
              #endif
              #endif
              #endif
              #endif
              #endif
              #endif
              default: f.Print("\nInternal error\n"); return 1;
            }
          }
          else
          {
            if (seqMode)
            {
              if (testIndex == 0 && seqStep == 0)
                res = Walk1Add(randAddresses, numCycles * numSpecCycles, 0);
              else
                res = Walk1(randAddresses, numCycles * numSpecCycles, 0);
            }
            else
              for (unsigned spec = 0; spec < numSpecCycles; spec++)
                if (testIndex == 0)
                  if (writeMode)
                    WalkWrite(randAddresses, numCycles);
                  else
                    res += WalkSum(randAddresses, numCycles);
                else
                  if (writeMode)
                    WalkWrite2(randAddresses, numCycles << (testIndex - 1), 1 << (testIndex - 1));
                  else
                    res += WalkSum2(randAddresses, numCycles << (testIndex - 1), 1 << (testIndex - 1));
          }
          double ticks = double(Int64(GetTimeCount() - startTime)) / numCycles / numSpecCycles / numThreads;
          // printf("\nTimer=%d\n", GetTimeCount() - startTime);
          sum += res;
          if (ticks < minValue)
            minValue = ticks;
          if (minValue == 0)
            minValue = 0.000000000000000001;
        }
        ticksResults[testIndex][sizeIndex] = minValue;
      }
      if (t != kNumSubItems)
        break;
    }
    sizeIndexMax = sizeIndex;
  }
  UInt64 countTotal = GetTimeCount() - startCount;
  UInt64 countTotal2 = GetTimeCount2() - startCount2;

  if (countTotal2 == 0)
    countTotal2 = 1;
  double freq = double((Int64)countTotal) * (Int64)GetFreq2() / (Int64)countTotal2;
  if (freq == 0)
    freq = 1;

  char temp[128];
  {
    f.Print("    Size");
    #ifdef SUPPORT_DIRECT
    if (alignMode)
    {
      for (int testIndex = 0; testIndex < numTests; testIndex++)
      {
        sprintf(temp, " %6d", testIndex == 0 ? 0 : (2 << testIndex));
        f.Print(temp);
      }
    }
    else
    #endif
    if (seqMode || bwMode)
    {
      f.Print((seqMode && seqStep == 0) ? "    Add" : " Linear");
      for (int testIndex = 1; testIndex < numTests; testIndex++)
      {
        sprintf(temp, " %6d", (2 << testIndex));
        f.Print(temp);
      }
    }
    else
      for (int testIndex = 0; testIndex < numTests; testIndex++)
      {
        sprintf(temp, " %6d", pageMode ? (pageBits + testIndex) : (testIndex + 1));
        f.Print(temp);
      }
    f.Print("\n\n");
  }

  for (int i = 0; i < sizeIndexMax; i++)
  {
    int size = (sizes[i] >> (10 - kNumMoveBits));
    if (size < (4 << 10))
      sprintf(temp, "%6d-K", size);
    else
      sprintf(temp, "%6d-M", size >> 10);
    f.Print(temp);

    for (int testIndex = 0; testIndex < numTests; testIndex++)
    {
      double value = ticksResults[testIndex][i];
      if (nsMode)
        value = double(value) * 1000000000 / freq;
      sprintf(temp, " %6.2f", value);
      f.Print(temp);
    }
    f.NewLine();
  }

  f.NewLine();
  if (sizeIndexMax > 0)
  {
    for (int iii = 0; iii < 3; iii++)
    {
      const int kMovs = 5 + iii;
      f.Print("BW-");
      if (latMode)
      {
        sprintf(temp, "%3d B", 1 << kMovs);
        f.Print(temp);
      }
      else
        f.Print("     ");


      for (int testIndex = 0; testIndex < numTests; testIndex++)
      {
        double memoryBandwidth = (freq / ticksResults[testIndex][sizeIndexMax - 1]) * (1 << kNumMoveBits);
        if (latMode)
          memoryBandwidth *= (1 << (kMovs - 2));
        else if (testIndex > 0)
          memoryBandwidth *= (1 << (testIndex - 1));
        sprintf(temp, " %6d", (UInt32)(memoryBandwidth / 1000000));
        f.Print(temp);
      }
      f.NewLine();
      if (!latMode)
        break;
    }
    f.NewLine();
  }

  if (sizeIndexMax > 0)
  {
    if (latMode)
    {
      int latencyIndex = 0;
      double cacheLatency = ticksResults[latencyIndex][0] / freq;
      sprintf(temp, "Cache latency    = %10.2f ns     = %10.2f cycles\n",
        cacheLatency * (1e9), ticksResults[latencyIndex][0]);
      f.Print(temp);
      double memoryLatency = ticksResults[latencyIndex][sizeIndexMax - 1] / freq;
      sprintf(temp, "Memory latency   = %10.2f ns     = %10.2f cycles\n",
        memoryLatency * (1e9), ticksResults[latencyIndex][sizeIndexMax - 1]);
      f.Print(temp);
    }
    {
      if (nsMode)
        f.Print("ns ");
      if (largePages)
        f.Print("LargePages ");

      if (bwMode)
        f.Print("BW-Sum");
      else if (seqMode)
        f.Print("Seq Step");
      else if (pageMode)
        f.Print("PageMode");
      else
      {
        sprintf(temp, "PageSize=%d \n", 1 << pageBits);
        f.Print(temp);
      }
      f.NewLine();
    }
  }

  f.NewLine();
  sprintf(temp, "Timer frequency  = %10.0f Hz\n", (double)(Int64)GetFreq2());
  f.Print(temp);
  sprintf(temp, "CPU frequency    = %10.2f MHz\n", freq / 1000000);
  f.Print(temp);

  #ifdef _WIN32
  VirtualFree(randAddressesBase, 0, MEM_RELEASE);
  #else
  free(randAddressesBase);
  #endif
  return (int)(sum >> 3);
}

static const size_t kTempSize = 20000;

#ifdef USE_GUI

static int g_TestNumber = 0;

static int OneTest(CPrinter &f, const char *s, const wchar_t *fileName2)
{
  f.Init();
  
  {
    wchar_t s2[64];
    for (int t = 0; t < 100; t++)
    {
      s2[t] = s[t];
      if (s[t] == 0)
        break;
    }
    SetWindowTextW(GetDlgItem(g_Hwnd, 110), s2);
    MY_PROGRESS(101, g_TestNumber++);
  }

  char args[10][32];
  int param = 1;
  int pos = 0;
  for (;;)
  {
	char c = *s++;
	if (pos >= 32 || param >= 10)
      return 1;
	if (c == ' ')
	{
	  args[param][pos++] = 0;
	  pos = 0;
	  param++;
	}
	else
	{
	  args[param][pos++] = c;
      if (c == 0)
	    break;
	}
  }

  const char *argsPtr[10];
  param++;
  argsPtr[0] = "memLat.exe";
  for (int i = 1; i < param; i++)
	argsPtr[i] = args[i];
  main2(f, param, argsPtr);
  
  wchar_t fileName[300];
  GetModuleFileNameW(0, fileName, 300);
  wchar_t *p = wcsrchr(fileName, L'\\');
  if (p == 0)
    return 0;
  p++;
  wcscpy(p, fileName2);
  FILE *inputHandle = _wfopen(fileName, L"w+");
  if (inputHandle == 0)
  {
    MessageBoxW(0, L"Can't open text file", L"Error", 0);
    return 1;
  }
  fprintf(inputHandle, "%s", f.buf);
  fclose(inputHandle);

  return 0;
}

static INT_PTR CALLBACK MyDlgProc(HWND /* hwnd */, UINT message, WPARAM wParam, LPARAM /* lParam */)
{
  switch(message)
  {
    case WM_COMMAND:
      switch(LOWORD(wParam))
      {
        case IDCANCEL:
          SetWindowTextW(GetDlgItem(g_Hwnd, 101), L"Stopping...");
          g_bAbort = TRUE;
          break;
      }
      break;
    default:
      return FALSE;
  }
  return TRUE;
}

int WINAPI WinMain(HINSTANCE,
    HINSTANCE,
    #ifdef UNDER_CE
    LPWSTR
    #else
    LPSTR
    #endif
    ,
    int)
{
  g_bAbort = FALSE;
  g_Hwnd = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(100), NULL, MyDlgProc);
  if (g_Hwnd == 0)
  {
    MessageBoxW(0, L"Error", L"Error", 0);
    return 1;
  }

  CPrinter f(kTempSize);

  OneTest(f, "2m n c", L"c.txt");
  OneTest(f, "2m n b", L"b.txt");
  OneTest(f, "2m n b w", L"bw.txt");

  OneTest(f, "16m n p d", L"pd.txt");
  OneTest(f, "16m n p2 d", L"p2d.txt");
  OneTest(f, "16m n p5 d", L"p5d.txt");
  OneTest(f, "16m n p6 d", L"p6d.txt");
  OneTest(f, "16m n p7 d", L"p7d.txt");
  OneTest(f, "16m n p12 d", L"p12d.txt");
  OneTest(f, "16m n p2", L"p2.txt");

  // OneTest("16m n p d u", L"pdu.txt");
  // OneTest("16m n p2 d u", L"p2du.txt");
  // OneTest("16m n p4 d u", L"p4du.txt");
  // OneTest("16m n p5 d u", L"p5du.txt");
  // OneTest("1m n c u", L"cu.txt");
  // OneTest("1m n b u", L"bu.txt");
  // OneTest("1m n b w u", L"bwu.txt");

  return 0;
}
#else
int
#ifdef _MSC_VER
_cdecl
#endif
main(int numargs, const char *args[])
{
  CPrinter f(kTempSize);
  int code = main2(f, numargs, args);
  fputs(f.buf, stdout);
  return code;
}
#endif
