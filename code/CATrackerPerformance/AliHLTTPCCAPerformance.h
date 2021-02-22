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

#ifndef ALIHLTTPCCAPERFORMANCE_H
#define ALIHLTTPCCAPERFORMANCE_H

#include "AliHLTTPCPerformanceBase.h"

// #include "AliHLTTPCCADef.h"
#include "AliHLTArray.h"
#include "AliHLTTPCCAMCTrack.h"
#include "AliHLTTPCCAMCPoint.h"

#include <fstream>
#include <cstdio>
#include <map>
#include <string>
#include <vector>
using std::string;

class TObject;
class TParticle;
class AliHLTTPCCAMCPoint;
class AliHLTTPCCAGBTracker;
class KFParticleTopoReconstructor;
class TDirectory;
class TH1D;
class TH2D;
class TProfile;

/**
 * @class AliHLTTPCCAPerformance
 *
 * Does performance evaluation of the HLT Cellular Automaton-based tracker
 * It checks performance for AliHLTTPCCATracker slice tracker
 * and for AliHLTTPCCAGBTracker global tracker
 *
 */

class AliHLTTPCCAPerformance
{
  public:

    AliHLTTPCCAPerformance();
    virtual ~AliHLTTPCCAPerformance();

      /// initialization before the new event
    bool SetNewEvent(AliHLTTPCCAGBTracker* const Tracker, string mcTracksFile, string mcPointsFile); // set info for new event
    void InitSubPerformances();

      /// Instance
    static AliHLTTPCCAPerformance &Instance();
    
      /// Efficiencies
    void ExecPerformance();

    /// functional is needed by DRAW option. TODO: clean up
  const AliHLTTPCCAMCTrack &MCTrack(int i) const { return fMCTracks[i]; }
  const AliHLTTPCCAHitLabel &HitLabel(int i) const { return fHitLabels[i]; }
  int HitLabelSize() { return fHitLabels.Size(); }
  const AliHLTTPCCAGBTracker *GetTracker(){ return fTracker; };
    

  void SetTracker( AliHLTTPCCAGBTracker* const tracker ){ fTracker = tracker; };
#ifdef KFPARTICLE
  void SetTopoReconstructor( KFParticleTopoReconstructor* const tr ){ fTopoReconstructor = tr; };
#endif
  void SetMCTracks(vector<AliHLTTPCCAMCTrack>& mcTracks);
  void SetMCPoints(vector<AliHLTTPCCALocalMCPoint>& mcPoints);
  void SetHitLabels(vector<AliHLTTPCCAHitLabel>& hitLabels);

  AliHLTResizableArray<AliHLTTPCCAHitLabel>     * GetHitLabels() { return &fHitLabels; }      // array of hit MC labels
  AliHLTResizableArray<AliHLTTPCCAMCTrack>      * GetMCTracks()  { return &fMCTracks;  }      // array of MC tracks
  AliHLTResizableArray<AliHLTTPCCALocalMCPoint> * GetMCPoints()  { return &fLocalMCPoints;}   // array of MC points in slices CS

  AliHLTTPCPerformanceBase* GetSubPerformance(string name);
  bool CreateHistos(string name);
  void WriteHistos();
  
  void SetOutputFile(TFile *oF) { fOutputFile = oF; }

  void SaveDataInFiles( string prefix ) const; // Save all MC Data in txt files. @prefix - prefix for file name. Ex: "./data/ev1"
  bool ReadDataFromFiles( string prefix ); // @prefix - prefix for file name. Ex: "./data/ev1"

  void SetRecoData(vector<int> &mcIndexes); //for KFTopoPerformance running without CATracker
  
  protected:

          /// Histograms
    void CreateHistos();
    
          /// Read\write MC information
    void ReadMCEvent( FILE *in );
    void ReadLocalMCPoints( FILE *in );

    void WriteMCEvent( FILE *out ) const;

      /// Sub-pefromances
    struct TSubPerformance{
      AliHLTTPCPerformanceBase* perf;
      string name;
      bool IsGlobalPerf;

      TSubPerformance(){};
      TSubPerformance(AliHLTTPCPerformanceBase* perf_, string name_, bool IsGlobalPerf_ = 1){
        perf = perf_;
        name = name_;
	IsGlobalPerf = IsGlobalPerf_;
#ifndef HLTCA_STANDALONE
	perf->SetHistoCreated(0);
#endif
      };
//       ~TSubPerformance(){if (perf) delete perf;};
      
      AliHLTTPCPerformanceBase& operator*(){return *perf;}
      AliHLTTPCPerformanceBase* operator->(){return perf;}
    };
    vector<TSubPerformance> subPerformances;
//     vector<AliHLTTPCCAPerformanceBase*> subPerformances;
    
    const AliHLTTPCCAGBTracker *fTracker; // pointer to the tracker
#ifdef KFPARTICLE
    const KFParticleTopoReconstructor* fTopoReconstructor;
#endif
       /// MC information
    AliHLTResizableArray<AliHLTTPCCAHitLabel> fHitLabels; // array of hit MC labels
    AliHLTResizableArray<AliHLTTPCCAMCTrack> fMCTracks;   // array of MC tracks
    AliHLTResizableArray<AliHLTTPCCALocalMCPoint> fLocalMCPoints;   // array of MC points in slices CS


    int fStatNEvents; // n of events proceed

    TFile *fOutputFile;
    TDirectory *fHistoDir; // ROOT directory with histogramms
  
  private:
    void WriteDir2Current( TObject *obj );
  
    AliHLTTPCCAPerformance( const AliHLTTPCCAPerformance& );
    AliHLTTPCCAPerformance &operator=( const AliHLTTPCCAPerformance& );


 public:
#ifndef HLTCA_STANDALONE
  void ShiftHitsToMC(); // Use spreaded MCposition instead of hits
  void ResimulateHits(); // Create new hits spreading MCPositions
#endif // HLTCA_STANDALONE
  void RematchHits();// Match hits with closest MCPoint
};

#endif
#endif //DO_TPCCATRACKER_EFF_PERFORMANCE
