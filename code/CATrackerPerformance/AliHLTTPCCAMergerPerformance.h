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
#if 0//def DO_TPCCATRACKER_EFF_PERFORMANCE outdated

#ifndef AliHLTTPCCAMergerPerformance_H
#define AliHLTTPCCAMergerPerformance_H


#include "AliHLTTPCCASlicePerformance.h"


#include "AliHLTTPCCAMCTrack.h"
#include "AliHLTTPCCAMCPoint.h"
#include <fstream>
#include <cstdio>
#include <map>

#ifndef HLTCA_STANDALONE
#include "TTree.h"
#include "TObject.h"
#include "TClonesArray.h"
#endif

class TObject;
class TParticle;
class AliHLTTPCCAMCPoint;
class AliHLTTPCCAGBTracker;
class TDirectory;
class TH1D;
class TH2D;
class TProfile;

class TFile;
class TBranch;
class AliHLTTPCCATracker;

/**
 * @class AliHLTTPCCAMergerPerformance
 */

#ifndef HLTCA_STANDALONE
 class MergerPerfDataEvent: public TObject{
 public:
  MergerPerfDataEvent() {};
  virtual ~MergerPerfDataEvent() 
  {
    if(MergerData) delete MergerData;
  }

 friend class AliHLTTPCCAMergerPerformance;
 
 protected:
  TClonesArray *MergerData;
  
  //ClassDef(MergerPerfDataEvent,3)
};
 
class MergerPerfData: public TObject{
 public:
  MergerPerfData() {};
  virtual ~MergerPerfData() {};

  bool IsMerged;
  bool IsDzDs;              // = 1 if dz/ds the same
  bool IsSameTrack;         // = 1 if tracks originated from the same MC track
  bool IsCovMatrixPositive;  // = 1 if the diagonal elements > 0
  bool IsCovMatrixFinite;   // = 1 if the elements are not huge ()
  float chi2;
  float delta2; 
  float Si;
  float dy;
  float dz;
  float dsin;
  
  float nClust1;
  float nClust2;
    
  //ClassDef(MergerPerfData,4)
};
#endif // HLTCA_STANDALONE
 
class AliHLTTPCCAMergerPerformance: public AliHLTTPCCATrackPerformanceBase
{
  public:

    AliHLTTPCCAMergerPerformance();
    virtual ~AliHLTTPCCAMergerPerformance()
    { 
      if(fMPDE) delete fMPDE;
    };

    virtual void SetNewEvent(const AliHLTTPCCAGBTracker * const Tracker,
                             AliHLTResizableArray<AliHLTTPCCAHitLabel> *hitLabels,
                             AliHLTResizableArray<AliHLTTPCCAMCTrack> *mcTracks,
                             AliHLTResizableArray<AliHLTTPCCALocalMCPoint> *localMCPoints);
    
      /// Calculate Efficiencies
    virtual void Exec(bool print = false);

#ifndef HLTCA_STANDALONE
      /// Histograms
    virtual void CreateHistos(string histoDir = "", TFile* outFile = 0);
    virtual void FillHistos();
    
    void FillTree();
#endif
  
    void AddMergerData(int step, int iSlice1, int iTrack1, int iSlice2, int iTrack2, bool IsDzDs, bool IsCovMatrixPositiv, 
                       bool IsCovMatrixFinite, float &chi2, float &delta2, float &Si, float &dy, float &dz, float &dsin, int nClust1, int nClust2);
    void SetMerged(int step);
    void FillMC();
    
  private:
    int iData;
    int iData2Step;
    vector<AliHLTTPCCASlicePerformance*> slicePerformances;
    bool first_call;
#ifndef HLTCA_STANDALONE
    TTree *MPDTree; // Merger Performance Data tree
    TTree *MPDTree2Step;
    TClonesArray *MergerData;
    MergerPerfDataEvent *fMPDE;
    MergerPerfDataEvent *fMPDE2Step;
#endif
};

#endif
#endif //DO_TPCCATRACKER_EFF_PERFORMANCE
