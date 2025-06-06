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

#include "../src/MixPresentationLoudness.h"

#include <gtest/gtest.h>
#include <juce_data_structures/juce_data_structures.h>

#include <array>

#include "substream_rdr/substream_rdr_utils/Speakers.h"

TEST(test_mix_presentation_loudness, validity) {
  // Create a mix presentation
  MixPresentationLoudness presentation1(juce::Uuid::null(), Speakers::kStereo);

  presentation1.replaceLargestLayout(Speakers::k7Point1Point2);

  // Create a second presentation from the tree of the first
  MixPresentationLoudness presentation2 =
      MixPresentationLoudness::fromTree(presentation1.toValueTree());

  // Ensure that both presentations are equal
  ASSERT_EQ(presentation1, presentation2);

  const std::array<LayoutLoudness, 2> kTestLayouts{
      LayoutLoudness(Speakers::kStereo),
      LayoutLoudness(Speakers::k5Point1Point4)};

  MixPresentationLoudness presentation3(juce::Uuid::null(),
                                        Speakers::k5Point1Point4);

  std::array<LayoutLoudness, 2> layouts = presentation3.getLayouts();

  for (int i = 0; i < 2; i++) {
    ASSERT_EQ(layouts[i], kTestLayouts[i]);
  }

  ASSERT_EQ(presentation3.getLargestLayout(), kTestLayouts[1].getLayout());

  // If the constructor is passed mono as the largest layout,
  // it should set stereo as the largest layout
  MixPresentationLoudness presentation4(juce::Uuid::null(), Speakers::kMono);

  const std::array<LayoutLoudness, 2> kTestLayouts2{
      LayoutLoudness(Speakers::kStereo), LayoutLoudness(Speakers::kMono)};

  layouts = presentation4.getLayouts();
  for (int i = 0; i < 2; i++) {
    ASSERT_EQ(layouts[i], kTestLayouts2[i]);
  }
}