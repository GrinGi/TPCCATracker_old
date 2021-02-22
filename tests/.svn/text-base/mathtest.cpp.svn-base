/*
    Copyright (C) 2009 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License, or (at your option) version 3.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.

*/

#include "unittest.h"
#include "cyclecounter.h"
#include <iomanip>
#include <cstdio>

inline float ApproxSqrt( const float &x, int magic1, int magic2, int magic3 )
{
  float r = x;
  float tmp = x;
  float tmp2;

//X   asm(
//X       "shr $22,%2\n\t"
//X       "shr %0\n\t"
//X       "test $3,%2\n\t" // MSB of Mantissa and LSB of Exponent
//X       "cmovp %3,%1\n\t"
//X       "add %1,%0\n\t"
//X       : "+r"( r ), "+r"( magic1 ), "+r"( tmp )
//X       : "r"( magic2 )
//X      );

  // [1-2)                     [2-4)
  // e   m-1
  // e/2 m/2+0.5
  asm(
      "shr %2\n\t"
      "mov %2,%3\n\t"
      "add $0x1f800000,%3\n\t"
      "shr $2,%2\n\t"
      "and $0x000fffff,%2\n\t"
      "or  $0x00100000,%2\n\t" // %2 == mantissa/8
      "imul $3,%2\n\t" // %2 == 3/4 * mantissa
      "and $0x007fffff,%2\n\t"
      "and $0x3f800000,%3\n\t"
      //"add $0x00800000,%3\n\t"
      "add %2,%3\n\t"

//X       "shr %0\n\t"
//X       "add %1,%0\n\t"
//X       "cmp %3,%0\n\t"
      "mov %3,%0\n\t"
      //"cmovg %3,%0\n\t"
//X       "shr $22,%2\n\t"
//X       "test $3,%2\n\t" // MSB of Mantissa and LSB of Exponent
//X       "cmovp %3,%1\n\t"
      : "+r"( r ), "+r"( magic1 ), "+r"( tmp ), "=r"( tmp2 )
      : "r"( magic2 )
     );
  return r;
}

inline float ApproxSqrt2( const float &x, const int magic1, const int magic2 )
{
  float r = x;
  {
  __m128 tmp;
  asm(
      "movss %0,%1\n\t"
      "sqrtss %1,%1\n\t"
      "movss %1,%0\n\t"
      : "+m"( r ), "=x"( tmp )
     );
  return r;
  }

  float tmp = magic1;
  asm(
      "shr %0\n\t"
      "sub %0,%1\n\t" // ((x >> 1) - 0x5f3759df
    "mov %1,%0\n\t"
      //"xor $0xffffffff,%0\n\t" // *-1 (almost)
      : "+r"( r ), "+r"( tmp )
     );
      return r;
  r *= 1.5f - 0.5f * x * r * r;
  return x * r;
}

void testApproxSqrt()
{
//X   CycleCounter tsc;
//X   for ( int i = 0; i < count; ++i ) {
//X     float cc = s[i];
//X     tsc.Start();
//X     for ( int j = 0; j < 1000; ++j ) {
//X       cc = ApproxSqrt( cc );
//X     }
//X     tsc.Stop();
//X     std::cout << tsc.Cycles() * 0.001 << " cycles to calculate a sqrt " << c << std::endl;
//X   }

  std::cout << std::setprecision( 12 );

  double err;
  double maxErr;
  double errB[8];
  // smallest overall error:
  //int magic[3] = { 0x1f3f775a, 0x5f41cf57, 0 };
  // smallest maximal error:
  int magic[3] = { 0x5f35097b, 0x5f41cf57, 0 };
  for ( int outerIt = 0; outerIt < 1; ++outerIt ) {
  for ( int magicIt = 0; magicIt < 1; ++magicIt ) {
    int goodMagic = magic[magicIt];
    int nextStep  = 0x00100000;
    int &m = magic[magicIt];
    double bestErr = 1.;
    enum State {
      Start, Up, Down, Done
    } state = Start;
    do {
      err = 0.;
      maxErr = 0.;
      double countB[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
      for ( int i = 0; i < 8; ++i ) {
        errB[i] = 0.;
      }
      for ( int i = 0; i <= 0x3fff; ++i ) {
        const float x = 1.0f + ( 3.0f / 0x3fff ) * i;
        const float as = ApproxSqrt2( x, magic[0], magic[1] );//, magic[2] );
        const float ss = std::sqrt( x );
        fprintf( stderr, "%.010f\t%.010f\n", x, as );
        const double ds = std::abs( static_cast<double>( as ) - static_cast<double>( ss ) );
        const double relErr = ds / ss;
        if ( relErr > maxErr ) {
          maxErr = relErr;
        }
        err += relErr;
        union {
          float f;
          unsigned int i;
        } xx = { x };
        int B = ( xx.i & 0x00e00000 ) >> 21;
        errB[B] += relErr;
        ++countB[B];
      }
      err /= 0x3fff;
      for ( int i = 0; i < 8; ++i ) {
        errB[i] /= countB[i];
      }
      if ( err < bestErr && maxErr < 0.002 ) {
        bestErr = err;
        goodMagic = m;
        nextStep >>= 1;
        state = Up;
        m += nextStep;
      } else if ( state == Up ) {
        state = Down;
        m = goodMagic - nextStep;
      } else if ( state == Down ) {
        nextStep >>= 1;
        if ( nextStep == 0 ) {
          break;
        }
        state = Up;
        m = goodMagic + nextStep;
      }
    } while ( false );
    m = goodMagic;
  }
  }
  std::cout << "overall error: " << err * 100.0 << "%, max error: " << maxErr * 100.0 << "%" << std::endl;
  printf( "magic = 0x%x, 0x%x, 0x%x\n", magic[0], magic[1], magic[2] );
  for ( int i = 0; i < 8; ++i ) {
    printf( "%d: %.012f%%\n", i, errB[i] * 100.0 );
  }
  // 1 indicates the set bit to test for
  printf( "100: %.012f%% %.012f%% %.012f%% %.012f%%\n", errB[4] * 100.0, errB[5] * 100.0, errB[6] * 100.0, errB[7] * 100.0 );
  printf( "010: %.012f%% %.012f%% %.012f%% %.012f%%\n", errB[2] * 100.0, errB[3] * 100.0, errB[6] * 100.0, errB[7] * 100.0 );
  printf( "001: %.012f%% %.012f%% %.012f%% %.012f%%\n", errB[1] * 100.0, errB[3] * 100.0, errB[5] * 100.0, errB[7] * 100.0 );

  // 0 indicates the unset bit to test for
  printf( "011: %.012f%% %.012f%% %.012f%% %.012f%%\n", errB[0] * 100.0, errB[1] * 100.0, errB[2] * 100.0, errB[3] * 100.0 );
  printf( "101: %.012f%% %.012f%% %.012f%% %.012f%%\n", errB[0] * 100.0, errB[1] * 100.0, errB[4] * 100.0, errB[5] * 100.0 );
  printf( "110: %.012f%% %.012f%% %.012f%% %.012f%%\n", errB[0] * 100.0, errB[2] * 100.0, errB[4] * 100.0, errB[6] * 100.0 );

  printf( "p0: %.012f%% %.012f%% %.012f%% %.012f%%\n", errB[0] * 100.0, errB[3] * 100.0, errB[5] * 100.0, errB[6] * 100.0 );
  printf( "p1: %.012f%% %.012f%% %.012f%% %.012f%%\n", errB[1] * 100.0, errB[2] * 100.0, errB[4] * 100.0, errB[7] * 100.0 );

}

int main()
{
  runTest( testApproxSqrt );
}
