//-*- Mode: C++ -*-
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************

#ifndef ALIHLTTPCCAINPUTDATA_H
#define ALIHLTTPCCAINPUTDATA_H

//#include "AliHLTTPCCADef.h"
#include "AliHLTTPCCATrackParam.h"
#include "AliHLTArray.h"
#include "AliHLTTPCCAGBHit.h"
#include "AliHLTTPCCAGBTrack.h"
#include "AliHLTTPCCATracker.h"
#include <AliHLTTPCCAInputSettings.h>


#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
using std::string;

class AliHLTTPCCAMerger;

/**
 * @class AliHLTTPCCAInputData
 *
 * Global Cellular Automaton-based HLT tracker for TPC detector
 * The class reconstructs tracks in the whole TPC
 * It calls the AliHLTTPCCATracker slice tracker and constructs
 * the global TPC tracks by merging the slice tracks
 *
 * The tracker is designed stand-alone.
 * It will be integrated to the HLT framework via AliHLTTPCCAInputDataComponent interface,
 * and to off-line framework via TPC/AliTPCtrackerCA class
 * The class is under construction.
 *
 */
class AliHLTTPCCAInputData
{

  public:
    AliHLTTPCCAInputData();
    ~AliHLTTPCCAInputData();
    void Init();

    void StartEvent();
    void SetNHits( int nHits );

    const AliHLTTPCCAGBHit *Hits() const { return fHits.Data(); }
    int NHits() const { return fNHits; }

    void ReadEvent( FILE *in );

    bool ReadHitsFromFile( string prefix );

    void SetHits( std::vector<AliHLTTPCCAGBHit> &hits);     // need for StRoot
    void SetHits( const AliHLTTPCCAGBHit *hits, int nHits );
    int  GetHitsSize() const {return fHits.Size();}

      /// Try to group close hits in row formed by one track. After sort hits.
//     void GroupHits(); // iklm
  protected:

    int fNHits;                //* N hits in event
    AliHLTResizableArray<AliHLTTPCCAGBHit> fHits;     //* hit array

  public:
    AliHLTTPCCAInputData( const AliHLTTPCCAInputData& a);
    const AliHLTTPCCAInputData& operator=( const AliHLTTPCCAInputData& a);
};

struct InputDataArray{
  InputDataArray():fInput(0){};
  ~InputDataArray() { if(fInput) delete [] fInput; if(fSettings) delete fSettings; }

  AliHLTTPCCAInputSettings *fSettings;
  AliHLTTPCCAInputData *fInput;
};

#endif
