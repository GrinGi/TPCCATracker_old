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
#include "AliHLTTPCCAGlobalPerformance.h"

#include "AliHLTTPCCADef.h"

#include "AliHLTTPCCAGBHit.h"
#include "AliHLTTPCCAMCTrack.h"
#include "AliHLTTPCCAMCPoint.h"
#include "AliHLTTPCCAOutTrack.h"
#include "AliHLTTPCCAGBTrack.h"
#include "AliHLTTPCCAGBTracker.h"

#include "AliHLTTPCCATracker.h"

#ifndef HLTCA_STANDALONE
#ifdef MAIN_DRAW
#define DRAW_GLOBALPERF
#endif

#include "TMath.h"
#include "TROOT.h"
#include "Riostream.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TStyle.h"
#endif // HLTCA_STANDALONE

#ifdef DRAW_GLOBALPERF
#include "AliHLTTPCCADisplay.h"
#include "AliHLTTPCCAPerformance.h"
#endif


// #include "TParticlePDG.h"
// #include "TDatabasePDG.h"

void AliHLTTPCCAGlobalPerformance::SetNewEvent(const AliHLTTPCCAGBTracker * const tracker,
                            AliHLTResizableArray<AliHLTTPCCAHitLabel> *hitLabels,
                            AliHLTResizableArray<AliHLTTPCCAMCTrack> *mcTracks,
                            AliHLTResizableArray<AliHLTTPCCALocalMCPoint> *localMCPoints)
{
  AliHLTTPCCATrackPerformanceBase::SetNewEvent(tracker, hitLabels, mcTracks, localMCPoints);

  if(fTracker)
    nRecoTracks = fTracker->NTracks();
  else
    nRecoTracks = 0;


} // void AliHLTTPCCAGlobalPerformance::SetNewEvent

void AliHLTTPCCAGlobalPerformance::CheckMCTracks()
{
  for ( int imc = 0; imc < nMCTracks; imc++ ) (*fMCTracks)[imc].SetNHits( 0 );

  mcData.resize(nMCTracks);
  for ( int imc = 0; imc < nMCTracks; imc++ ) {
    AliHLTTPCCAMCTrack &mc = (*fMCTracks)[imc];
    AliHLTTPCCAPerformanceMCTrackData &mcTrackData = mcData[imc];
    // ---
    std::map<int,int> rowCounter;
    rowCounter.clear();
    int nHits = fTracker->NHits();
    int nTrHits = 0;
    for ( int ih = 0; ih < nHits; ih++ ) {
      const AliHLTTPCCAGBHit &hit = fTracker->Hit( ih );
      const AliHLTTPCCAHitLabel &l = (*fHitLabels)[hit.ID()];
      const int iMC = (*fLocalMCPoints)[mc.FirstMCPointID()].TrackI();
      if ( l.fLab[0] != iMC && l.fLab[1] != iMC && l.fLab[2] != iMC )
	continue;

      nTrHits++;
      rowCounter.insert( std::pair<int,int>( hit.IRow(), ih ) );
    }
    mc.SetNHits( nTrHits );
    int tmpnr = mc.NMCRows();
    mc.SetNMCRows( rowCounter.size() );
    mc.SetSet( 0 );
    mc.SetNReconstructed( 0 );
    mc.SetNTurns( 1 );
    if( mc.NMCPoints() > 72 || mc.NHits() > 72 ) continue;	// Loopers cut
    // ---
    if ( mc.NHits() >= 4 && mc.NHits() < 15 ) {
	mcTrackData.SetAsReconstructable_mc4_15_r4();
    }
    else {
	mcTrackData.SetAsNonReconstructable_mc4_15_r4();
    }

    if ( mc.NHits() >= 4 ) {
	mcTrackData.SetAsReconstructable_mc4_r4();
    }
    else {
	mcTrackData.SetAsNonReconstructable_mc4_r4();
    }

    mcTrackData.SetSet_mc4_15_r4( 0 );
    if ( mc.NHits() >= 4 && mc.NHits() < 15 ) {
      if ( mc.P() >= AliHLTTPCCAParameters::ExtraThreshold ) {
        if ( mc.P() >= AliHLTTPCCAParameters::RefThreshold ) {
          mcTrackData.SetSet_mc4_15_r4( 2 );
          if ( tmpnr >= fTracker->Slice(0).Param().NRows() ) {
            mcTrackData.SetSet_mc4_15_r4( 3 );
          }
        }
        else{
          mcTrackData.SetSet_mc4_15_r4( 1 );
        }
      }
    }

    mcTrackData.SetSet_mc4_r4( 0 );
    if ( mc.NHits() >= 4 ) {
      if ( mc.P() >= AliHLTTPCCAParameters::ExtraThreshold ) {
        if ( mc.P() >= AliHLTTPCCAParameters::RefThreshold ) {
          mcTrackData.SetSet_mc4_r4( 2 );
          if ( tmpnr >= fTracker->Slice(0).Param().NRows() ) {
            mcTrackData.SetSet_mc4_r4( 3 );
          }
        }
        else{
          mcTrackData.SetSet_mc4_r4( 1 );
        }
      }
    }
    // ---
    if ( mc.NHits() >= /*PParameters::MinimumHitsForMCTrack*/10 ){
      mcTrackData.SetAsReconstructable();
//    }

      if ( mc.NMCRows() >= 30 && mc.NHits() <= 72 ) {
        mcTrackData.SetAsReconstructable30();
      }
      else {
        mcTrackData.SetAsNonReconstructable30();
      }

      if ( mc.NMCRows() < 30 && mc.NHits() <= 72 ) {
        mcTrackData.SetAsReconstructable_l30();
        mcTrackData.SetAsReconstructable_l30_r10();
      }
      else {
        mcTrackData.SetAsNonReconstructable_l30();
        mcTrackData.SetAsNonReconstructable_l30_r10();
      }
    // ---
      mc.SetSet( 1 );
      mcTrackData.SetSet( 1 );
    // ---
      if( mc.NHits() <= 20 ) {
//      if( mc.NMCRows() >= 15 && mc.NMCRows() <= 20 ) {
	  mc.SetSet1( 1 );
	  mcTrackData.SetSet1( 1 );
      }
//      if( mc.NMCPoints() > 20 ) {
      if( mc.NHits() > 20 ) {
//    if( mc.NMCRows() > 20 ) {
	  mc.SetSet1( 2 );
	  mcTrackData.SetSet1( 2 );
      }
    // ---

      if ( mc.P() >= AliHLTTPCCAParameters::ExtraThreshold ) {
        if ( mc.P() >= AliHLTTPCCAParameters::RefThreshold ) {
          mc.SetSet( 2 );
          mcTrackData.SetSet( 2 );
//        if ( mc.NMCRows() >= fTracker->Slice(0).Param().NRows() ) {
          if ( tmpnr >= fTracker->Slice(0).Param().NRows() ) {
            mc.SetSet( 3 );
            mcTrackData.SetSet( 3 );
          }
        }
        else{
          mc.SetSet( 1 );
          mcTrackData.SetSet( 1);
        }
      }

      mc.SetSet30( 0 );
      mcTrackData.SetSet30( 0 );
      if ( mc.NMCRows() >= 30 && mc.NHits() <= 72 ) {
        if ( mc.P() >= AliHLTTPCCAParameters::ExtraThreshold ) {
          if ( mc.P() >= AliHLTTPCCAParameters::RefThreshold ) {
            mc.SetSet30( 2 );
            mcTrackData.SetSet30( 2 );
            if ( mc.NMCRows() >= fTracker->Slice(0).Param().NRows() ) {
//        if ( tmpnr >= fTracker->Slice(0).Param().NRows() ) {
              mc.SetSet30( 3 );
              mcTrackData.SetSet30( 3 );
            }
          }
          else{
            mc.SetSet30( 1 );
            mcTrackData.SetSet30( 1 );
          }
        }
      }

      mcTrackData.SetSet_l30( 0 );
      mcTrackData.SetSet_l30_r10( 0 );
      if ( mc.NMCRows() < 30 && mc.NHits() <= 72 ) {
        if ( mc.P() >= AliHLTTPCCAParameters::ExtraThreshold ) {
          if ( mc.P() >= AliHLTTPCCAParameters::RefThreshold ) {
//          mc.SetSet_l30( 2 );
            mcTrackData.SetSet_l30( 2 );
            mcTrackData.SetSet_l30_r10( 2 );
            if ( mc.NMCRows() >= fTracker->Slice(0).Param().NRows() ) {
//        if ( tmpnr >= fTracker->Slice(0).Param().NRows() ) {
//            mc.SetSet30( 3 );
              mcTrackData.SetSet_l30( 3 );
              mcTrackData.SetSet_l30_r10( 3 );
            }
          }
          else{
//          mc.SetSet30( 1 );
            mcTrackData.SetSet_l30( 1 );
            mcTrackData.SetSet_l30_r10( 1 );
          }
        }
      }
    }
  } // for iMC

} // void AliHLTTPCCAGlobalPerformance::CheckMCTracks()

// --- For merging debug and tuning only ---
#include "AliHLTTPCCASliceTrack.h"
#include "AliHLTTPCCASliceOutput.h"
// ---

static float purCut = 0.75;

void AliHLTTPCCAGlobalPerformance::MatchTracks()
{
  std::map<int, float> dzds_map, qpt_map;
  std::map<int, int> loopers_map;
  int mergedMT(0), allMT(0);
  int rst(0);
  recoData.resize(nRecoTracks);
  for ( int itr = 0; itr < nRecoTracks; itr++ ) {
    int traLabels = -1;
    double traPurity = 0;
    const AliHLTTPCCAGBTrack &tCA = fTracker->Tracks()[itr];
    AliHLTTPCCAGBTrack &tCA1 = fTracker->Tracks()[itr];
#ifdef LOOPER_TEST
    tCA1.SetFirstMC(traLabels);
#endif
    tCA1.SetReco( 0 );
    tCA1.SetClone( false );
// --- For merging debug and tuning only ---
//    const AliHLTTPCCATracker& slice = fTracker->Slice(0);
// ---
    const int nhits = tCA.NHits();
//    if( nhits < 10 ) continue;
    int *lb = new int[nhits*3];
    int nla = 0;

    if ( fHitLabels->Size() > 0 )
    for ( int ihit = 0; ihit < nhits; ihit++ ) {
      const int index = fTracker->TrackHit( tCA.FirstHitRef() + ihit );
      const AliHLTTPCCAHitLabel &l = (*fHitLabels)[fTracker->Hit( index ).ID()];
      if ( l.fLab[0] >= 0 ) lb[nla++] = l.fLab[0];
      if ( l.fLab[1] >= 0 ) lb[nla++] = l.fLab[1];
      if ( l.fLab[2] >= 0 ) lb[nla++] = l.fLab[2];
    }
    sort( lb, lb + nla );
    int labmax = -1, labcur = -1, lmax = 0, lcurr = 0;
    for ( int i = 0; i < nla; i++ ) {
      if ( lb[i] != labcur ) {
        if ( labcur >= 0 && lmax < lcurr ) {
          lmax = lcurr;
          labmax = labcur;
        }
        labcur = lb[i];
        lcurr = 0;
      }
      lcurr++;
    }
    if ( labcur >= 0 && lmax < lcurr ) {
      lmax = lcurr;
      labmax = labcur;
    }
    lmax = 0;
    if ( fHitLabels->Size() > 0 )
    for ( int ihit = 0; ihit < nhits; ihit++ ) {
      const int index = fTracker->TrackHit( tCA.FirstHitRef() + ihit );
      const AliHLTTPCCAHitLabel &l = (*fHitLabels)[fTracker->Hit( index ).ID()];
      if ( l.fLab[0] == labmax || l.fLab[1] == labmax || l.fLab[2] == labmax
         ) lmax++;
    }
    traLabels = labmax;
    traPurity = ( ( nhits > 0 ) ? double( lmax ) / double( nhits ) : 0 );
    // ---
    if( traLabels < 0 ) { traPurity = 0; continue; }
    // ---
    const AliHLTTPCCAMCTrack &mct = (*fMCTracks)[traLabels];
    if ( lb ) delete[] lb;

    recoData[itr].SetMCTrack(traLabels, traPurity, nhits);
    if( tCA.IsLooper() ) recoData[itr].SetLooper( tCA.IsLooper() );
#ifdef LOOPER_TEST
    tCA1.SetFirstMC(traLabels);
#endif

    if( (loopers_map.find(traLabels)->second == 1) && !tCA.IsMerged() && mcData[traLabels].IsReconstructed() ) {
      allMT++;
    }
//    if( loopers_map.find(traLabels)->second == 2 ) {
//	allMT--;
//	loopers_map.find(traLabels)->second++;
//    }
//    if( tCA.IsLooper() ) std::cout<<"> itr: "<<itr<<";   looper: "<<tCA.IsLooper()<<";   nHits: "<<nhits<<";   nMCHits: "<<(*fMCTracks)[traLabels].NHits()<<";   purity: "<<traPurity<<"\n";

    if ( /*mcData[traLabels].IsLooper() &&*/ recoData[itr].IsLooper() && recoData[itr].IsReco(0,15) ) {
//      mcData[traLabels].AddReconstructedLooper();
      mcData[traLabels].SetAsLooper();
    }

    if ( /*mcData[traLabels].IsLooper() &&*/ recoData[itr].IsLooper() && recoData[itr].IsReco(0.9,15) ) {
      mcData[traLabels].AddReconstructedLooper();
    }
    if ( mcData[traLabels].IsReconstructable30() && recoData[itr].IsReco(purCut,15) ) {
      mcData[traLabels].AddReconstructed30();
      recoData[itr].SetReconstructed();
      if( mcData[traLabels].GetNClones30() ) recoData[itr].SetClone();
    }

    if ( mcData[traLabels].IsReconstructable_l30() && recoData[itr].IsReco(purCut,15) ) {
      mcData[traLabels].AddReconstructed_l30();
//      recoData[itr].SetReconstructed();
//      if( mcData[traLabels].GetNClones_l30() ) recoData[itr].SetClone();
    }

    if ( mcData[traLabels].IsReconstructable_l30_r10() && recoData[itr].IsReco(purCut,10) ) {
      mcData[traLabels].AddReconstructed_l30_r10();
//      recoData[itr].SetReconstructed();
//      if( mcData[traLabels].GetNClones_l30() ) recoData[itr].SetClone();
    }

    if ( mcData[traLabels].IsReconstructable_mc4_15_r4() && recoData[itr].IsReco(purCut,4) ) {
      mcData[traLabels].AddReconstructed_mc4_15_r4();
    }

    if ( mcData[traLabels].IsReconstructable_mc4_r4() && recoData[itr].IsReco(purCut,4) ) {
      mcData[traLabels].AddReconstructed_mc4_r4();
    }

    if ( recoData[itr].IsReco(0.75,10) ) {
	mcData[traLabels].AddReconstructed1();
    }
    if ( recoData[itr].IsReco(/*PParameters::MinTrackPurity, PParameters::MinimumHitsForRecoTrack*/purCut,10) ) {
	if( !mcData[traLabels].IsReconstructed() && tCA.IsMerged() ){
	    allMT++;
	    mergedMT++;
	}
	mcData[traLabels].AddReconstructed();
	tCA1.SetReco( 1 );
if( mcData[traLabels].NReconstructed() == 1 ) {
    AliHLTTPCCAGBTrack &tCA1 = fTracker->Tracks()[itr];
//    tCA1.SetFirstMC(traLabels);
    mcData[traLabels].SetFirstTrackID(itr);
}
if( (mcData[traLabels].NReconstructed() > 1) && !(recoData[itr].IsGhost(PParameters::MinTrackPurity)) ) {
    AliHLTTPCCAGBTrack &tCA1 = fTracker->Tracks()[itr];
    tCA1.SetClone();
}
    }
    if ( recoData[itr].IsReco(PParameters::MinTrackPurity, PParameters::MinimumHitsForRecoTrack) ) {
	rst++;
    }
  } // for iReco
  if( allMT == 0 ) allMT = 1;
} // void AliHLTTPCCAGlobalPerformance::MatchTracks()


void AliHLTTPCCAGlobalPerformance::EfficiencyPerformance( )
{
  for ( int iRTr = 0; iRTr < nRecoTracks; iRTr++ ) {
#if 0
//    if (  recoData[iRTr].IsGhost(PParameters::MinTrackPurity) )
    if (  !recoData[iRTr].IsReco(0,15) )
      {
	const AliHLTTPCCAGBTrack &tCA = fTracker->Tracks()[iRTr];
	  fEff.ghosts++;
      }
    else
      /*if( recoData[iRTr].GetNHits() > 30 )*/ fEff.IncA( recoData[iRTr].GetNHits(), recoData[iRTr].GetPurity() );
#else

  const AliHLTTPCCAGBTrack &tCA = fTracker->Tracks()[iRTr];
#ifdef LOOPER_TEST
  int mct = tCA.GetFirstMC();
  if( mct < 0 || recoData[iRTr].IsGhost(purCut) ) {
#else
  if( recoData[iRTr].IsGhost(purCut) ) {
#endif
    if( tCA.NHits() >= 10 )
      fEff.ghosts++;
  }
  else {
    fEff.IncA( recoData[iRTr].GetNHits(), recoData[iRTr].GetPurity() );
  }
#endif
  }

  fEff.SetNRecoTracks( nRecoTracks );
  int rbl(0), rtd(0), rr(0);

  int notFound(0), wrongFound(0), found(0);
  for ( int iMCTr = 0; iMCTr < nMCTracks; iMCTr++ ) {
    AliHLTTPCCAPerformanceMCTrackData &mc = mcData[iMCTr];
//    if( mc.IsReconstructable() || mc.IsReconstructed() )
    if( mc.IsReconstructable() && !mc.IsReconstructed() ) notFound++;
    if( !mc.IsReconstructable() && mc.IsReconstructed() ) wrongFound++;
    if( mc.IsReconstructable() && mc.IsReconstructed() ) found++;
    if( mc.IsReconstructable() ) rbl++;
    if( mc.IsReconstructed() ) rtd++;
    if( mc.IsReconstructable() && mc.IsReconstructed() ) rr++;
    if ( !mc.IsReconstructable() ) continue;
    const bool reco = mc.IsReconstructed();
    const int clones = mc.GetNClones();
    // ---
    const int clones1 = mc.GetNClones1();
    const bool reco11 = mc.IsReconstructed1();
    // ---
    const int clones4 = mc.GetNClones_mc4_15_r4();
    const bool reco4 = mc.IsReconstructed_mc4_15_r4();

    const bool reco_all4 = mc.IsReconstructed_mc4_15_r4() || mc.IsReconstructed();
    const int clones_all4 = mc.GetNClones_all4();
    // ---

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
    if ( mc.GetSet() == 0){
	//
    }
    else {
    if( mc.GetSet1() == 1 ) {
	fEff.Inc(reco,clones,"all_15_20");
    }
    if( mc.GetSet1() == 2 ) {
	fEff.Inc(reco,clones,"all_20");
    }
    }

    if ( mc.GetSet() == 0){
	//
    }
    else {
      fEff.Inc(reco11,clones,"all_mc15_r10");
    }
  } // for iMCTr
  // ---
  for ( int iMCTr = 0; iMCTr < nMCTracks; iMCTr++ ) {
    AliHLTTPCCAPerformanceMCTrackData &mc = mcData[iMCTr];
    if ( !mc.IsReconstructable30() ) continue;
    const bool reco = mc.IsReconstructed30();
    const int clones = mc.GetNClones30();
    if ( mc.GetSet30() == 0){ // rest, out track
      //
    }
    else {
      fEff.Inc(reco,clones,"total30");
      if ( mc.GetSet30() == 1){
        fEff.Inc(reco,clones,"extra30");
      }
      else if ( mc.GetSet30() == 2 ) {
        fEff.Inc(reco,clones,"ref30");
      }
      else {
        fEff.Inc(reco,clones,"ref30");
        fEff.Inc(reco,clones,"long_ref30");
      }
    }
  }

  for ( int iMCTr = 0; iMCTr < nMCTracks; iMCTr++ ) {
    AliHLTTPCCAPerformanceMCTrackData &mc = mcData[iMCTr];
    if ( !mc.IsReconstructable_l30() ) continue;
    const bool reco = mc.IsReconstructed_l30();
    const int clones = mc.GetNClones_l30();
    if ( mc.GetSet_l30() == 0){ // rest, out track
      //
    }
    else {
      fEff.Inc(reco,clones,"total_l30");
      if ( mc.GetSet_l30() == 1){
        fEff.Inc(reco,clones,"extra_l30");
      }
      else if ( mc.GetSet_l30() == 2 ) {
        fEff.Inc(reco,clones,"ref_l30");
      }
      else {
        fEff.Inc(reco,clones,"ref_l30");
        fEff.Inc(reco,clones,"long_ref_l30");
      }
    }
  }

  for ( int iMCTr = 0; iMCTr < nMCTracks; iMCTr++ ) {
    AliHLTTPCCAPerformanceMCTrackData &mc = mcData[iMCTr];
    if ( !mc.IsReconstructable_l30_r10() ) continue;
    const bool reco = mc.IsReconstructed_l30_r10();
    const int clones = mc.GetNClones_l30_r10();
    if ( mc.GetSet_l30() == 0){ // rest, out track
      //
    }
    else {
      fEff.Inc(reco,clones,"total_l30_r10");
      if ( mc.GetSet_l30_r10() == 1){
        fEff.Inc(reco,clones,"extra_l30_r10");
      }
      else if ( mc.GetSet_l30_r10() == 2 ) {
        fEff.Inc(reco,clones,"ref_l30_r10");
      }
      else {
        fEff.Inc(reco,clones,"ref_l30_r10");
        fEff.Inc(reco,clones,"long_ref_l30_r10");
      }
    }
  }
  // ---
  for ( int iMCTr = 0; iMCTr < nMCTracks; iMCTr++ ) {
    AliHLTTPCCAPerformanceMCTrackData &mc = mcData[iMCTr];
    if ( !mc.IsReconstructable_mc4_15_r4() ) continue;
    const int clones4 = mc.GetNClones_mc4_15_r4();
    const bool reco4 = mc.IsReconstructed_mc4_15_r4();
    if( mc.GetSet_mc4_15_r4() > 0 ) {
      fEff.Inc(reco4,clones4,"total_4_15");
    }
  }

  for ( int iMCTr = 0; iMCTr < nMCTracks; iMCTr++ ) {
    AliHLTTPCCAPerformanceMCTrackData &mc = mcData[iMCTr];
    if ( !mc.IsReconstructable_mc4_r4() ) continue;
    const int clones4 = mc.GetNClones_mc4_r4();
    const bool reco4 = mc.IsReconstructed_mc4_r4();
    if( mc.GetSet_mc4_r4() > 0 ) {
      fEff.Inc(reco4,clones4,"total_4");
    }
  }

  for ( int iMCTr = 0; iMCTr < nMCTracks; iMCTr++ ) {
    AliHLTTPCCAPerformanceMCTrackData &mc = mcData[iMCTr];
    if ( !mc.IsLooper() ) continue;
    int clones_l = 0;
    if( mc.GetNClonesLooper() ) clones_l = 1;
    const bool reco_l = mc.IsReconstructedLooper();
//    if( mc.GetSet_mc4_r4() > 0 ) {
      fEff.Inc(reco_l,clones_l,"loopers");
//    }
  }
  // ---

  AliHLTTPCCATrackPerformanceBase::EfficiencyPerformance();
} // void AliHLTTPCCAGlobalPerformance::EfficiencyPerformance( )

#ifndef HLTCA_STANDALONE
void AliHLTTPCCAGlobalPerformance::FillHistos()
{
  AliHLTTPCCATrackPerformanceBase::FillHistos();

  const int NMCTracks = (*fMCTracks).Size();
if( !NMCTracks ) return;
  vector<int> mcTrackNRecoHits;
  vector<int> nHitsVsRow;
  vector<int> nMCPointsVsRow;
  const int Multiplicity = (*fMCTracks).Size();
  
  mcTrackNRecoHits.resize(NMCTracks, 0);
  nHitsVsRow.resize(AliHLTTPCCAParameters::MaxNumberOfRows8);
  nMCPointsVsRow.resize(AliHLTTPCCAParameters::MaxNumberOfRows8);
  for(int iH=0; iH < fTracker->NHits(); iH++){
    const AliHLTTPCCAGBHit &hit = fTracker->Hit( iH );
    
    nHitsVsRow[hit.IRow()]++;

    if ( fHitLabels->Size() <= 0 ) continue;
    const AliHLTTPCCAHitLabel &l = (*fHitLabels)[hit.ID()];
if( l.fLab[0] < 0 ) continue;
    if ( l.fLab[0] >= 0 ) mcTrackNRecoHits[l.fLab[0]]++;
    if ( l.fLab[1] >= 0 ) mcTrackNRecoHits[l.fLab[1]]++;
    if ( l.fLab[2] >= 0 ) mcTrackNRecoHits[l.fLab[2]]++;
  }
  for(int i=0; i < NMCTracks; i++){
    AliHLTTPCCAMCTrack &mcT = (*fMCTracks)[i];
    // ---
    if( mcT.Set30() <= 0 ) continue;
if( mcTrackNRecoHits[i] < 30 ) continue;
    // ---
        
    GetHisto(kmcTrackNRecoHits)->Fill( mcTrackNRecoHits[i] );
    GetHisto(knMCPointsVsMCMom)->Fill( mcT.P(), mcT.NMCPoints() );
    
    if ( mcT.NMCPoints() > 0 ) {
      double mcEx = mcT.Px();
      double mcEy = mcT.Py();
      double mcEz = mcT.Pz();
      double mcEt = TMath::Sqrt( mcEx * mcEx + mcEy * mcEy );

      const double dZdS = mcEz/mcEt;

      GetHisto(knHitsOverNMCPointsVsMCMom)->Fill( mcT.P(), float(mcTrackNRecoHits[i])/float(mcT.NMCPoints()) );
      GetHisto(knHitsOverNMCPointsVsMCDzDs)->Fill( dZdS, float(mcTrackNRecoHits[i])/float(mcT.NMCPoints()) );
    }
  }

  for(int i=0; i < (*fLocalMCPoints).Size(); i++){
    nMCPointsVsRow[(*fLocalMCPoints)[i].IRow()]++;
  }
  for(int i=0; i < const_cast<AliHLTTPCCAGBTracker *>(fTracker)->Slice(0).Param().NRows(); i++){
    if ( nMCPointsVsRow[i] > 0 ) {
      GetHisto(knHitsOverNMCPointsVsRow)->Fill( i, float(nHitsVsRow[i])/float(nMCPointsVsRow[i]) );
      GetHisto(knHitsOverNMCPointsVsNMCTracks)->Fill( Multiplicity, float(nHitsVsRow[i])/float(nMCPointsVsRow[i]) );
    }
  }
  
  int recoTrC1(0), recoTrC2(0), ghostTr(0);
  for(int iRTr=0; iRTr < nRecoTracks; iRTr++){  // TODO: make common
    AliHLTTPCCAPerformanceRecoTrackData &recoD = recoData[iRTr];
    // ---
    const AliHLTTPCCAGBTrack &tCA = fTracker->Tracks()[iRTr];
#ifdef LOOPER_TEST
    int mct = tCA.GetFirstMC();
#endif
    AliHLTTPCCAMCTrack &mcTr1 = (*fMCTracks)[ recoD.GetMCTrackId() ];
    if( recoD.IsClone() ) continue;
    recoTrC1++;
    // ---

    const AliHLTTPCCAGBTrack &recoTr = fTracker->Tracks()[iRTr];  // TODO: make common

    double RecoPt  = 1. / fabs(recoTr.InnerParam().QPt());
    double RecoMom = RecoPt * sqrt(1. + recoTr.InnerParam().DzDs()*recoTr.InnerParam().DzDs());
      //     fNVsMom->Fill( param.GetY());
      //     fLengthVsMom->Fill( param.GetY(), t.NHits());

    GetHisto(kpurity)->Fill( recoData[iRTr].GetPurity() );
//    if (  recoD.IsGhost(SPParameters::MinTrackPurity) ) {
    if (  recoD.IsGhost(0) ) {
      GetHisto(kghostsLength)->Fill( recoTr.NHits() );
      GetHisto(kghostsRMom)->Fill( RecoMom );
      GetHisto(kghostsRPt)->Fill( RecoPt );
      GetHisto(kghostsLengthAndRMom)->Fill( recoTr.NHits(), RecoMom );
      GetHisto(kghostsChi2)->Fill( recoTr.InnerParam().GetChi2() );
      GetHisto(kghostsProb)->Fill( TMath::Prob(recoTr.InnerParam().GetChi2(),recoTr.InnerParam().GetNDF()));

      ghostTr++;
    }
    else {
      AliHLTTPCCAMCTrack &mcTr = (*fMCTracks)[ recoD.GetMCTrackId() ];
      recoTrC2++;
      GetHisto(krecosLength)->Fill( recoTr.NHits() );
      GetHisto(krecosRMom)->Fill( RecoMom );
      GetHisto(krecosMCMom)->Fill( mcTr.P() );
      GetHisto(krecosRPt)->Fill( RecoPt );
      GetHisto(krecosMCPt)->Fill( mcTr.Pt() );
      GetHisto(krecosLengthAndMCMom)->Fill( recoTr.NHits() , mcTr.P() );
      GetHisto(krecosLengthAndRMom)->Fill( recoTr.NHits() , RecoMom );
      GetHisto(krecosChi2)->Fill( recoTr.InnerParam().GetChi2() );
      GetHisto(krecosProb)->Fill( TMath::Prob(recoTr.InnerParam().GetChi2(),recoTr.InnerParam().GetNDF()));
      /*if( mcTr.P() > 0.5 ) */GetHisto(knHitsRecoTOverNHitsMCT)->Fill( float(recoTr.NHits()) / mcTrackNRecoHits[recoD.GetMCTrackId()] );

//      GetHisto(krecoMCToHits2D)->Fill( recoTr.NHits(), mcTrackNRecoHits[recoD.GetMCTrackId()] );
      GetHisto(krecoMCToHits2D)->Fill( mcTrackNRecoHits[recoD.GetMCTrackId()], recoTr.NHits() );
    }
  }
//  std::cout<<"___ recoTrC1: "<<recoTrC1<<";   recoTrC2: "<<recoTrC2<<";   ghostTr: "<<ghostTr<<"\n";
  
  // global tracker performance
  {
    int nWrittenTracks = 0;
    for ( int itr = 0; itr < nRecoTracks; itr++ ) {
      const int iMC = recoData[itr].GetMCTrackId();
//      if ( recoData[itr].IsGhost(PParameters::MinTrackPurity) ) continue;
      AliHLTTPCCAMCTrack &mc = (*fMCTracks)[iMC];
      // ---
      if( mc.Set30() <= 0 ) continue;
      nWrittenTracks++;
      // ---
      const AliHLTTPCCAGBTrack &tCA = fTracker->Tracks()[itr];

      AliHLTTPCCAPerformanceRecoTrackData &recoD = recoData[itr];
      if( recoD.IsClone() ) continue;
      if( tCA.NHits() < 15 ) continue;

      const int nhits = tCA.NHits();
      for ( int ihit = 0; ihit < nhits; ihit++ ) {
        const int index = fTracker->TrackHit( tCA.FirstHitRef() + ihit );
        const AliHLTTPCCAGBHit &hit = fTracker->Hit( index );
      }
      // ---
      
      int nFirstMC = mc.FirstMCPointID();
      int nMCPoints = mc.NMCPoints();

      AliHLTTPCCALocalMCPoint *points = &((*fLocalMCPoints).Data()[nFirstMC]);

      const AliHLTTPCCAGBTrack &t = fTracker->Tracks()[itr];
      AliHLTTPCCATrackParam p = t.Param();
      // ---
      int iSlice = fTracker->Hit( fTracker->TrackHit( tCA.FirstHitRef() ) ).ISlice();
      float alpha = fTracker->Slice( iSlice ).Param().Alpha();
      float x = p.X();
      float y = p.Y();
      float z = p.Z();
      float gx = x * cos(alpha) - y * sin(alpha);
      float gy = y * cos(alpha) + x * sin(alpha);
      float gx1 = gy;
      float gy1 = -gx;
      float gz = z;
      // ---
      int MCindex=-1;
      for(int iMCPoint=0; iMCPoint<nMCPoints; iMCPoint++)
      {
	if(fabs(points[iMCPoint].X() - gx1)< 2.f)
        {
          if(fabs(gy1 - points[iMCPoint].Y())<2 && fabs(p.Z() - points[iMCPoint].Z())<2) {
            MCindex = iMCPoint;
          }
        }
      }
      if(MCindex == -1)
      {
        continue;
      }
      // track resolutions
      while ( 1/*mc.Set() == 2 && TMath::Abs( mc.TPCPar()[0] ) + TMath::Abs( mc.TPCPar()[1] ) > 1*/ ) {
//        double cosA = TMath::Cos( t.Alpha() );
//        double sinA = TMath::Sin( t.Alpha() );
	  double cosA = TMath::Cos( alpha );
	  double sinA = TMath::Sin( alpha );
//        double mcX0 =  mc.TPCPar()[0] * cosA + mc.TPCPar()[1] * sinA;
//        double mcY0 = -mc.TPCPar()[0] * sinA + mc.TPCPar()[1] * cosA;
//        double mcZ0 =  mc.TPCPar()[2];

        double mcY0 =  points[MCindex].X() * cosA + points[MCindex].Y() * sinA;
        double mcX0 = -(-points[MCindex].X() * sinA + points[MCindex].Y() * cosA);
        double mcZ0 =  points[MCindex].Z();
//	  double mcY0 =  -points[MCindex].X() * cosA + points[MCindex].Y() * sinA;
//	  double mcX0 = points[MCindex].X() * sinA + points[MCindex].Y() * cosA;
//	  double mcZ0 =  points[MCindex].Z();
        double mcEy0 =  mc.Par(3) * cosA + mc.Par(4) * sinA;
        double mcEx0 = -(-mc.Par(3) * sinA + mc.Par(4) * cosA);
        double mcEz0 =  mc.Par(5);
        double mcEt0 = TMath::Sqrt( mcEx0 * mcEx0 + mcEy0 * mcEy0 );
//        if ( TMath::Abs( mcEt ) < 1.e-4 ) break;
        double mcSinPhi0 = mcEy0 / mcEt0;
        double mcDzDs0   = mcEz0 / mcEt0;
        double mcQPt0 = mc.Par(6) / mcEt0;
//        if ( TMath::Abs( mcQPt ) < 1.e-4 ) break;
        double mcPt0 = 1. / TMath::Abs( mcQPt0 );
//        if ( mcPt < Parameters::RefThreshold ) break;
//        if ( t.NHits() <  PParameters::MinimumHitsForMCTrack ) break;
//        double bz = fTracker->Slice( 0 ).Param().Bz();
        // ---
        double mcQP_ = points[MCindex].QP();
        double px0_ = -(-points[MCindex].Px() * sinA + points[MCindex].Py() * cosA);
        double py0_ = points[MCindex].Px() * cosA + points[MCindex].Py() * sinA;
        double mcEx_ = px0_*mcQP_;
        double mcEy_ = py0_*mcQP_;
        double mcEz_ = points[MCindex].Pz()*mcQP_;
        double mcEt_ = TMath::Sqrt( mcEx_ * mcEx_ + mcEy_ * mcEy_ );
        double mcSinPhi_ = mcEy_ / mcEt_;
        // ---
	
	
        double mcX =  points[MCindex].X();
        double mcY =  points[MCindex].Y();
        double mcZ =  points[MCindex].Z();
        double mcQP = points[MCindex].QP();
        double mcEx = points[MCindex].Px()*mcQP;
        double mcEy = points[MCindex].Py()*mcQP;
        double mcEz = points[MCindex].Pz()*mcQP;
        double mcEt = TMath::Sqrt( mcEx * mcEx + mcEy * mcEy );
        if ( TMath::Abs( mcEt ) < 1.e-4 ) break;
        double mcSinPhi = mcEy / mcEt;
        double mcDzDs   = mcEz / mcEt;
        double mcQPt =  mcQP / mcEt;
        if ( TMath::Abs( mcQPt ) < 1.e-6 ) break;
        double mcPt = 1. / TMath::Abs( mcQPt );
//        if ( mcPt < Parameters::RefThreshold ) break;
//	if ( t.NHits() <  PParameters::MinimumHitsForMCTrack ) break;
        double bz = fTracker->Slice( 0 ).Param().cBz();
	
        if ( !p.TransportToXWithMaterial( mcX0, bz ) ) break;
        // ---
              float xc = p.X();
              float yc = p.Y();
              float zc = p.Z();
              float gxc = xc * cos(alpha) - yc * sin(alpha);
              float gyc = yc * cos(alpha) + xc * sin(alpha);
              float gx1c = gyc;
              float gy1c = -gx;
              float gzc = zc;
              // ---
        if ( p.GetCosPhi()*mcEx < 0 ) { // change direction
          mcSinPhi = -mcSinPhi;
          mcDzDs = -mcDzDs;
          mcQPt = -mcQPt;
        }

        if ( p.GetCosPhi()*mcEx_ < 0 ) {
          mcSinPhi_ = -mcSinPhi_;
        }
	double qPt = p.GetQPt();
        double pt = 1. / TMath::Abs( qPt );
	
        if ( mcPt < 0.010 ) break;
        if( (mcDzDs>0 && p.GetDzDs()<0) || (mcDzDs<0 && p.GetDzDs()>0) ) {
          mcDzDs = -mcDzDs;
        }

        if ( mcQPt * p.GetQPt() < 0) {
            mcQPt = -mcQPt;
        }
        GetHisto(kresY)->Fill( p.GetY() - mcY0 );
//        GetHisto(kresY)->Fill( gy1c - mcY );
        GetHisto(kresZ)->Fill( p.GetZ() - mcZ );
        GetHisto(kresSinPhi)->Fill( p.GetSinPhi() - mcSinPhi_ );
        GetHisto(kresDzDs)->Fill( p.GetDzDs() - mcDzDs );
        if(CAMath::Abs(qPt) > 1.e-8){
          GetHisto(kresPt)->Fill( (pt - mcPt)/mcPt );
        }
        if ( p.GetErr2Y() > 0 ) GetHisto(kpullY)->Fill( ( p.GetY() - mcY0 ) / TMath::Sqrt( p.GetErr2Y() ) );
//        if ( p.GetErr2Y() > 0 ) GetHisto(kpullY)->Fill( ( gy1c - mcY ) / TMath::Sqrt( p.GetErr2Y() ) );
        if ( p.GetErr2Z() > 0 ) GetHisto(kpullZ)->Fill( ( p.GetZ() - mcZ ) / TMath::Sqrt( p.GetErr2Z() ) );

        if ( p.GetErr2SinPhi() > 0 ) GetHisto(kpullSinPhi)->Fill( ( p.GetSinPhi() - mcSinPhi_ ) / TMath::Sqrt( p.GetErr2SinPhi() ) );
        if ( p.GetErr2DzDs() > 0 ) GetHisto(kpullDzDs)->Fill( ( p.DzDs() - mcDzDs ) / TMath::Sqrt( p.GetErr2DzDs() ) );
        if(CAMath::Abs(qPt) > 1.e-7 && p.GetErr2QPt()>0 ) GetHisto(kpullQPt)->Fill( (qPt - mcQPt)/TMath::Sqrt(p.GetErr2QPt()) );

        break;
      }
    }

  }

  // distribution of cluster errors

  {
    int nHits = fTracker->NHits();

    if ( fHitLabels->Size() > 0 )
    for ( int ih = 0; ih < nHits; ih++ ) {
      const AliHLTTPCCAGBHit &hit = fTracker->Hit( ih );
      const AliHLTTPCCAHitLabel &l = (*fHitLabels)[hit.ID()];
//       fhHitErrY->Fill( hit.ErrY() );
//       fhHitErrZ->Fill( hit.ErrZ() );
      int nmc = 0;
      for ( int il = 0; il < 3; il++ ) if ( l.fLab[il] >= 0 ) nmc++;
//       if ( nmc == 1 ) fhHitShared->Fill( hit.IRow(), 0 );
//       else if ( nmc > 1 ) fhHitShared->Fill( hit.IRow(), 1 );
    }
  }

} // void AliHLTTPCCAGlobalPerformance::FillHistos()

void AliHLTTPCCAGlobalPerformance::Draw()
{
  std::cout<<"___Draw___\n";

  std::cout<<" fTracker->NHits(): "<<fTracker->NHits()<<"\n";
//  for( int i = 0; i < fTracker->NHits(); i++ ) std::cout<<" --- i: "<<i<<";   segmentId: "<<fTracker->TrackHitSegmentId(i)<<"\n";
//  StartLooperTest();
//  FillSegments();
//  CheckSegments();

//#define DRAW_GLOBALPERF

#ifndef KFPARTICLE
#ifdef DRAW_GLOBALPERF
  if ( AliHLTTPCCADisplay::Instance().DrawType() != 3 ) return;
  
  // AliHLTTPCCAPerformance::Instance().Init();
  AliHLTTPCCAPerformance& gbPerfo = AliHLTTPCCAPerformance::Instance();
  AliHLTTPCCADisplay &disp = AliHLTTPCCADisplay::Instance();
  disp.SetGB( gbPerfo.GetTracker() );
  disp.SetTPC( fTracker->Slices()[0].Param() );
  disp.SetTPCView();
//  disp.DrawTPC();
//  disp.DrawGBHits( *gbPerfo.GetTracker(), kGreen, 0.03, 1  );

  // ---
//  disp.ClearViewPT();
//  disp.DrawSliceHitsPT(1, 0.5);
//  disp.DrawSliceLinksPT(-1,-1,1);
  // ---
#if 0
  for ( int irt = 0; irt < nRecoTracks; irt++ ) {
        const AliHLTTPCCAGBTrack &track = fTracker->Track( irt );
        const AliHLTTPCCAGBTrack &tCA = fTracker->Tracks()[irt];
        int mct = tCA.GetFirstMC();
//        if( fTracker->TrackHit(track.FirstHitRef()) != fTracker->TrackHit(track.FirstHitRef()+track.NHits()-1) ) continue;
        const AliHLTTPCCAGBHit &hit1 = fTracker->Hit(fTracker->TrackHit(track.FirstHitRef()));
        const AliHLTTPCCAGBHit &hit2 = fTracker->Hit(fTracker->TrackHit(track.FirstHitRef()+track.NHits()-1));
//        if( track.IsLooper() ) continue;
//        if( hit1.IRow() != hit2.IRow() ) continue;
        if( hit1.IRow() != 5 ) continue;
//        disp.SpecDrawMCTrackHitsGlobal( (*fMCTracks)[mct], fLocalMCPoints, fHitLabels, kRed, 0.5 );
        std::cout<<"> Draw track "<<irt<<"\n";
//        std::cout<<" - track.FirstHitRef(): "<<track.FirstHitRef()<<"\n";
        std::cout<<" --- first hit: "<<fTracker->TrackHit(track.FirstHitRef())<<";   nHits: "<<track.NHits()<<";   last hit: "<<fTracker->TrackHit(track.FirstHitRef()+track.NHits()-1)<<"\n";
        std::cout<<"     first row: "<<hit1.IRow()<<";   last row: "<<hit2.IRow()<<"\n";
        disp.SpecDrawRecoTrackGlobal( irt, kBlue, 0.3 );
//        if( tCA.IsMerged() ) disp.SpecDrawRecoTrackGlobal( irt, kGreen, 0.3 );
        disp.Ask();
        disp.DrawTPC();
  }
  disp.Ask();
  disp.Ask();
#endif

#if 0 // MC info
  for ( int imc = 0; imc < nMCTracks; imc++ ) {
      bool doDraw = true;
      doDraw &= (*fMCTracks)[imc].NMCPoints() < 47;
      doDraw &= mcData[imc].IsReconstructable();
      if(doDraw&&(*fMCTracks)[imc].P()<0.5) disp.SpecDrawMCTrackGlobalPT( (*fMCTracks)[imc], fLocalMCPoints, kBlue, 0.3 );
      doDraw &= (*fMCTracks)[imc].P() > 0.5;
      if ( !doDraw ) continue;
      disp.SpecDrawMCTrackGlobalPT( (*fMCTracks)[imc], fLocalMCPoints, kRed, 0.3 );
  }
#else // reco info

#if 0
  std::vector<int> recoIndexes;
  std::vector<float> recoZmin;
  std::vector<float> recoZmax;
  for ( int imc = 0; imc < nMCTracks; imc++ ) {
    if( imc != testMC ) continue;
    for ( int irt = 0; irt < nRecoTracks; irt++ ) {
      const AliHLTTPCCAGBTrack &track = fTracker->Tracks()[irt];
      int mct = track.GetFirstMC();
      if( mct != imc ) continue;
      recoIndexes.push_back(irt);
    }
  }
  disp.SpecDrawMCTrackHitsGlobal( (*fMCTracks)[testMC], fLocalMCPoints, fHitLabels, kRed, 0.5 );
  int minZj = 0;
  float minZ = 250.f;
  for( int i = 0; i < recoIndexes.size(); i++ ) {
    minZj = i;
    const AliHLTTPCCAGBTrack &track_i = fTracker->Tracks()[recoIndexes[i]];
    minZ = std::min( track_i.Param().Z(), track_i.OuterParam().Z() );
    for( int j = i+1; j < recoIndexes.size(); j++ ) {
      const AliHLTTPCCAGBTrack &track_j = fTracker->Tracks()[recoIndexes[j]];
      float zz = std::min( track_j.Param().Z(), track_j.OuterParam().Z() );
      if( zz < minZ ) {
	minZ = zz;
	minZj = j;
      }
    }
    if( minZj != i ) {
      std::swap( recoIndexes[i], recoIndexes[minZj] );
    }
  }
  for( int i = 0; i < recoIndexes.size(); i++ ) {
    const AliHLTTPCCAGBTrack &track_i = fTracker->Tracks()[recoIndexes[i]];
    float minZ = std::min( track_i.Param().Z(), track_i.OuterParam().Z() );
    float maxZ = std::max( track_i.Param().Z(), track_i.OuterParam().Z() );
    recoZmin.push_back( minZ );
    recoZmax.push_back( maxZ );
//    disp.SpecDrawRecoTrackGlobal( recoIndexes[i], kBlue, 0.3 );
//    disp.Ask();
  }
  float x000 = fTracker->Tracks()[recoIndexes[0]].Param().X();
  for( int i = 0; i < recoIndexes.size(); i++ ) {
    disp.SpecDrawRecoTrackGlobal( recoIndexes[i], kBlue, 0.3 );
    for( int j = i+1; j < recoIndexes.size(); j++ ) {
      //TODO: use h
      if( fabs( recoZmax[i] - recoZmin[j] ) < ( 10*fabs(recoZmax[i]-recoZmin[i]) ) ) {
	std::cout<<" --- check tracks i: "<<i<<" and j: "<<j<<"\n";
	const AliHLTTPCCAGBTrack &track_i = fTracker->Tracks()[recoIndexes[i]];
	const AliHLTTPCCAGBTrack &track_j = fTracker->Tracks()[recoIndexes[j]];
	std::cout<<"       <<< "<<i<<" >>>\n";
	std::cout<<"       sinPhi: "<<track_i.Param().SinPhi()<<";   DzDs: "<<track_i.Param().DzDs()<<";   QPt: "<<track_i.Param().QPt()<<"\n";
	std::cout<<"     ErSinPhi: "<<sqrt(track_i.Param().GetErr2SinPhi())<<"; ErDzDs: "<<sqrt(track_i.Param().GetErr2DzDs())<<"; ErQPt: "<<sqrt(track_i.Param().GetErr2QPt())<<"\n";
	std::cout<<"       <<< "<<j<<" >>>\n";
	std::cout<<"       sinPhi: "<<track_j.Param().SinPhi()<<";   DzDs: "<<track_j.Param().DzDs()<<";   QPt: "<<track_j.Param().QPt()<<"\n";
	std::cout<<"     ErSinPhi: "<<sqrt(track_j.Param().GetErr2SinPhi())<<"; ErDzDs: "<<sqrt(track_j.Param().GetErr2DzDs())<<"; ErQPt: "<<sqrt(track_j.Param().GetErr2QPt())<<"\n\n";
	float dDzDs = fabs( fabs(track_i.Param().DzDs()) - fabs(track_j.Param().DzDs()) );
	float dQPt = fabs( fabs(track_i.Param().QPt()) - fabs(track_j.Param().QPt()) );
	std::cout<<".......dQPt: "<<dQPt<<";   max_dQPt: "<<2*( sqrt(track_i.Param().GetErr2QPt()) + sqrt(track_j.Param().GetErr2QPt()) )<<"\n";
	std::cout<<".......dDzDs: "<<dDzDs<<";   max_dDzDs: "<<3*( sqrt(track_i.Param().GetErr2DzDs()) + sqrt(track_j.Param().GetErr2DzDs()) )<<"\n";
	if( dDzDs < 3*( sqrt(track_i.Param().GetErr2DzDs()) + sqrt(track_j.Param().GetErr2DzDs()) ) &&
	    dQPt  < 2*( sqrt(track_i.Param().GetErr2QPt()) + sqrt(track_j.Param().GetErr2QPt()) ) ) {
	  const AliHLTTPCCAGBHit &hit1 = fTracker->Hit(fTracker->TrackHit(track_i.FirstHitRef()));
	  const AliHLTTPCCAGBHit &hit2 = fTracker->Hit(fTracker->TrackHit(track_j.FirstHitRef()));
	  float x1 = track_i.Param().X();
	  float y1 = track_i.Param().Y();
	  float z1 = track_i.Param().Z();
	  float x2 = track_j.Param().X();
	  float y2 = track_j.Param().Y();
	  float z2 = track_j.Param().Z();
	  if( track_i.OuterParam().Z() > z1 ) {
	    x1 = track_i.OuterParam().X();
	    y1 = track_i.OuterParam().Y();
	    z1 = track_i.OuterParam().Z();
	  }
	  if( track_j.OuterParam().Z() < z2 ) {
	    x2 = track_j.OuterParam().X();
	    y2 = track_j.OuterParam().Y();
	    z2 = track_j.OuterParam().Z();
	  }
	  // ---
	  float x_in_1 = track_i.Param().X();
	  float y_in_1 = track_i.Param().Y();
	  float z_in_1 = track_i.Param().Z();
	  float sinPhi_in_1 = track_i.Param().GetSinPhi();
	  float dzds_in_1 = track_i.Param().GetDzDs();
	  float qpt_in_1 = track_i.Param().GetQPt();
	  float px_in_1 = sqrt( ( 1 - sinPhi_in_1*sinPhi_in_1 )/( qpt_in_1*qpt_in_1 ) );
	  float py_in_1 = sinPhi_in_1 / fabs(qpt_in_1);
	  float pz_in_1 = dzds_in_1 / fabs(qpt_in_1);
	  float pp_in_1 = sqrt( px_in_1*px_in_1 + py_in_1*py_in_1 + pz_in_1*pz_in_1 );
//	  disp.SpecDrawLine( x_in_1, y_in_1, z_in_1, hit1.ISlice(), x_in_1-px_in_1*300, y_in_1-py_in_1*300, z_in_1-pz_in_1*300, hit1.ISlice(), kOrange, 2.5 );
//	  disp.SpecDrawLine( x_in_1, y_in_1, z_in_1, hit1.ISlice(), x_in_1+py_in_1*300, y_in_1-px_in_1*300, z_in_1+pz_in_1*300, hit1.ISlice(), kOrange, 2.5 );
	  AliHLTTPCCAMCTrack &mcT = (*fMCTracks)[testMC];
	  int nFirstMC = mcT.FirstMCPointID();
	  int nMCPoints = mcT.NMCPoints();
	  AliHLTTPCCALocalMCPoint *points = &((*fLocalMCPoints).Data()[nFirstMC]);
	  int iSlice = fTracker->Hit( fTracker->TrackHit( track_i.FirstHitRef() ) ).ISlice();
	  float alpha = fTracker->Slice( iSlice ).Param().Alpha();
	  float gx_in_1 = x_in_1 * cos(alpha) - y_in_1 * sin(alpha);
	  float gy_in_1 = y_in_1 * cos(alpha) + x_in_1 * sin(alpha);
	  float gx1_in_1 = gy_in_1;
	  float gy1_in_1 = -gx_in_1;
	  float gz_in_1 = z_in_1;
	  std::cout<<" <----- Segment1: "<<recoIndexes[i]<<" ----->\n";
	  std::cout<<"  | X: "<<x_in_1<<";   Y: "<<y_in_1<<";   Z: "<<z_in_1<<"\n";
	  std::cout<<"  | SinPhi: "<<sinPhi_in_1<<";   DzDs: "<<dzds_in_1<<";   QPt: "<<qpt_in_1<<" | SignCosPhi: "<<track_i.Param().SignCosPhi()<<"\n";
	  std::cout<<"  | px: "<<px_in_1<<";   py: "<<py_in_1<<";   pz: "<<pz_in_1<<";   P: "<<pp_in_1<<"\n";
	  std::cout<<" <--------------->\n";
	  std::cout<<"  | gX: "<<gx1_in_1<<";   gY: "<<gy1_in_1<<";   gZ: "<<gz_in_1<<"\n";
	  std::cout<<" <--------------->\n";
	  for(int iMCPoint=0; iMCPoint<nMCPoints; iMCPoint++) {
	    if( fabs( gx1_in_1 - points[iMCPoint].X() ) < 2.f && fabs( gy1_in_1 - points[iMCPoint].Y() ) < 2.f && fabs( gz_in_1 - points[iMCPoint].Z() ) < 2.f ) {
	      float pp_mc_in_1 = sqrt( points[iMCPoint].Px()*points[iMCPoint].Px() + points[iMCPoint].Py()*points[iMCPoint].Py() + points[iMCPoint].Pz()*points[iMCPoint].Pz() );
	      std::cout<<"  | iMCP: "<<iMCPoint<<";   x: "<<points[iMCPoint].X()<<";   y: "<<points[iMCPoint].Y()<<";   z: "<<points[iMCPoint].Z()<<"\n";
	      std::cout<<"  | ----- px: "<<points[iMCPoint].Px()<<";   py: "<<points[iMCPoint].Py()<<";   pz: "<<points[iMCPoint].Pz()<<";   P_mc: "<<pp_mc_in_1<<"\n";
	    }
	  }
	  int h1(0), h2((track_i.NHits()-1)/2), h3(track_i.NHits()-1);
	  if( track_i.NHits() > 7 ) {
	    h1++;
	    h3--;
	  }
	  const AliHLTTPCCAGBHit &hit1r = fTracker->Hit(fTracker->TrackHit(track_i.FirstHitRef()+h1));
	  const AliHLTTPCCAGBHit &hit2r = fTracker->Hit(fTracker->TrackHit(track_i.FirstHitRef()+h2));
	  const AliHLTTPCCAGBHit &hit3r = fTracker->Hit(fTracker->TrackHit(track_i.FirstHitRef()+h3));
	  float x1l(hit1r.X()), x2l(hit2r.X()), x3l(hit3r.X());
	  float y1l(hit1r.Y()), y2l(hit2r.Y()), y3l(hit3r.Y());
	  float A = x2l - x1l;
	  float B = y2l - y1l;
	  float C = x3l - x1l;
	  float D = y3l - y1l;
	  float E = A * ( x1l + x2l ) + B * ( y1l + y2l );
	  float F = C * ( x1l + x3l ) + D * ( y1l + y3l );
	  float G = 2 * ( A * ( y3l - y2l ) - B * ( x3l - x2l ) );
	  float Cx(0.), Cy(0.), Cr(0.);
	  if( G != 0 ) {
	    Cx = ( D * E - B * F ) / G;
	    Cy = ( A * F - C * E ) / G;
	    Cr = sqrt( (x1l-Cx)*(x1l-Cx) + (y1l-Cy)*(y1l-Cy) );
	    float cos1(fTracker->Slice(hit1.ISlice()).Param().SinAlpha());
	    float sin1(fTracker->Slice(hit1.ISlice()).Param().CosAlpha());
	    float Cyg = Cx * cos1 + Cy * sin1;
	    float Cxg = -(Cy * cos1 - Cx * sin1);
	    std::cout<<"               Cx: "<<Cx<<";   Cy: "<<Cy<<";   Cr: "<<Cr<<"\n";
	    // ---
	  }
	  // --- circle/line
	  float xl1_cl = Cx;
	  float yl1_cl = Cy;
	  float xl2_cl = 0;
	  float yl2_cl = 0;
	  float k_cl = ( yl1_cl - yl2_cl ) / ( xl1_cl - xl2_cl );
	  float b_cl = yl1_cl - k_cl*xl1_cl;
	  float d_cl = (pow((2*k_cl*b_cl - 2*Cx-2*Cy*k_cl),2)-(4+4*k_cl*k_cl)*(b_cl*b_cl-Cr*Cr+Cx*Cx+Cy*Cy-2*Cy*b_cl));
	  float x1_r = ((-(2*k_cl*b_cl - 2*Cx - 2*Cy*k_cl)-sqrt(d_cl))/(2+2*k_cl*k_cl));
	  float x2_r = ((-(2*k_cl*b_cl - 2*Cx - 2*Cy*k_cl)+sqrt(d_cl))/(2+2*k_cl*k_cl));
	  float y1_r = k_cl*x1_r + b_cl;
	  float y2_r = k_cl*x2_r + b_cl;
	  std::cout<<"<><><> x1_r: "<<x1_r<<";   y1_r: "<<y1_r<<"\n";
	  std::cout<<"<><><> x2_r: "<<x2_r<<";   y2_r: "<<y2_r<<"\n";
	  float x_lp = x1_r;
	  float y_lp = y1_r;
	  float x_up = x2_r;
	  float y_up = y2_r;
	  if( x1_r > x2_r ) {
	    x_lp = x2_r;
	    y_lp = y2_r;
	    x_up = x1_r;
	    y_up = y1_r;
	  }
	  else {
//	    disp.SpecDrawLine( Cx, Cy, z_in_1, hit1.ISlice(), x2_r, y2_r, z_in_1, hit1.ISlice(), kOrange, 2.5 );
//	    x_lp = x2_r;
//	    y_lp = y2_r;
//	    x_up = x1_r;
//	    y_up = y1_r;
//	    z_lp = track_i.OuterParam().Z();
//	    z_up = track_i.Param().Z();
//	    std::swap( r0_l, r0_u );
	  }
	  float cos1(fTracker->Slice(hit1.ISlice()).Param().SinAlpha());
	  float sin1(fTracker->Slice(hit1.ISlice()).Param().CosAlpha());
	  float x_lp_g = -(y_lp * cos1 - x_lp * sin1);
	  float y_lp_g = x_lp * cos1 + y_lp * sin1;
	  float x_up_g = -(y_up * cos1 - x_up * sin1);
	  float y_up_g = x_up * cos1 + y_up * sin1;
	  // ---
	  float z1l(hit1r.Z()), z2l(hit2r.Z()), z3l(hit3r.Z());
	  float x1l_n(x1l), x2l_n(x2l), x3l_n(x3l), y1l_n(y1l), y2l_n(y2l), y3l_n(y3l);
	  std::cout<<"\n\n\n --- x1l_n: "<<x1l_n<<";   x2l_n: "<<x2l_n<<";   x1_r: "<<x1_r<<";   x2_r: "<<x2_r<<"\n";
	  std::cout<<" ___ x_lp: "<<x_lp<<";   x_up: "<<x_up<<"\n";
	  if( x1_r < x2_r ) {
//	    std::swap( x1l_n, x3l_n );
//	    std::swap( y1l_n, y3l_n );
//	    std::swap( z1l, z3l );
	  } else {
	    std::swap( x1l_n, x3l_n );
	    std::swap( y1l_n, y3l_n );
//	    std::swap( z1l, z3l );
	  }
	  float AB_xy = sqrt( (x1l_n-x3l_n)*(x1l_n-x3l_n) + (y1l_n-y3l_n)*(y1l_n-y3l_n) );
	  float BC_xy = sqrt( (x_lp-x1l_n)*(x_lp-x1l_n) + (y_lp-y1l_n)*(y_lp-y1l_n) );
	  float BC_xy_u = sqrt( (x_up-x3l_n)*(x_up-x3l_n) + (y_up-y3l_n)*(y_up-y3l_n) );
	  float AB_xy_l = 2*Cr*asin( 0.5*AB_xy/Cr );
	  float BC_xy_l = 2*Cr*asin( 0.5*BC_xy/Cr );
	  float BC_xy_u_l = 2*Cr*asin( 0.5*BC_xy_u/Cr );
	  std::cout<<". AB_xy: "<<AB_xy<<";   AB_xy_l: "<<AB_xy_l<<"\n";
	  std::cout<<". BC_xy: "<<BC_xy<<";   BC_xy_l: "<<BC_xy_l<<"\n";
	  float AB_z = fabs( z1l - z3l );
	  float BC_z = AB_z*BC_xy/AB_xy;
	  float BC_z_u = AB_z*BC_xy_u/AB_xy;
	  std::cout<<" > BC_z: "<<BC_z<<";   BC_z_u: "<<BC_z_u<<"\n";
	  // ---
	  float _dz1 = (AB_z*BC_xy_l)/AB_xy_l;
	  float _dz2 = ( AB_z*BC_xy_u_l )/AB_xy_l;
	  std::cout<<" dz: "<<AB_z<<";   dl1: "<<BC_xy_l<<";   dl: "<<AB_xy_l<<"\n";
	  std::cout<<" _ _ _ dz1: "<<_dz1<<";   dz2: "<<_dz2<<"\n";
	  if( track_i.Param().Z() < track_i.OuterParam().Z() ) {
	    disp.DrawHitGlobal( x_lp_g, y_lp_g, z1l-_dz1, kGreen, 0.75 );
	    disp.DrawHitGlobal( x_up_g, y_up_g, z3l+_dz2, kGreen, 0.75 );
	    disp.SpecDrawLine( x_lp+1, y_lp, z1l-_dz1, hit1r.ISlice(), x1l, y1l, z1l, hit1r.ISlice(), kGreen, 1. );
	    disp.SpecDrawLine( x_up+1, y_up, z3l+_dz2, hit1r.ISlice(), x3l, y3l, z3l, hit1r.ISlice(), kGreen, 1. );
	  } else {
	    disp.DrawHitGlobal( x_lp_g, y_lp_g, z1l+_dz1, kOrange, 0.75 );
	    disp.DrawHitGlobal( x_up_g, y_up_g, z3l-_dz2, kOrange, 0.75 );
	    disp.SpecDrawLine( x_lp+3, y_lp, z1l+_dz1, hit1r.ISlice(), x1l, y1l, z1l, hit1r.ISlice(), kOrange, 1. );
	    disp.SpecDrawLine( x_up+3, y_up, z3l-_dz2, hit1r.ISlice(), x3l, y3l, z3l, hit1r.ISlice(), kOrange, 1. );
	  }
	  float h = 2*3.14*Cr*AB_z/AB_xy_l;
	  std::cout<<"> AB_xy: "<<AB_xy<<"\n";
	  std::cout<<"> z1: "<<z1l<<";   z3: "<<z3l<<";   dz: "<<fabs( z1l - z3l )<<"\n";
	  std::cout<<"> Cr: "<<Cr<<";   dl: "<<AB_xy_l<<";   dz: "<<AB_z<<"\n";
	  std::cout<<" _ _ _ _ _ h: "<<h<<"\n";
	  disp.SpecDrawLineZ( i*3.5, 0, z1l, hit1r.ISlice(), x1l, y1l, z1l, hit1r.ISlice(), kGreen, 0.5 );
	  disp.SpecDrawLineZ( i*3.5, 0, z1l, hit1r.ISlice(), i*3.5, 0, z1l+h, hit1r.ISlice(), kGreen, 3.5 );
	}
      }
    }
    disp.Ask();
  }
  disp.SaveCanvasToFile( "DrawLoopersTest.pdf" );
#endif
  // ---

//  disp.Ask();
#if 0
  for ( int imc = 0; imc < nMCTracks; imc++ ) {

      if( (((*fMCTracks)[imc]).NHits() <= 100) || (((*fMCTracks)[imc]).NMCRows() > 60) || (((*fMCTracks)[imc]).NMCRows() <= 5) ) continue;
      std::cout<<" > imc: "<<imc<<";   nHits: "<<((*fMCTracks)[imc]).NHits()<<"\n";
      disp.SpecDrawMCTrackHitsGlobal( (*fMCTracks)[imc], fLocalMCPoints, fHitLabels, kRed, 0.5 );
//      disp.Ask();
      for ( int irt = 0; irt < nRecoTracks; irt++ ) {
	const AliHLTTPCCAGBTrack &track = fTracker->Tracks()[irt];
	int mct = track.GetFirstMC();
	if( mct != imc ) continue;
	if( track.NHits() < 6 ) continue;
	// ---
	int h1(0), h2((track.NHits()-1)/2), h3(track.NHits()-1);
	const AliHLTTPCCAGBHit &hit1 = fTracker->Hit(fTracker->TrackHit(track.FirstHitRef()+h1));
	const AliHLTTPCCAGBHit &hit2 = fTracker->Hit(fTracker->TrackHit(track.FirstHitRef()+h2));
	const AliHLTTPCCAGBHit &hit3 = fTracker->Hit(fTracker->TrackHit(track.FirstHitRef()+h3));
	float x1l(hit1.X()), x2l(hit2.X()), x3l(hit3.X());
	float y1l(hit1.Y()), y2l(hit2.Y()), y3l(hit3.Y());
	int iSl1(hit1.ISlice()), iSl2(hit2.ISlice()), iSl3(hit3.ISlice());
	float cos1(fTracker->Slice(iSl1).Param().SinAlpha()), cos2(fTracker->Slice(iSl2).Param().SinAlpha()), cos3(fTracker->Slice(iSl3).Param().SinAlpha());
	float sin1(fTracker->Slice(iSl1).Param().CosAlpha()), sin2(fTracker->Slice(iSl2).Param().CosAlpha()), sin3(fTracker->Slice(iSl3).Param().CosAlpha());
//	*xv = x * fCos + y * fSin;
//	*yv = y * fCos - x * fSin;
	float y1 = x1l * cos1 + y1l * sin1;
	float x1 = -(y1l * cos1 - x1l * sin1);
	float y2 = x2l * cos2 + y2l * sin2;
	float x2 = -(y2l * cos2 - x2l * sin2);
	float y3 = x3l * cos3 + y3l * sin3;
	float x3 = -(y3l * cos3 - x3l * sin3);
	float A = x2 - x1;
	float B = y2 - y1;
	float C = x3 - x1;
	float D = y3 - y1;
	float E = A * ( x1 + x2 ) + B * ( y1 + y2 );
	float F = C * ( x1 + x3 ) + D * ( y1 + y3 );
	float G = 2 * ( A * ( y3 - y2 ) - B * ( x3 - x2 ) );
	float Cx(0.), Cy(0.);
	if( G != 0 ) {
	  Cx = ( D * E - B * F ) / G;
	  Cy = ( A * F - C * E ) / G;
	  disp.DrawHitGlobal( Cx, Cy, 0., kBlack, 0.5 );
	}
	if( Cx == 0 && Cy == 0 ) continue;

	std::cout<<" . itr: "<<irt<<";   mct: "<<mct<<";   isReco: "<<track.IsReco()<<";   mcReco(0,15): "<<recoData[irt].IsReco(0,15)<<";   nMCHits: "<<(*fMCTracks)[mct].NHits()<<";   nMCPoints: "<<(*fMCTracks)[mct].NMCPoints()<<"\n";
	std::cout<<"       sinPhi: "<<track.Param().SinPhi()<<";   DzDs: "<<track.Param().DzDs()<<";   QPt: "<<track.Param().QPt()<<"\n";
	std::cout<<"     ErSinPhi: "<<sqrt(track.Param().GetErr2SinPhi())<<"; ErDzDs: "<<sqrt(track.Param().GetErr2DzDs())<<"; ErQPt: "<<sqrt(track.Param().GetErr2QPt())<<"\n";
	disp.SpecDrawRecoTrackGlobal( irt, kBlue, 0.3 );
	if( track.IsMerged() ) disp.SpecDrawRecoTrackGlobal( irt, kGreen, 0.3 );

	// ---
	disp.Ask();
      }

  }
#endif
//  disp.DrawBadMCHits( fHitLabels );
#if 0
  for ( int imc = 0; imc < nMCTracks; imc++ ) {
      if( !mcData[imc].IsLooper() ) continue;
      std::cout<<" - imc: "<<imc<<";   mcRows: "<<(*fMCTracks)[imc].NMCRows()<<";   mcHits: "<<(*fMCTracks)[imc].NHits()<<"\n";
      disp.SpecDrawMCTrackHitsGlobal( (*fMCTracks)[imc], fLocalMCPoints, fHitLabels, kRed, 0.5 );
      for ( int irt = 0; irt < nRecoTracks; irt++ ) {
        const AliHLTTPCCAGBTrack &tCA = fTracker->Tracks()[irt];
        int mct = tCA.GetFirstMC();
        if( mct != imc ) continue;
#if 0
        if( !tCA.IsLooper() ) {
          std::cout<<" >>> irt: "<<irt<<";   QPt: "<<tCA.Param().QPt()<<"\n";
          disp.SpecDrawRecoTrackGlobal( irt, kBlack, 0.3 );
          // ---
          int h1(0), h2((tCA.NHits()-1)/2), h3(tCA.NHits()-1);
          const AliHLTTPCCAGBHit &hit1r = fTracker->Hit(fTracker->TrackHit(tCA.FirstHitRef()+h1));
          const AliHLTTPCCAGBHit &hit2r = fTracker->Hit(fTracker->TrackHit(tCA.FirstHitRef()+h2));
          const AliHLTTPCCAGBHit &hit3r = fTracker->Hit(fTracker->TrackHit(tCA.FirstHitRef()+h3));
          float x_seg_1(hit1r.X()), x_seg_2(hit2r.X()), x_seg_3(hit3r.X());
          float y_seg_1(hit1r.Y()), y_seg_2(hit2r.Y()), y_seg_3(hit3r.Y());
          float z_seg_1(hit1r.Z()), z_seg_2(hit2r.Z()), z_seg_3(hit3r.Z());
          //
          float cos11(fTracker->Slice(hit1r.ISlice()).Param().SinAlpha());
          float sin11(fTracker->Slice(hit1r.ISlice()).Param().CosAlpha());
          float cos22(fTracker->Slice(hit2r.ISlice()).Param().SinAlpha());
          float sin22(fTracker->Slice(hit2r.ISlice()).Param().CosAlpha());
          float cos33(fTracker->Slice(hit3r.ISlice()).Param().SinAlpha());
          float sin33(fTracker->Slice(hit3r.ISlice()).Param().CosAlpha());
          float x_seg_1_g = -(y_seg_1 * cos11 - x_seg_1 * sin11);
          float y_seg_1_g = x_seg_1 * cos11 + y_seg_1 * sin11;
          float x_seg_2_g = -(y_seg_2 * cos22 - x_seg_2 * sin22);
          float y_seg_2_g = x_seg_2 * cos22 + y_seg_2 * sin22;
          float x_seg_3_g = -(y_seg_3 * cos33 - x_seg_3 * sin33);
          float y_seg_3_g = x_seg_3 * cos33 + y_seg_3 * sin33;
            // Center of the circle in global coords
          float cos1(fTracker->Slice(hit2r.ISlice()).Param().SinAlpha());
          float sin1(fTracker->Slice(hit2r.ISlice()).Param().CosAlpha());
          float A_g = x_seg_2_g - x_seg_1_g;
          float B_g = y_seg_2_g - y_seg_1_g;
          float C_g = x_seg_3_g - x_seg_1_g;
          float D_g = y_seg_3_g - y_seg_1_g;
          float E_g = A_g * ( x_seg_1_g + x_seg_2_g ) + B_g * ( y_seg_1_g + y_seg_2_g );
          float F_g = C_g * ( x_seg_1_g + x_seg_3_g ) + D_g * ( y_seg_1_g + y_seg_3_g );
          float G_g = 2 * ( A_g * ( y_seg_3_g - y_seg_2_g ) - B_g * ( x_seg_3_g - x_seg_2_g ) );
          float Cxg(0.), Cyg(0.), Cr(0.);
          if( G_g != 0 ) {
            Cxg = ( D_g * E_g - B_g * F_g ) / G_g;
            Cyg = ( A_g * F_g - C_g * E_g ) / G_g;
            Cr = sqrt( (x_seg_1_g-Cxg)*(x_seg_1_g-Cxg) + (y_seg_1_g-Cyg)*(y_seg_1_g-Cyg) );
          }
          disp.DrawHitGlobal( Cxg, Cyg, z_seg_2, kBlack, 0.75 );
          float k_cl_g = Cyg / Cxg;
          float b_cl_g = Cyg - k_cl_g*Cxg;
          float d_cl_g = (pow((2*k_cl_g*b_cl_g - 2*Cxg-2*Cyg*k_cl_g),2)-(4+4*k_cl_g*k_cl_g)*(b_cl_g*b_cl_g-Cr*Cr+Cxg*Cxg+Cyg*Cyg-2*Cyg*b_cl_g));
          float x_dn_r_g = ((-(2*k_cl_g*b_cl_g - 2*Cxg - 2*Cyg*k_cl_g)-sqrt(d_cl_g))/(2+2*k_cl_g*k_cl_g));
          float x_up_r_g = ((-(2*k_cl_g*b_cl_g - 2*Cxg - 2*Cyg*k_cl_g)+sqrt(d_cl_g))/(2+2*k_cl_g*k_cl_g));
          float y_dn_r_g = k_cl_g*x_dn_r_g + b_cl_g;
          float y_up_r_g = k_cl_g*x_up_r_g + b_cl_g;
          float rr_dn = sqrt( x_dn_r_g*x_dn_r_g + y_dn_r_g*y_dn_r_g );
          float rr_up = sqrt( x_up_r_g*x_up_r_g + y_up_r_g*y_up_r_g );
          if( rr_dn > rr_up ) {
            std::swap( x_dn_r_g, x_up_r_g );
            std::swap( y_dn_r_g, y_up_r_g );
          }
          std::cout<<"x_dn_r_g: "<<x_dn_r_g<<";   y_dn_r_g: "<<y_dn_r_g<<"\n";
          std::cout<<"x_up_r_g: "<<x_up_r_g<<";   y_up_r_g: "<<y_up_r_g<<"\n";
          disp.DrawHitGlobal( x_dn_r_g, y_dn_r_g, z_seg_2, kGreen, 0.75 );
          disp.DrawHitGlobal( x_up_r_g, y_up_r_g, z_seg_2, kOrange, 0.75 );
          // ---
        }
        else {
          disp.SpecDrawRecoTrackGlobal( irt, kBlue, 0.3 );
        }
#endif
      }
//      disp.Ask();
//      disp.ClearView();
//      disp.DrawTPC();
  }
  disp.SaveCanvasToFile( "DrawLoopersMCOnly.pdf" );
  disp.Ask();
#endif

  int badTrack = 0;
  std::cout<<"nRecoTracks: "<<nRecoTracks<<"\n";
#if 0
  for( int imc = 0; imc < nMCTracks; imc++ ) {
    disp.SpecDrawMCTrackHitsGlobal( (*fMCTracks)[imc], fLocalMCPoints, fHitLabels, kRed, 0.5 );
  }
#endif

#if 0
  for ( int irt = 0; irt < nRecoTracks; irt++ ) {
      const AliHLTTPCCAGBTrack &tCA = fTracker->Tracks()[irt];
      int mct = tCA.GetFirstMC();
      if( mct < 0 ) {
	  badTrack++;
//	  continue;
      }

      if( tCA.IsLooper() > 0 ) {
	disp.SpecDrawRecoTrackGlobal( irt, kBlue, 0.3 );
	std::cout<<" >>> irt: "<<irt<<";   isRevers: "<<tCA.IsReverse()<<"\n";
	disp.Ask();
	disp.DrawTPC();
      }
  }
  disp.SaveCanvasToFile( "DrawGlobalPerf_MC_Reco.pdf" );

#endif

  disp.Ask();
  disp.DrawTPC();

  for ( int irt = 0; irt < nRecoTracks; irt++ ) {
      const AliHLTTPCCAGBTrack &track = fTracker->Track( irt );

      const AliHLTTPCCAGBTrack &tCA = fTracker->Tracks()[irt];
    disp.SpecDrawRecoTrackGlobal( irt, kBlue, 0.3 );
  }
  std::cout<<" --- ghost is not labeld: "<<badTrack<<"\n";
#endif
  
  disp.SaveCanvasToFile( "DrawGlobalPerformance.pdf" );
//  disp.SaveCanvasToFilePT( "DrawGlobalPerformancePT.pdf" );
  disp.Ask();
  
#endif // DRAW_GLOBALPERF
#endif //KFPARTICLE
} // void AliHLTTPCCAGlobalPerformance::Draw()

#if 0
void AliHLTTPCCAGlobalPerformance::DrawLooperFull( int mcTr, AliHLTTPCCADisplay &disp )
{
  disp.ClearView();
  disp.DrawTPC();
  std::cout<<"mcTr: "<<mcTr<<"\n";
  std::vector<int> recoIndexes;
  for ( int irt = 0; irt < nRecoTracks; irt++ ) {
    const AliHLTTPCCAGBTrack &track = fTracker->Tracks()[irt];
    int mct = track.GetFirstMC();
if( fabs(track.Param().QPt()) < 5 && fabs(track.OuterParam().QPt()) < 5 ) continue;
    if( mct != mcTr ) continue;
    recoIndexes.push_back(irt);
  }
  if( !recoIndexes.size() ) return;
  disp.SpecDrawMCTrackHitsGlobal( (*fMCTracks)[mcTr], fLocalMCPoints, fHitLabels, kRed, 0.5 );
  int minZj = 0;
  float minZ = 250.f;
  for( int i = 0; i < recoIndexes.size(); i++ ) {
    minZj = i;
    const AliHLTTPCCAGBTrack &track_i = fTracker->Tracks()[recoIndexes[i]];
    minZ = std::min( track_i.Param().Z(), track_i.OuterParam().Z() );
    for( int j = i+1; j < recoIndexes.size(); j++ ) {
      const AliHLTTPCCAGBTrack &track_j = fTracker->Tracks()[recoIndexes[j]];
      float zz = std::min( track_j.Param().Z(), track_j.OuterParam().Z() );
      if( zz < minZ ) {
	minZ = zz;
	minZj = j;
      }
    }
    if( minZj != i ) {
      std::swap( recoIndexes[i], recoIndexes[minZj] );
    }
  }
  for( int i = 0; i < recoIndexes.size(); i++ ) {
    disp.SpecDrawRecoTrackGlobal( recoIndexes[i], kBlue, 0.3 );
    const AliHLTTPCCAGBTrack &track = fTracker->Tracks()[recoIndexes[i]];
//if( track.Param().QPt() < 5 ) continue;
    std::cout<<" > Reco track: "<<recoIndexes[i]<<";   QPt: "<<track.Param().QPt()<<" ... "<<track.OuterParam().QPt()
	<<";   DzDs: "<<track.Param().DzDs()<<" ... "<<track.OuterParam().DzDs()<<"\n";
    const float tDzDs_inner = track.Param().DzDs();
    const float tDzDs_outer = track.OuterParam().DzDs();
    const float tQPt_inner = track.Param().QPt();
    const float tQPt_outer = track.OuterParam().QPt();
    int h1(0), h2((track.NHits()-1)/2), h3(track.NHits()-1);
    if( track.NHits() > 7 ) {
      h1++;
      h3--;
    }
    const AliHLTTPCCAGBHit &hit1r = fTracker->Hit(fTracker->TrackHit(track.FirstHitRef()+h1));
    const AliHLTTPCCAGBHit &hit2r = fTracker->Hit(fTracker->TrackHit(track.FirstHitRef()+h2));
    const AliHLTTPCCAGBHit &hit3r = fTracker->Hit(fTracker->TrackHit(track.FirstHitRef()+h3));
    float x_seg_1(hit1r.X()), x_seg_2(hit2r.X()), x_seg_3(hit3r.X());
    float y_seg_1(hit1r.Y()), y_seg_2(hit2r.Y()), y_seg_3(hit3r.Y());
    float z_seg_1(hit1r.Z()), z_seg_2(hit2r.Z()), z_seg_3(hit3r.Z());
    // Center and Radius of the circle
    float A = x_seg_2 - x_seg_1;
    float B = y_seg_2 - y_seg_1;
    float C = x_seg_3 - x_seg_1;
    float D = y_seg_3 - y_seg_1;
    float E = A * ( x_seg_1 + x_seg_2 ) + B * ( y_seg_1 + y_seg_2 );
    float F = C * ( x_seg_1 + x_seg_3 ) + D * ( y_seg_1 + y_seg_3 );
    float G = 2 * ( A * ( y_seg_3 - y_seg_2 ) - B * ( x_seg_3 - x_seg_2 ) );
    float Cx(0.), Cy(0.), Cr(0.);
    if( G != 0 ) {
      Cx = ( D * E - B * F ) / G;
      Cy = ( A * F - C * E ) / G;
      Cr = sqrt( (x_seg_1-Cx)*(x_seg_1-Cx) + (y_seg_1-Cy)*(y_seg_1-Cy) );
    }
#if 1	// Center of the circle in global coords
    float cos1(fTracker->Slice(hit2r.ISlice()).Param().SinAlpha());
    float sin1(fTracker->Slice(hit2r.ISlice()).Param().CosAlpha());
    float Cyg = Cx * cos1 + Cy * sin1;
    float Cxg = -(Cy * cos1 - Cx * sin1);
#endif
    // Nearest end farthest points of the circle to (0;0)
    float k_cl = Cy / Cx;
    float b_cl = Cy - k_cl*Cx;
    float d_cl = (pow((2*k_cl*b_cl - 2*Cx-2*Cy*k_cl),2)-(4+4*k_cl*k_cl)*(b_cl*b_cl-Cr*Cr+Cx*Cx+Cy*Cy-2*Cy*b_cl));
    float x_dn_r = ((-(2*k_cl*b_cl - 2*Cx - 2*Cy*k_cl)-sqrt(d_cl))/(2+2*k_cl*k_cl));
    float x_up_r = ((-(2*k_cl*b_cl - 2*Cx - 2*Cy*k_cl)+sqrt(d_cl))/(2+2*k_cl*k_cl));
    float y_dn_r = k_cl*x_dn_r + b_cl;
    float y_up_r = k_cl*x_up_r + b_cl;
    if( x_dn_r > x_up_r ) {
      std::swap( x_dn_r, x_up_r );
      std::swap( y_dn_r, y_up_r );
      std::swap( x_seg_1, x_seg_3 );
      std::swap( y_seg_1, y_seg_3 );
      std::swap( z_seg_1, z_seg_3 );
    }
#if 1   // Nearest and farthest points XY in global coords
    float cos11(fTracker->Slice(hit1r.ISlice()).Param().SinAlpha());
    float sin11(fTracker->Slice(hit1r.ISlice()).Param().CosAlpha());
    float cos33(fTracker->Slice(hit3r.ISlice()).Param().SinAlpha());
    float sin33(fTracker->Slice(hit3r.ISlice()).Param().CosAlpha());
    float x_dn_r_g = -(y_dn_r * cos1 - x_dn_r * sin1);
    float y_dn_r_g = x_dn_r * cos1 + y_dn_r * sin1;
    float x_up_r_g = -(y_up_r * cos1 - x_up_r * sin1);
    float y_up_r_g = x_up_r * cos1 + y_up_r * sin1;

    float x_seg_1_g = -(y_seg_1 * cos11 - x_seg_1 * sin11);
    float y_seg_1_g = x_seg_1 * cos11 + y_seg_1 * sin11;
    float x_seg_3_g = -(y_seg_3 * cos33 - x_seg_3 * sin33);
    float y_seg_3_g = x_seg_3 * cos33 + y_seg_3 * sin33;
#endif
    float AB_xy = sqrt( (x_seg_1-x_seg_3)*(x_seg_1-x_seg_3) + (y_seg_1-y_seg_3)*(y_seg_1-y_seg_3) );
    float BC_xy_dn = sqrt( (x_dn_r-x_seg_1)*(x_dn_r-x_seg_1) + (y_dn_r-y_seg_1)*(y_dn_r-y_seg_1) );
    float BC_xy_up = sqrt( (x_up_r-x_seg_3)*(x_up_r-x_seg_3) + (y_up_r-y_seg_3)*(y_up_r-y_seg_3) );
    float AB_xy_curve = 2*Cr*asin( 0.5*AB_xy/Cr );
    float BC_xy_dn_curve = 2*Cr*asin( 0.5*BC_xy_dn/Cr );
    float BC_xy_up_curve = 2*Cr*asin( 0.5*BC_xy_up/Cr );
    float AB_z = fabs( z_seg_1 - z_seg_3 );
    float BC_z_dn = AB_z*BC_xy_dn/AB_xy;
    float BC_z_up = AB_z*BC_xy_up/AB_xy;
    float dz_dn = ( AB_z*BC_xy_dn_curve ) / AB_xy_curve;
    float dz_up = ( AB_z*BC_xy_up_curve ) / AB_xy_curve;
    float h = 2*3.14*Cr*AB_z/AB_xy_curve;
    std::cout<<"   dn point: X: "<<x_dn_r_g<<";   Y: "<<y_dn_r_g<<";   Z: "<<z_seg_1-dz_dn<<"\n";
    std::cout<<"   up point: X: "<<x_up_r_g<<";   Y: "<<y_up_r_g<<";   Z: "<<z_seg_3-dz_up<<"\n";
    std::cout<<"   Mid: X: "<<Cxg<<";   Y: "<<Cyg<<";   R: "<<Cr<<"\n";
    std::cout<<"     h: "<<h<<"\n";
    disp.DrawHitGlobal( Cxg, Cyg, 0., kBlack, 0.5 );
    if( track.Param().Z() < track.OuterParam().Z() ) {
      disp.DrawHitGlobal( x_dn_r_g, y_dn_r_g, z_seg_1-dz_dn, kGreen, 0.75 );
      disp.DrawHitGlobal( x_up_r_g, y_up_r_g, z_seg_3+dz_up, kGreen, 0.75 );
      disp.SpecDrawLineG( x_dn_r_g, y_dn_r_g, z_seg_1-dz_dn, x_seg_1_g, y_seg_1_g, z_seg_1, kGreen, 1. );
      disp.SpecDrawLineG( x_up_r_g, y_up_r_g, z_seg_3+dz_up, x_seg_3_g, y_seg_3_g, z_seg_3, kGreen, 1. );
    } else {
      disp.DrawHitGlobal( x_dn_r_g, y_dn_r_g, z_seg_1+dz_dn, kOrange, 0.75 );
      disp.DrawHitGlobal( x_up_r_g, y_up_r_g, z_seg_3-dz_up, kOrange, 0.75 );
      disp.SpecDrawLineG( x_dn_r_g, y_dn_r_g, z_seg_1+dz_dn, x_seg_1_g, y_seg_1_g, z_seg_1, kOrange, 1. );
      disp.SpecDrawLineG( x_up_r_g, y_up_r_g, z_seg_3-dz_up, x_seg_3_g, y_seg_3_g, z_seg_3, kOrange, 1. );
    }
    disp.Ask();
  }
}
#endif
#endif // HLTCA_STANDALONE

//#define LOOPER_TEST
#ifdef LOOPER_TEST
void AliHLTTPCCAGlobalPerformance::FillSegments()
{
  std::cout<<":::FillSegments:::\n";
  for( int irt = 0; irt < nRecoTracks; irt++ ) {
    const AliHLTTPCCAGBTrack &track = fTracker->Tracks()[irt];
    if( fabs( track.Param().QPt() ) < 5 && fabs( track.OuterParam().QPt() ) < 5 ) continue;
    int h1(0), h2((track.NHits()-1)/2), h3(track.NHits()-1);
    if( track.NHits() > 7 ) { h1++; h3--; }
    const AliHLTTPCCAGBHit &hit1r = fTracker->Hit(fTracker->TrackHit(track.FirstHitRef()+h1));
    const AliHLTTPCCAGBHit &hit2r = fTracker->Hit(fTracker->TrackHit(track.FirstHitRef()+h2));
    const AliHLTTPCCAGBHit &hit3r = fTracker->Hit(fTracker->TrackHit(track.FirstHitRef()+h3));
    float x_seg_1(hit1r.X()), x_seg_2(hit2r.X()), x_seg_3(hit3r.X());
    float y_seg_1(hit1r.Y()), y_seg_2(hit2r.Y()), y_seg_3(hit3r.Y());
    float z_seg_1(hit1r.Z()), z_seg_2(hit2r.Z()), z_seg_3(hit3r.Z());
    //
    float cos11(fTracker->Slice(hit1r.ISlice()).Param().SinAlpha());
    float sin11(fTracker->Slice(hit1r.ISlice()).Param().CosAlpha());
    float cos22(fTracker->Slice(hit2r.ISlice()).Param().SinAlpha());
    float sin22(fTracker->Slice(hit2r.ISlice()).Param().CosAlpha());
    float cos33(fTracker->Slice(hit3r.ISlice()).Param().SinAlpha());
    float sin33(fTracker->Slice(hit3r.ISlice()).Param().CosAlpha());
    float x_seg_1_g = -(y_seg_1 * cos11 - x_seg_1 * sin11);
    float y_seg_1_g = x_seg_1 * cos11 + y_seg_1 * sin11;
    float x_seg_2_g = -(y_seg_2 * cos22 - x_seg_2 * sin22);
    float y_seg_2_g = x_seg_2 * cos22 + y_seg_2 * sin22;
    float x_seg_3_g = -(y_seg_3 * cos33 - x_seg_3 * sin33);
    float y_seg_3_g = x_seg_3 * cos33 + y_seg_3 * sin33;
    // Center and Radius of the circle
//    float A = x_seg_2 - x_seg_1;
//    float B = y_seg_2 - y_seg_1;
//    float C = x_seg_3 - x_seg_1;
//    float D = y_seg_3 - y_seg_1;
//    float E = A * ( x_seg_1 + x_seg_2 ) + B * ( y_seg_1 + y_seg_2 );
//    float F = C * ( x_seg_1 + x_seg_3 ) + D * ( y_seg_1 + y_seg_3 );
//    float G = 2 * ( A * ( y_seg_3 - y_seg_2 ) - B * ( x_seg_3 - x_seg_2 ) );
//    float Cx(0.), Cy(0.), Cr(0.);
//    if( G != 0 ) {
//      Cx = ( D * E - B * F ) / G;
//      Cy = ( A * F - C * E ) / G;
//      Cr = sqrt( (x_seg_1-Cx)*(x_seg_1-Cx) + (y_seg_1-Cy)*(y_seg_1-Cy) );
//    }
  #if 1	// Center of the circle in global coords
    float cos1(fTracker->Slice(hit2r.ISlice()).Param().SinAlpha());
    float sin1(fTracker->Slice(hit2r.ISlice()).Param().CosAlpha());
//    float Cyg = Cx * cos1 + Cy * sin1;
//    float Cxg = -(Cy * cos1 - Cx * sin1);
    float A_g = x_seg_2_g - x_seg_1_g;
    float B_g = y_seg_2_g - y_seg_1_g;
    float C_g = x_seg_3_g - x_seg_1_g;
    float D_g = y_seg_3_g - y_seg_1_g;
    float E_g = A_g * ( x_seg_1_g + x_seg_2_g ) + B_g * ( y_seg_1_g + y_seg_2_g );
    float F_g = C_g * ( x_seg_1_g + x_seg_3_g ) + D_g * ( y_seg_1_g + y_seg_3_g );
    float G_g = 2 * ( A_g * ( y_seg_3_g - y_seg_2_g ) - B_g * ( x_seg_3_g - x_seg_2_g ) );
    float Cxg(0.), Cyg(0.), Cr(0.);
    if( G_g != 0 ) {
      Cxg = ( D_g * E_g - B_g * F_g ) / G_g;
      Cyg = ( A_g * F_g - C_g * E_g ) / G_g;
      Cr = sqrt( (x_seg_1_g-Cxg)*(x_seg_1_g-Cxg) + (y_seg_1_g-Cyg)*(y_seg_1_g-Cyg) );
    }
  #endif
      // Nearest end farthest points of the circle to (0;0)
//    float k_cl = Cy / Cx;
//    float b_cl = Cy - k_cl*Cx;
//    float d_cl = (pow((2*k_cl*b_cl - 2*Cx-2*Cy*k_cl),2)-(4+4*k_cl*k_cl)*(b_cl*b_cl-Cr*Cr+Cx*Cx+Cy*Cy-2*Cy*b_cl));
//    float x_dn_r = ((-(2*k_cl*b_cl - 2*Cx - 2*Cy*k_cl)-sqrt(d_cl))/(2+2*k_cl*k_cl));
//    float x_up_r = ((-(2*k_cl*b_cl - 2*Cx - 2*Cy*k_cl)+sqrt(d_cl))/(2+2*k_cl*k_cl));
//    float y_dn_r = k_cl*x_dn_r + b_cl;
//    float y_up_r = k_cl*x_up_r + b_cl;
//    if( x_dn_r > x_up_r ) {
//      std::swap( x_dn_r, x_up_r );
//      std::swap( y_dn_r, y_up_r );
//      std::swap( x_seg_1, x_seg_3 );
//      std::swap( y_seg_1, y_seg_3 );
//      std::swap( z_seg_1, z_seg_3 );
//    }
  #if 1   // Nearest and farthest points XY in global coords
//    float cos11(fTracker->Slice(hit1r.ISlice()).Param().SinAlpha());
//    float sin11(fTracker->Slice(hit1r.ISlice()).Param().CosAlpha());
//    float cos33(fTracker->Slice(hit3r.ISlice()).Param().SinAlpha());
//    float sin33(fTracker->Slice(hit3r.ISlice()).Param().CosAlpha());

    /*float x_dn_r_g = -(y_dn_r * cos1 - x_dn_r * sin1);
    float y_dn_r_g = x_dn_r * cos1 + y_dn_r * sin1;
    float x_up_r_g = -(y_up_r * cos1 - x_up_r * sin1);
    float y_up_r_g = x_up_r * cos1 + y_up_r * sin1;*/
    float k_cl_g = Cyg / Cxg;
    float b_cl_g = Cyg - k_cl_g*Cxg;
    float d_cl_g = (pow((2*k_cl_g*b_cl_g - 2*Cxg-2*Cyg*k_cl_g),2)-(4+4*k_cl_g*k_cl_g)*(b_cl_g*b_cl_g-Cr*Cr+Cxg*Cxg+Cyg*Cyg-2*Cyg*b_cl_g));
    float x_dn_r_g = ((-(2*k_cl_g*b_cl_g - 2*Cxg - 2*Cyg*k_cl_g)-sqrt(d_cl_g))/(2+2*k_cl_g*k_cl_g));
    float x_up_r_g = ((-(2*k_cl_g*b_cl_g - 2*Cxg - 2*Cyg*k_cl_g)+sqrt(d_cl_g))/(2+2*k_cl_g*k_cl_g));
    float y_dn_r_g = k_cl_g*x_dn_r_g + b_cl_g;
    float y_up_r_g = k_cl_g*x_up_r_g + b_cl_g;
    float rr_dn = sqrt( x_dn_r_g*x_dn_r_g + y_dn_r_g*y_dn_r_g );
    float rr_up = sqrt( x_up_r_g*x_up_r_g + y_up_r_g*y_up_r_g );
    if( rr_dn > rr_up ) {
      std::swap( x_dn_r_g, x_up_r_g );
      std::swap( y_dn_r_g, y_up_r_g );
    }

//    float x_seg_1_g = -(y_seg_1 * cos11 - x_seg_1 * sin11);
//    float y_seg_1_g = x_seg_1 * cos11 + y_seg_1 * sin11;
//    float x_seg_3_g = -(y_seg_3 * cos33 - x_seg_3 * sin33);
//    float y_seg_3_g = x_seg_3 * cos33 + y_seg_3 * sin33;
if( fabs(x_dn_r_g) > 3 && fabs(y_dn_r_g) > 3 ) {
//  disp.SpecDrawRecoTrackGlobal( irt, kBlue, 0.3 );
//  std::cout<<"x_dn_r_g: "<<x_dn_r_g<<";   y_dn_r_g: "<<y_dn_r_g<<"\n";
} else continue;

if( ( fabs(x_dn_r_g) < 3 && fabs(y_dn_r_g) < 3 ) || ( fabs(x_up_r_g) < 3 && fabs(y_up_r_g) < 3 ) ) continue;
#endif
    float AB_xy = sqrt( (x_seg_1_g-x_seg_3_g)*(x_seg_1_g-x_seg_3_g) + (y_seg_1_g-y_seg_3_g)*(y_seg_1_g-y_seg_3_g) );
    float BC_xy_dn = sqrt( (x_dn_r_g-x_seg_1_g)*(x_dn_r_g-x_seg_1_g) + (y_dn_r_g-y_seg_1_g)*(y_dn_r_g-y_seg_1_g) );
    float BC_xy_up = sqrt( (x_up_r_g-x_seg_3_g)*(x_up_r_g-x_seg_3_g) + (y_up_r_g-y_seg_3_g)*(y_up_r_g-y_seg_3_g) );
    float AB_xy_curve = 2*Cr*asin( 0.5*AB_xy/Cr );
    float BC_xy_dn_curve = 2*Cr*asin( 0.5*BC_xy_dn/Cr );
    float BC_xy_up_curve = 2*Cr*asin( 0.5*BC_xy_up/Cr );
    float AB_z = fabs( z_seg_1 - z_seg_3 );
    float BC_z_dn = AB_z*BC_xy_dn/AB_xy;
    float BC_z_up = AB_z*BC_xy_up/AB_xy;
    float dz_dn = ( AB_z*BC_xy_dn_curve ) / AB_xy_curve;
    float dz_up = ( AB_z*BC_xy_up_curve ) / AB_xy_curve;
    float h = 2*3.14*Cr*AB_z/AB_xy_curve;
//      disp.DrawHitGlobal( Cxg, Cyg, 0., kBlack, 0.5 );
    float z_dn_r = z_seg_1+dz_dn;
    float z_up_r = z_seg_3-dz_up;
    if( track.Param().Z() < track.OuterParam().Z() ) {
      z_dn_r = z_seg_1-dz_dn;
      z_up_r = z_seg_3+dz_up;
//        disp.DrawHitGlobal( x_dn_r_g, y_dn_r_g, z_dn_r, kGreen, 0.75 );
//        disp.DrawHitGlobal( x_up_r_g, y_up_r_g, z_up_r, kGreen, 0.75 );
//        disp.SpecDrawLine( x_dn_r+1, y_dn_r, z_dn_r, hit2r.ISlice(), x_seg_1, y_seg_1, z_seg_1, hit2r.ISlice(), kGreen, 1. );
//        disp.SpecDrawLine( x_up_r+1, y_up_r, z_up_r, hit2r.ISlice(), x_seg_3, y_seg_3, z_seg_3, hit2r.ISlice(), kGreen, 1. );
    } //else {
//        disp.DrawHitGlobal( x_dn_r_g, y_dn_r_g, z_dn_r, kOrange, 0.75 );
//        disp.DrawHitGlobal( x_up_r_g, y_up_r_g, z_up_r, kOrange, 0.75 );
//        disp.SpecDrawLine( x_dn_r+3, y_dn_r, z_dn_r, hit1r.ISlice(), x_seg_1, y_seg_1, z_seg_1, hit1r.ISlice(), kOrange, 1. );
//        disp.SpecDrawLine( x_up_r+3, y_up_r, z_up_r, hit1r.ISlice(), x_seg_3, y_seg_3, z_seg_3, hit1r.ISlice(), kOrange, 1. );
//    }
//      disp.Ask();
    LooperSegment* segment = new LooperSegment;
    segment->iTr = irt;
    segment->QPt_abs = fabs( track.Param().QPt() );
    segment->Cx = Cxg;
    segment->Cy = Cyg;
    segment->Cr = Cr;
    segment->x_up = x_up_r_g;
    segment->y_up = y_up_r_g;
    segment->z_up = z_up_r;
    segment->x_dn = x_dn_r_g;
    segment->y_dn = y_dn_r_g;
    segment->z_dn = z_dn_r;
    segment->x_h_up = x_seg_3_g;
    segment->y_h_up = y_seg_3_g;
    segment->z_h_up = z_seg_3;
    segment->x_h_dn = x_seg_1_g;
    segment->y_h_dn = y_seg_1_g;
    segment->z_h_dn = z_seg_1;
    segment->h = h;
    segment->slice_mid = hit2r.ISlice();
    segment->iLooper = -1;
    segment->isUsed = false;
    segment->iMC = track.GetFirstMC();
    fSegments.push_back( *segment );
  }
//  std::cout<<" NSegments: "<<fSegments.size()<<"\n";
//  for( int i = 0; i < fSegments.size(); i++ ) {
//    std::cout<<" - iSeg: "<<i<<";   Cx: "<<fSegments[i].Cx<<";   Cy: "<<fSegments[i].Cy<<";   Cr: "<<fSegments[i].Cr<<"\n";
//  }
//  disp.SaveCanvasToFile( "DrawLoopersTest.pdf" );
//  disp.Ask();
}

void AliHLTTPCCAGlobalPerformance::CheckSegments()
{
#if 0
  //
    AliHLTTPCCAPerformance& gbPerfo = AliHLTTPCCAPerformance::Instance();
      AliHLTTPCCADisplay &disp = AliHLTTPCCADisplay::Instance();
      disp.SetGB( gbPerfo.GetTracker() );
      disp.SetTPC( fTracker->Slices()[0].Param() );
      disp.SetTPCView();
      disp.DrawTPC();
  //
#endif
  std::cout<<" :::CheckSegments() - fSegments.size(): "<<fSegments.size()<<"\n";
  vector<int> loopers;
  for( int iSeg = 0; iSeg < fSegments.size(); iSeg++ ) {
      bool newLooper = true;
      if( fSegments[iSeg].isUsed ) {
	newLooper = false;
      } else {
	fSegments[iSeg].isUsed = true;
	fSegments[iSeg].iLooper = fNLoopers;
	loopers.push_back(1);
	fNLoopers++;
      }
//    std::cout<<" - iSeg: "<<iSeg<<";   Cx: "<<fSegments[iSeg].Cx<<";   Cy: "<<fSegments[iSeg].Cy<<";   Cr: "<<fSegments[iSeg].Cr<<"\n";
    for( int jSeg = iSeg+1; jSeg < fSegments.size(); jSeg++ ) {
      if( fSegments[jSeg].isUsed ) continue;
      if( fSegments[iSeg].x_h_up == fSegments[jSeg].x_h_up && fSegments[iSeg].y_h_up == fSegments[jSeg].y_h_up
	  && fSegments[iSeg].x_h_dn == fSegments[jSeg].x_h_dn && fSegments[iSeg].y_h_dn == fSegments[jSeg].y_h_dn ) continue;
//      std::cout<<" --- jSeg: "<<jSeg<<";   Cx: "<<fSegments[jSeg].Cx<<";   Cy: "<<fSegments[jSeg].Cy<<";   Cr: "<<fSegments[jSeg].Cr<<"\n";
      if( fabs(fSegments[iSeg].x_dn - fSegments[jSeg].x_dn) < 5 && fabs(fSegments[iSeg].x_up - fSegments[jSeg].x_up) < 5 ) {
//	std::cout<<" ----- Catch x_dn\n";
	if( fabs(fSegments[iSeg].y_dn - fSegments[jSeg].y_dn) < 5 && fabs(fSegments[iSeg].y_up - fSegments[jSeg].y_up) < 5 ) {
//	  std::cout<<" ----- Catch y_dn";
	  float r_lim = (fSegments[iSeg].Cr + fSegments[jSeg].Cr)/2;
	  r_lim *= 0.23;
	  if( fabs(fSegments[iSeg].Cr - fSegments[jSeg].Cr) < r_lim ) {
//	    std::cout<<" -----> Catch R\n";
//	    if( !fSegments[iSeg].isUsed ) {
//	      fSegments[iSeg].isUsed = true;
//	      fSegments[iSeg].iLooper = fNLoopers;
//	    }
	    fSegments[jSeg].isUsed = true;
	    fSegments[jSeg].iLooper = fSegments[iSeg].iLooper;
	    loopers[fSegments[iSeg].iLooper]++;
//	    fNLoopers++;
//	    disp.SpecDrawRecoTrackGlobal( fSegments[iSeg].iTr, kBlue, 0.3 );
//	    disp.SpecDrawRecoTrackGlobal( fSegments[jSeg].iTr, kBlue, 0.3 );
//	    disp.Ask();
	  }
	}
      }
    }
//    std::cout<<">>> fNLoopers: "<<fNLoopers<<"\n";
//    disp.Ask();
  }
#if 0
  for( int i = 0; i < fNLoopers; i++ ) {
    std::cout<<"> looper: "<<i<<";   nSegments: "<<loopers[i]<<"\n";
    if( loopers[i] < 2 ) continue;
//    disp.ClearView();
    disp.DrawTPC();
    bool dr = false;
    for( int iSeg = 0; iSeg < fSegments.size(); iSeg++ ) {
      if( fSegments[iSeg].iLooper != i ) continue;
      std::cout<<" --- iSeg: "<<iSeg<<";   Cx: "<<fSegments[iSeg].Cx<<";   Cy: "<<fSegments[iSeg].Cy<<";   Cr: "<<fSegments[iSeg].Cr<<"\n";
      if( !dr ) {
	disp.SpecDrawMCTrackHitsGlobal( (*fMCTracks)[fSegments[iSeg].iMC], fLocalMCPoints, fHitLabels, kRed, 0.5 );
	dr = true;
      }
      disp.DrawHitGlobal( fSegments[iSeg].Cx, fSegments[iSeg].Cy, fSegments[iSeg].z_dn, kBlack, 0.5 );
      disp.DrawHitGlobal( fSegments[iSeg].x_dn, fSegments[iSeg].y_dn, fSegments[iSeg].z_dn, kGreen, 0.75 );
      disp.DrawHitGlobal( fSegments[iSeg].x_up, fSegments[iSeg].y_up, fSegments[iSeg].z_up, kOrange, 0.75 );
//      disp.DrawCircle( fSegments[iSeg].Cx, fSegments[iSeg].Cy, fSegments[iSeg].Cr, kBlack, 1.5 );
      disp.SpecDrawRecoTrackGlobal( fSegments[iSeg].iTr, kBlue, 0.3 );
    }
    disp.SaveCanvasToFile( "DrawLoopersTest.pdf" );
    disp.Ask();
  }
//  disp.SaveCanvasToFile( "DrawLoopersTest.pdf" );
//  disp.Ask();
#endif
}
#endif

#endif //DO_TPCCATRACKER_EFF_PERFORMANCE
