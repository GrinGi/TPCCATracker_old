/*
    Copyright (C) 2009 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) version 3.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include "AliHLTTPCCAGBTracker.h"
#include "AliHLTTPCCATracker.h"
#include "AliHLTTPCCAPerformance.h"
#include "AliHLTTPCCADisplay.h"

#include "TApplication.h"

#include <fstream>
#include <iostream>

#include <cstdlib>
#include <cstdio>

#ifdef DRAW
#error "Do not -DDRAW"
#endif

static void readMcEvent( Int_t eventIndex, AliHLTTPCCAPerformance *perf )
{
  char name[32];
  sprintf( name, "Events/%i.mcevent.dat", eventIndex );
  std::fstream mcevent;
  mcevent.open(name, std::ios::in);
  if( !mcevent.is_open() ) {
    std::exit( 1 );
  }
  perf->ReadMCEvent(mcevent);
  mcevent.close();
}

static bool readBinaryStore( Int_t eventIndex, AliHLTTPCCAGBTracker *tracker )
{
  char name[32];
  sprintf( name, "%i.dat", eventIndex );
  FILE *f = std::fopen( name, "rb" );
  if ( f == NULL ) {
    return false;
  }
  tracker->RestoreFromFile( f );
  std::fclose( f );
  return true;
}

static void drawSlice( AliHLTTPCCADisplay &display, AliHLTTPCCATracker &slice )
{
  display.ClearView();
  display.SetCurrentSlice( &slice );
  display.DrawSlice( &slice );
  display.DrawSliceHits();
  display.Ask();
  for ( int trackIndex = 0; trackIndex < *slice.NOutTracks(); ++trackIndex ) {
    display.DrawSliceOutTrack( trackIndex );
  }
  display.Ask();
}

int main( int argc, char **argv )
{
  int eventNumber = 0;
  int sliceNumber = -1;

  TApplication app( "CA Display", 0, 0 ); //, &argc, argv );
  //app.Run();

  if ( argc > 1 ) {
    eventNumber = atoi( argv[1] );
    if ( argc > 2 ) {
      sliceNumber = atoi( argv[2] );
    }
  }

  AliHLTTPCCAGBTracker tracker;
  if ( !readBinaryStore( eventNumber, &tracker ) ) {
    std::cerr << eventNumber << ".dat does not exist. Run CA first." << std::endl;
    return 1;
  }

  AliHLTTPCCAPerformance &perf = AliHLTTPCCAPerformance::Instance();
  perf.SetTracker( &tracker );

  readMcEvent( eventNumber, &perf );

  AliHLTTPCCADisplay display;
  display.Init();
  //display.SetPerformance( &perf );
  display.SetGB( &tracker );

  if ( sliceNumber < 0 ) {
    display.SetTPCView();
    display.DrawTPC();
    display.DrawGBHits( tracker );
    display.Ask();
    for ( int trackIndex = 0; trackIndex < tracker.NTracks(); ++trackIndex ) {
      display.DrawGBTrack( trackIndex );
    }
    display.Ask();

    display.SetSliceView();

    for ( int i = 0; i < tracker.NSlices(); ++i ) {
      if ( tracker.Slice( i ).Data().NumberOfHits() > 0 ) {
        drawSlice( display, tracker.Slices()[i] );
      }
    }
  } else {
    display.SetSliceView();
    drawSlice( display, tracker.Slices()[sliceNumber] );
  }

  return 0;
}
