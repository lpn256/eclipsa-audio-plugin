add_library(gpac INTERFACE)
target_include_directories(gpac INTERFACE "include/")

add_library(gpac_archive STATIC IMPORTED)
set_target_properties(gpac_archive PROPERTIES
  IMPORTED_LOCATION         "${CMAKE_CURRENT_LIST_DIR}/lib/libgpac_static.a"
  IMPORTED_LOCATION_DEBUG   "${CMAKE_CURRENT_LIST_DIR}/lib/libgpac_static.a"
  IMPORTED_LOCATION_RELEASE   "${CMAKE_CURRENT_LIST_DIR}/lib/libgpac_static.a"
  IMPORTED_LOCATION_RELWITHDEBINFO   "${CMAKE_CURRENT_LIST_DIR}/lib/libgpac_static.a"
  LINKER_LANGUAGE CXX)

# z-lib is an OSX system library that we have to link against here.
target_link_libraries(gpac INTERFACE gpac_archive z)