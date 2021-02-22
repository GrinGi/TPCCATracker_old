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
#include "AliHLTTPCCASlicesPerformance.h"


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
#endif // HLTCA_STANDALONE

void AliHLTTPCCASlicesPerformance::SetNewEvent(const AliHLTTPCCAGBTracker * const tracker,
                            AliHLTResizableArray<AliHLTTPCCAHitLabel> *hitLabels,
                            AliHLTResizableArray<AliHLTTPCCAMCTrack> *mcTracks,
                            AliHLTResizableArray<AliHLTTPCCALocalMCPoint> *localMCPoints)
{
  AliHLTTPCCATrackPerformanceBase::SetNewEvent(tracker, hitLabels, mcTracks, localMCPoints);

    /// Init subperformances
  if (fFirstCall){
    if(fTracker)
      slicePerformances.resize(fTracker->NSlices());
    else
      slicePerformances.resize(0);
    for (unsigned int iPerf = 0; iPerf < slicePerformances.size(); iPerf++){
      slicePerformances[iPerf] = new AliHLTTPCCASlicePerformance(iPerf);
    }
  }
  fFirstCall = false;
  
  for (unsigned int iPerf = 0; iPerf < slicePerformances.size(); iPerf++){
    slicePerformances[iPerf]->SetNewEvent(fTracker, fHitLabels, fMCTracks, fLocalMCPoints);
  }

} // void AliHLTTPCCASlicesPerformance::SetNewEvent

#ifndef HLTCA_STANDALONE
void AliHLTTPCCASlicesPerformance::CreateHistos(string histoDir, TFile* outFile)
{
  
  for (unsigned int iPerf = 0; iPerf < slicePerformances.size(); iPerf++){
    // slicePerformances[iPerf]->CreateHistos( histoDir + (string)TString(char(iPerf)), outFile ); // just set diff names, they anyway won't be written
    slicePerformances[iPerf]->CreateHistos( "", 0 );
  }
  
  AliHLTTPCCATrackPerformanceBase::CreateHistos(histoDir, outFile);
}
#endif

void AliHLTTPCCASlicesPerformance::Exec(bool print)
{
  for (unsigned int iPerf = 0; iPerf < slicePerformances.size(); iPerf++){
    slicePerformances[iPerf]->Exec(0);
  }
  AliHLTTPCCATrackPerformanceBase::Exec(print);
} // void AliHLTTPCCASlicesPerformance::Exec

void AliHLTTPCCASlicesPerformance::EfficiencyPerformance()
{
  for (unsigned int iPerf = 0; iPerf < slicePerformances.size(); iPerf++){
    fEff += slicePerformances[iPerf]->GetEff();
  }

  AliHLTTPCCATrackPerformanceBase::EfficiencyPerformance();
} // void AliHLTTPCCASlicesPerformance::EfficiencyPerformance()

#ifndef HLTCA_STANDALONE
void AliHLTTPCCASlicesPerformance::FillHistos()
{
  for (int iH = 0; iH < NHisto; iH++){
    for (unsigned int iPerf = 0; iPerf < slicePerformances.size(); iPerf++){
        // move all data
      fHistos[iH]->Add(slicePerformances[iPerf]->fHistos[iH]);
      slicePerformances[iPerf]->fHistos[iH]->Reset();
    }
  }
} // void AliHLTTPCCASlicesPerformance::FillHistos()
#endif
#endif //DO_TPCCATRACKER_EFF_PERFORMANCE
