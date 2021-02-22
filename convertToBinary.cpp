#define HLTCA_STANDALONE
#include <AliHLTTPCCAGBHit.h>
#include <AliHLTTPCCAMCTrack.h>
#include <AliHLTTPCCAPerformance.h>
#include <AliHLTArray.h>
#include <fstream>
#include <cstdio>

int main()
{
  char name[255];
  for ( int i = 0; i < 100; ++i ) {
#ifdef HIT_CONVERT
    sprintf( name, "Events/%i.event.dat", i );
#else
    sprintf( name, "Events/%i.mcevent.dat", i );
#endif
    std::fstream file;
    file.open( name, std::ios::in );
    if ( !file.is_open() ) {
      return 1;
    }

#ifdef HIT_CONVERT
    int nHits;
    file >> nHits;
    AliHLTResizableArray<AliHLTTPCCAGBHit> sliceData( nHits );
    int hitIndex = 0;
    AliHLTTPCCAGBHit hit;
    hit.SetIsUsed( 0 );
    for ( int j = 0; j < nHits; ++j ) {
      float tmp;
      int itmp;
      file >> tmp; hit.SetX( tmp );
      file >> tmp; hit.SetY( tmp );
      file >> tmp; hit.SetZ( tmp );
      file >> tmp; hit.SetErrY( tmp );
      file >> tmp; hit.SetErrZ( tmp );
      file >> tmp; hit.SetAmp( tmp );
      file >> itmp; hit.SetID( itmp );
      file >> itmp; hit.SetISlice( itmp );
      file >> itmp; hit.SetIRow( itmp );
      sliceData[hitIndex++] = hit;
    }
#else
    int n1, n2;
    file >> n1;
    AliHLTResizableArray<AliHLTTPCCAMCTrack> fMCTracks( n1 );
    for ( int j = 0; j < n1; ++j ) {
      AliHLTTPCCAMCTrack &t = fMCTracks[j];
      int k;
      float f;
      file >> k;
      file >> k; t.SetPDG( k );
      for ( int l = 0; l < 7; l++ ) { file >> f; t.SetPar( l, f );}
      for ( int l = 0; l < 7; l++ ) { file >> f; t.SetTPCPar( l, f );}
      file >> f; t.SetP( f );
      file >> f; t.SetPt( f );
      file >> k; t.SetNHits( k );
      file >> k; t.SetNMCPoints( k );
      file >> k; t.SetFirstMCPointID( k );
      file >> k; t.SetNReconstructed( k );
      file >> k; t.SetSet( k );
      file >> k; t.SetNTurns( k );
    }
    file >> n2;
    AliHLTResizableArray<AliHLTTPCCAPerformance::AliHLTTPCCAHitLabel> fHitLabels( n2 );
    for ( int j = 0; j < n2; ++j ) {
      AliHLTTPCCAPerformance::AliHLTTPCCAHitLabel &l = fHitLabels[j];
      file >> l.fLab[0] >> l.fLab[1] >> l.fLab[2];
    }
#endif
    file.close();

#ifdef HIT_CONVERT
    sprintf( name, "Events/%i.bin", i );
#else
    sprintf( name, "Events/%i.mc.bin", i );
#endif
    FILE *f = std::fopen( name, "wb" );
#ifdef HIT_CONVERT
    std::fwrite( &nHits, sizeof( int ), 1, f );
    std::fwrite( sliceData.Data(), sizeof( AliHLTTPCCAGBHit ), nHits, f );
#else
    std::fwrite( &n1, sizeof( int ), 1, f );
    std::fwrite( &n2, sizeof( int ), 1, f );
    std::fwrite( fMCTracks.Data(), sizeof( AliHLTTPCCAMCTrack ), fMCTracks.Size(), f );
    std::fwrite( fHitLabels.Data(), sizeof( AliHLTTPCCAPerformance::AliHLTTPCCAHitLabel ), fHitLabels.Size(), f );
#endif
    std::fflush( f );
    std::fclose( f );
  }
}
