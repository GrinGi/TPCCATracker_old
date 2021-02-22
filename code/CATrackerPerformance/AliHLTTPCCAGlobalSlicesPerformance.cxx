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

#include "AliHLTTPCCounters.h"

#include "AliHLTTPCCATrackPerformanceBase.h"
#include "AliHLTTPCCAGlobalSlicesPerformance.h"


#include "AliHLTTPCCAGBHit.h"
#include "AliHLTTPCCAMCTrack.h"
#include "AliHLTTPCCAMCPoint.h"
#include "AliHLTTPCCAOutTrack.h"
#include "AliHLTTPCCAGBTrack.h"
#include "AliHLTTPCCAGBTracker.h"

#include "AliHLTTPCCATracker.h"

#ifndef HLTCA_STANDALONE
#include "AliHLTTPCCADisplay.h"

#include "TMath.h"
#include "TROOT.h"
#include "Riostream.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TStyle.h"
#endif

void AliHLTTPCCAGlobalSlicesPerformance::SetNewEvent(const AliHLTTPCCAGBTracker * const tracker,
                            AliHLTResizableArray<AliHLTTPCCAHitLabel> *hitLabels,
                            AliHLTResizableArray<AliHLTTPCCAMCTrack> *mcTracks,
                            AliHLTResizableArray<AliHLTTPCCALocalMCPoint> *localMCPoints)
{
  AliHLTTPCCATrackPerformanceBase::SetNewEvent(tracker, hitLabels, mcTracks, localMCPoints);

    /// Init subperformances
//   static bool first_call = true;
  if (first_call){
    if(fTracker)
      slicePerformances.resize(fTracker->NSlices());
    else
      slicePerformances.resize(0);
    for (unsigned int iPerf = 0; iPerf < slicePerformances.size(); iPerf++){
      slicePerformances[iPerf] = new AliHLTTPCCASlicePerformance(iPerf);
    }
  }
  first_call = false;
  
  for (unsigned int iPerf = 0; iPerf < slicePerformances.size(); iPerf++){
    slicePerformances[iPerf]->SetNewEvent(fTracker, fHitLabels, fMCTracks, fLocalMCPoints);
  }

} // void AliHLTTPCCAGlobalSlicesPerformance::SetNewEvent

#ifndef HLTCA_STANDALONE
void AliHLTTPCCAGlobalSlicesPerformance::CreateHistos(string histoDir, TFile* outFile)
{
  UNUSED_PARAM2(histoDir, outFile);
  // don't use histos. Histos are builded in the SlicesPerformance.

  for (unsigned int iPerf = 0; iPerf < slicePerformances.size(); iPerf++){
    // slicePerformances[iPerf]->CreateHistos( histoDir + (string)TString(char(iPerf)), outFile ); // just set diff names, they anyway won't be written
    slicePerformances[iPerf]->CreateHistos( "" );
  }
}
#endif

void AliHLTTPCCAGlobalSlicesPerformance::Exec(bool print)
{
  for (unsigned int iPerf = 0; iPerf < slicePerformances.size(); iPerf++){
    slicePerformances[iPerf]->Exec(0);
  }
  AliHLTTPCCATrackPerformanceBase::Exec(print);
} // void AliHLTTPCCAGlobalSlicesPerformance::Exec

void AliHLTTPCCAGlobalSlicesPerformance::CheckMCTracks()
{
  mcData.resize(nMCTracks);
    // find reconstructable tracks
  for (unsigned int iPerf = 0; iPerf < slicePerformances.size(); iPerf++){
    for ( int iMCTrack = 0; iMCTrack < nMCTracks; iMCTrack++ ){
 //     if ( (*fMCTracks)[iMCTrack].NMCPoints() >= PParameters::MinimumMCPointsForMCTrack ) 
      if ( (*fMCTracks)[iMCTrack].NMCRows() >= PParameters::MinimumMCPointsForMCTrack ) 
		  mcData[iMCTrack].SetAsReconstructable(); // global defition of reconstractable track
    } // iMCTrack
  } // iSlice
  
    // calc set
  for ( int iMCTrack = 0; iMCTrack < nMCTracks; iMCTrack++ ){
    mcData[iMCTrack].SetSet( slicePerformances[0]->mcData[iMCTrack].GetSet() );
  } // iMCTrack
} // void AliHLTTPCCAGlobalSlicesPerformance::CheckMCTracks()

#define IsOutTrack1
void AliHLTTPCCAGlobalSlicesPerformance::MatchTracks()
{
    // get all reco tracks
  nRecoTracks = 0;
  for (unsigned int iPerf = 0; iPerf < slicePerformances.size(); iPerf++){
    vector<AliHLTTPCCAPerformanceRecoTrackData> &rTrs = slicePerformances[iPerf]->recoData;
    int nSectorRecoTracks = rTrs.size();
    for ( int itr = 0; itr < nSectorRecoTracks; itr++ ) {
      recoData.push_back(rTrs[itr]);
    }
    nRecoTracks += nSectorRecoTracks;
  } // iSlice
    // find reconstructed tracks
  for ( int itr = 0; itr < nRecoTracks; itr++ ) {
    AliHLTTPCCAPerformanceRecoTrackData &rTr = recoData[itr];
    if ( rTr.IsReco( SPParameters::MinTrackPurity, SPParameters::MinimumHitsForRecoTrack) ) mcData[rTr.GetMCTrackId()].AddReconstructed();
  }
}


void AliHLTTPCCAGlobalSlicesPerformance::EfficiencyPerformance()
{
  for ( int iRTr = 0; iRTr < nRecoTracks; iRTr++ ) {
    if (  recoData[iRTr].IsGhost( PParameters::MinTrackPurity ) )
      fEff.ghosts++;
  }

  for ( int iMCTr = 0; iMCTr < nMCTracks; iMCTr++ ) {
    AliHLTTPCCAPerformanceMCTrackData &mc = mcData[iMCTr];
    if ( !mc.IsReconstructable() ) continue;
    const bool reco = mc.IsReconstructed();
    const int clones = mc.GetNClones();

    if ( mc.GetSet() == 0){ // rest, out track
      fEff.Inc(reco,clones,"rest");
    }
    else{ // good
      fEff.Inc(reco,clones,"total");
      if ( mc.GetSet() == 1){
        fEff.Inc(reco,clones,"extra");
      }
      else if ( mc.GetSet() == 2 ) {
        fEff.Inc(reco,clones,"ref");
      }
      else {
        fEff.Inc(reco,clones,"ref");
        fEff.Inc(reco,clones,"long_ref");
      }
    }
  } // for iMCTr

  AliHLTTPCCATrackPerformanceBase::EfficiencyPerformance();
} // void AliHLTTPCCAGlobalSlicesPerformance::EfficiencyPerformance()
#endif //DO_TPCCATRACKER_EFF_PERFORMANCE

