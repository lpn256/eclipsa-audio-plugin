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

#include "../implementation/MixPresentationLoudnessRepository.h"

#include <data_structures/data_structures.h>
#include <gtest/gtest.h>

#include <array>

#include "data_structures/src/MixPresentation.h"
#include "data_structures/src/MixPresentationLoudness.h"
#include "substream_rdr/substream_rdr_utils/Speakers.h"

// Just a sanity check -- the test_base_repository suite should cover all
// meaningful repository logic
TEST(test_mix_presentation_loudness_repository, update) {
  juce::ValueTree test{"test"};
  MixPresentationLoudnessRepository repositoryInstance(test);

  const juce::Uuid presentationUuid = juce::Uuid();
  MixPresentationLoudness presentation(presentationUuid, Speakers::kStereo);

  const juce::Uuid presentationUuid2 = juce::Uuid();
  MixPresentationLoudness presentation2(presentationUuid2,
                                        Speakers::k7Point1Point2);

  repositoryInstance.add(presentation);
  repositoryInstance.add(presentation2);

  ASSERT_EQ(repositoryInstance.getItemCount(), 2);

  const std::array<LayoutLoudness, 2> kTestLayouts{
      LayoutLoudness(Speakers::kStereo), LayoutLoudness(Speakers::kMono)};
  const std::array<LayoutLoudness, 2> kTestLayouts2{
      LayoutLoudness(Speakers::kStereo),
      LayoutLoudness(Speakers::k7Point1Point2)};

  // swap the largest layout of the two presentations
  presentation.replaceLargestLayout(Speakers::k7Point1Point2);
  presentation2.replaceLargestLayout(Speakers::kStereo);

  repositoryInstance.update(presentation);
  repositoryInstance.update(presentation2);

  presentation = repositoryInstance.get(presentationUuid).value();
  std::array<LayoutLoudness, 2> layouts = presentation.getLayouts();

  presentation2 = repositoryInstance.get(presentationUuid2).value();
  std::array<LayoutLoudness, 2> layouts2 = presentation2.getLayouts();

  for (int i = 0; i < 2; i++) {
    ASSERT_EQ(layouts[i], kTestLayouts2[i]);
    ASSERT_EQ(layouts2[i], kTestLayouts[i]);
  }
}