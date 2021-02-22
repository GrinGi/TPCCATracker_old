find_path(ROOTSYS_DIR include/TObject.h PATHS
   "$ENV{ROOTSYS}"
   "/opt/aliroot_installations/shared/root/root_v5-18-00a"
   "~/src/root-5.22.00a"
   "/data.local1/fairsoft/fairsoft_jan12/tools/root/"
   )
if(NOT ROOTSYS_DIR)
   find_path(ROOTSYS_DIR include/root/TObject.h PATHS
     "$ENV{ROOTSYS}"
     "/opt/aliroot_installations/shared/root/root_v5-18-00a"
     "~/src/root-5.22.00a"
     )
endif(NOT ROOTSYS_DIR)

if(ROOTSYS_DIR)
   message(STATUS "ROOT found at ${ROOTSYS_DIR}")
   set(ROOT_FOUND true)
   find_library(ROOT_CORE_LIBRARY    Core PATHS       "${ROOTSYS_DIR}/lib" "${ROOTSYS_DIR}/lib/root")
   find_library(ROOT_CINT_LIBRARY    Cint PATHS       "${ROOTSYS_DIR}/lib" "${ROOTSYS_DIR}/lib/root")
   find_library(ROOT_HIST_LIBRARY    Hist PATHS       "${ROOTSYS_DIR}/lib" "${ROOTSYS_DIR}/lib/root")
   find_library(ROOT_MATRIX_LIBRARY  Matrix PATHS     "${ROOTSYS_DIR}/lib" "${ROOTSYS_DIR}/lib/root")
   find_library(ROOT_PHYSICS_LIBRARY Physics PATHS    "${ROOTSYS_DIR}/lib" "${ROOTSYS_DIR}/lib/root")
   find_library(ROOT_EG_LIBRARY      EG   PATHS       "${ROOTSYS_DIR}/lib" "${ROOTSYS_DIR}/lib/root")
   find_library(ROOT_GPAD_LIBRARY    Gpad PATHS       "${ROOTSYS_DIR}/lib" "${ROOTSYS_DIR}/lib/root")
   find_library(ROOT_GRAF_LIBRARY    Graf PATHS       "${ROOTSYS_DIR}/lib" "${ROOTSYS_DIR}/lib/root")
   find_library(ROOT_GRAF3D_LIBRARY  Graf3d PATHS     "${ROOTSYS_DIR}/lib" "${ROOTSYS_DIR}/lib/root")
   find_library(ROOT_RIO_LIBRARY     RIO  PATHS       "${ROOTSYS_DIR}/lib" "${ROOTSYS_DIR}/lib/root")
   find_library(ROOT_MATHCORE_LIBRARY MathCore  PATHS "${ROOTSYS_DIR}/lib" "${ROOTSYS_DIR}/lib/root")

   set(ROOT_LIBS dl "${ROOT_CORE_LIBRARY}" "${ROOT_CINT_LIBRARY}" "${ROOT_HIST_LIBRARY}"
      "${ROOT_MATRIX_LIBRARY}" "${ROOT_PHYSICS_LIBRARY}" "${ROOT_EG_LIBRARY}" "${ROOT_GPAD_LIBRARY}"
      "${ROOT_GRAF_LIBRARY}" "${ROOT_GRAF3D_LIBRARY}" "${ROOT_RIO_LIBRARY}" "${ROOT_RIO_LIBRARY}")
   if(ROOT_MATHCORE_LIBRARY)
      set(ROOT_LIBS "${ROOT_LIBS}" "${ROOT_MATHCORE_LIBRARY}")
   endif(ROOT_MATHCORE_LIBRARY)

   if(NOT ROOT_LIBS)
      message(STATUS "ROOT NOT found!")
      set(ROOT_FOUND false)
   endif(NOT ROOT_LIBS)

   find_path(ROOT_INCLUDE_DIR TObject.h PATHS "${ROOTSYS_DIR}/include" "${ROOTSYS_DIR}/include/root")
else(ROOTSYS_DIR)
   message(STATUS "ROOT NOT found!")
   set(ROOT_FOUND false)
endif(ROOTSYS_DIR)

mark_as_advanced(ROOTSYS_DIR ROOT_CINT_LIBRARY ROOT_CORE_LIBRARY ROOT_HIST_LIBRARY ROOT_MATRIX_LIBRARY ROOT_PHYSICS_LIBRARY ROOT_EG_LIBRARY ROOT_GPAD_LIBRARY ROOT_GRAF_LIBRARY ROOT_GRAF3D_LIBRARY ROOT_RIO_LIBRARY ROOT_MATHCORE_LIBRARY ROOT_LIBS ROOT_INCLUDE_DIR)

