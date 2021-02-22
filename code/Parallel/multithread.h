#ifndef multithread_h
#define multithread_h

#include <map>
using std::map;

#include <cstring>
using std::string;

#include <vector>
using std::vector;

#include "tbb/blocked_range.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/task_scheduler_observer.h"
#include "tbb/parallel_for.h"
#include "tbb/spin_mutex.h"
#include "tbb/tick_count.h"
#include "pthread.h"

// #define DEBUG_THREADS
using namespace tbb;
typedef spin_mutex TMyMutex;
TMyMutex mutex;
#ifdef DEBUG_THREADS
typedef spin_mutex TMyMutex2;
TMyMutex2 mutex2;
typedef spin_mutex TMyMutex3;
TMyMutex3 mutex3;
#endif // DEBUG_THREADS

#include <iostream>
#include <AliHLTTPCCAGBTracker.h>
#include <AliHLTTPCCAInputData.h>
#include <AliHLTTPCCAInputSettings.h>
//#include "RunTracker.h"

//int threads_counter = -1; // for count number of current threads. Need for cpu-thread correspondence making.
tick_count time0 = tick_count::now(); // reference time

map<int,long> threadToCpuMap; // let get cpuId by threadId
map<int,int> threadNumberToCpuMap; // let get cpuId by threadNumber (see threads_counter)

class TMyObserver: public  task_scheduler_observer
{
 public:
  TMyObserver():threads_counter(-1) {};
  void FInit(); // set cpu - thread correspondence
  void InitThreadsCounter() { threads_counter = -1; }
 protected:
  void on_scheduler_entry(bool Is_worker);  // run at begin of each thread execution
  void on_scheduler_exit(bool Is_worker);   // run at end of each thread execution

  int threads_counter; // for count number of current threads. Need for cpu-thread correspondence making.
};

  /// set cpu - thread correspondence
void TMyObserver::FInit()
{
  threads_counter = -1;
    //  default
//   for (int i=0; i<16; i+=1){
//     threadNumberToCpuMap[i+0] = 15-i;
//   };

  // for (int i=0; i<48; i+=1){
  //   threadNumberToCpuMap[i+0] = 47-i;
  // };
#if 0
  //lxir039
  for (int i=0; i<8; i++){ 
     threadNumberToCpuMap[2*i+0] = 15-i; 
     threadNumberToCpuMap[2*i+1] = 15-(i+8); 
   }
#endif

#if 0 
  //lxir075
  for(int iProc=0; iProc<4; iProc++)
  {
    for(int i=0; i<8; i++){ 
      threadNumberToCpuMap[2*i+0 + iProc*20] = 4*i + iProc; 
      threadNumberToCpuMap[2*i+1 + iProc*20] = 4*i + 32 + iProc;
    }
    for(int i=0; i<2; i++){
      threadNumberToCpuMap[2*i+0 + 16 + iProc*20] = 4*i + iProc + 64; 
      threadNumberToCpuMap[2*i+1 + 16 + iProc*20] = 4*i + 8 + iProc + 64;
    }
  }
#endif

#if 0
  //lxir039
    for (int i=0; i<16; i++){
      threadNumberToCpuMap[2*i+0] = i;
      threadNumberToCpuMap[2*i+1] = i+16;
    }
#endif

#if 1
    for (int i=1; i<240; i++){
      threadNumberToCpuMap[i-1] = i;
    }
    threadNumberToCpuMap[239] = 0;
#endif

} // TMyObserver::FInit()

  /// redefine function, which will be run at begining of execution of each thread
#define handle_error_en(en, msg) do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
void TMyObserver::on_scheduler_entry(bool Is_worker)
{
//   pthread_t I = pthread_self();
//   TMyMutex::scoped_lock lock;
//   lock.acquire(mutex);
//   ++threads_counter;
//   int cpuId = threadNumberToCpuMap[threads_counter%/*L1Algo::nthreads*/80];
// #ifdef DEBUG_THREADS
//     cout << "ThrId=" << I << " thread have been created " << threads_counter << "-th.";
//     cout << " And was run on cpu " << cpuId << endl;
// #endif // DEBUG_THREADS
//   lock.release();
//   threadToCpuMap[I] = cpuId;
// 
//   int s;
//   cpu_set_t cpuset;
//   pthread_t thread = I;
//   CPU_ZERO(&cpuset);
//   CPU_SET(cpuId, &cpuset);
// // cout << "before" << endl; //FIXME: segmentation fault somethere.
//   s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
//     if (s != 0){ cout << " pthread_setaffinity_np " << endl; handle_error_en(s, "pthread_setaffinity_np");}
// cout << "end set for " << cpuId << endl;

//   s = pthread_getaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
// //   if (s != 0)  handle_error_en(s, "pthread_getaffinity_np");
//
// //  printf("Set returned by pthread_getaffinity_np() contained:\n");
//   for (int j = 0; j < CPU_SETSIZE; j++)
//       if (CPU_ISSET(j, &cpuset))
//           printf("    CPU %d uses\n ", j);

} // TMyObserver::on_scheduler_entry(bool Is_worker)


  /// run at end of each thread execution
void TMyObserver::on_scheduler_exit(bool Is_worker)  //FIXME: don't run
{
//   pthread_t I = pthread_self();
//   cout << "Thread with number " << I << " was ended " << threads_counter;
// --threads_counter;
} // TMyObserver::on_scheduler_exit(bool Is_worker)

// inline void FFF(){};
// inline void FFF(int thrId){
//   pthread_t I = pthread_self();
//   TMyMutex::scoped_lock lock;
//   lock.acquire(mutex);
//   ++threads_counter;
//   int cpuId = thrId + threadNumberToCpuMap[threads_counter%L1AlgoInter::nCores];
//   lock.release();
// //   threadToCpuMap[I] = cpuId;
// 
//   int s;
//   cpu_set_t cpuset;
//   pthread_t thread = I;
//   CPU_ZERO(&cpuset);
//   CPU_SET(cpuId, &cpuset);
// 
//   s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
//   if (s != 0){ cout << " pthread_setaffinity_np " << endl; handle_error_en(s, "pthread_setaffinity_np");}
// }

struct TrackersArray{
  TrackersArray():fTracker(0){};
  ~TrackersArray() { if(fTracker) delete [] fTracker; }
  
  AliHLTTPCCAGBTracker *fTracker;
};

// struct InputDataArray{
//   InputDataArray():fInput(0){};
//   ~InputDataArray() { if(fInput) delete [] fInput; if(fSettings) delete fSettings; }
// 
//   AliHLTTPCCAInputSettings *fSettings;
//   AliHLTTPCCAInputData *fInput;
// };

class ApplyReco{

  InputDataArray *const InpData;
  int nEvPerThr;

 public:

   void operator()(const blocked_range<size_t> &range) const
   {
     for(unsigned int i = range.begin(); i != range.end(); ++i){
       AliHLTTPCCAGBTracker Tracker;
       Tracker.SetSettings( InpData[i].fSettings->GetSettings() );
       for (int j = 0; j < nEvPerThr; j++){
         Tracker.SetHits(InpData[i].fInput[j].Hits(), InpData[i].fInput[j].NHits());
         Tracker.FindTracks();
       }
     }
   }

  ApplyReco(InputDataArray* const InpD_, const int nEvPerThr_): InpData(InpD_),nEvPerThr(nEvPerThr_){}

  ~ApplyReco() {}
};

class ApplyMem{

  const InputDataArray& InpDataPerThread;
  InputDataArray* const InpData;
  int nEvPerThr;

 public:

  void operator()(const blocked_range<size_t> &range) const
  {
    const int NEvLocal = nEvPerThr;
    for(unsigned int i = range.begin(); i != range.end(); ++i){
      InpData[i].fSettings = new AliHLTTPCCAInputSettings;
      InpData[i].fInput = new AliHLTTPCCAInputData[NEvLocal];
      *(InpData[i].fSettings) = *(InpDataPerThread.fSettings);
      for(int j=0; j<nEvPerThr; j++){
        InpData[i].fInput[j] = InpDataPerThread.fInput[j];
      }
    }
  }

  ApplyMem(const InputDataArray& tot, InputDataArray *const InpData_, const int nEvPerThr_)
    : InpDataPerThread(tot),InpData(InpData_),nEvPerThr(nEvPerThr_) {}

  ~ApplyMem() {}
};

#undef DEBUG_THREADS

#endif // multithread_h
