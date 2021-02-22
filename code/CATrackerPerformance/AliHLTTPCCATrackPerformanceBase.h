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

#ifndef ALIHLTTPCCATrackPERFORMANCEBASE_H
#define ALIHLTTPCCATrackPERFORMANCEBASE_H

#include "AliHLTTPCCounters.h"
#include <algorithm>
using std::sort;

#include "AliHLTTPCPerformanceBase.h"

#include "AliHLTArray.h"
#include "AliHLTTPCCAMCTrack.h"
#include "AliHLTTPCCAMCPoint.h"
#include <fstream>
#include <cstdio>
#include <map>

#include <string>
using std::string;

#include <iostream>
using std::ostream;
using std::istream;

class TObject;
class TParticle;
class AliHLTTPCCAMCPoint;
class AliHLTTPCCAGBTracker;
class TDirectory;
class TH1;

class TFile;

/**
 * @class AliHLTTPCCATrackPerformanceBase
 */
class AliHLTTPCCATrackPerformanceBase: public AliHLTTPCPerformanceBase
{
 public:

  AliHLTTPCCATrackPerformanceBase();
  virtual ~AliHLTTPCCATrackPerformanceBase(){};

#ifndef HLTCA_STANDALONE
    /// Histograms
  virtual void CreateHistos(string histoDir = "", TFile* outFile = 0);

 protected:

  virtual void FillHistos();


    /// Histos
  enum{ 
    NTracksPulls = 10,
    NHitsPulls = 5+2,
    NHits2DPulls = 3+3+3,

    NAllHisto = 2,
    NAllProfiles = 4,
    NAll2DHisto = 1,
    
    NGhostsHisto = 5,
    NGhostsProfiles = 0,
    NGhosts2DHisto = 2,
        
    NRecoTracksHisto = 8,
    NRecoTracksProfiles = 14,
    NRecoTracks2DHisto = 3,

    NRecoMCToHits2D = 1
  };
  
  //Histos names. Should be consistent with the list in constructor
  enum{
    kresY, kresZ, kresSinPhi, kresDzDs, kresPt, 
    kpullY, kpullZ, kpullSinPhi, kpullDzDs, kpullQPt,
    
    kresXHit, kresYHit, kresZHit, kpullYHit, kpullZHit, 
    
    kxMCPoint, krMCPoint, 
    
    kresXHitVsZ, kresYHitVsZ, kresZHitVsZ, kresXHitVsX, kresYHitVsX, 
    kresZHitVsX, kresXHitVsIS, kresYHitVsIS, kresZHitVsIS,

    kpurity, kmcTrackNRecoHits, knHitsOverNMCPointsVsRow, knHitsOverNMCPointsVsMCMom, 
    knHitsOverNMCPointsVsMCDzDs, knHitsOverNMCPointsVsNMCTracks, knMCPointsVsMCMom,
    
    kghostsLength, kghostsRMom, kghostsRPt, kghostsChi2, kghostsProb,
    kghostsLengthAndRMom, kghostsLengthAndChi2,

    krecosLength, krecosRMom, krecosMCMom, krecosRPt, krecosMCPt, 
    krecosChi2, krecosProb, knHitsRecoTOverNHitsMCT, krecosEffVsMCNHits, 
    krecosEffVsMCMom, krecosEffVsMCMomPrim, krecosEffVsMCMomRefPrim,
    krecosEffVsMCMomSec, krecosEffVsMCPtAll, krecosEffVsMCPtPrim,  
    krecosEffVsMCPtRefPrim, krecosEffVsMCPtSec, krecosEffVsMCPhiAll, krecosEffVsMCPhiPrim, 
    krecosEffVsMCPhiRefPrim, krecosEffVsMCPhiSec, krecosEffVsNMCTracks, 
    krecosLengthAndRMom, krecosLengthAndMCMom, krecosLengthAndChi2,

    krecoMCToHits2D
  };

#endif
};

#endif
#endif //DO_TPCCATRACKER_EFF_PERFORMANCE
