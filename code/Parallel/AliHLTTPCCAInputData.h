/*
 * This file is part of TPCCATracker package
 * Copyright (C) 2007-2020 FIAS Frankfurt Institute for Advanced Studies
 *               2007-2020 Goethe University of Frankfurt
 *               2007-2020 Ivan Kisel <I.Kisel@compeng.uni-frankfurt.de>
 *               2007-2019 Sergey Gorbunov
 *               2007-2019 Maksym Zyzak
 *               2007-2014 Igor Kulakov
 *
 * TPCCATracker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TPCCATracker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
