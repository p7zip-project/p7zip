/* Walk.h -- memory walks
2010-12-17 : Igor Pavlov : Public domain */

#ifndef __WALK_H
#define __WALK_H

#include <stddef.h>

#ifdef _MSC_VER
#if _MSC_VER >= 1300
#define MY_NO_INLINE __declspec(noinline)
#else
#define MY_NO_INLINE
#endif
#define MY_FAST_CALL MY_NO_INLINE __fastcall
#else
#define MY_FAST_CALL
#endif

#ifdef __cplusplus
extern "C" {
#endif

unsigned MY_FAST_CALL WalkSum(const unsigned *p, unsigned num);
unsigned MY_FAST_CALL WalkSum2(const unsigned *p, unsigned num, unsigned step);
void MY_FAST_CALL WalkWrite(unsigned *p, unsigned num);
void MY_FAST_CALL WalkWrite2(unsigned *p, unsigned num, unsigned step);
unsigned MY_FAST_CALL Walk1Add(const unsigned *p, unsigned num, unsigned a0);
unsigned MY_FAST_CALL Walk1Nop(const unsigned *p, unsigned num, unsigned a0);

#define U1 unsigned a0
#define U2 U1, unsigned a1
#define U3 U2, unsigned a2
#define U4 U3, unsigned a3
#define U5 U4, unsigned a4
#define U6 U5, unsigned a5
#define U7 U6, unsigned a6
#define U8 U7, unsigned a7
#define U9 U8, unsigned a8
#define U10 U9, unsigned a9
#define U11 U10, unsigned a10
#define U12 U11, unsigned a11

#define WWW_DECL(n) \
  unsigned MY_FAST_CALL Walk ## n(const unsigned *p, unsigned num, U ## n)

WWW_DECL(1);
WWW_DECL(2);
WWW_DECL(3);
WWW_DECL(4);
WWW_DECL(5);
WWW_DECL(6);
WWW_DECL(7);
WWW_DECL(8);
WWW_DECL(9);
WWW_DECL(10);
WWW_DECL(11);
WWW_DECL(12);

#define RW_DECL(n) \
  unsigned MY_FAST_CALL Walk ## n ## w(unsigned *p, unsigned num, U ## n)

RW_DECL(1);
RW_DECL(2);
RW_DECL(3);
RW_DECL(4);
RW_DECL(5);
RW_DECL(6);

#define P1 void **a0
#define P2 P1, void **a1
#define P3 P2, void **a2
#define P4 P3, void **a3
#define P5 P4, void **a4
#define P6 P5, void **a5
#define P7 P6, void **a6
#define P8 P7, void **a7
#define P9 P8, void **a8
#define P10 P9, void **a9

#define DDD_DECL(n) \
  unsigned MY_FAST_CALL Walk ## n ## d(unsigned num, P ## n)

DDD_DECL(1);
DDD_DECL(2);
DDD_DECL(3);
DDD_DECL(4);
DDD_DECL(5);
DDD_DECL(6);
DDD_DECL(7);
DDD_DECL(8);
DDD_DECL(9);
DDD_DECL(10);

#define WD_DECL(n) \
  unsigned MY_FAST_CALL Walk ## n ## wd(unsigned num, P ## n)

WD_DECL(1);
WD_DECL(2);
WD_DECL(3);

#ifdef __cplusplus
}
#endif


#endif
