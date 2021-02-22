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

#include "AliHLTTPCPerformanceBase.h"
#include "AliHLTTPCCAGBHit.h"
#include "AliHLTTPCCAMCTrack.h"
#include "AliHLTTPCCAMCPoint.h"
#include "AliHLTTPCCAOutTrack.h"
#include "AliHLTTPCCAGBTrack.h"
#include "AliHLTTPCCAGBTracker.h"
#include "AliHLTTPCCATracker.h"
#include "AliHLTTPCCATracklet.h"

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
#endif // HLTCA_STANDALONE


#include <string>
using std::string;

void AliHLTTPCPerformanceBase::SetNewEvent( const AliHLTTPCCAGBTracker * const tracker,
                                AliHLTResizableArray<AliHLTTPCCAHitLabel> *hitLabels,
                                AliHLTResizableArray<AliHLTTPCCAMCTrack> *mcTracks,
                                AliHLTResizableArray<AliHLTTPCCALocalMCPoint> *localMCPoints)
{
  fTracker = tracker;
  
  fHitLabels = hitLabels;
  fMCTracks = mcTracks;
  fLocalMCPoints = localMCPoints;
  
  nMCTracks = (*fMCTracks).Size();

  fEff = AliHLTTPCEfficiencies();
  mcData.resize(0);
  recoData.resize(0);
} // void AliHLTTPCPerformanceBase::SetNewEvent

AliHLTTPCPerformanceBase::AliHLTTPCPerformanceBase():
  fStatNEvents(0),
#ifndef HLTCA_STANDALONE
  NHisto(0),
#endif
  fTracker(0),fHitLabels(0),fMCTracks(0),fLocalMCPoints(0),nRecoTracks(0),nMCTracks(0)
#ifndef HLTCA_STANDALONE
  ,fHistoDir(0)
#endif
{

}

AliHLTTPCPerformanceBase::~AliHLTTPCPerformanceBase()
{
#ifndef HLTCA_STANDALONE
  if (fHistoDir == 0) // don't write in file
    for( int i = 0; i < NHisto; i++ ){
      if (fHistos[i]) delete fHistos[i];
    }
#endif
}

void AliHLTTPCPerformanceBase::Exec( bool PrintFlag )
{
    // Efficiency
  CheckMCTracks();
  MatchTracks();
  EfficiencyPerformance();
  if (PrintFlag) PrintEfficiency();

#ifndef HLTCA_STANDALONE
    // Histos
  FillHistos();

  Draw();
#endif
  
  fStatNEvents++;
} // Exec


void AliHLTTPCPerformanceBase::EfficiencyPerformance() // TODO add common parts of code
{
  fEff.IncNEvents();
  fEffStat += fEff;
}

#ifndef HLTCA_STANDALONE
TH1 *AliHLTTPCPerformanceBase::GetHisto( int iHisto )
{
  
//   assert ( (iHisto != NHisto) || (string("") == string(" wrong histo name ")) );
  if ( (iHisto >= NHisto) || (iHisto<0) ){
    cout << "ERROR: wrong histo number: " << iHisto << endl;
    exit(1);
  }
  
  return fHistos[iHisto];
}
#endif

#endif //DO_TPCCATRACKER_EFF_PERFORMANCE

