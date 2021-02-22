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
#include <AliHLTTPCCAGrid.h>
#include <AliHLTArray.h>

void testTrivialSquareGrid()
{
  AliHLTTPCCAGrid grid;
  // 1 bin in the square 0..1
  grid.Create( 0.f, 1.f, 0.f, 1.f, 1 );

  for ( float y = -1.f; y < 2.f; y += 0.01f ) {
    for ( float z = -1.f; z < 2.f; z += 0.01f ) {
      VERIFY( grid.GetBin( y, z ) == 0u );
    }
  }
}

void test2by2Grid()
{
  AliHLTTPCCAGrid grid;
  // 4 bins in the square 0..1
  grid.Create( 0.f, 1.f, 0.f, 1.f, 4 );

  const float step = 0.001f;

  for ( float y = 0.f; y < 0.5f; y += step ) {
    for ( float z = 0.f; z < 0.5f; z += step ) {
      VERIFY( 0u == grid.GetBin( y, z ) );
    }
    for ( float z = 0.5f; z <= 1.0f; z += step ) {
      VERIFY( 2u == grid.GetBin( y, z ) );
    }
  }
  for ( float y = 0.5f; y <= 1.0f; y += step ) {
    for ( float z = 0.f; z < 0.5f; z += step ) {
      VERIFY( 1u == grid.GetBin( y, z ) );
    }
    for ( float z = 0.5f; z <= 1.0f; z += step ) {
      VERIFY( 3u == grid.GetBin( y, z ) );
    }
  }
}

void testSquareGrids()
{
  AliHLTTPCCAGrid grid;
  for ( unsigned int n = 3; n < 10; ++n ) {
    unsigned int n2 = n * n;
    // n^2 bins in the square 0..1
    grid.Create( 0.f, 1.f, 0.f, 1.f, n2 );
    VERIFY( grid.StepYInv() > 0.f );
    VERIFY( grid.StepZInv() > 0.f );

    AliHLTResizableArray<float> border( n + 1 );
    const float fn = static_cast<float>(n);
    for ( unsigned int i = 0; i <= n; ++i ) {
      border[ i ] = i / fn;
    }

    unsigned int by, bz;
    const float step = 0.0005f;

    for ( unsigned int i = 0; i < n; ++i ) {
      for ( float y = border[ i ]; y < border[ i + 1 ]; y += step ) {
        for ( unsigned int j = 0; j < n; ++j ) {
          for ( float z = border[ j ]; z < border[ j + 1 ]; z += step ) {
            VERIFY( j * n + i == grid.GetBin( y, z ) );
            grid.GetBin( y, z, &by, &bz );
            VERIFY( i == by );
            VERIFY( j == bz );

            VERIFY( j * n + i == grid.GetBinNoCheck( y, z ) );
            grid.GetBinNoCheck( y, z, &by, &bz );
            VERIFY( i == by );
            VERIFY( j == bz );
          }
        }
      }
    }
  }
}

void testRectGrids()
{
  const float step = 0.05f;

  AliHLTTPCCAGrid grid;
  for ( float sy = 0.1f; sy < 20.0f; sy *= 2.0f ) {
    for ( float sz = 0.1f; sz < 20.0f; sz *= 2.0f ) {
      grid.Create( 0.f, 1.f, 0.f, 1.f, sy, sz );

      for ( unsigned int i = 0; i < 10; ++i ) {
        for ( float y = i * sy; y < ( i+1 ) * sy; y += step ) {
          for ( float z = 0.0f; z < sz; z += step ) {
            VERIFY( i == grid.GetBinNoCheck( y, z ) );
          }
        }
      }
    }
  }
}

int main()
{
  runTest( testTrivialSquareGrid );
  runTest( test2by2Grid );
  runTest( testSquareGrids );
  runTest( testRectGrids );
  return 0;
}
