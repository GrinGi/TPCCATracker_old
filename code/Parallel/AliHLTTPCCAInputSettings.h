//-*- Mode: C++ -*-
// ************************************************************************
// This file is property of and copyright by the ALICE HLT Project        *
// ALICE Experiment at CERN, All rights reserved.                         *
// See cxx source for full Copyright notice                               *
//                                                                        *
//*************************************************************************

#ifndef ALIHLTTPCCAINPUTSETTINGS_H
#define ALIHLTTPCCAINPUTSETTINGS_H

//#include "AliHLTTPCCADef.h"
#include "AliHLTTPCCATrackParam.h"
#include "AliHLTArray.h"
#include "AliHLTTPCCAGBHit.h"
#include "AliHLTTPCCAGBTrack.h"
#include "AliHLTTPCCATracker.h"

#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
using std::string;

class AliHLTTPCCAMerger;

/**
 * @class AliHLTTPCCAInputSettings
 *
 * Global Cellular Automaton-based HLT tracker for TPC detector
 * The class reconstructs tracks in the whole TPC
 * It calls the AliHLTTPCCATracker slice tracker and constructs
 * the global TPC tracks by merging the slice tracks
 *
 * The tracker is designed stand-alone.
 * It will be integrated to the HLT framework via AliHLTTPCCAInputSettingsComponent interface,
 * and to off-line framework via TPC/AliTPCtrackerCA class
 * The class is under construction.
 *
 */
class AliHLTTPCCAInputSettings
{

  public:
    AliHLTTPCCAInputSettings();
    ~AliHLTTPCCAInputSettings();
    void Init();

    void StartEvent();
    void SetNSlices( int N );

    const std::vector<AliHLTTPCCAParam>& GetSettings() const { return fSettings; }
    int NSlices() const { return fNSlices; }

    void ReadSettings( std::istream &in );
    bool ReadSettingsFromFile( string prefix );

    void SetSettings( const std::vector<AliHLTTPCCAParam>& settings ); // need for StRoot

  protected:

    int fNSlices;              //* N slices
    std::vector<AliHLTTPCCAParam> fSettings;

  public:
    AliHLTTPCCAInputSettings( const AliHLTTPCCAInputSettings& a);
    const AliHLTTPCCAInputSettings& operator=( const AliHLTTPCCAInputSettings& a);
};

#endif
