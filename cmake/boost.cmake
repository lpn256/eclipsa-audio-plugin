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

message(STATUS "Fetching Boost")
set(BOOST_INCLUDE_LIBRARIES algorithm math optional variant lockfree log log_setup thread filesystem)
set(BOOST_ENABLE_CMAKE ON)
include(FetchContent)
FetchContent_Declare(
  Boost
  USES_TERMINAL_DOWNLOAD TRUE
  DOWNLOAD_NO_EXTRACT FALSE
  OVERRIDE_FIND_PACKAGE
  URL "https://github.com/boostorg/boost/releases/download/boost-1.82.0/boost-1.82.0.tar.gz"
  CMAKE_ARGS -DBUILD_SHARED_LIBS=OFF)
FetchContent_MakeAvailable(Boost)
find_package(Boost 1.82.0 REQUIRED COMPONENTS algorithm math optional variant lockfree log log_setup thread filesystem)
