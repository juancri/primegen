#include "primegen.h"
#include "primegen_impl.h"
#include "int64.h"

static const uint32 two[32] = {
  0x00000001 , 0x00000002 , 0x00000004 , 0x00000008
, 0x00000010 , 0x00000020 , 0x00000040 , 0x00000080
, 0x00000100 , 0x00000200 , 0x00000400 , 0x00000800
, 0x00001000 , 0x00002000 , 0x00004000 , 0x00008000
, 0x00010000 , 0x00020000 , 0x00040000 , 0x00080000
, 0x00100000 , 0x00200000 , 0x00400000 , 0x00800000
, 0x01000000 , 0x02000000 , 0x04000000 , 0x08000000
, 0x10000000 , 0x20000000 , 0x40000000 , 0x80000000
} ;

static void clear(register uint32 (*buf)[B32])
{
  register int i;
  register int j;

  for (j = 0;j < 16;++j) {
    for (i = 0;i < B32;++i)
      (*buf)[i] = ~0;
    ++buf;
  }
}

static void doit4(register uint32 *a,register long x,register long y,int64 start)
{
  long i0;
  long y0;
  register long i;
  register uint32 data;
  register uint32 pos;
  register uint32 bits;

  x += x; x += 15;
  y += 15;

  start += 1000000000;
  while (start < 0) { start += x; x += 30; }
  start -= 1000000000;
  i = start;

  while (i < B) { i += x; x += 30; }

  for (;;) {
    x -= 30;
    if (x <= 15) return;
    i -= x;

    while (i < 0) { i += y; y += 30; }

    i0 = i; y0 = y;
    while (i < B) {
      pos = i; data = i;
      pos >>= 5; data &= 31;
      i += y; y += 30;
      bits = a[pos]; data = two[data];
      bits ^= data;
      a[pos] = bits;
    }
    i = i0; y = y0;
  }
}

static void doit6(register uint32 *a,register long x,register long y,int64 start)
{
  long i0;
  long y0;
  register long i;
  register uint32 data;
  register uint32 pos;
  register uint32 bits;

  x += 5;
  y += 15;

  start += 1000000000;
  while (start < 0) { start += x; x += 10; }
  start -= 1000000000;
  i = start;
  while (i < B) { i += x; x += 10; }

  for (;;) {
    x -= 10;
    if (x <= 5) return;
    i -= x;

    while (i < 0) { i += y; y += 30; }

    i0 = i; y0 = y;
    while (i < B) {
      pos = i; data = i;
      pos >>= 5; data &= 31;
      i += y; y += 30;
      bits = a[pos]; data = two[data];
      bits ^= data;
      a[pos] = bits;
    }
    i = i0; y = y0;
  }
}

static void doit12(register uint32 *a,register long x,register long y,int64 start)
{
  long i0;
  long y0;
  register long i;
  register uint32 data;
  register uint32 pos;
  register uint32 bits;

  x += 5;

  start += 1000000000;
  while (start < 0) { start += x; x += 10; }
  start -= 1000000000;
  i = start;
  while (i < 0) { i += x; x += 10; }

  y += 15;
  x += 10;

  for (;;) {
    while (i >= B) {
      if (x <= y) return;
      i -= y;
      y += 30;
    }
    i0 = i;
    y0 = y;
    while ((i >= 0) && (y < x)) {
      pos = i; data = i;
      pos >>= 5; data &= 31;
      i -= y;
      y += 30;
      bits = a[pos]; data = two[data];
      bits ^= data;
      a[pos] = bits;
    }
    i = i0;
    y = y0;
    i += x - 10;
    x += 10;
  }
}

static const int deltainverse[60] = {
 -1,B32 * 0,-1,-1,-1,-1,-1,B32 * 1,-1,-1,-1,B32 * 2,-1,B32 * 3,-1
,-1,-1,B32 * 4,-1,B32 * 5,-1,-1,-1,B32 * 6,-1,-1,-1,-1,-1,B32 * 7
,-1,B32 * 8,-1,-1,-1,-1,-1,B32 * 9,-1,-1,-1,B32 * 10,-1,B32 * 11,-1
,-1,-1,B32 * 12,-1,B32 * 13,-1,-1,-1,B32 * 14,-1,-1,-1,-1,-1,B32 * 15
} ;

static void squarefree1big(uint32 (*buf)[B32],uint64 base,uint32 q,uint64 qq)
{
  uint64 i;
  uint32 pos;
  int n;
  uint64 bound = base + 60 * B;

  while (qq < bound) {
    if (bound < 2000000000)
      i = qq - (((uint32) base) % ((uint32) qq));
    else
      i = qq - (base % qq);
    if (!(i & 1)) i += qq;

    if (i < B * 60) { 
      pos = i;
      n = deltainverse[pos % 60];
      if (n >= 0) {
        pos /= 60;
        (*buf)[n + (pos >> 5)] |= two[pos & 31];
      }
    }

    qq += q; q += 1800;
  }
}

static void squarefree1(register uint32 (*buf)[B32],uint64 L,uint32 q)
{
  uint32 qq;
  register uint32 qqhigh;
  uint32 i;
  register uint32 ilow;
  register uint32 ihigh;
  register int n;
  uint64 base;

  base = 60 * L;
  qq = q * q;
  q = 60 * q + 900;

  while (qq < B * 60) {
    if (base < 2000000000)
      i = qq - (((uint32) base) % qq);
    else
      i = qq - (base % qq);
    if (!(i & 1)) i += qq;

    if (i < B * 60) { 
      qqhigh = qq / 60;
      ilow = i % 60; ihigh = i / 60;
  
      qqhigh += qqhigh;
      while (ihigh < B) {
        n = deltainverse[ilow];
        if (n >= 0)
          (*buf)[n + (ihigh >> 5)] |= two[ihigh & 31];

        ilow += 2; ihigh += qqhigh;
        if (ilow >= 60) { ilow -= 60; ihigh += 1; }
      }
    }

    qq += q; q += 1800;
  }

  squarefree1big(buf,base,q,qq);
}

static void squarefree49big(uint32 (*buf)[B32],uint64 base,uint32 q,uint64 qq)
{
  uint64 i;
  uint32 pos;
  int n;
  uint64 bound = base + 60 * B;

  while (qq < bound) {
    if (bound < 2000000000)
      i = qq - (((uint32) base) % ((uint32) qq));
    else
      i = qq - (base % qq);
    if (!(i & 1)) i += qq;

    if (i < B * 60) { 
      pos = i;
      n = deltainverse[pos % 60];
      if (n >= 0) {
        pos /= 60;
        (*buf)[n + (pos >> 5)] |= two[pos & 31];
      }
    }

    qq += q; q += 1800;
  }
}

static void squarefree49(register uint32 (*buf)[B32],uint64 L,uint32 q)
{
  uint32 qq;
  register uint32 qqhigh;
  uint32 i;
  register uint32 ilow;
  register uint32 ihigh;
  register int n;
  uint64 base = 60 * L;

  qq = q * q;
  q = 60 * q + 900;

  while (qq < B * 60) {
    if (base < 2000000000)
      i = qq - (((uint32) base) % qq);
    else
      i = qq - (base % qq);
    if (!(i & 1)) i += qq;

    if (i < B * 60) { 
      qqhigh = qq / 60;
      ilow = i % 60; ihigh = i / 60;
  
      qqhigh += qqhigh;
      qqhigh += 1;
      while (ihigh < B) {
        n = deltainverse[ilow];
        if (n >= 0)
          (*buf)[n + (ihigh >> 5)] |= two[ihigh & 31];

        ilow += 38; ihigh += qqhigh;
        if (ilow >= 60) { ilow -= 60; ihigh += 1; }
      }
    }

    qq += q; q += 1800;
  }

  squarefree49big(buf,base,q,qq);
}

/* squares of primes >= 7, < 240 */
uint32 qqtab[49] = {
  49,121,169,289,361,529,841,961,1369,1681
 ,1849,2209,2809,3481,3721,4489,5041,5329,6241,6889
 ,7921,9409,10201,10609,11449,11881,12769,16129,17161,18769
 ,19321,22201,22801,24649,26569,27889,29929,32041,32761,36481
 ,37249,38809,39601,44521,49729,51529,52441,54289,57121
} ;

/* (qq * 11 + 1) / 60 or (qq * 59 + 1) / 60 */
uint32 qq60tab[49] = {
  9,119,31,53,355,97,827,945,251,1653
 ,339,405,515,3423,3659,823,4957,977,6137
 ,1263,7789,1725,10031,1945,2099,11683,2341,2957
 ,16875,3441,18999,21831,22421,4519,4871,5113,5487
 ,31507,32215,35873,6829,7115,38941,43779,9117,9447,51567,9953,56169
} ;

static void squarefreetiny(register uint32 *a,uint32 *Lmodqq,int d)
{
  int j;
  register uint32 k;
  register uint32 qq;
  register uint32 pos;
  register uint32 data;
  register uint32 bits;

  for (j = 0;j < 49;++j) {
    qq = qqtab[j];
    k = qq - 1 - ((Lmodqq[j] + qq60tab[j] * d - 1) % qq);
    while (k < B) {
      pos = k;
      data = k;
      pos >>= 5;
      data &= 31;
      k += qq;
      bits = a[pos];
      data = two[data];
      bits |= data;
      a[pos] = bits;
    }
  }
}

typedef struct { char index; char f; char g; char k; } todo;

static const todo for4[] = {
  {0,2,15,4} , {0,3,5,1} , {0,3,25,11} , {0,5,9,3}
, {0,5,21,9} , {0,7,15,7} , {0,8,15,8} , {0,10,9,8}
, {0,10,21,14} , {0,12,5,10} , {0,12,25,20} , {0,13,15,15}
, {0,15,1,15} , {0,15,11,17} , {0,15,19,21} , {0,15,29,29}
, {3,1,3,0} , {3,1,27,12} , {3,4,3,1} , {3,4,27,13}
, {3,6,7,3} , {3,6,13,5} , {3,6,17,7} , {3,6,23,11}
, {3,9,7,6} , {3,9,13,8} , {3,9,17,10} , {3,9,23,14}
, {3,11,3,8} , {3,11,27,20} , {3,14,3,13} , {3,14,27,25}
, {4,2,1,0} , {4,2,11,2} , {4,2,19,6} , {4,2,29,14}
, {4,7,1,3} , {4,7,11,5} , {4,7,19,9} , {4,7,29,17}
, {4,8,1,4} , {4,8,11,6} , {4,8,19,10} , {4,8,29,18}
, {4,13,1,11} , {4,13,11,13} , {4,13,19,17} , {4,13,29,25}
, {7,1,5,0} , {7,1,25,10} , {7,4,5,1} , {7,4,25,11}
, {7,5,7,2} , {7,5,13,4} , {7,5,17,6} , {7,5,23,10}
, {7,10,7,7} , {7,10,13,9} , {7,10,17,11} , {7,10,23,15}
, {7,11,5,8} , {7,11,25,18} , {7,14,5,13} , {7,14,25,23}
, {9,2,9,1} , {9,2,21,7} , {9,3,1,0} , {9,3,11,2}
, {9,3,19,6} , {9,3,29,14} , {9,7,9,4} , {9,7,21,10}
, {9,8,9,5} , {9,8,21,11} , {9,12,1,9} , {9,12,11,11}
, {9,12,19,15} , {9,12,29,23} , {9,13,9,12} , {9,13,21,18}
, {10,2,5,0} , {10,2,25,10} , {10,5,1,1} , {10,5,11,3}
, {10,5,19,7} , {10,5,29,15} , {10,7,5,3} , {10,7,25,13}
, {10,8,5,4} , {10,8,25,14} , {10,10,1,6} , {10,10,11,8}
, {10,10,19,12} , {10,10,29,20} , {10,13,5,11} , {10,13,25,21}
, {13,1,15,3} , {13,4,15,4} , {13,5,3,1} , {13,5,27,13}
, {13,6,5,2} , {13,6,25,12} , {13,9,5,5} , {13,9,25,15}
, {13,10,3,6} , {13,10,27,18} , {13,11,15,11} , {13,14,15,16}
, {13,15,7,15} , {13,15,13,17} , {13,15,17,19} , {13,15,23,23}
, {14,1,7,0} , {14,1,13,2} , {14,1,17,4} , {14,1,23,8}
, {14,4,7,1} , {14,4,13,3} , {14,4,17,5} , {14,4,23,9}
, {14,11,7,8} , {14,11,13,10} , {14,11,17,12} , {14,11,23,16}
, {14,14,7,13} , {14,14,13,15} , {14,14,17,17} , {14,14,23,21}
} ;

static const todo for6[] = {
  {1,1,2,0} , {1,1,8,1} , {1,1,22,8} , {1,1,28,13}
, {1,3,10,2} , {1,3,20,7} , {1,7,10,4} , {1,7,20,9}
, {1,9,2,4} , {1,9,8,5} , {1,9,22,12} , {1,9,28,17}
, {5,1,4,0} , {5,1,14,3} , {5,1,16,4} , {5,1,26,11}
, {5,5,2,1} , {5,5,8,2} , {5,5,22,9} , {5,5,28,14}
, {5,9,4,4} , {5,9,14,7} , {5,9,16,8} , {5,9,26,15}
, {8,3,2,0} , {8,3,8,1} , {8,3,22,8} , {8,3,28,13}
, {8,5,4,1} , {8,5,14,4} , {8,5,16,5} , {8,5,26,12}
, {8,7,2,2} , {8,7,8,3} , {8,7,22,10} , {8,7,28,15}
, {11,1,10,1} , {11,1,20,6} , {11,3,4,0} , {11,3,14,3}
, {11,3,16,4} , {11,3,26,11} , {11,7,4,2} , {11,7,14,5}
, {11,7,16,6} , {11,7,26,13} , {11,9,10,5} , {11,9,20,10}
} ;

static const todo for12[] = {
  {2,2,1,0} , {2,2,11,-2} , {2,2,19,-6} , {2,2,29,-14}
, {2,3,4,0} , {2,3,14,-3} , {2,3,16,-4} , {2,3,26,-11}
, {2,5,2,1} , {2,5,8,0} , {2,5,22,-7} , {2,5,28,-12}
, {2,7,4,2} , {2,7,14,-1} , {2,7,16,-2} , {2,7,26,-9}
, {2,8,1,3} , {2,8,11,1} , {2,8,19,-3} , {2,8,29,-11}
, {2,10,7,4} , {2,10,13,2} , {2,10,17,0} , {2,10,23,-4}
, {6,1,10,-2} , {6,1,20,-7} , {6,2,7,-1} , {6,2,13,-3}
, {6,2,17,-5} , {6,2,23,-9} , {6,3,2,0} , {6,3,8,-1}
, {6,3,22,-8} , {6,3,28,-13} , {6,4,5,0} , {6,4,25,-10}
, {6,6,5,1} , {6,6,25,-9} , {6,7,2,2} , {6,7,8,1}
, {6,7,22,-6} , {6,7,28,-11} , {6,8,7,2} , {6,8,13,0}
, {6,8,17,-2} , {6,8,23,-6} , {6,9,10,2} , {6,9,20,-3}
, {12,1,4,-1} , {12,1,14,-4} , {12,1,16,-5} , {12,1,26,-12}
, {12,2,5,-1} , {12,2,25,-11} , {12,3,10,-2} , {12,3,20,-7}
, {12,4,1,0} , {12,4,11,-2} , {12,4,19,-6} , {12,4,29,-14}
, {12,6,1,1} , {12,6,11,-1} , {12,6,19,-5} , {12,6,29,-13}
, {12,7,10,0} , {12,7,20,-5} , {12,8,5,2} , {12,8,25,-8}
, {12,9,4,3} , {12,9,14,0} , {12,9,16,-1} , {12,9,26,-8}
, {15,1,2,-1} , {15,1,8,-2} , {15,1,22,-9} , {15,1,28,-14}
, {15,4,7,-1} , {15,4,13,-3} , {15,4,17,-5} , {15,4,23,-9}
, {15,5,4,0} , {15,5,14,-3} , {15,5,16,-4} , {15,5,26,-11}
, {15,6,7,0} , {15,6,13,-2} , {15,6,17,-4} , {15,6,23,-8}
, {15,9,2,3} , {15,9,8,2} , {15,9,22,-5} , {15,9,28,-10}
, {15,10,1,4} , {15,10,11,2} , {15,10,19,-2} , {15,10,29,-10}
} ;

void primegen_sieve(primegen *pg)
{
  uint32 (*buf)[B32];
  uint64 L;
  int i;
  uint32 Lmodqq[49];

  buf = pg->buf;
  L = pg->L;

  if (L > 2000000000)
    for (i = 0;i < 49;++i)
      Lmodqq[i] = L % qqtab[i];
  else
    for (i = 0;i < 49;++i)
      Lmodqq[i] = ((uint32) L) % qqtab[i];

  clear(buf);

  for (i = 0;i < 16;++i)
    doit4(buf[0],for4[i].f,for4[i].g,(int64) for4[i].k - L);
  squarefreetiny(buf[0],Lmodqq,1);
  for (;i < 32;++i)
    doit4(buf[3],for4[i].f,for4[i].g,(int64) for4[i].k - L);
  squarefreetiny(buf[3],Lmodqq,13);
  for (;i < 48;++i)
    doit4(buf[4],for4[i].f,for4[i].g,(int64) for4[i].k - L);
  squarefreetiny(buf[4],Lmodqq,17);
  for (;i < 64;++i)
    doit4(buf[7],for4[i].f,for4[i].g,(int64) for4[i].k - L);
  squarefreetiny(buf[7],Lmodqq,29);
  for (;i < 80;++i)
    doit4(buf[9],for4[i].f,for4[i].g,(int64) for4[i].k - L);
  squarefreetiny(buf[9],Lmodqq,37);
  for (;i < 96;++i)
    doit4(buf[10],for4[i].f,for4[i].g,(int64) for4[i].k - L);
  squarefreetiny(buf[10],Lmodqq,41);
  for (;i < 112;++i)
    doit4(buf[13],for4[i].f,for4[i].g,(int64) for4[i].k - L);
  squarefreetiny(buf[13],Lmodqq,49);
  for (;i < 128;++i)
    doit4(buf[14],for4[i].f,for4[i].g,(int64) for4[i].k - L);
  squarefreetiny(buf[14],Lmodqq,53);

  for (i = 0;i < 12;++i)
    doit6(buf[1],for6[i].f,for6[i].g,(int64) for6[i].k - L);
  squarefreetiny(buf[1],Lmodqq,7);
  for (;i < 24;++i)
    doit6(buf[5],for6[i].f,for6[i].g,(int64) for6[i].k - L);
  squarefreetiny(buf[5],Lmodqq,19);
  for (;i < 36;++i)
    doit6(buf[8],for6[i].f,for6[i].g,(int64) for6[i].k - L);
  squarefreetiny(buf[8],Lmodqq,31);
  for (;i < 48;++i)
    doit6(buf[11],for6[i].f,for6[i].g,(int64) for6[i].k - L);
  squarefreetiny(buf[11],Lmodqq,43);

  for (i = 0;i < 24;++i)
    doit12(buf[2],for12[i].f,for12[i].g,(int64) for12[i].k - L);
  squarefreetiny(buf[2],Lmodqq,11);
  for (;i < 48;++i)
    doit12(buf[6],for12[i].f,for12[i].g,(int64) for12[i].k - L);
  squarefreetiny(buf[6],Lmodqq,23);
  for (;i < 72;++i)
    doit12(buf[12],for12[i].f,for12[i].g,(int64) for12[i].k - L);
  squarefreetiny(buf[12],Lmodqq,47);
  for (;i < 96;++i)
    doit12(buf[15],for12[i].f,for12[i].g,(int64) for12[i].k - L);
  squarefreetiny(buf[15],Lmodqq,59);

  squarefree49(buf,L,247);
  squarefree49(buf,L,253);
  squarefree49(buf,L,257);
  squarefree49(buf,L,263);
  squarefree1(buf,L,241);
  squarefree1(buf,L,251);
  squarefree1(buf,L,259);
  squarefree1(buf,L,269);
}
