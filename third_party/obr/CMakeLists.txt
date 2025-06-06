add_library(obr INTERFACE)
target_compile_options(obr INTERFACE "-w") # Silence is golden
set(BUILD_TESTING OFF)
set(EIGEN_BUILD_TESTING OFF)
set(EIGEN_MPL2_ONLY ON)
set(EIGEN_BUILD_PKGCONFIG OFF)
set(EIGEN_BUILD_DOC OFF)
include("${CMAKE_SOURCE_DIR}/cmake/eigen.cmake") # Fetch Eigen for compilation dependencies.
include("${CMAKE_SOURCE_DIR}/cmake/pffft.cmake")
target_include_directories(obr INTERFACE "." "./obr" "./obr/renderer" "./obr/ambisonic_encoder")

add_library(obr_dylib SHARED IMPORTED)
set_target_properties(obr_dylib PROPERTIES
  IMPORTED_LOCATION         "${CMAKE_CURRENT_LIST_DIR}/lib/obr.dylib"
  IMPORTED_LOCATION_DEBUG   "${CMAKE_CURRENT_LIST_DIR}/lib/obr.dylib"
  IMPORTED_LOCATION_RELEASE "${CMAKE_CURRENT_LIST_DIR}/lib/obr.dylib"
  IMPORTED_LOCATION_RELWITHDEBINFO "${CMAKE_CURRENT_LIST_DIR}/lib/obr.dylib"
  IMPORTED_SONAME "obr.dylib"
  LINKER_LANGUAGE CXX)

target_link_libraries(obr 
  INTERFACE 
  obr_dylib
    Eigen
    pffft)