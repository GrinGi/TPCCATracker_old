// **************************************************************************
// This file is property of and copyright by the ALICE HLT Project          *
// ALICE Experiment at CERN, All rights reserved.                           *
//                                                                          *
// Primary Authors: Sergey Gorbunov <sergey.gorbunov@kip.uni-heidelberg.de> *
//                  Ivan Kisel <kisel@kip.uni-heidelberg.de>                *
//                  for The ALICE HLT Project.                              *
//                                                                          *
// Developed by:   Igor Kulakov <I.Kulakov@gsi.de>                          *
//                 Maksym Zyzak <M.Zyzak@gsi.de>                            *
//                                                                          *
// Permission to use, copy, modify and distribute this software and its     *
// documentation strictly for non-commercial purposes is hereby granted     *
// without fee, provided that the above copyright notice appears in all     *
// copies and that both the copyright notice and this permission notice     *
// appear in the supporting documentation. The authors make no claims       *
// about the suitability of this software for any purpose. It is            *
// provided "as is" without express or implied warranty.                    *
//                                                                          *
//***************************************************************************

#include <AliHLTTPCCAGBTracker.h>
#ifdef KFPARTICLE
#include "KFParticleTopoReconstructor.h"
#ifndef HLTCA_STANDALONE
#include "KFTopoPerformance.h"
#endif
#include <KFPInputData.h>

#include "KFParticle.h"
#include "KFParticleSIMD.h"

#endif

#ifdef MAIN_DRAW
#include "AliHLTTPCCADisplay.h"
#endif

#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstring>

using namespace std;

#ifndef NVALGRIND
#include <valgrind/callgrind.h>
#endif

#include "AliHLTTPCCAPerformance.h"

#ifndef HLTCA_STANDALONE
#include "TFile.h"
#endif

// #undef WITHSCIF

static bool SAVE = false;
extern bool SINGLE_THREADED;
extern bool DRAW_EVERY_LINK;

static bool file_exists( const char *filename )
{
    FILE *f = 0;
    if ( ( f = std::fopen( filename, "r" ) ) != NULL ) {
        std::fclose( f );
        return true;
    }
    return false;
}


static void usage(const char *execName)
{
  std::cout << "Usage: " << execName << " [OPTION] [event number]\n"
     "Reconstruct slice tracks and merge them from cluster data read from the Events directory.\n\n"
     "  -h, --help print this message\n"
#ifdef MAIN_DRAW
     "  -links     let it draw every vector of links the NeighboursFinder found.\n"
#else
     "  -single    force tracker to run on only one core. Per default all available cores will be used\n"
#endif
     "  -save      dump result of the tracker/merger into a file for later analysis\n"
#ifndef HLTCA_STANDALONE
     "  -perf      do a performance analysis against Monte-Carlo information right after reconstruction\n\n"
#endif
     "You may specify a specific event to reconstruct if there is more than one available in the Events directory"
     << std::endl;
}

int main(int argc, char **argv)
{
  bool fullTiming = false;

  AliHLTTPCCAPerformance *perf = 0;
#ifndef HLTCA_STANDALONE
  TFile* perfHistoFile = new TFile("HLTTPCCATrackerPerformance.root","RECREATE");
#endif
  
#ifdef MAIN_DRAW
#ifdef DRAW
  AliHLTTPCCADisplay::Instance().SetDrawType( 1 );
#endif
#ifdef DRAW2
  AliHLTTPCCADisplay::Instance().SetDrawType( 2 );
#endif
#ifdef DRAW3
  AliHLTTPCCADisplay::Instance().SetDrawType( 3 );
#endif
#endif // MAIN_DRAW
  
  int firstEvent = 0;
  int lastEvent = 1000000;
  int iHLT = 0;
  int iXeonPhi = 1;
  string filePrefix = "./Events/"; 
  for( int i=1; i < argc; i++ ){
    if ( !std::strcmp( argv[i], "-h" ) || !std::strcmp( argv[i], "--help" ) || !std::strcmp( argv[i], "-help" ) ) {
      usage(argv[0]);
      return 0;
#ifdef DRAW
    } else if ( !std::strcmp( argv[i], "-links" ) ) {
      DRAW_EVERY_LINK = true;
#else
    } else if ( !std::strcmp( argv[i], "-single" ) ) {
      SINGLE_THREADED = true;
#endif
    } else if ( !std::strcmp( argv[i], "-save" ) ) {
      SAVE = true;
#ifndef HLTCA_STANDALONE
    } else if ( !std::strcmp( argv[i], "-perf" ) ) {
      perf = &AliHLTTPCCAPerformance::Instance();
#endif
    } else if ( !std::strcmp( argv[i], "-time" ) ) {
      fullTiming = true;
    } else if ( !std::strcmp( argv[i], "-dir" ) && ++i < argc ) {
      filePrefix = argv[i];
    } else if ( !std::strcmp( argv[i], "-HLT" ) && ++i < argc ) {
      iHLT = atoi( argv[i] );
    } else if ( !std::strcmp( argv[i], "-XeonPhi" ) && ++i < argc ) {
      iXeonPhi = atoi( argv[i] );
      if( !(iXeonPhi == 1 || iXeonPhi == 2)  )
      {
        std::cout << "ERROR! Index of the Xeon Phi card is incorrect. It can take values '1' or '2'. The default value of '1' is set." << std::endl;
        iXeonPhi = 1;
      }
    } else if ( !std::strcmp( argv[i], "-ev" ) && ++i < argc ) {
      firstEvent = atoi( argv[i] );
      if (++i < argc){
        lastEvent = atoi( argv[i] );
        std::cout << "Only process events " << firstEvent << ",..," << lastEvent << std::endl;
      }
      else{
        lastEvent = firstEvent;
        std::cout << "Only process event " << lastEvent << std::endl;
      }
    } else {
      firstEvent = atoi( argv[i] );
      lastEvent = firstEvent;
      std::cout << "Only process event " << lastEvent << std::endl;
    }
    
  }
#ifndef HLTCA_STANDALONE
#ifdef MAIN_DRAW
  SINGLE_THREADED = true;
  perf = &AliHLTTPCCAPerformance::Instance();
#endif
#endif

  AliHLTTPCCAGBTracker *tracker = 0;
  const AliHLTTPCCAGBTracker *trackerConst = 0;

  tracker = new AliHLTTPCCAGBTracker;

  filePrefix += "/";
  tracker->ReadSettingsFromFile(filePrefix);
  trackerConst = tracker;

#ifdef WITHSCIF
  //check if the node with index iXeonPhi exists
//   scif_get_nodeIDs(uint16_t* \*'nodes'*, int* 'len'*, uint16_t* \*'self'*);
  scif_epd_t endpoint;
  const uint16_t portSenderId = 2000 + iHLT;
  struct scif_portID portRecieverId;
  portRecieverId.node = 0;//iXeonPhi;
  portRecieverId.port = 3000 + iHLT;
  
  endpoint = scif_open( );
  if( endpoint == SCIF_OPEN_FAILED ) 
  {
    std::cout << "Fail openning SCIF endpoint." << std::endl;
    return 0 ;
  }
  
  int ret = 0;
  
  ret = scif_bind(endpoint, portSenderId);
  if( ret == -1 )
  {
    std::cout << "Fail binding SCIF. " << std::endl;
    return 0;
  }
  
  ret = -1;
  while( ret == -1 )
  {
    std::cout << " Connecting to the Xeon Phi card. " << std::endl; 
    sleep(1);
    ret = scif_connect(endpoint, &portRecieverId);
  }
  
  std::cout << "Connected!!!! " << std::endl;

  float Bz = tracker->Slice(0).Param().Bz();
  scif_send(endpoint, &Bz, sizeof(Bz), SCIF_SEND_BLOCK); // synchronization
  
  int runnumber = 0;
  scif_send(endpoint, &runnumber, sizeof(runnumber), SCIF_SEND_BLOCK); // synchronization
  
  off_t offsetServer;
  scif_recv(endpoint, &offsetServer, sizeof(offsetServer), 1);
  int maxEventSize = 0;
  scif_recv(endpoint, &maxEventSize, sizeof(maxEventSize), 1);
  
  //create and register the host buffer 
  void* buffer;
  posix_memalign(&buffer, 0x1000, maxEventSize);
  off_t offsetSender = scif_register(endpoint, buffer, maxEventSize, 0, SCIF_PROT_READ | SCIF_PROT_WRITE,0);
  if( offsetSender < -1 ) { std::cout << "Fail registerring SCIF." << std::endl; return 0; }  

  //synchronize with the XeonPhi
  int controlSignal = portSenderId;
  scif_send(endpoint, &controlSignal, sizeof(int), SCIF_SEND_BLOCK); // synchronization
#endif //WITHSCIF
  
  for ( int kEvents = firstEvent; kEvents <= lastEvent; kEvents++ ) {
    char buf[6];
    sprintf( buf, "%d", kEvents );
    const string fileName = filePrefix + "event" + string(buf) + "_";

    std::cout << "Loading Event " << kEvents << "..." << std::endl;
    if (!tracker->ReadHitsFromFile(fileName)) {
        cout << "Hits Data for Event " << kEvents << " can't be read." << std::endl;
        break;
    }
    std::cout << "Event " << kEvents << " CPU reconstruction..." << std::endl;

#ifdef KFPARTICLE
    KFParticleTopoReconstructor* topoReconstructor = new KFParticleTopoReconstructor(); // TODO don't recreate with each event
#endif

    if ( perf ) {
#ifndef HLTCA_STANDALONE
      perf->SetOutputFile(perfHistoFile);
#endif
      perf->SetTracker(tracker);
#ifdef KFPARTICLE
      perf->SetTopoReconstructor(topoReconstructor);
#endif

#ifndef HLTCA_STANDALONE
      std::cout << "Loading Monte-Carlo Data for Event " << kEvents << "..." << std::endl;
      if (!perf->ReadDataFromFiles(fileName)) {
        cout << "Monte-Carlo Data for Event " << kEvents << " can't be read. Program proceeds with out MC." << std::endl;
      }
        // perf->ShiftHitsToMC();
        // perf->ResimulateHits();
#endif //HLTCA_STANDALONE

    }
    
    tracker->FindTracks();

#if 0
    tracker->WriteTracks(fileName);
#endif // 0

#ifndef HLTCA_STANDALONE
    if ( perf ) {
      if (trackerConst->NHits() > 0) {
        perf->InitSubPerformances();
        perf->ExecPerformance();
      }
      else {
        cout << "Event " << kEvents << " contains 0 hits." << std::endl;
      }
    }
#endif

#ifdef KFPARTICLE

#ifdef USE_TIMERS
    Stopwatch timer;
    timer.Start();
#endif // USE_TIMERS

    vector<int> pdg(tracker->NTracks(), -1);
#ifndef HLTCA_STANDALONE
    if ( perf ) {
      if ( AliHLTTPCCAPerformance::Instance().GetSubPerformance("Global Performance")->fMCTracks->Size() > 0 )
      for(unsigned int iTr=0; iTr<tracker->NTracks(); iTr++)
      {
        int iMC = AliHLTTPCCAPerformance::Instance().GetSubPerformance("Global Performance")->GetRecoData()[iTr].GetMCTrackId();
        int PDG = (*AliHLTTPCCAPerformance::Instance().GetSubPerformance("Global Performance")->fMCTracks)[iMC].PDG();
        if( PDG==0)
          PDG = -1;
        pdg[iTr] = PDG;
      }
    }
#endif //HLTCA_STANDALONE

#if 0
    {
      char buf[6];
      sprintf( buf, "%d", kEvents );
      const std::string fileName =  "KFPData/event" + std::string(buf) + "_KFPTracks.data";

      KFPInputData InputDataPerThread;

      if (!InputDataPerThread.ReadDataFromFile(fileName)) {
        cout << "Data for Event " << kEvents << " can't be read." << std::endl;
        break;
      }    
      KFParticle::SetField( -InputDataPerThread.GetBz() ); // to understand -1 see SetField
      KFParticleSIMD::SetField( -InputDataPerThread.GetBz() ); // to understand -1 see SetField
    
      topoReconstructor->Init(InputDataPerThread.GetTracks(),
                              InputDataPerThread.GetPV(),
                              InputDataPerThread.GetCluster()
                             );
      
      topoReconstructor->ReconstructParticles();
    }
#else

    topoReconstructor->Init( tracker,&pdg );
//     topoReconstructor->Init( tracker);

//   
    topoReconstructor->ReconstructPrimVertex();
    topoReconstructor->SortTracks();
    if(SAVE)
      topoReconstructor->SaveInputParticles();
#ifdef WITHSCIF
    topoReconstructor->SendDataToXeonPhi(iHLT, endpoint, buffer, offsetServer, offsetSender, Bz);
#else
    topoReconstructor->ReconstructParticles();
#endif //WITHSKIF
#endif
#ifdef USE_TIMERS
    timer.Stop();
    topoReconstructor->SetTime(timer.RealTime());
#endif // USE_TIMERS

#ifndef WITHSCIF
#ifndef HLTCA_STANDALONE
    if ( perf ) {
      if (trackerConst->NHits() > 0) {
        KFTopoPerformance* tp = (KFTopoPerformance*) (perf->GetSubPerformance("Topo Performance"));
        tp->SetPrintEffFrequency(100);
        perf->GetSubPerformance("Topo Performance")->Exec(0);
      }
    }
#endif
#endif

    topoReconstructor->DeInit();
#endif

    const bool ifAverageTime = 1;
    if (!ifAverageTime){
        
      std::cout << "Reconstruction Time"
        << " Real = " << std::setw( 10 ) << (trackerConst->SliceTrackerTime() + trackerConst->StatTime( 9 )) * 1.e3 << " ms,"
        << " CPU = " << std::setw( 10 ) << (trackerConst->SliceTrackerCpuTime() + trackerConst->StatTime( 10 )) * 1.e3 << " ms,"
        << " parallelization speedup (only SectorTracker): " << trackerConst->SliceTrackerCpuTime() / trackerConst->SliceTrackerTime()
        << std::endl;
      if ( fullTiming ) {
        std::cout
          << " |  ------ Sector trackers (w\\o init): " << std::setw( 10 ) << trackerConst->StatTime( 0 ) * 1000. << " ms\n"
          << " |      Initialization: "   << std::setw( 10 )  << trackerConst->StatTime( 12 ) * 1000. << " ms\n"
          << " |    NeighboursFinder: " << std::setw( 10 ) << trackerConst->StatTime( 1 ) * 1000. << " ms, " << std::setw( 12 ) << trackerConst->StatTime( 5 ) << " cycles\n"
          << " |   NeighboursCleaner: " << std::setw( 10 ) << trackerConst->StatTime( 11 ) * 1000. << " ms\n"
          << " |     StartHitsFinder: " << std::setw( 10 ) << trackerConst->StatTime( 4 ) * 1000. << " ms\n"
          << " | TrackletConstructor: " << std::setw( 10 ) << trackerConst->StatTime( 2 ) * 1000. << " ms, " << std::setw( 12 ) << trackerConst->StatTime( 7 ) << " cycles\n"
          << " |    TrackletSelector: " << std::setw( 10 ) << trackerConst->StatTime( 3 ) * 1000. << " ms, " << std::setw( 12 ) << trackerConst->StatTime( 8 ) << " cycles\n"
          << " |         WriteOutput: " << std::setw( 10 ) << trackerConst->StatTime( 6 ) * 1000. << " ms\n"
          << " |  -------------------------   Merge: " << std::setw( 10 ) << trackerConst->StatTime( 9 ) * 1000. << " ms\n"
          << " |      Initialization: " << std::setw( 10 )  << trackerConst->StatTime( 13 ) * 1000. << " ms\n"
          << " | -- NoOverlapTrackMerge: " << std::setw( 10 )  << trackerConst->StatTime( 14 ) * 1000. << " ms\n"
          << " |               Merge: " << std::setw( 10 )  << trackerConst->StatTime( 16 ) * 1000. << " ms\n"
          << " |           DataStore: " << std::setw( 10 )  << trackerConst->StatTime( 18 ) * 1000. << " ms\n"
          << " | ---- OverlapTrackMerge: " << std::setw( 10 )  << trackerConst->StatTime( 15 ) * 1000. << " ms\n"
          << " |               Merge: " << std::setw( 10 )  << trackerConst->StatTime( 17 ) * 1000. << " ms\n"
          << " |           DataStore: " << std::setw( 10 )  << trackerConst->StatTime( 19 ) * 1000. << " ms\n"
          ;
      } 

#ifdef KFPARTICLE
#ifdef USE_TIMERS
      std::cout << "Topo reconstruction time"
        << " Real = " << std::setw( 10 ) << topoReconstructor->Time() * 1.e3 << " ms" << std::endl;
      if ( fullTiming ) {
        std::cout << "    Init                " << topoReconstructor->StatTime(0) * 1.e3 << " ms" << std::endl;
        std::cout << "    PV Finder           " << topoReconstructor->StatTime(1) * 1.e3 << " ms" << std::endl;
        std::cout << "    Sort Tracks         " << topoReconstructor->StatTime(2) * 1.e3 << " ms" << std::endl;
        std::cout << "    KF Particle Finder  " << topoReconstructor->StatTime(3) * 1.e3 << " ms" << std::endl;
      }
#endif // USE_TIMERS
#endif
    }
    else{
      const int NTimers = trackerConst->NTimers();
      static int statIEvent = 0;
      static double *statTime = new double[NTimers];
      static double statTime_SliceTrackerTime = 0;
      static double statTime_SliceTrackerCpuTime = 0;

      if (!statIEvent){
        for (int i = 0; i < NTimers; i++){
          statTime[i] = 0;
        }
      }

      statIEvent++;
      for (int i = 0; i < NTimers; i++){
        statTime[i] += trackerConst->StatTime( i );
      }
      statTime_SliceTrackerTime += trackerConst->SliceTrackerTime();
      statTime_SliceTrackerCpuTime += trackerConst->SliceTrackerCpuTime();
      
        
      std::cout << "Reconstruction Time"
          << " Real = " << std::setw( 10 ) << 1./statIEvent*(statTime_SliceTrackerTime+statTime[ 9 ]) * 1.e3 << " ms,"
          << " CPU = " << std::setw( 10 ) << 1./statIEvent*(statTime_SliceTrackerCpuTime+statTime[ 10 ]) * 1.e3 << " ms,"
          << " parallelization speedup (only SectorTracker): " << statTime_SliceTrackerCpuTime / statTime_SliceTrackerTime
          << std::endl;
      if ( fullTiming ) {
        std::cout
            << " |  ------ Sector trackers (w\\o init): " << std::setw( 10 ) << 1./statIEvent*statTime[ 0 ] * 1000. << " ms\n"
            << " |      Initialization: " << std::setw( 10 )  << 1./statIEvent*statTime[ 12 ] * 1000. << " ms\n"
            << " |    NeighboursFinder: " << std::setw( 10 ) << 1./statIEvent*statTime[ 1 ] * 1000. << " ms, " << std::setw( 12 ) << 1./statIEvent*statTime[ 5 ] << " cycles\n"
            << " |   NeighboursCleaner: " << std::setw( 10 ) << 1./statIEvent*statTime[ 11 ] * 1000. << " ms\n"
            << " |     StartHitsFinder: " << std::setw( 10 ) << 1./statIEvent*statTime[ 4 ] * 1000. << " ms\n"
            << " | TrackletConstructor: " << std::setw( 10 ) << 1./statIEvent*statTime[ 2 ] * 1000. << " ms, " << std::setw( 12 ) << 1./statIEvent*statTime[ 7 ] << " cycles\n"
            << " |    TrackletSelector: " << std::setw( 10 ) << 1./statIEvent*statTime[ 3 ] * 1000. << " ms, " << std::setw( 12 ) << 1./statIEvent*statTime[ 8 ] << " cycles\n"
            << " |         WriteOutput: " << std::setw( 10 ) << 1./statIEvent*statTime[ 6 ] * 1000. << " ms\n"
            << " |  --------------------------  Merge: " << std::setw( 10 ) << 1./statIEvent*statTime[ 9 ] * 1000. << " ms\n"
            << " |      Initialization: " << std::setw( 10 )  << 1./statIEvent*statTime[ 13 ] * 1000. << " ms\n"
            << " | -- NoOverlapTrackMerge: " << std::setw( 10 )  << 1./statIEvent*statTime[ 14 ] * 1000. << " ms\n"
            << " |               Merge: " << std::setw( 10 )  << 1./statIEvent*statTime[ 16 ] * 1000. << " ms\n"
            << " |           DataStore: " << std::setw( 10 )  << 1./statIEvent*statTime[ 18 ] * 1000. << " ms\n"
            << " | ---- OverlapTrackMerge: " << std::setw( 10 )  << 1./statIEvent*statTime[ 15 ] * 1000. << " ms\n"
            << " |               Merge: " << std::setw( 10 )  << 1./statIEvent*statTime[ 17 ] * 1000. << " ms\n"
            << " |           DataStore: " << std::setw( 10 )  << 1./statIEvent*statTime[ 19 ] * 1000. << " ms\n"
          ;
      } 
      #ifdef MAIN_DRAW
          getchar();
      #endif

#ifdef KFPARTICLE
#ifdef USE_TIMERS
      static double statTimeKFP = 0.;
      static double statTimeKFPParts[4] = {0.};

      statTimeKFP += topoReconstructor->Time();
      for(int iT=0; iT<4; iT++)
        statTimeKFPParts[iT] += topoReconstructor->StatTime(iT);

      std::cout << "Topo reconstruction time"
        << " Real = " << std::setw( 10 ) << statTimeKFP/statIEvent * 1.e3 << " ms" << std::endl;
      if ( fullTiming ) {
        std::cout << "    Init                " << statTimeKFPParts[0]/statIEvent * 1.e3 << " ms" << std::endl;
        std::cout << "    PV Finder           " << statTimeKFPParts[1]/statIEvent * 1.e3 << " ms" << std::endl;
        std::cout << "    Sort Tracks         " << statTimeKFPParts[2]/statIEvent * 1.e3 << " ms" << std::endl;
        std::cout << "    KF Particle Finder  " << statTimeKFPParts[3]/statIEvent * 1.e3 << " ms" << std::endl;
      }
#endif //USE_TIMERS
#endif
    }

#ifdef KFPARTICLE
    if (topoReconstructor) delete topoReconstructor;
#endif
    
  } // kEvent
#ifndef HLTCA_STANDALONE
  if ( perf) {
    perf->WriteHistos();
  }
  perfHistoFile->Close();
#endif

#ifdef WITHSCIF
  ret = scif_unregister(endpoint, offsetSender, maxEventSize);
  if(ret<0) std::cout << "Fail unregisterring buffer." << std::endl;
  if(buffer) free(buffer);
#endif
  
  if (tracker) delete tracker;
  return 0;
}
