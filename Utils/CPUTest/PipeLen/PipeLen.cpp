// PipeLen.cpp: measures CPU pipeline length

#include <stdio.h>

#ifdef _MSC_VER
#pragma warning(disable : 4996 )
#endif

#include "../Benchmark.h"
#include "../MyVersion.h"

#define __fastcall /* */

// #define _USE_ASM

unsigned GetCyclesPerPrec()
{
  #ifdef TIMER_IS_FAST
  return 1 << 12;
  #else
  return 1 << 20; // increase it, if you need more precision
  #endif
}

typedef unsigned char MyType;

static const size_t kTempSize = 1 << 17;

static void PrintHelp(CPrinter &f)
{
  f.Print("use: pipelen.exe Size\n  Size - limit in KB\n");
}

#define C_NAME(x) C_##x
#define A_NAME(x) PL_##x

#define HHH(x) unsigned __fastcall C_NAME(x)(unsigned char *values, unsigned bufferSize)
#define DDD(x) unsigned __fastcall A_NAME(x)(unsigned char *values, unsigned bufferSize)

extern "C"
{
#ifdef _USE_ASM
DDD( 0); DDD( 1); DDD( 2); DDD( 3); DDD( 4); DDD( 5); DDD( 6); DDD( 7); DDD( 8); DDD( 9);
DDD(10); DDD(11); DDD(12); DDD(13); DDD(14); DDD(15); DDD(16); DDD(17); DDD(18); DDD(19);
DDD(20); DDD(21); DDD(22); DDD(23); DDD(24); DDD(25); DDD(26); DDD(27); DDD(28); DDD(29);
DDD(30); DDD(31); DDD(32); DDD(33); DDD(34); DDD(35); DDD(36); DDD(37); DDD(38); DDD(39);
DDD(40); DDD(41); DDD(42); DDD(43); DDD(44); DDD(45); DDD(46); DDD(47); DDD(48); DDD(49);
// DDD(50); DDD(51); DDD(52); DDD(53);
#define NUM_ASM_TESTS 50
#else
#define NUM_ASM_TESTS 0
#endif

#define NUM_C_MUL_TESTS 6
#define NUM_C_READ1_TESTS 3
#define NUM_C_READ2_TESTS 5

#define NUM_C_TESTS (NUM_C_MUL_TESTS + NUM_C_READ1_TESTS + NUM_C_READ2_TESTS)
#define LATEST_TEST_INDEX (NUM_C_TESTS + NUM_ASM_TESTS - 1)

#define CCC(x) case               x: { do sum += C_NAME(x)(values, bufferSize); while (--numPrecCycles); break; }
#define AAA(x) case NUM_C_TESTS + x: { do sum += A_NAME(x)(values, bufferSize); while (--numPrecCycles); break; }

#define PRE_SIZE (256 + 4)
#define ZZZ(d, s) d = s ^ values[(size_t)s - 256];
#define RRR ZZZ(c, c);
#define TTT c ^= values[(size_t)c - PRE_SIZE];
#define VVV(b, G) if ((c & (1 << b)) != 0) { G } TTT; TTT;

#define WWW(b, G) { UInt32 t; ZZZ(t, c); if ((c & (1 << b)) != 0) \
{ ZZZ(c, t); G; ZZZ(t, c); } ZZZ(c, t); TTT; }

#define MMM c = (c * m) ^ d
#define NNN c = (c * m2) ^ d
#define BBB(b, G) if ((c & (1 << b)) != 0) { RRR; G } NNN; NNN;


UInt32 g_Mult = 1;
UInt32 g_Mult2 = 1;
UInt32 g_Add = 0;

#define JJJ(n, G) \
HHH(n) { \
  UInt32 d = g_Add; UInt32 m = g_Mult; UInt32 m2 = g_Mult2; UInt32 c = 0; \
  for (unsigned i = 0; i < bufferSize; i++) { \
    c = values[(c >> 8) + i]; \
    BBB(0, G); BBB(1, G); BBB(2, G); BBB(3, G); BBB(4, G); BBB(5, G); BBB(6, G); BBB(7, G); \
  } return c; }

#define GGG(n, G, Y) \
HHH(n) { \
  UInt32 c = 0; \
  for (unsigned i = 0; i < bufferSize; i++) { \
    c = values[(c >> 8) + i]; \
    Y(0, G); Y(1, G); Y(2, G); Y(3, G); Y(4, G); Y(5, G); Y(6, G); Y(7, G); \
  } return c; }

#define KKK(n, G) GGG(n, G, VVV)
#define UUU(n, G) GGG(n, G, WWW)

JJJ(0, MMM;)
JJJ(1, MMM; MMM;)
JJJ(2, MMM; MMM; MMM;)
JJJ(3, MMM; MMM; MMM; MMM;)
JJJ(4, MMM; MMM; MMM; MMM; MMM;)
JJJ(5, MMM; MMM; MMM; MMM; MMM; MMM;)

UUU(6, RRR;)
UUU(7, RRR; RRR;)
UUU(8, RRR; RRR; RRR;)

KKK(9, RRR;)
KKK(10, RRR; RRR;)
KKK(11, RRR; RRR; RRR;)
KKK(12, RRR; RRR; RRR; RRR;)
KKK(13, RRR; RRR; RRR; RRR; RRR;)

}

static void PrintRes(CPrinter &f, const char *s, double v, bool nsMode, const char *post)
{
  char temp[128];
  sprintf(temp, "Pipeline length %s = %6.2f %s", s, v, nsMode ? "ns" : "stages");
  f.Print(temp);
  if (post)
    f.Print(post);
  f.NewLine();
}

static void PrintSize(CPrinter &f, UInt32 n)
{
  char s[32];
  if (n >= 1 << 20)
    sprintf(s, " %4d-M", n >> 20);
  else if (n >= 1 << 10)
    sprintf(s, " %4d-K", n >> 10);
  else
    sprintf(s, "   %4d", n);
  f.Print(s);
}

const char *kNames[][2] =
{
  { "if (c & mask) { c^=v[c-256]; REP-N(c=c*1^0) } REP-2(c=c*1^0)", " - TEST" },
  { "MOVZX XOR ; if (c & mask) { REP-N(c^=v[c-256]) } REP-2(c^=v[c-260])", " + MOVZX + XOR - TEST" },
  { "if (c & mask) { REP-N(c^=v[c-256]) } REP-2(c^=v[c-260])", " - TEST" },
  { "REP-12 (ADD EBX, EAX) ; TEST EBX, (1 << NN) ; JNZ", " - TEST" },
  { "REP-14 (ADD EBX, EAX) ; ADD EBX, EBX ; JC", NULL },
  { "REP-5  (MOVZX, XOR) ; SHR 1 ; JC (Unaligned)", NULL },
  { "REP-10 (MOVZX, XOR) ; SHR 1 ; JC", NULL },
  { "REP-4  (IMUL EBX, EBX, 1) ; ADD EBX, EBX; JC", NULL },
};

const unsigned kNumTests[] =
{
  NUM_C_MUL_TESTS,
  NUM_C_READ1_TESTS,
  NUM_C_READ2_TESTS,
  13,
  15,
  6,
  11,
  5
};

const unsigned kNumPosBranches[] =
{
   8,
   8,
   8,
  32,
  32,
   8,
   8,
  32
};

int main2(CPrinter &f, int numArgs, const char *args[])
{
  f.Print("PipeLen " MY_VERSION_COPYRIGHT_DATE "\n\n");
  if (numArgs < 1 || numArgs > 4)
  {
    PrintHelp(f);
    return 1;
  }

  unsigned bufferSizeMax = (64 << 10);
  bool nsMode = true;
  #ifdef CYCLES_MODE
  nsMode = false;
  #endif

  const unsigned kBasePowerMax = 28;
  unsigned testIndexBegin = 0;
  unsigned testIndexEnd = LATEST_TEST_INDEX;

  for (int yy = 1; yy < numArgs; yy++)
  {
    const char *arg = args[yy];
    if (arg[0] == 'n')
      nsMode = true;
    else if (arg[0] == 'r')
    {
      arg++;
      unsigned len = (unsigned)strlen(arg);
      if (len > 0)
      {
        testIndexBegin = atoi(arg);
        if (testIndexBegin > LATEST_TEST_INDEX)
          return 1;
        testIndexEnd = testIndexBegin;
      }
    }
    else
    {
      bufferSizeMax = atoi(arg);
      if (bufferSizeMax < 1 || bufferSizeMax > ((1 << (kBasePowerMax - 10))))
      {
        PrintHelp(f);
        return 2;
      }
      bufferSizeMax <<= 10;
    }
  }

  MY_SET_AFFINITY;

  MyType *values = (MyType *)malloc((bufferSizeMax + 4 + PRE_SIZE) * sizeof(MyType));
  if (values == 0)
  {
    f.Print("Not enough memory\n");
    return 1;
  }
  {
    for (int i = 0; i < PRE_SIZE; i++)
      *values++ = 0;
  }

  unsigned sum2 = 0;

  {
    for (unsigned k = 0; k < (1 << 28); k++)
      sum2 += (k >> 3);
  }
  
  for (unsigned testIndex = testIndexBegin; testIndex <= testIndexEnd; testIndex++)
  {
  UInt64 startCount = GetTimeCount();
  UInt64 startCount2 = GetTimeCount2();

    for (unsigned k = 0; k < (1 << 25); k++)
      sum2 += (k >> 3);

  CRandomGenerator rg;

  const unsigned kNumSubItemsBits = 0;
  const unsigned kNumSubItems = 1 << kNumSubItemsBits;
  const unsigned kNumMaxItems = (kBasePowerMax + 1) * kNumSubItems;
  unsigned sizes[kNumMaxItems];
  
  const unsigned kNumSimMax = 4;
  double ticksResults[kNumSimMax][kNumMaxItems];

  const unsigned kStartPower = 2;
  unsigned sum = 0;

  unsigned indexMax = 0;

  unsigned numPosBranches = 32;
  for (int numSimSpec = 0; numSimSpec < kNumSimMax; numSimSpec++)
  {
    int index = 0;
    for (int k = kStartPower; k < 64; k++)
    {
      int t;
      for (t = 0; t < kNumSubItems; t++, index++)
      {
        unsigned bufferSize = (1 << k);
        bufferSize += t * (1 << (k - kNumSubItemsBits));
        UInt32 numBranches = bufferSize * 8;
        numBranches /= numPosBranches;
        numBranches *= numPosBranches;
        bufferSize = numBranches / 8;

        if (bufferSize > bufferSizeMax)
          break;
        sizes[index] = numBranches;
          
        unsigned i;

        for (i = 0; i < bufferSize; i++)
            values[i] = (MyType)((numSimSpec < 2) ? ((numSimSpec & 1) ? 0xFF : 0) :
            ((numSimSpec == 2) ? 0xAA : ((rg.GetRnd() >> 6) & 0xFF)));
       
        const unsigned kNumTestCycles = 5;
        const unsigned numPrecCycles2 = (1 + GetCyclesPerPrec() / bufferSize);
        UInt64 ticks[kNumTestCycles];
        for (i = 0; i < kNumTestCycles; i++)
        {
          UInt64 startTime = GetTimeCount();
          unsigned numPrecCycles = numPrecCycles2;
          switch(testIndex)
          {
            CCC( 0) CCC( 1) CCC( 2) CCC( 3) CCC( 4) CCC( 5) CCC( 6) CCC( 7) CCC( 8) CCC( 9)
            CCC(10) CCC(11) CCC(12) CCC(13) // CCC(14) CCC(15) CCC(16) CCC(17) CCC(18)
            #ifdef _USE_ASM
            AAA( 0) AAA( 1) AAA( 2) AAA( 3) AAA( 4) AAA( 5) AAA( 6) AAA( 7) AAA( 8) AAA( 9)
            AAA(10) AAA(11) AAA(12) AAA(13) AAA(14) AAA(15) AAA(16) AAA(17) AAA(18) AAA(19)
            AAA(20) AAA(21) AAA(22) AAA(23) AAA(24) AAA(25) AAA(26) AAA(27) AAA(28) AAA(29)
            AAA(30) AAA(31) AAA(32) AAA(33) AAA(34) AAA(35) AAA(36) AAA(37) AAA(38) AAA(39)
            AAA(40) AAA(41) AAA(42) AAA(43) AAA(44) AAA(45) AAA(46) AAA(47) AAA(48) AAA(49)
            // AAA(50) AAA(51) AAA(52) AAA(53)
            #endif
            default: return 1;
          }
          ticks[i] = GetTimeCount() - startTime;
        }

        double minValue = 1 << 30;
        for (i = 0; i < kNumTestCycles; i++)
        {
          double d = (double)(Int64)ticks[i] / (bufferSize * 8) / numPrecCycles2;
          if (d < minValue)
            minValue = d;
        }
        if (minValue == 0)
          minValue = 1e-30;
        ticksResults[numSimSpec][index] = minValue;
      }
      if (t != kNumSubItems)
        break;
    }
    indexMax = index;
  }
  UInt64 countTotal = GetTimeCount() - startCount;
  UInt64 countTotal2 = GetTimeCount2() - startCount2;
  if (countTotal2 == 0)
    countTotal2 = 1;

  double freq = double((Int64)countTotal) * (Int64)GetFreq2() / (Int64)countTotal2;
  if (freq == 0)
    freq = 1;

  f.Print("#Branch   #B/P       0       1     0-1  Random    Len1    Len2\n\n");

  double res[2];

  unsigned num1 = 0;
  unsigned startSum = 0;
  unsigned group;
  for (group = 0; group < sizeof(kNumTests) / sizeof(kNumTests[0]); group++)
  {
    unsigned sum2 = startSum + kNumTests[group];
    if (testIndex < sum2)
    {
      num1 = testIndex - startSum;
      if (group <= 1)
        num1++;
      break;
    }
    startSum = sum2;
  }

  numPosBranches = kNumPosBranches[group];
  for (unsigned i = 0; i < indexMax; i++)
  {
    UInt32 numBranches = sizes[i];
    PrintSize(f, numBranches);
    PrintSize(f, numBranches / numPosBranches);
    for (unsigned numSim = 0; numSim <= kNumSimMax + 1; numSim++)
    {
      double value = (numSim == kNumSimMax) ?
          ticksResults[kNumSimMax - 1][i] * 2 - ticksResults[0][i]  - ticksResults[1][i]:
          (numSim == kNumSimMax + 1) ?
          ticksResults[kNumSimMax - 1][i] * 2 - ticksResults[2][i] * 2 :
          ticksResults[numSim][i];
      if (nsMode)
        value = double(value) * 1000000000 / freq;
      if (numSim >= kNumSimMax)
        res[numSim - kNumSimMax] = value;
      char temp[32];
      sprintf(temp, " %7.2f", value);
      f.Print(temp);
    }
    f.NewLine();
  }
  f.NewLine();

  char temp[128];

  sprintf(temp, "Test #%2d (%2d): %s \n", group, num1, kNames[group][0]);
  f.Print(temp);
  sprintf(temp, "Timer frequency  = %10.0f Hz\n", (double)(Int64)GetFreq2());
  f.Print(temp);
  sprintf(temp, "CPU frequency    = %10.2f MHz\n", freq / 1000000);
  f.Print(temp);

  const char *kPost = kNames[group][1];
  if (indexMax > 0)
  {
    char s[32];
    for (unsigned i = 0; i < 2; i++)
    {
      sprintf(s, "v.%d", i + 1);
      PrintRes(f, s, res[i], nsMode, kPost);
    }
  }
  PrintRes(f, "   ", (res[0] + res[1]) / 2, nsMode, kPost);

  f.NewLine();
  f.NewLine();
  sum2 += sum;

  }
  return (unsigned)sum2;
}

int
#ifdef _MSC_VER
_cdecl
#endif
main(int numArgs, const char *args[])
{
  CPrinter f(kTempSize);
  int res = main2(f, numArgs, args);
  fputs(f.buf, stdout);
  return res;
}
