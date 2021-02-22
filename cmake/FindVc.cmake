# Locate the Vc template library. Vc can be found at http://www.gitorious.org/Vc/
#
# Copyright 2009, Matthias Kretz <kretz@kde.org>
#
# This module defines
# VC_FOUND
# VC_INCLUDE_DIR
# VC_LIBRARIES

find_path(VC_INSTALL_PREFIX include/Vc/Vc)

if(VC_INSTALL_PREFIX)
   set(VC_INCLUDE_DIR "${VC_INSTALL_PREFIX}/include" CACHE PATH "The include directory for Vc")
   find_library(VC_LIBRARIES Vc HINTS "${VC_INSTALL_PREFIX}/lib")
   find_library(VC_MIC_LIBRARIES Vc_MIC HINTS "${VC_INSTALL_PREFIX}/lib")
endif(VC_INSTALL_PREFIX)

if(VC_INSTALL_PREFIX AND VC_INCLUDE_DIR AND VC_LIBRARIES)
   if(VC_MIC_LIBRARIES)
     message(STATUS "Vc template libraries ${VC_LIBRARIES} \n and                     ${VC_MIC_LIBRARIES} \n and include directory   ${VC_INCLUDE_DIR} are found.")
     set(VC_MIC_LIBRARIES "-L${VC_INSTALL_PREFIX}/lib ${VC_MIC_LIBRARIES}")
   else(VC_MIC_LIBRARIES)
     message(STATUS "Vc template library ${VC_LIBRARIES} \n and include directory ${VC_INCLUDE_DIR} are found.")
   endif(VC_MIC_LIBRARIES)
   set(VC_FOUND true)
else(VC_INSTALL_PREFIX AND VC_INCLUDE_DIR AND VC_LIBRARIES)
   set(VC_FOUND false)
   if(Vc_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find required Vc template library.")
   else(Vc_FIND_REQUIRED)
      message(STATUS "Could not find Vc template library.")
   endif(Vc_FIND_REQUIRED)
endif(VC_INSTALL_PREFIX AND VC_INCLUDE_DIR AND VC_LIBRARIES)

mark_as_advanced(VC_INCLUDE_DIR VC_LIBRARIES)
