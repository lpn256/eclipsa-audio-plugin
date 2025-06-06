/*
 * Copyright 2025 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include "iamf_dec/m2m_rdr.h"
#include "substream_rdr/substream_rdr_utils/Speakers.h"

////////////////////////////////
struct LayoutPairRdrMat {
  struct LayoutPair {
    Speakers::AudioElementSpeakerLayout in, out;
  };
  const LayoutPair layouts;
  const float* rdrMat;
};

// Table mapping a bed layout pair to its render matrix.
// Ideally, this would be STL map, but as making objects
// with dynamic allocation static potentially introduces UB we use this
// workaround.
static const std::array<LayoutPairRdrMat, 110> LayoutTranscodes = {{
    // Mono matrix mappings
    {{Speakers::kMono, Speakers::kMono}, (float*)mono_mono},
    {{Speakers::kMono, Speakers::kStereo}, (float*)mono_bs020},
    {{Speakers::kMono, Speakers::k5Point1}, (float*)mono_bs050},
    {{Speakers::kMono, Speakers::k5Point1Point2}, (float*)mono_bs250},
    {{Speakers::kMono, Speakers::k5Point1Point4}, (float*)mono_bs450},
    {{Speakers::kMono, Speakers::k7Point1}, (float*)mono_bs070},
    {{Speakers::kMono, Speakers::k7Point1Point4}, (float*)mono_bs470},
    {{Speakers::kMono, Speakers::k3Point1Point2}, (float*)mono_iamf312},
    {{Speakers::kMono, Speakers::k7Point1Point2}, (float*)mono_iamf712},
    {{Speakers::kMono, Speakers::kExpl9Point1Point6}, (float*)mono_iamf916},

    // Stereo matrix mappings
    {{Speakers::kStereo, Speakers::kMono}, (float*)stereo_mono},
    {{Speakers::kStereo, Speakers::kStereo}, (float*)stereo_bs020},
    {{Speakers::kStereo, Speakers::k5Point1}, (float*)stereo_bs050},
    {{Speakers::kStereo, Speakers::k5Point1Point2}, (float*)stereo_bs250},
    {{Speakers::kStereo, Speakers::k5Point1Point4}, (float*)stereo_bs450},
    {{Speakers::kStereo, Speakers::k7Point1}, (float*)stereo_bs070},
    {{Speakers::kStereo, Speakers::k7Point1Point4}, (float*)stereo_bs470},
    {{Speakers::kStereo, Speakers::k3Point1Point2}, (float*)stereo_iamf312},
    {{Speakers::kStereo, Speakers::k7Point1Point2}, (float*)stereo_iamf712},
    {{Speakers::kStereo, Speakers::kExpl9Point1Point6}, (float*)stereo_iamf916},

    // 3.1.2 matrix mappings
    {{Speakers::k3Point1Point2, Speakers::kMono}, (float*)iamf312_mono},
    {{Speakers::k3Point1Point2, Speakers::kStereo}, (float*)iamf312_bs020},
    {{Speakers::k3Point1Point2, Speakers::k5Point1}, (float*)iamf312_bs050},
    {{Speakers::k3Point1Point2, Speakers::k5Point1Point2},
     (float*)iamf312_bs250},
    {{Speakers::k3Point1Point2, Speakers::k5Point1Point4},
     (float*)iamf312_bs450},
    {{Speakers::k3Point1Point2, Speakers::k7Point1}, (float*)iamf312_bs070},
    {{Speakers::k3Point1Point2, Speakers::k7Point1Point4},
     (float*)iamf312_bs470},
    {{Speakers::k3Point1Point2, Speakers::k3Point1Point2},
     (float*)iamf312_iamf312},
    {{Speakers::k3Point1Point2, Speakers::k7Point1Point2},
     (float*)iamf312_iamf712},
    {{Speakers::k3Point1Point2, Speakers::kExpl9Point1Point6},
     (float*)iamf312_iamf916},

    // 5.1.0 matrix mappings
    {{Speakers::k5Point1, Speakers::kMono}, (float*)iamf51_mono},
    {{Speakers::k5Point1, Speakers::kStereo}, (float*)iamf51_bs020},
    {{Speakers::k5Point1, Speakers::k5Point1}, (float*)iamf51_bs050},
    {{Speakers::k5Point1, Speakers::k5Point1Point2}, (float*)iamf51_bs250},
    {{Speakers::k5Point1, Speakers::k5Point1Point4}, (float*)iamf51_bs450},
    {{Speakers::k5Point1, Speakers::k7Point1}, (float*)iamf51_bs070},
    {{Speakers::k5Point1, Speakers::k7Point1Point4}, (float*)iamf51_bs470},
    {{Speakers::k5Point1, Speakers::k3Point1Point2}, (float*)iamf51_iamf312},
    {{Speakers::k5Point1, Speakers::k7Point1Point2}, (float*)iamf51_iamf712},
    {{Speakers::k5Point1, Speakers::kExpl9Point1Point6},
     (float*)iamf51_iamf916},

    // 5.1.2 matrix mappings
    {{Speakers::k5Point1Point2, Speakers::kMono}, (float*)iamf512_mono},
    {{Speakers::k5Point1Point2, Speakers::kStereo}, (float*)iamf512_bs020},
    {{Speakers::k5Point1Point2, Speakers::k5Point1}, (float*)iamf512_bs050},
    {{Speakers::k5Point1Point2, Speakers::k5Point1Point2},
     (float*)iamf512_bs250},
    {{Speakers::k5Point1Point2, Speakers::k5Point1Point4},
     (float*)iamf512_bs450},
    {{Speakers::k5Point1Point2, Speakers::k7Point1}, (float*)iamf512_bs070},
    {{Speakers::k5Point1Point2, Speakers::k7Point1Point4},
     (float*)iamf512_bs470},
    {{Speakers::k5Point1Point2, Speakers::k3Point1Point2},
     (float*)iamf512_iamf312},
    {{Speakers::k5Point1Point2, Speakers::k7Point1Point2},
     (float*)iamf512_iamf712},
    {{Speakers::k5Point1Point2, Speakers::kExpl9Point1Point6},
     (float*)iamf512_iamf916},

    // 5.1.4 matrix mappings
    {{Speakers::k5Point1Point4, Speakers::kMono}, (float*)iamf514_mono},
    {{Speakers::k5Point1Point4, Speakers::kStereo}, (float*)iamf514_bs020},
    {{Speakers::k5Point1Point4, Speakers::k5Point1}, (float*)iamf514_bs050},
    {{Speakers::k5Point1Point4, Speakers::k5Point1Point2},
     (float*)iamf514_bs250},
    {{Speakers::k5Point1Point4, Speakers::k5Point1Point4},
     (float*)iamf514_bs450},
    {{Speakers::k5Point1Point4, Speakers::k7Point1}, (float*)iamf514_bs070},
    {{Speakers::k5Point1Point4, Speakers::k7Point1Point4},
     (float*)iamf514_bs470},
    {{Speakers::k5Point1Point4, Speakers::k3Point1Point2},
     (float*)iamf514_iamf312},
    {{Speakers::k5Point1Point4, Speakers::k7Point1Point2},
     (float*)iamf514_iamf712},
    {{Speakers::k5Point1Point4, Speakers::kExpl9Point1Point6},
     (float*)iamf514_iamf916},

    // 7.1.0 matrix mappings
    {{Speakers::k7Point1, Speakers::kMono}, (float*)iamf71_mono},
    {{Speakers::k7Point1, Speakers::kStereo}, (float*)iamf71_bs020},
    {{Speakers::k7Point1, Speakers::k5Point1}, (float*)iamf71_bs050},
    {{Speakers::k7Point1, Speakers::k5Point1Point2}, (float*)iamf71_bs250},
    {{Speakers::k7Point1, Speakers::k5Point1Point4}, (float*)iamf71_bs450},
    {{Speakers::k7Point1, Speakers::k7Point1}, (float*)iamf71_bs070},
    {{Speakers::k7Point1, Speakers::k7Point1Point4}, (float*)iamf71_bs470},
    {{Speakers::k7Point1, Speakers::k3Point1Point2}, (float*)iamf71_iamf312},
    {{Speakers::k7Point1, Speakers::k7Point1Point2}, (float*)iamf71_iamf712},
    {{Speakers::k7Point1, Speakers::kExpl9Point1Point6},
     (float*)iamf71_iamf916},

    // 7.1.2 matrix mappings
    {{Speakers::k7Point1Point2, Speakers::kMono}, (float*)iamf712_mono},
    {{Speakers::k7Point1Point2, Speakers::kStereo}, (float*)iamf712_bs020},
    {{Speakers::k7Point1Point2, Speakers::k5Point1}, (float*)iamf712_bs050},
    {{Speakers::k7Point1Point2, Speakers::k5Point1Point2},
     (float*)iamf712_bs250},
    {{Speakers::k7Point1Point2, Speakers::k5Point1Point4},
     (float*)iamf712_bs450},
    {{Speakers::k7Point1Point2, Speakers::k7Point1}, (float*)iamf712_bs070},
    {{Speakers::k7Point1Point2, Speakers::k7Point1Point4},
     (float*)iamf712_bs470},
    {{Speakers::k7Point1Point2, Speakers::k3Point1Point2},
     (float*)iamf712_iamf312},
    {{Speakers::k7Point1Point2, Speakers::k7Point1Point2},
     (float*)iamf712_iamf712},
    {{Speakers::k7Point1Point2, Speakers::kExpl9Point1Point6},
     (float*)iamf712_iamf916},

    // 7.1.4 matrix mappings
    {{Speakers::k7Point1Point4, Speakers::kMono}, (float*)iamf714_mono},
    {{Speakers::k7Point1Point4, Speakers::kStereo}, (float*)iamf714_bs020},
    {{Speakers::k7Point1Point4, Speakers::k5Point1}, (float*)iamf714_bs050},
    {{Speakers::k7Point1Point4, Speakers::k5Point1Point2},
     (float*)iamf714_bs250},
    {{Speakers::k7Point1Point4, Speakers::k5Point1Point4},
     (float*)iamf714_bs450},
    {{Speakers::k7Point1Point4, Speakers::k7Point1}, (float*)iamf714_bs070},
    {{Speakers::k7Point1Point4, Speakers::k7Point1Point4},
     (float*)iamf714_bs470},
    {{Speakers::k7Point1Point4, Speakers::k3Point1Point2},
     (float*)iamf714_iamf312},
    {{Speakers::k7Point1Point4, Speakers::k7Point1Point2},
     (float*)iamf714_iamf712},
    {{Speakers::k7Point1Point4, Speakers::kExpl9Point1Point6},
     (float*)iamf714_iamf916},

    // Binaural matrix mappings
    {{Speakers::kBinaural, Speakers::kMono}, (float*)stereo_mono},
    {{Speakers::kBinaural, Speakers::kStereo}, (float*)stereo_bs020},
    {{Speakers::kBinaural, Speakers::k5Point1}, (float*)stereo_bs050},
    {{Speakers::kBinaural, Speakers::k5Point1Point2}, (float*)stereo_bs250},
    {{Speakers::kBinaural, Speakers::k5Point1Point4}, (float*)stereo_bs450},
    {{Speakers::kBinaural, Speakers::k7Point1}, (float*)stereo_bs070},
    {{Speakers::kBinaural, Speakers::k7Point1Point4}, (float*)stereo_bs470},
    {{Speakers::kBinaural, Speakers::k3Point1Point2}, (float*)stereo_iamf312},
    {{Speakers::kBinaural, Speakers::k7Point1Point2}, (float*)stereo_iamf712},
    {{Speakers::kBinaural, Speakers::kExpl9Point1Point6},
     (float*)stereo_iamf916},

    // 9.1.6 matrix mappings
    {{Speakers::kExpl9Point1Point6, Speakers::kMono}, (float*)iamf916_mono},
    {{Speakers::kExpl9Point1Point6, Speakers::kStereo}, (float*)iamf916_bs020},
    {{Speakers::kExpl9Point1Point6, Speakers::k5Point1}, (float*)iamf916_bs050},
    {{Speakers::kExpl9Point1Point6, Speakers::k5Point1Point2},
     (float*)iamf916_bs250},
    {{Speakers::kExpl9Point1Point6, Speakers::k5Point1Point4},
     (float*)iamf916_bs450},
    {{Speakers::kExpl9Point1Point6, Speakers::k7Point1}, (float*)iamf916_bs070},
    {{Speakers::kExpl9Point1Point6, Speakers::k7Point1Point4},
     (float*)iamf916_bs470},
    {{Speakers::kExpl9Point1Point6, Speakers::k3Point1Point2},
     (float*)iamf916_iamf312},
    {{Speakers::kExpl9Point1Point6, Speakers::k7Point1Point2},
     (float*)iamf916_iamf712},
    {{Speakers::kExpl9Point1Point6, Speakers::kExpl9Point1Point6},
     (float*)iamf916_iamf916},
}};

inline const float* getMatrixFromLayouts(
    const Speakers::AudioElementSpeakerLayout in,
    const Speakers::AudioElementSpeakerLayout out) {
  auto it =
      std::find_if(LayoutTranscodes.begin(), LayoutTranscodes.end(),
                   [in, out](const LayoutPairRdrMat& pm) {
                     return (pm.layouts.in == in && pm.layouts.out == out);
                   });
  return it != LayoutTranscodes.end() ? it->rdrMat : nullptr;
}