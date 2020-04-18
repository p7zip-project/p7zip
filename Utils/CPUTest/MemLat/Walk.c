/* Walk.c -- memory walks
2010-10-14 : Igor Pavlov : Public domain */

#include "Walk.h"

unsigned MY_FAST_CALL WalkSum(const unsigned *p, unsigned num)
{
  unsigned b = 0;
  unsigned i;
  for (i = 0; i < num; i += 16)
  {
    b = b + p[i] +
      p[i + 1] +
      p[i + 2] +
      p[i + 3] +
      p[i + 4] +
      p[i + 5] +
      p[i + 6] +
      p[i + 7] +
      p[i + 8] +
      p[i + 9] +
      p[i + 10] +
      p[i + 11] +
      p[i + 12] +
      p[i + 13] +
      p[i + 14] +
      p[i + 15];
  }
  return b;
}

unsigned MY_FAST_CALL WalkSum2(const unsigned *p, unsigned num, unsigned step)
{
  unsigned b = 0;
  unsigned i;
  for (i = 0; i < num;)
  {
    b += p[i]; i += step;
    b += p[i]; i += step;
    b += p[i]; i += step;
    b += p[i]; i += step;
    b += p[i]; i += step;
    b += p[i]; i += step;
    b += p[i]; i += step;
    b += p[i]; i += step;
  }
  return b;
}

void MY_FAST_CALL WalkWrite(unsigned *p, unsigned num)
{
  unsigned i;
  for (i = 0; i < num; i += 8)
  {
    p[i] = 0;
    p[i + 1] = 0;
    p[i + 2] = 0;
    p[i + 3] = 0;
    p[i + 4] = 0;
    p[i + 5] = 0;
    p[i + 6] = 0;
    p[i + 7] = 0;
  }
}

void MY_FAST_CALL WalkWrite2(unsigned *p, unsigned num, unsigned step)
{
  unsigned i;
  for (i = 0; i < num;)
  {
    p[i] = 0; i += step;
    p[i] = 0; i += step;
    p[i] = 0; i += step;
    p[i] = 0; i += step;
    p[i] = 0; i += step;
    p[i] = 0; i += step;
    p[i] = 0; i += step;
    p[i] = 0; i += step;
  }
}

unsigned MY_FAST_CALL Walk1Add(const unsigned *p, unsigned num, unsigned a0)
{
  unsigned i;
  num >>= 4;
  for (i = 0; i < num; i++)
  {
    a0 += p[a0]; a0 += p[a0]; a0 += p[a0]; a0 += p[a0];
    a0 += p[a0]; a0 += p[a0]; a0 += p[a0]; a0 += p[a0];
    a0 += p[a0]; a0 += p[a0]; a0 += p[a0]; a0 += p[a0];
    a0 += p[a0]; a0 += p[a0]; a0 += p[a0]; a0 += p[a0];
  }
  return a0;
}

unsigned MY_FAST_CALL Walk1Nop(const unsigned *p, unsigned num, unsigned a0)
{
  unsigned k = (a0 < 0xFFFFFFFF) ? 0 : 1;
  unsigned i;
  for (i = 0; i < num; i++)
  {
    a0 += k;
    a0 = p[a0];
  }
  return a0;
}

#define R1      (unsigned)(size_t)a0
#define R2 R1 + (unsigned)(size_t)a1
#define R3 R2 + (unsigned)(size_t)a2
#define R4 R3 + (unsigned)(size_t)a3
#define R5 R4 + (unsigned)(size_t)a4
#define R6 R5 + (unsigned)(size_t)a5
#define R7 R6 + (unsigned)(size_t)a6
#define R8 R7 + (unsigned)(size_t)a7
#define R9 R8 + (unsigned)(size_t)a8
#define R10 R9 + (unsigned)(size_t)a9
#define R11 R10 + (unsigned)(size_t)a10
#define R12 R11 + (unsigned)(size_t)a11

#define A1    a0 = p[a0];
#define A2 A1 a1 = p[a1];
#define A3 A2 a2 = p[a2];
#define A4 A3 a3 = p[a3];
#define A5 A4 a4 = p[a4];
#define A6 A5 a5 = p[a5];
#define A7 A6 a6 = p[a6];
#define A8 A7 a7 = p[a7];
#define A9 A8 a8 = p[a8];
#define A10 A9 a9 = p[a9];
#define A11 A10 a10 = p[a10];
#define A12 A11 a11 = p[a11];

WWW_DECL(1)
{
  unsigned i;
  num >>= 4;
  for (i = 0; i < num; i++)
    a0 = p[p[p[p[p[p[p[p[p[p[p[p[p[p[p[p[a0]]]]]]]]]]]]]]]];
  return a0;
}

#define WWW(n) WWW_DECL(n) \
{ unsigned i; num >>= 2; \
  for (i = 0; i < num; i++) { \
    A ## n; A ## n; A ## n; A ## n; \
  } return R ## n ; }

WWW(2)
WWW(3)
WWW(4)
WWW(5)
WWW(6)
WWW(7)
WWW(8)
WWW(9)
WWW(10)
WWW(11)
WWW(12)

#define D1    a0 = *a0;
#define D2 D1 a1 = *a1;
#define D3 D2 a2 = *a2;
#define D4 D3 a3 = *a3;
#define D5 D4 a4 = *a4;
#define D6 D5 a5 = *a5;
#define D7 D6 a6 = *a6;
#define D8 D7 a7 = *a7;
#define D9 D8 a8 = *a8;
#define D10 D9 a9 = *a9;

unsigned MY_FAST_CALL Walk1d(unsigned num, P1)
{
  unsigned i;
  num >>= 4;
  for (i = 0; i < num; i++)
    a0 = *(void **)*(void **)*(void **)*(void **)
        *(void **)*(void **)*(void **)*(void **)
        *(void **)*(void **)*(void **)*(void **)
        *(void **)*(void **)*(void **)*a0;
  return R1;
}

#define DDD(n) DDD_DECL(n) \
{ unsigned i; num >>= 3; \
  for (i = 0; i < num; i++) { \
    D ## n; D ## n; D ## n; D ## n; \
    D ## n; D ## n; D ## n; D ## n; \
  } return R ## n ; }

DDD(2)
DDD(3)
DDD(4)
DDD(5)
DDD(6)
DDD(7)
DDD(8)
DDD(9)
DDD(10)

#define PPP(x, y) y = p[x]; p[x] = y;

#define B1     PPP(a0, z0)
#define B2  B1 PPP(a1, z1)
#define B3  B2 PPP(a2, z2)
#define B4  B3 PPP(a3, z3)
#define B5  B4 PPP(a4, z4)
#define B6  B5 PPP(a5, z5)

#define C1     PPP(z0, a0)
#define C2  C1 PPP(z1, a1)
#define C3  C2 PPP(z2, a2)
#define C4  C3 PPP(z3, a3)
#define C5  C4 PPP(z4, a4)
#define C6  C5 PPP(z5, a5)

#define Z1     z0
#define Z2 Z1, z1
#define Z3 Z2, z2
#define Z4 Z3, z3
#define Z5 Z4, z4
#define Z6 Z5, z5

#define RW(n) RW_DECL(n) \
{ unsigned i; num >>= 3; for (i = 0; i < num; i++) { \
    unsigned Z ## n; \
    B ## n; C ## n; \
    B ## n; C ## n; \
    B ## n; C ## n; \
    B ## n; C ## n; \
  } return R ## n ; }

RW(1)
RW(2)
RW(3)
RW(4)
RW(5)
RW(6)


#define MMM(x, y) y = *x; *x = y;

#define E1     MMM(a0, z0)
#define E2  E1 MMM(a1, z1)
#define E3  E2 MMM(a2, z2)

#define F1     MMM(z0, a0)
#define F2  F1 MMM(z1, a1)
#define F3  F2 MMM(z2, a2)

#define Y1     **z0
#define Y2 Y1, **z1
#define Y3 Y2, **z2

#define WD(n) WD_DECL(n) \
{ unsigned i; num >>= 3; \
  for (i = 0; i < num; i++) { \
    void Y ## n; \
    E ## n; F ## n; \
    E ## n; F ## n; \
    E ## n; F ## n; \
    E ## n; F ## n; \
  } return R ## n ; }

WD(1)
WD(2)
WD(3)
