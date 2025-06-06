add_library(iamftools INTERFACE)
target_compile_options(iamftools INTERFACE "-w") # Silence is golden
target_include_directories(iamftools INTERFACE "include/")

add_library(iamftools_dylib SHARED IMPORTED)
set_target_properties(iamftools_dylib PROPERTIES
  IMPORTED_LOCATION         "${CMAKE_CURRENT_LIST_DIR}/lib/libiamf_renderer_encoder.dylib"
  IMPORTED_LOCATION_DEBUG   "${CMAKE_CURRENT_LIST_DIR}/lib/libiamf_renderer_encoder.dylib"
  IMPORTED_LOCATION_RELEASE "${CMAKE_CURRENT_LIST_DIR}/lib/libiamf_renderer_encoder.dylib"
  IMPORTED_LOCATION_RELWITHDEBINFO "${CMAKE_CURRENT_LIST_DIR}/lib/libiamf_renderer_encoder.dylib"
  IMPORTED_SONAME "libiamf_renderer_encoder.dylib"
  LINKER_LANGUAGE CXX)

# Parse the commit hash
# Set the path to the README.md file
set(README_FILE "${CMAKE_CURRENT_LIST_DIR}/README.md")

file(STRINGS "${README_FILE}" COMMIT_LINE REGEX "Commit: https://github.com/AOMediaCodec/iamf-tools/commit/([a-f0-9]+)")
string(REGEX REPLACE ".*commit/([a-f0-9]+).*" "GitHub/iamf-tools@\\1" COMMIT_HASH "${COMMIT_LINE}")

# Set the path to the header and template file
set(HEADER_FILE "${CMAKE_CURRENT_LIST_DIR}/include/iamftools/commit_hash.h")
set(TEMPLATE_FILE "${CMAKE_CURRENT_LIST_DIR}/include/iamftools/commit_hash.h.in")

# Configure the header file
configure_file(${TEMPLATE_FILE} ${HEADER_FILE} @ONLY)

# Find required protobuf file generate fx
include("${CMAKE_SOURCE_DIR}/third_party/protobuf/cmake/protobuf-generate.cmake")

set(PROTO_FILES
  "proto/arbitrary_obu.proto"
  "proto/audio_element.proto"
  "proto/audio_frame.proto"
  "proto/codec_config.proto"
  "proto/ia_sequence_header.proto"
  "proto/mix_presentation.proto"
  "proto/obu_header.proto"
  "proto/param_definitions.proto"
  "proto/parameter_block.proto"
  "proto/parameter_data.proto"
  "proto/temporal_delimiter.proto"
  "proto/test_vector_metadata.proto"
  "proto/user_metadata.proto"
)

set(PROTO_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")

# Generate protobuf files
add_library(proto-objects OBJECT ${PROTO_FILES})
target_include_directories(proto-objects PUBLIC ${PROTO_BINARY_DIR})
target_link_libraries(proto-objects PUBLIC protobuf::libprotobuf)
protobuf_generate(
  TARGET proto-objects
  IMPORT_DIRS "${CMAKE_CURRENT_LIST_DIR}/proto"
  PROTOC_OUT_DIR "${PROTO_BINARY_DIR}"
)

target_link_libraries(iamftools
  INTERFACE
    iamftools_dylib
    proto-objects
)

if(CI_TEST OR INTERNAL_TEST)
  eclipsa_add_test(test_iamf_sanity test/libiamf_test.cpp "iamftools")
endif()