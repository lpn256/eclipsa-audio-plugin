// Copyright 2025 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "substream_rdr.h"

#include "bed2bed_rdr/BedToBedRdr.cpp"
#include "bin_rdr/BinauralRdr.cpp"
#include "hoa2bed_rdr/HOAToBedRdr.cpp"
#include "rdr_factory/RendererFactory.cpp"
#include "substream_rdr_utils/Speakers.cpp"
#include "surround_panner/AmbisonicPanner.cpp"
#include "surround_panner/BinauralPanner.cpp"
#include "surround_panner/MonoToSpeakerPanner.cpp"