# Copyright 2024 Google LLC
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#      http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

message(STATUS "Fetching LibIAMF")

include(FetchContent) # Ensure FetchContent is included

FetchContent_Declare(
  libiamf
  GIT_REPOSITORY https://github.com/AOMediaCodec/libiamf.git
  GIT_TAG 48b8b5dc06971e14826d114fd032dab285f4c944
)

# Set options for libiamf before making it available
set(CODEC_CAP ON CACHE BOOL "Codec capability check" FORCE)
set(MULTICHANNEL_BINAURALIZER OFF CACHE BOOL "Enable multichannel binaural rendering" FORCE)
set(HOA_BINAURALIZER OFF CACHE BOOL "Enable HOA binaural rendering" FORCE)

# Make libiamf available
FetchContent_GetProperties(libiamf)
if(NOT libiamf_POPULATED)
  FetchContent_Populate(libiamf)
  
  # Handle macOS-specific library issues BEFORE adding the subdirectory
  if(APPLE)
    # Path to our vendored libraries
    set(VENDOR_LIB_PATH "${CMAKE_SOURCE_DIR}/third_party/libiamf/third_party/lib/macos")
    
    # Remove incompatible bundled libraries
    file(REMOVE
      "${libiamf_SOURCE_DIR}/code/dep_codecs/lib/libopus.a"
      "${libiamf_SOURCE_DIR}/code/dep_codecs/lib/libfdk-aac.a"
      "${libiamf_SOURCE_DIR}/code/dep_codecs/lib/libFLAC.a"
    )
    
    # Copy our vendored libraries to the expected location
    file(COPY 
      "${VENDOR_LIB_PATH}/libopus.a"
      "${VENDOR_LIB_PATH}/libFLAC.a"
      "${VENDOR_LIB_PATH}/libfdk-aac.a"
      "${VENDOR_LIB_PATH}/libogg.a"
      DESTINATION "${libiamf_SOURCE_DIR}/code/dep_codecs/lib/"
    )
    
    # Add the library path to the linker
    link_directories("${VENDOR_LIB_PATH}")
  endif()
  
  # Add the subdirectory to create the iamf target
  add_subdirectory(${libiamf_SOURCE_DIR}/code ${libiamf_BINARY_DIR})
endif()

if(TARGET iamf)
  target_link_libraries(iamf ogg)
endif()

# --- Define LIBIAMF_INCLUDE_DIRS ---
# Include directories needed for compiling against libiamf AND iamfdec_utils
set(LIBIAMF_INCLUDE_DIRS
  ${libiamf_SOURCE_DIR}/code/include
  ${libiamf_SOURCE_DIR}/code/src/common
  ${libiamf_SOURCE_DIR}/code/src/iamf_dec
  ${libiamf_SOURCE_DIR}/code/src/iamf_dec/arch
  ${libiamf_SOURCE_DIR}/code/src/iamf_dec/arch/arm
  ${libiamf_SOURCE_DIR}/code/src/iamf_dec/arch/x86
  ${libiamf_SOURCE_DIR}/code/src/iamf_dec/aac
  ${libiamf_SOURCE_DIR}/code/src/iamf_dec/flac
  ${libiamf_SOURCE_DIR}/code/src/iamf_dec/opus
  ${libiamf_SOURCE_DIR}/code/src/iamf_dec/pcm
  ${libiamf_SOURCE_DIR}/code/dep_external/include/wav
  ${libiamf_SOURCE_DIR}/code/test/tools/iamfdec/include
  CACHE INTERNAL "LIBIAMF include directories"
)
# Force update the cache variable if it changed
set(LIBIAMF_INCLUDE_DIRS ${LIBIAMF_INCLUDE_DIRS} CACHE INTERNAL "LIBIAMF include directories" FORCE)


# --- Build iamfdec_utils library WITHOUT main() ---
# Check if the source directory for the utils exists
if(EXISTS "${libiamf_SOURCE_DIR}/code/test/tools/iamfdec/src")
  # Explicitly list source files, EXCLUDING the one with the main function (test_iamfdec.c)
  set(IAMFDEC_UTIL_SOURCES
      ${libiamf_SOURCE_DIR}/code/test/tools/iamfdec/src/dmemory.c
      ${libiamf_SOURCE_DIR}/code/test/tools/iamfdec/src/iamf_header.c
      ${libiamf_SOURCE_DIR}/code/test/tools/iamfdec/src/mp4demux.c
      ${libiamf_SOURCE_DIR}/code/test/tools/iamfdec/src/mp4iamfpar.c
      ${libiamf_SOURCE_DIR}/code/test/tools/iamfdec/src/vlogging_iamfmp4_sr.c
  )

  if(IAMFDEC_UTIL_SOURCES)
    # Create a STATIC library containing only the utility functions
    add_library(iamfdec_utils STATIC ${IAMFDEC_UTIL_SOURCES})

    # Add necessary include directories for compiling and using this library
    target_include_directories(iamfdec_utils PUBLIC
        ${libiamf_SOURCE_DIR}/code/test/tools/iamfdec/include # Public headers for utils
        $<TARGET_PROPERTY:iamf,INTERFACE_INCLUDE_DIRECTORIES> # Include dirs from core iamf
    )
    # Add internal include directories if needed by the sources themselves
    target_include_directories(iamfdec_utils PRIVATE
        ${LIBIAMF_INCLUDE_DIRS} # Includes internal iamf headers
    )

    # Link iamfdec_utils against the core iamf library
    if(TARGET iamf)
      target_link_libraries(iamfdec_utils PUBLIC iamf)
    else()
      message(WARNING "Core 'iamf' target not found for linking iamfdec_utils.")
    endif()
    message(STATUS "Created iamfdec_utils library (without main).")
  else()
     message(WARNING "Could not find source files for iamfdec_utils library.")
  endif()
else()
    message(WARNING "Path for iamfdec utility sources does not exist: ${libiamf_SOURCE_DIR}/code/test/tools/iamfdec/src")
endif()

message(STATUS "LIBIAMF source directory: ${libiamf_SOURCE_DIR}")