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
#include <AliHLTTPCCAHitArea.h>
#include <AliHLTTPCCARow.h>
#include <AliHLTTPCCAHit.h>
#include <AliHLTTPCCAGrid.h>

static unsigned short content[100];
static ushort2 hitData[25];
static AliHLTTPCCARow row[3];

void testTrivialArea()
{
  AliHLTTPCCAHitArea area;
  area.Init( row[0].fGrid, content, 0, 0.0f, 0.0f, 0.1f, 0.1f );

  AliHLTTPCCAHit hit;
  while ( -1 != area.GetNext( hitData, row[0], content, &hit ) ) {
    VERIFY( 0 == 0 );
  }
}

int main()
{
  for ( unsigned short i = 0; i < 100; ++i ) {
    content[i] = i;
    hitData[i].x = i;
    hitData[i].y = i;
  }

  AliHLTTPCCAGrid grid;
  grid.Create( 0.0f, 1.0f, 0.0f, 1.0f, 9 );
  row[0].fGrid = grid;

  runTest( testTrivialArea );
  return 0;
}
