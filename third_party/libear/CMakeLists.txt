add_library(libear INTERFACE)
target_include_directories(libear INTERFACE "include/")
target_compile_options(libear INTERFACE "-w") # Silence is golden

add_library(libear_archive STATIC IMPORTED)
set_target_properties(libear_archive PROPERTIES
  IMPORTED_LOCATION         "${CMAKE_CURRENT_LIST_DIR}/lib/libear.a"
  IMPORTED_LOCATION_DEBUG   "${CMAKE_CURRENT_LIST_DIR}/lib/libear.a"
  IMPORTED_LOCATION_RELEASE "${CMAKE_CURRENT_LIST_DIR}/lib/libear.a"
  IMPORTED_LOCATION_RELWITHDEBINFO "${CMAKE_CURRENT_LIST_DIR}/lib/libear.a")

target_link_libraries(libear 
  INTERFACE 
    libear_archive
    Boost::optional 
    Boost::variant 
    Boost::math 
    Boost::algorithm)