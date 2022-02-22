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

#include <AliHLTTPCCAGBTracker.h>
#ifdef KFPARTICLE
#include "KFParticleTopoReconstructor.h"
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

#include <AliHLTTPCCAInputData.h>

#include "Stopwatch.h"
#include "pthread.h"
#include <errno.h>

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

int main(int argc, char **argv)
{
  bool fullTiming = false;
  int firstEvent = 0;
  int lastEvent = 1000000;
  int nThreads = 1;
  int Step=1;
  int nRuns=1;
  int repetitions = 1;
  string filePrefix = "./Events/"; 
  for( int i=1; i < argc; i++ ){
    if ( !std::strcmp( argv[i], "-h" ) || !std::strcmp( argv[i], "--help" ) || !std::strcmp( argv[i], "-help" ) ) {
      std::cout << "Please look in readme.txt" << std::endl;
      return 0;
    } else if ( !std::strcmp( argv[i], "-time" ) ) {
      fullTiming = true;
    } else if ( !std::strcmp( argv[i], "-dir" ) && ++i < argc ) {
      filePrefix = argv[i];
    } else if ( !std::strcmp( argv[i], "-nThreads" ) && ++i < argc ) {
      nThreads = atoi( argv[i] );
    } else if ( !std::strcmp( argv[i], "-Step" ) && ++i < argc ) {
      Step = atoi( argv[i] );
    } else if ( !std::strcmp( argv[i], "-nRuns" ) && ++i < argc ) {
      nRuns = atoi( argv[i] );
    } else if ( !std::strcmp( argv[i], "-ev" ) && ++i < argc ) {
      firstEvent = atoi( argv[i] );
      if (++i < argc){
        lastEvent = atoi( argv[i] );
//        std::cout << "Only process events " << firstEvent << ",..," << lastEvent << std::endl;
      }
      else{
        lastEvent = firstEvent;
//        std::cout << "Only process event " << lastEvent << std::endl;
      }
    } else {
      firstEvent = atoi( argv[i] );
      lastEvent = firstEvent;
//      std::cout << "Only process event " << lastEvent << std::endl;
    }
  }

  const int NEventsPerThread = lastEvent - firstEvent + 1;
  fstream OutTime;
  OutTime.open( "outtime.dat", ios::out );

  InputDataArray InputDataPerThread;
  {
    std::string LocalfilePrefix = filePrefix;
    LocalfilePrefix += "/";
    const int NEvLocal = NEventsPerThread;

    InputDataPerThread.fInput = new AliHLTTPCCAInputData[NEvLocal];
    InputDataPerThread.fSettings = new AliHLTTPCCAInputSettings;

    if(!InputDataPerThread.fSettings->ReadSettingsFromFile(LocalfilePrefix)) {
      std::cout << "Setting can not be read" << std::endl;
    }

    for ( int kEvents = firstEvent; kEvents <= lastEvent; kEvents++ ) {
      char buf[6];
      sprintf( buf, "%d", kEvents );
      const std::string fileName = LocalfilePrefix + "event" + std::string(buf) + "_";

      if (!InputDataPerThread.fInput[kEvents].ReadHitsFromFile(fileName)) {
        cout << "Hits Data for Event " << kEvents << " can't be read." << std::endl;
        break;
      }
    }
  }

  std::cout << nThreads << "  " << Step << "  " << 1/*nRuns*/ << "  " << NEventsPerThread << std::endl;
  std::cout << std::endl << std::endl;
  std::cout << " --- tbb  ---"<<std::endl;
  

//   int threadNumberToCpuMap[240];
//   for (int i=1; i<240; i++){
//     threadNumberToCpuMap[i-1] = i;
//   }
//   threadNumberToCpuMap[239] = 0;
  
  int threadNumberToCpuMap[32];
  for (int i=0; i<16; i++){
    threadNumberToCpuMap[2*i+0] = i;
    threadNumberToCpuMap[2*i+1] = i+16;
  }
    
  #define handle_error_en(en, msg) do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

  for(int iThreads=Step; iThreads <= nThreads; iThreads+=Step)
  {
    InputDataArray *InputDataParallel = new InputDataArray[iThreads];
    Stopwatch timer;

    #pragma omp parallel for num_threads(iThreads)
    for(int i = 0; i < iThreads; ++i){
      int s;
      cpu_set_t cpuset;
      int cpuId = threadNumberToCpuMap[i];
      pthread_t thread  = pthread_self();
      CPU_ZERO(&cpuset);
      CPU_SET(cpuId, &cpuset);
      s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
      if (s != 0){ cout << " pthread_setaffinity_np " << endl; handle_error_en(s, "pthread_setaffinity_np");}

      InputDataParallel[i].fSettings = new AliHLTTPCCAInputSettings;
      InputDataParallel[i].fInput = new AliHLTTPCCAInputData[NEventsPerThread];
      *(InputDataParallel[i].fSettings) = *(InputDataPerThread.fSettings);
      for ( int iE = firstEvent; iE <= lastEvent; iE++ ) {
//std::cout << i << "  " << j << "  " << omp_get_num_threads() << "  " << I <<  std::endl;
        InputDataParallel[i].fInput[iE] = InputDataPerThread.fInput[iE];
      }
    }

//     if(iVerbose == 0)
    {
      timer.Start();
      for(int iTimes = 0; iTimes<nRuns; iTimes++)
      {
        #pragma omp parallel for num_threads(iThreads)
        for(int i = 0; i < iThreads; ++i){
          int s;
          cpu_set_t cpuset;
          int cpuId = threadNumberToCpuMap[i];
          pthread_t thread  = pthread_self();
          CPU_ZERO(&cpuset);
          CPU_SET(cpuId, &cpuset);
          s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
          if (s != 0){ cout << " pthread_setaffinity_np " << endl; handle_error_en(s, "pthread_setaffinity_np");}

          AliHLTTPCCAGBTracker Tracker;
          Tracker.SetSettings( InputDataParallel[i].fSettings->GetSettings() );

          for ( int iE = firstEvent; iE <= lastEvent; iE++ ) {
            Tracker.SetHits(InputDataParallel[i].fInput[iE].Hits(), InputDataParallel[i].fInput[iE].NHits());

            Tracker.FindTracks();

#ifdef KFPARTICLE
            KFParticleTopoReconstructor topoReconstructor; // TODO don't recreate with each event
            topoReconstructor.Init( &Tracker);
// 
            topoReconstructor.ReconstructPrimVertex();
            topoReconstructor.ReconstructParticles();
#endif            
          }
        }
      }
      timer.Stop();
    }
//     else
//     {
//       Performance *Perf = new Performance[iThreads];
//       InputMCDataArray *InputMCDataParallel = new InputMCDataArray[iThreads];
//       parallel_for_simpleEqNThr(1,iThreads,ApplyMemMCData(InputMCDataPerThread,InputMCDataParallel, Perf, NEventsPerThread));
// 
//       timer.Start();
//       for(int iTimes = 0; iTimes<nRuns; iTimes++)
//       {
//         obs.InitThreadsCounter();
//         parallel_for_simpleEqNThr(1,iThreads,ApplyRecoWithPerf(InputDataParallel, InputMCDataParallel, Perf, NEventsPerThread, iVerbose));
//       }
//       timer.Stop();
// 
//       for(int iTh=0; iTh<iThreads; iTh++)
//       {
//         cout << endl;
//         cout << "L1 ACCUMULATED STAT    : " << Perf[iTh].L1_NEVENTS << " EVENTS "               << endl << endl;
//         Perf[iTh].L1_NTRA.PrintEff();
//         cout << "MC tracks/event found  : " <<
//           int(double(Perf[iTh].L1_NTRA.reco.counters[Perf[iTh].L1_NTRA.indices["total"]])/double(Perf[iTh].L1_NEVENTS)) << endl;
//         cout<<endl;
//         cout<<"CA Track Finder: " << Perf[iTh].L1_CATIME/Perf[iTh].L1_NEVENTS << " s/ev" << endl << endl;
//       }
// 
//       if(InputMCDataParallel) delete [] InputMCDataParallel;
//       if(Perf) delete [] Perf;
//     }

    double rtime = timer.RealTime();
    double ctime = timer.CpuTime();
    cout << iThreads << ":" << endl;
    cout << " Estimated CATime = " << rtime/nRuns << " Estimated CATime/nEvents = " << (rtime/iThreads/nRuns/NEventsPerThread) << endl;

    if(InputDataParallel) delete [] InputDataParallel;
  }

  OutTime.close();

  return 0;
}
