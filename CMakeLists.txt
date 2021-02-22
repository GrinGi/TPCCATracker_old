cmake_minimum_required(VERSION 2.6.2)
project(CA)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# set( CMAKE_VERBOSE_MAKEFILE on )

find_package(Vc REQUIRED)
include_directories(${VC_INCLUDE_DIR})

find_package(SSE)
#find_package(TBB REQUIRED)
#include_directories(${TBB_INCLUDE_DIRS})

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebug RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set(COMPILE_FOR_MIC FALSE CACHE BOOL "Compile for mic card.")
if(COMPILE_FOR_MIC)
   set(VC_LIBRARIES "${VC_MIC_LIBRARIES}")
   message("${VC_LIBRARIES} will be used")
endif(COMPILE_FOR_MIC)

find_package(TBB)
include_directories(${TBB_INCLUDE_DIRS})
#set(TBB_REALY_FOUND = ${TBB_FOUND})

if(TBB_FOUND)
   set(ENABLE_TBB FALSE CACHE BOOL "Use TBB. (TBB is found and can be anabled).")
elseif(TBB_FOUND)
   set(ENABLE_TBB FALSE CACHE BOOL "Use TBB. (TBB is not found).")
endif(TBB_FOUND)

if(ENABLE_TBB)
   message("-- TBB will be used.")
elseif(ENABLE_TBB)
   message("-- TBB will not be used.")
endif(ENABLE_TBB)
		     
set(ENABLE_ROOT TRUE CACHE BOOL "Enable usage of ROOT if it is found.")
if(ENABLE_ROOT)
   find_package(Root)
else(ENABLE_ROOT)
   set(ROOT_FOUND FALSE)
endif(ENABLE_ROOT)

if(NUM_THREADS)
   add_definitions(-DNUM_THREADS=${NUM_THREADS})
endif(NUM_THREADS)
if(VC_NO_GATHER_TRICKS)
   add_definitions(-DVC_NO_GATHER_TRICKS)
endif(VC_NO_GATHER_TRICKS)

macro(add_target_property _target _prop _value)
   get_target_property(_oldprop "${_target}" ${_prop})
   if(NOT _oldprop)
      set_target_properties("${_target}" PROPERTIES ${_prop} "${_value}")
   else(NOT _oldprop)
      set_target_properties("${_target}" PROPERTIES ${_prop} "${_oldprop} ${_value}")
   endif(NOT _oldprop)
endmacro(add_target_property)

   add_definitions(-DSTAR_STANDALONE)
   add_definitions(-DDO_TPCCATRACKER_EFF_PERFORMANCE) # when comment it disable ROOT in 'cmake -i' as well
    add_definitions(-DUSE_TIMERS)
   
   if(COMPILE_FOR_MIC)
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmic")
     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mmic")
   else(COMPILE_FOR_MIC)
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse2")
     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse -msse2")
   endif(COMPILE_FOR_MIC)

if(CMAKE_COMPILER_IS_GNUCXX)
   ##################################################################################################
   #                                              GCC                                               #
   ##################################################################################################
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W -Wall -Wswitch -Wformat -Wchar-subscripts -Wno-parentheses -Wmultichar -Wtrigraphs -Wpointer-arith -Wcast-align -Wreturn-type -Wno-unused-function -ansi -pedantic -Wno-long-long -Wno-variadic-macros -Wno-unused-local-typedefs -Wshadow -fno-threadsafe-statics -fopenmp -std=c++14")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wswitch -Wformat -Wchar-subscripts -Wno-parentheses -Wmultichar -Wtrigraphs -Wpointer-arith -Wcast-align -Wreturn-type -Wno-unused-function -ansi -pedantic -Wno-long-long -Wno-variadic-macros -Wno-unused-local-typedefs -Wshadow -fno-threadsafe-statics -fopenmp -std=c++14")

   if(ENABLE_TBB)
      set(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG} -DTBB_DO_ASSERT=1")
   endif(ENABLE_TBB)
   set(CMAKE_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS_MINSIZEREL}")
   set(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE} -O3")
   set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O3")
   if(ENABLE_TBB)
      set(CMAKE_C_FLAGS_DEBUG          "${CMAKE_C_FLAGS_DEBUG} -DTBB_DO_ASSERT=1")
   endif(ENABLE_TBB)
   set(CMAKE_C_FLAGS_MINSIZEREL     "${CMAKE_C_FLAGS_MINSIZEREL}")
   set(CMAKE_C_FLAGS_RELEASE        "${CMAKE_C_FLAGS_RELEASE} -O3")
   set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -O3")

   
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lrt")

   if(CMAKE_BUILD_TYPE STREQUAL "Debug")
      add_definitions(-DNO_OPTIMIZATION)
      add_definitions(-g)
   elseif(CMAKE_BUILD_TYPE STREQUAL "" AND NOT CMAKE_CXX_FLAGS MATCHES "-O[123]")
      add_definitions(-DNO_OPTIMIZATION)
   endif(CMAKE_BUILD_TYPE STREQUAL "Debug")
   if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
      set(ENABLE_STRICT_ALIASING true CACHE BOOL "Enables strict aliasing rules for more aggressive optimizations")
      if(NOT ENABLE_STRICT_ALIASING)
         set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing")
         set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing")
      endif(NOT ENABLE_STRICT_ALIASING)
      set(DISABLE_DEBUG true CACHE BOOL "Disables debugging code, like assertions")
      if(DISABLE_DEBUG)
         set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG -DNODEBUG")
         set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DNDEBUG -DNODEBUG")
      endif(DISABLE_DEBUG)
   endif(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")

   set(USE_SSSE3 ${SSSE3_FOUND} CACHE BOOL "Use SSE3 and SSSE3. If (S)SSE3 instructions are not enabled they will be emulated.")
   set(USE_SSE4_1 ${SSE4_1_FOUND} CACHE BOOL "Use SSE4.1. If SSE4.1 instructions are not enabled they will be emulated.")
   if(USE_SSSE3)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse3 -mssse3")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse3 -mssse3")
   endif(USE_SSSE3)
   if(USE_SSE4_1)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse4.1")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")
   endif(USE_SSE4_1)
#   set(USE_AVX ${AVX_FOUND} CACHE BOOL "Use AVX. If AVX instructions are not enabled they will be emulated.")
   if(AVX_FOUND)
      message(STATUS "AVX is found (disabled by default).")
      set(USE_AVX FALSE CACHE BOOL "Use AVX. (AVX is supported and can be anabled).")
   elseif(AVX_FOUND)
      set(USE_AVX FALSE CACHE BOOL "Use AVX. (AVX is not supported).")
   endif(AVX_FOUND)
   if(USE_AVX)
      message(STATUS "AVX will be used.")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mavx")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx")
   endif(USE_AVX)
elseif(CMAKE_CXX_COMPILER MATCHES "/(icpc|icc)$")
   ##################################################################################################
   #                                          Intel Compiler                                        #
   ##################################################################################################
   #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wformat -Wno-parentheses -Wmultichar -Wtrigraphs -Wreturn-type -Wno-unused-function -ansi -pedantic -Wno-long-long -Wno-variadic-macros -Wshadow -std=c++11 -openmp -lscif -DWITHSCIF")
   #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W -Wall -Wformat -Wno-parentheses -Wmultichar -Wtrigraphs -Wreturn-type -Wno-unused-function -ansi -pedantic -Wno-long-long -Wno-variadic-macros -Wshadow -std=c++11 -openmp -lscif -DWITHSCIF")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wformat -Wno-parentheses -Wmultichar -Wtrigraphs -Wreturn-type -Wno-unused-function -ansi -pedantic -Wno-long-long -Wno-variadic-macros -Wshadow -std=c++11 -openmp -lscif")
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W -Wall -Wformat -Wno-parentheses -Wmultichar -Wtrigraphs -Wreturn-type -Wno-unused-function -ansi -pedantic -Wno-long-long -Wno-variadic-macros -Wshadow -std=c++11 -openmp -lscif")

   # add_definitions(-Wno-pragmas) #   root has unknown for ICC pragmas
  
   if(ENABLE_TBB)   
      set(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG} -DTBB_DO_ASSERT=1")
   endif(ENABLE_TBB)

   set(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE} -O3")
   set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -DNDEBUG -DNODEBUG -O3")
   if(ENABLE_TBB)
      set(CMAKE_C_FLAGS_DEBUG          "${CMAKE_C_FLAGS_DEBUG} -DTBB_DO_ASSERT=1")
   endif(ENABLE_TBB)
   set(CMAKE_C_FLAGS_RELEASE          "${CMAKE_C_FLAGS_RELEASE} -O3")
   set(CMAKE_C_FLAGS_RELWITHDEBINFO   "${CMAKE_C_FLAGS_RELWITHDEBINFO} -DNDEBUG -DNODEBUG -O3")

   set(ALIAS_FLAGS "-no-ansi-alias")
   if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
      set(ENABLE_STRICT_ALIASING true CACHE BOOL "Enables strict aliasing rules for more aggressive optimizations")
      if(ENABLE_STRICT_ALIASING)
         set(ALIAS_FLAGS "-ansi-alias")
      endif(ENABLE_STRICT_ALIASING)
   endif(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")

   # per default icc is not IEEE compliant, but we need that for verification
   set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${ALIAS_FLAGS} -w1 -fp-model precise")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ALIAS_FLAGS} -w1 -fp-model precise")

   if(CMAKE_BUILD_TYPE STREQUAL "Debug")
      add_definitions(-DNO_OPTIMIZATION)
      add_definitions(-g)
   elseif(CMAKE_BUILD_TYPE STREQUAL "" AND NOT CMAKE_CXX_FLAGS MATCHES "-O[123]")
      add_definitions(-DNO_OPTIMIZATION)
   endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

   # set(USE_SSSE3 ${SSSE3_FOUND} CACHE BOOL "Use SSE3 and SSSE3. If (S)SSE3 instructions are not enabled they will be emulated.")
   # set(USE_SSE4_1 ${SSE4_1_FOUND} CACHE BOOL "Use SSE4.1. If SSE4.1 instructions are not enabled they will be emulated.")
   # if(USE_SSSE3)
   #    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse3 -mssse3")
   #    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse3 -mssse3")
   # endif(USE_SSSE3)
   # if(USE_SSE4_1)
   #    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse4.1")
   #    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")
   # endif(USE_SSE4_1)
   if(COMPILE_FOR_MIC)
   else(COMPILE_FOR_MIC)
    if(AVX_FOUND)
        message(STATUS "AVX is found (enabled by default).")
        set(USE_AVX TRUE CACHE BOOL "Use AVX. (AVX is supported and can be anabled).")
    elseif(AVX_FOUND)
        set(USE_AVX FALSE CACHE BOOL "Use AVX. (AVX is not supported).")
    endif(AVX_FOUND)
    if(USE_AVX)
        message(STATUS "AVX will be used.")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -xAVX")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -xAVX")
    endif(USE_AVX)
  endif(COMPILE_FOR_MIC)
endif(CMAKE_COMPILER_IS_GNUCXX)

if(COMPILE_FOR_MIC)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DFORMIC")
endif(COMPILE_FOR_MIC)

if(CMAKE_BUILD_TYPE STREQUAL "" AND NOT CMAKE_CXX_FLAGS MATCHES "-O[123]")
   message(STATUS "WARNING! It seems you are compiling without optimization. Please set CMAKE_BUILD_TYPE.")
endif(CMAKE_BUILD_TYPE STREQUAL "" AND NOT CMAKE_CXX_FLAGS MATCHES "-O[123]")

set(FAST_MATH FALSE CACHE BOOL "Use floating point math shortcuts. Might not be IEEE compliant!")
if(FAST_MATH)
   if(CMAKE_COMPILER_IS_GNUCXX)
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffast-math")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffast-math")
   elseif(CMAKE_CXX_COMPILER MATCHES "/(icpc|icc)$")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fp-model fast=2")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fp-model fast=2")
   elseif(MSVC)
   endif(CMAKE_COMPILER_IS_GNUCXX)
endif(FAST_MATH)

set(PROFILING FALSE CACHE BOOL "Compile the code for profiling with gprof. Can slow down the code.")
if(PROFILING)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pg")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
endif(PROFILING)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/code/CATracker
  ${CMAKE_CURRENT_SOURCE_DIR}/code/KFParticle ${CMAKE_CURRENT_SOURCE_DIR}/code/vectors
  ${CMAKE_CURRENT_SOURCE_DIR}/code/CATrackerPerformance
  ${CMAKE_CURRENT_SOURCE_DIR}/code/KFParticlePerformance
  ${CMAKE_CURRENT_SOURCE_DIR}/code/KFParticleTest
  ${CMAKE_CURRENT_SOURCE_DIR}/code/Parallel
)

if(ROOT_FOUND)
   include_directories(${ROOT_INCLUDE_DIR})
endif(ROOT_FOUND)

set(CATrackerCode
   code/CATracker/AliHLTTPCCAClusterData.cxx
   code/CATracker/AliHLTTPCCAGBHit.cxx
   code/CATracker/Reconstructor.cpp
   code/CATracker/AliHLTTPCCANeighboursFinder.cxx
   code/CATracker/AliHLTTPCCAHitArea.cxx
   code/CATracker/AliHLTTPCCAGrid.cxx
   code/CATracker/AliHLTTPCCATracker.cxx
   code/CATracker/AliHLTTPCCARow.cxx
   code/CATracker/AliHLTTPCCAPackHelper.cxx
   code/CATracker/AliHLTTPCCAParam.cxx
   code/CATracker/AliHLTTPCCATrackParam.cxx
   code/CATracker/AliHLTTPCCATrackParamVector.cxx
   code/CATracker/AliHLTTPCCAOutTrack.cxx
   code/CATracker/AliHLTTPCCASliceOutput.cxx
   code/CATracker/AliHLTTPCCANeighboursCleaner.cxx
   code/CATracker/AliHLTTPCCAStartHitsFinder.cxx
   code/CATracker/AliHLTTPCCATrackletConstructor.cxx
   code/CATracker/AliHLTTPCCATrackletSelector.cxx
   code/CATracker/AliHLTTPCCATrackletVector.cxx
   code/CATracker/AliHLTTPCCAMerger.cxx
   code/CATracker/AliHLTTPCCAGBTrack.cxx
   code/CATracker/AliHLTTPCCAGBTracker.cxx
   code/CATracker/AliHLTTPCCALooperMerger.cxx
   )
set(CATrackerParallelCode
   code/Parallel/AliHLTTPCCAInputData.cxx
   code/Parallel/AliHLTTPCCAInputSettings.cxx
   )
set(DisplayCode
   code/CATrackerPerformance/AliHLTTPCCADisplay.cxx
   )   
set(PerformanceCode
   code/CATrackerPerformance/AliHLTTPCCAPerformance.cxx
   code/CATrackerPerformance/AliHLTTPCPerformanceBase.cxx	
   code/CATrackerPerformance/AliHLTTPCCATrackPerformanceBase.cxx
   code/CATrackerPerformance/AliHLTTPCCASlicesPerformance.cxx
   code/CATrackerPerformance/AliHLTTPCCASlicePerformance.cxx
   code/CATrackerPerformance/AliHLTTPCCAGlobalSlicesPerformance.cxx
   code/CATrackerPerformance/AliHLTTPCCAGlobalPerformance.cxx
   code/CATrackerPerformance/AliHLTTPCCAStiPerformance.cxx
   code/CATrackerPerformance/AliHLTTPCCASliceLinksPerformance.cxx
   code/CATrackerPerformance/AliHLTTPCCASlicesLinksPerformance.cxx
#       code/Performance/AliHLTTPCCAMergerPerformance.cxx  #	TODO

   code/CATrackerPerformance/AliHLTTPCCAMCTrack.cxx
   code/CATrackerPerformance/AliHLTTPCCAMCPoint.cxx
   )
set(KFParticleCode
   code/KFParticle/KFParticleTopoReconstructor.cxx
   code/KFParticle/KFVertex.cxx	
   code/KFParticle/KFPTrack.cxx
   code/KFParticle/KFPTrackVector.cxx
   code/KFParticle/KFPVertex.cxx
   code/KFParticle/KFParticlePVReconstructor.cxx
   code/KFParticle/KFParticleDatabase.cxx
   code/KFParticle/KFParticleBase.cxx
   code/KFParticle/KFParticleBaseSIMD.cxx
   code/KFParticle/KFParticle.cxx
   code/KFParticle/KFParticleSIMD.cxx
   code/KFParticle/KFParticleFinder.cxx
   code/KFParticleTest/KFParticleTest.cxx
   code/KFParticlePerformance/KFPHistogram/KFPHistogramSet.cxx
)
set(KFPPerformanceCode
   code/KFParticlePerformance/KFMCVertex.cxx
   code/KFParticlePerformance/KFParticlePerformanceBase.cxx
   code/KFParticlePerformance/KFTopoPerformance.cxx
   code/KFParticlePerformance/KFMCParticle.cxx
)

set(VALGRIND_ENABLED FALSE CACHE BOOL "Enable valgrind client calls")
if(NOT VALGRIND_ENABLED)
   add_definitions(-DNVALGRIND)
endif(NOT VALGRIND_ENABLED)

set(TESTS_ENABLED FALSE CACHE BOOL "Enable build of unit tests")
if(TESTS_ENABLED)
   add_subdirectory(tests)
   enable_testing()
endif(TESTS_ENABLED)

set(DEBUG_MESSAGES "0" CACHE STRING "Enable debug messages (1: Seeding, 2: Fitting, 4: Kalman Filter, 8: TrackletSelector, 16: Tracker::WriteOutput)")
if(DEBUG_MESSAGES)
   add_definitions(-DDEBUG_MESSAGES=${DEBUG_MESSAGES})
endif(DEBUG_MESSAGES)

#add_definitions(-DENABLE_VECTORIZATION)
#add_definitions(-DDO_NOT_MERGE)

set(ENABLE_ARRAY_BOUNDS_CHECKING FALSE CACHE BOOL "Enable Array bounds checking. Slow!")
if(ENABLE_ARRAY_BOUNDS_CHECKING)
   add_definitions(-DENABLE_ARRAY_BOUNDS_CHECKING)
endif(ENABLE_ARRAY_BOUNDS_CHECKING)

set(USE_SIMD TRUE CACHE BOOL "When compiling on x86 use SSE Vector class instead of Simple implementation")
if(USE_SIMD)
# if(NOT COMPILE_FOR_MIC)
#    add_definitions(-DVC_IMPL=SSE)
# endif(NOT COMPILE_FOR_MIC)
else(USE_SIMD)
   add_definitions(-DVc_IMPL=Scalar)
endif(USE_SIMD)
set(CATrackerCode ${CATrackerCode} code/CATracker/AliHLTTPCCASliceDataVector.cxx)

set(DUMP_LINKS FALSE CACHE BOOL "dump links into Links directory after NeighboursFinder")
if(DUMP_LINKS)
   add_definitions(-DDUMP_LINKS)
endif(DUMP_LINKS)
set(DUMP_TRACKLETCONSTRUCTION FALSE CACHE BOOL "dump Tracklet Construction states into TrackletConstruction directory")
if(DUMP_TRACKLETCONSTRUCTION)
   add_definitions(-DDUMP_TRACKLETCONSTRUCTION)
endif(DUMP_TRACKLETCONSTRUCTION)
set(DUMP_TC_OUTPUT FALSE CACHE BOOL "dump Tracklet Construction output to stdout")
if(DUMP_TC_OUTPUT)
   add_definitions(-DDUMP_TC_OUTPUT)
endif(DUMP_TC_OUTPUT)

#########################################################################################

#add_executable(convertToBinary convertToBinary.cpp code/AliHLTTPCCAMCTrack.cxx)
#target_link_libraries(convertToBinary ${ROOT_LIBS} ${VC_LIBRARIES})

#add_executable(benchmark benchmark.cpp)

# ROOT independent packages

   add_library(CATracker ${CATrackerCode})
   if(CMAKE_CXX_COMPILER MATCHES "/(icpc|icc)$")
   add_target_property(CATracker COMPILE_FLAGS "-inline-forceinline")
   endif(CMAKE_CXX_COMPILER MATCHES "/(icpc|icc)$")

   if(ENABLE_TBB)
      add_target_property(CATracker COMPILE_FLAGS "-DUSE_TBB")
      target_link_libraries(CATracker ${TBB_RELEASE_LIBRARIES} ${VC_LIBRARIES})

      add_executable(CA_parallel CA_parallel.cpp ${CATrackerParallelCode})
      target_link_libraries(CA_parallel CATracker ${TBB_RELEASE_LIBRARIES} ${VC_LIBRARIES})
      add_target_property(CA_parallel COMPILE_FLAGS "-DHLTCA_STANDALONE")
   else(ENABLE_TBB)
   
      add_executable(CA_parallel CA_parallel.cpp ${CATrackerParallelCode})
      target_link_libraries(CA_parallel CATracker ${VC_LIBRARIES})
      add_target_property(CA_parallel COMPILE_FLAGS "-DHLTCA_STANDALONE")
      
      target_link_libraries(CATracker ${VC_LIBRARIES})
   endif(ENABLE_TBB)

   add_library(CATrackerPerf ${PerformanceCode})
   if(CMAKE_CXX_COMPILER MATCHES "/(icpc|icc)$")
   add_target_property(CATrackerPerf COMPILE_FLAGS "-inline-forceinline")
   endif(CMAKE_CXX_COMPILER MATCHES "/(icpc|icc)$")

   add_executable(CA CA.cpp)
   target_link_libraries(CA CATracker CATrackerPerf)

#   add_library(KFParticle ${KFParticleCode})
#   if(ENABLE_TBB)
#      add_target_property(KFParticle COMPILE_FLAGS "-DUSE_TBB")
#      target_link_libraries(KFParticle ${TBB_RELEASE_LIBRARIES} ${VC_LIBRARIES})
#   else(ENABLE_TBB)
#      target_link_libraries(KFParticle ${VC_LIBRARIES})
#   endif(ENABLE_TBB)
#   if(CMAKE_CXX_COMPILER MATCHES "/(icpc|icc)$")
#   add_target_property(KFParticle COMPILE_FLAGS "-inline-forceinline")
#   endif(CMAKE_CXX_COMPILER MATCHES "/(icpc|icc)$")

   add_executable(KFP CA.cpp)
#   add_executable(KFPStandalone KFP.cpp)
#   add_executable(KFParticleTest KFParticleTest.cpp)
#   add_executable(KFPServer KFPServer.cpp)
#   add_executable(KFPSender KFPSender.cpp)
   
# ROOT dependent part  
if(NOT ROOT_FOUND) #	TODO KFParticle
   # compile only standalone CA

      add_executable(KFP_parallel CA_parallel.cpp ${CATrackerParallelCode})
      target_link_libraries(KFP_parallel CATracker KFParticle)
      add_target_property(KFP_parallel COMPILE_FLAGS "-DHLTCA_STANDALONE -DKFPARTICLE -DKFPWITHTRACKER -DHomogeneousField -DKFParticleStandalone")
      
      
   add_target_property(CATracker COMPILE_FLAGS "-DHLTCA_STANDALONE")

   add_target_property(CATrackerPerf COMPILE_FLAGS "-DHLTCA_STANDALONE")
      
   add_target_property(CA COMPILE_FLAGS "-DHLTCA_STANDALONE")
   if(ENABLE_TBB)
   endif(ENABLE_TBB)
   
#   add_target_property(KFParticle COMPILE_FLAGS "-DHLTCA_STANDALONE -DKFPARTICLE -DKFPWITHTRACKER -DHomogeneousField -DKFParticleStandalone")
   
   target_link_libraries(KFP CATracker KFParticle)
   add_target_property(KFP COMPILE_FLAGS "-DHLTCA_STANDALONE -DKFPARTICLE -DKFPWITHTRACKER -DHomogeneousField -DKFParticleStandalone")

#   target_link_libraries(KFParticleTest CATracker KFParticle)
#   add_target_property(KFParticleTest COMPILE_FLAGS "-DHLTCA_STANDALONE -DKFPARTICLE -DKFPWITHTRACKER -DHomogeneousField -DKFParticleStandalone")
   
#   target_link_libraries(KFPStandalone CATracker KFParticle)
#   add_target_property(KFPStandalone COMPILE_FLAGS "-DHLTCA_STANDALONE -DKFPARTICLE -DKFPWITHTRACKER -DHomogeneousField -DKFParticleStandalone")

#   target_link_libraries(KFPServer KFParticle CATracker)
#   add_target_property(KFPServer COMPILE_FLAGS "-DHLTCA_STANDALONE -DKFPARTICLE -DKFPWITHTRACKER -DHomogeneousField -DKFParticleStandalone")
   
#   target_link_libraries(KFPSender KFParticle CATracker)
#   add_target_property(KFPSender COMPILE_FLAGS "-DHLTCA_STANDALONE -DKFPARTICLE -DKFPWITHTRACKER -DHomogeneousField -DKFParticleStandalone")
   
    # needed for STARHLT. same as CA, but a library
    # note: target_link_libraries(TPCCATracker CATracker) doesn't work because cmake can't do the lib from no source
   add_library(TPCCATracker ${CATrackerCode})
   if(ENABLE_TBB)
      add_target_property(TPCCATracker COMPILE_FLAGS "-DUSE_TBB")
      target_link_libraries(TPCCATracker ${TBB_RELEASE_LIBRARIES} ${VC_LIBRARIES})
   else(ENABLE_TBB)
      target_link_libraries(TPCCATracker ${VC_LIBRARIES})
   endif(ENABLE_TBB)
   add_target_property(TPCCATracker COMPILE_FLAGS "-DHLTCA_STANDALONE")
   
else(NOT ROOT_FOUND)
   
   # compile standalone CA with -perf support
   target_link_libraries(CATrackerPerf ${ROOT_LIBS})

#   add_library(KFParticlePerf ${PerformanceCode} ${KFPPerformanceCode}) # we have to recompile the CATracker code with -DKFPARTICLE -DKFPWITHTRACKER -DHomogeneousField -DKFParticleStandalone
#   target_link_libraries(KFParticlePerf ${ROOT_LIBS})
#   add_target_property(KFParticlePerf COMPILE_FLAGS "-DKFPARTICLE -DKFPWITHTRACKER -DHomogeneousField -DKFParticleStandalone")
#   if(CMAKE_CXX_COMPILER MATCHES "/(icpc|icc)$")
#     add_target_property(KFParticlePerf COMPILE_FLAGS "-inline-forceinline")
#   endif(CMAKE_CXX_COMPILER MATCHES "/(icpc|icc)$")

#   add_target_property(KFParticle COMPILE_FLAGS "-DKFPARTICLE -DKFPWITHTRACKER -DHomogeneousField -DKFParticleStandalone")

#   target_link_libraries(KFP CATracker KFParticle KFParticlePerf)
#   add_target_property(KFP COMPILE_FLAGS "-DKFPARTICLE -DKFPWITHTRACKER -DHomogeneousField -DKFParticleStandalone")

#   target_link_libraries(KFParticleTest CATracker KFParticle KFParticlePerf)
#   add_target_property(KFParticleTest COMPILE_FLAGS "-DKFPARTICLE -DKFPWITHTRACKER -DHomogeneousField -DKFParticleStandalone")
   
#   target_link_libraries(KFPStandalone CATracker KFParticle KFParticlePerf)
#   add_target_property(KFPStandalone COMPILE_FLAGS "-DKFPARTICLE -DKFPWITHTRACKER -DHomogeneousField -DKFParticleStandalone")

#   target_link_libraries(KFPServer KFParticle KFParticlePerf CATracker)
#   add_target_property(KFPServer COMPILE_FLAGS "-DKFPARTICLE -DKFPWITHTRACKER -DHomogeneousField -DKFParticleStandalone")

#   target_link_libraries(KFPSender KFParticle KFParticlePerf CATracker)
#   add_target_property(KFPSender COMPILE_FLAGS "-DKFPARTICLE -DKFPWITHTRACKER -DHomogeneousField -DKFParticleStandalone")
   
#   add_executable(KFPStandalonePlotHistograms KFPStandalonePlotHistograms.cpp)
#   target_link_libraries(KFPStandalonePlotHistograms KFParticlePerf CATracker KFParticle)
#   add_target_property(KFPStandalonePlotHistograms COMPILE_FLAGS "-DKFPARTICLE -DKFPWITHTRACKER -DHomogeneousField -DKFParticleStandalone")
   
   #  draw

   add_library(CATracker-draw ${CATrackerCode} ${PerformanceCode})
   target_link_libraries(CATracker-draw ${VC_LIBRARIES} ${ROOT_LIBS})

   add_target_property(CATracker-draw COMPILE_FLAGS " -DMAIN_DRAW")
   
#   add_library(KFParticle-draw ${KFParticleCode} ${KFPPerformanceCode})
#   target_link_libraries(KFParticle-draw ${VC_LIBRARIES} ${ROOT_LIBS})

#   add_target_property(KFParticle-draw COMPILE_FLAGS "-DKFPARTICLE -DKFPWITHTRACKER -DHomogeneousField -DKFParticleStandalone -DMAIN_DRAW")
   
#   add_executable(KFP-draw CA.cpp ${DisplayCode})
#   add_target_property(KFP-draw COMPILE_FLAGS "-DKFPARTICLE -DKFPWITHTRACKER -DHomogeneousField -DKFParticleStandalone -DMAIN_DRAW -DDRAW3")
#   target_link_libraries(KFP-draw CATracker-draw KFParticle-draw)
 
   add_executable(CA-draw CA.cpp ${DisplayCode})
   add_target_property(CA-draw COMPILE_FLAGS "-DMAIN_DRAW -DDRAW")
   target_link_libraries(CA-draw CATracker-draw)

   add_executable(CA-draw2 CA.cpp ${DisplayCode})
   add_target_property(CA-draw2 COMPILE_FLAGS "-DMAIN_DRAW -DDRAW2")
   target_link_libraries(CA-draw2 CATracker-draw)
 
   add_executable(CA-draw3 CA.cpp ${DisplayCode})
   add_target_property(CA-draw3 COMPILE_FLAGS "-DMAIN_DRAW -DDRAW3")
   target_link_libraries(CA-draw3 CATracker-draw)
   
   add_executable(dataConverter dataConverter.cpp)
   target_link_libraries(dataConverter CATracker CATrackerPerf) 
    # needed for STARHLT. same as CA, but a library
    # note: target_link_libraries(TPCCATracker CATracker CATrackerPerf) doesn't work because cmake can't do the lib from no source
   add_library(TPCCATracker ${CATrackerCode} ${PerformanceCode})
   if(ENABLE_TBB)
      add_target_property(TPCCATracker COMPILE_FLAGS "-DUSE_TBB")
      target_link_libraries(TPCCATracker ${ROOT_LIBS} ${TBB_RELEASE_LIBRARIES} ${VC_LIBRARIES})
   else(ENABLE_TBB)
      target_link_libraries(TPCCATracker ${ROOT_LIBS} ${VC_LIBRARIES})
   endif(ENABLE_TBB)
endif(NOT ROOT_FOUND)

# install dir is not interesting for now
mark_as_advanced(CMAKE_INSTALL_PREFIX)
