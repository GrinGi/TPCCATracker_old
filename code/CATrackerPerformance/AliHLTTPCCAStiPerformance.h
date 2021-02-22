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
#if 1//def DO_TPCCATRACKER_EFF_PERFORMANCE

#ifndef AliHLTTPCCAStiPerformance_H
#define AliHLTTPCCAStiPerformance_H


#include "AliHLTTPCCATrackPerformanceBase.h"

#include "AliHLTTPCCAOutTrack.h"

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
class AliHLTTPCCAGBTrack;
class TDirectory;
class TH1D;
class TH2D;
class TProfile;

class TFile;
class AliHLTTPCCATracker;

/**
 * @class AliHLTTPCCAStiPerformance
 */
class AliHLTTPCCAStiPerformance: public AliHLTTPCCATrackPerformanceBase // TODO public GlobalPerfo
{
  public:

    AliHLTTPCCAStiPerformance(){ };
    virtual ~AliHLTTPCCAStiPerformance(){};

    virtual void SetNewEvent(const AliHLTTPCCAGBTracker * const tracker,
                             AliHLTResizableArray<AliHLTTPCCAHitLabel> *hitLabels,
                             AliHLTResizableArray<AliHLTTPCCAMCTrack> *mcTracks,
                             AliHLTResizableArray<AliHLTTPCCALocalMCPoint> *localMCPoints);
  void SetTracker(const AliHLTTPCCAGBTracker * const tracker) { fTracker = tracker;};
  
      /// Efficiency
      // Check if MC track is reconstructable. Calculate set of MC track. Etc.
    virtual void CheckMCTracks(); // fill mcData.
      // Find reco-MCTracks correspondence
    virtual void MatchTracks();   // fill recoData.
      // Calculate efficiencies
    virtual void EfficiencyPerformance();

 #ifndef HLTCA_STANDALONE
      /// Histograms
    virtual void FillHistos();
  
    virtual void Draw(); // draw diff things after performance
#endif
  
    int NTracks() {return Tracks.size();}
    void SetRecoTracks(vector<AliHLTTPCCAGBTrack> &Trs_){Tracks.clear(); for (unsigned i = 0; i < Trs_.size(); i++) Tracks.push_back(Trs_[i]); }
    void AddHit  (int &Hi) {HitIndex.push_back(Hi);}

    
  private:
    int fISlice;
    int firstSliceHit, endSliceHit;
    vector<AliHLTTPCCAGBTrack> Tracks;
    vector<int> HitIndex;
};

#endif
#endif //DO_TPCCATRACKER_EFF_PERFORMANCE
