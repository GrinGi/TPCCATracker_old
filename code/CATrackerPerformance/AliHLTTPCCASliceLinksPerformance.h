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
#if 1//def DO_TPCCATRACKER_EFF_PERFORMANCE

#ifndef ALIHLTTPCCASLICELINKSPERFORMANCE_H
#define ALIHLTTPCCASLICELINKSPERFORMANCE_H


#include "AliHLTTPCCASlicePerformance.h"

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

struct PerfoTrack
{
  vector<int> hits; // index of hit in global array
};

/**
 * @class AliHLTTPCCASliceLinksPerformance
 */
class AliHLTTPCCASliceLinksPerformance: public AliHLTTPCCASlicePerformance
{
 public:

  AliHLTTPCCASliceLinksPerformance(int iSlice):AliHLTTPCCASlicePerformance(iSlice){};
  virtual ~AliHLTTPCCASliceLinksPerformance(){};

  virtual void SetNewEvent(const AliHLTTPCCAGBTracker * const Tracker,
                           AliHLTResizableArray<AliHLTTPCCAHitLabel> *hitLabels,
                           AliHLTResizableArray<AliHLTTPCCAMCTrack> *mcTracks,
                           AliHLTResizableArray<AliHLTTPCCALocalMCPoint> *localMCPoints);


    /// Execute standart chain of procedures
  virtual void Exec(bool print = false);
  
    /// Efficiency
    // Find reco-MCTracks correspondence
  virtual void MatchTracks();   // fill recoData.

    /// Histograms
  // virtual void CreateHistos(string histoDir = "", TFile* outFile = 0);

#ifndef HLTCA_STANDALONE
  virtual void Draw(); // draw links and MC tracks
#endif
  
 protected:
  
#ifndef HLTCA_STANDALONE
  virtual void FillHistos();
#endif

  //   /// Histos
  // enum{ 
  //   NGhostsHisto = 2,
  //   NGhosts2DHisto = 1,
        
  //   NRecoTracksHisto = 2,
  //   NRecoTracks2DHisto = 1
  // };
  
  friend class AliHLTTPCCASlicesLinksPerformance;
 private:
    /// create tracks from linked hits chains
  void CollectTracks();

  vector<PerfoTrack> fRecoTracks;

};

#endif
#endif //DO_TPCCATRACKER_EFF_PERFORMANCE
