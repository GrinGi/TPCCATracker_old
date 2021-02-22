/*
 * This file is part of TPCCATracker package
 * Copyright (C) 2007-2020 FIAS Frankfurt Institute for Advanced Studies
 *               2007-2020 Goethe University of Frankfurt
 *               2007-2020 Ivan Kisel <I.Kisel@compeng.uni-frankfurt.de>
 *               2007-2019 Sergey Gorbunov
 *               2007-2019 Maksym Zyzak
 *               2007-2014 Igor Kulakov
 *               2014-2020 Grigory Kozlov
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

#ifndef ALIHLTTPCCAGLOBALPERFORMANCE_H
#define ALIHLTTPCCAGLOBALPERFORMANCE_H


#include "AliHLTTPCCATrackPerformanceBase.h"

#include "AliHLTArray.h"
#include "AliHLTTPCCAMCTrack.h"
#include "AliHLTTPCCAMCPoint.h"
#include <fstream>
#include <cstdio>
#include <map>

class TObject;
class TParticle;
class AliHLTTPCCAMCPoint;
class AliHLTTPCCAGBTracker;
class TDirectory;
class TH1D;
class TH2D;
class TProfile;

class TFile;
class AliHLTTPCCATracker;

//class AliHLTTPCCADisplay;

#define IsOutTrack1

/**
 * @class AliHLTTPCCAGlobalPerformance
 */
class AliHLTTPCCAGlobalPerformance: public AliHLTTPCCATrackPerformanceBase
{
  public:
  
    AliHLTTPCCAGlobalPerformance(){ };
    virtual ~AliHLTTPCCAGlobalPerformance(){};

    virtual void SetNewEvent(const AliHLTTPCCAGBTracker * const Tracker,
                             AliHLTResizableArray<AliHLTTPCCAHitLabel> *hitLabels,
                             AliHLTResizableArray<AliHLTTPCCAMCTrack> *mcTracks,
                             AliHLTResizableArray<AliHLTTPCCALocalMCPoint> *localMCPoints);
    
      /// Efficiency
      // Check if MC track is reconstructable. Calculate set of MC track. Etc.
    virtual void CheckMCTracks(); // fill mcData.
      // Find reco-MCTracks correspondence
    virtual void MatchTracks();   // fill recoData.
      // Calculate efficiencies
    virtual void EfficiencyPerformance();

#ifndef HLTCA_STANDALONE
    virtual void Draw();
#if 0
    void DrawLooperFull( int mcTr, AliHLTTPCCADisplay &disp );
#endif
      /// Histograms
//     virtual void CreateHistos(string histoDir);
    virtual void FillHistos();
#endif

//#define LOOPER_TEST
//#ifdef LOOPER_TEST
//  public:
    struct LooperSegment {
      float iTr;
      float QPt_abs;
      float Cx, Cy, Cr;		// Circle x, y, r
      float x_up, y_up, z_up;
      float x_dn, y_dn, z_dn;
      float x_h_up, y_h_up, z_h_up;
      float x_h_dn, y_h_dn, z_h_dn;
      float h;
      int slice_mid;
      int iLooper;
      int iMC;
      bool isUsed;
    };

    void StartLooperTest()
    {
      fSegments.clear();
      fNLoopers = 0;
    }

    void FillSegments();
    void CheckSegments();
    //
  private:
    vector<LooperSegment> fSegments;
    int fNLoopers;
    //
//#endif
};

#endif
