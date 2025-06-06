# Copyright 2025 Google LLC
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

# Function to add a test executable for CTest.
# test_name:    Test executable name.
# test_source:  Test source file name.
# test_libs:    Test link libraries as ';' separated list.
function(eclipsa_add_test test_name test_source test_libs)
    add_executable(${test_name} ${test_source})
    target_compile_definitions(${test_name} 
        PUBLIC
            JUCE_WEB_BROWSER=0
            JUCE_USE_CURL=0
            JUCE_VST3_CAN_REPLACE_VST2=0
            JUCE_SILENCE_XCODE_15_LINKER_WARNING)
    target_link_options(${test_name}
        PUBLIC
            "-Wl"
            "-ld_classic")
            
    # Add codec libraries for all test targets if on macOS
    if(APPLE)
        set(VENDOR_LIB_PATH "${CMAKE_SOURCE_DIR}/third_party/libiamf/third_party/lib/macos")
        target_link_directories(${test_name} PRIVATE ${VENDOR_LIB_PATH})
        # Append only if not already present in test_libs
        list(FIND test_libs "opus" opus_found)
        if(opus_found EQUAL -1)
            list(APPEND test_libs opus)
        endif()
        list(FIND test_libs "ogg" ogg_found)
        if(ogg_found EQUAL -1)
            list(APPEND test_libs ogg)
        endif()
    endif()

    # Add IAMF include directories if 'iamf' or 'iamfdec_utils' is requested
    if("${test_libs}" MATCHES "iamf" OR "${test_libs}" MATCHES "iamfdec_utils")
        if(DEFINED LIBIAMF_INCLUDE_DIRS)
             target_include_directories(${test_name} PRIVATE ${LIBIAMF_INCLUDE_DIRS})
        else()
             message(WARNING "LIBIAMF_INCLUDE_DIRS not defined, but required by ${test_name}")
        endif()
    endif()


    target_link_libraries(${test_name}
         PRIVATE
            ${test_libs}
        PUBLIC
            juce::juce_recommended_config_flags
            juce::juce_recommended_lto_flags
            juce::juce_recommended_warning_flags
            GTest::gtest_main)
    gtest_discover_tests(${test_name} 
    DISCOVERY_MODE PRE_TEST)

    # If iamfdec_utils was requested, make sure iamf is also linked (dependency)
    if(TARGET iamf AND "${test_libs}" MATCHES "iamfdec_utils")
        # Check if iamf is already in the list to avoid duplicates
        list(FIND test_libs "iamf" iamf_already_linked)
        if(iamf_already_linked EQUAL -1)
            # Link iamf privately as it's a dependency of a private lib
            target_link_libraries(${test_name} PRIVATE iamf)
            message(STATUS "Also linking core 'iamf' library as dependency for ${test_name}")
        endif()
    endif()
endfunction()