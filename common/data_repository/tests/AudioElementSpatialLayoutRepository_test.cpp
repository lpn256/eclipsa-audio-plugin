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

#include "../implementation/AudioElementSpatialLayoutRepository.h"

#include <data_structures/data_structures.h>
#include <gtest/gtest.h>

#include "substream_rdr/substream_rdr_utils/Speakers.h"

TEST(test_audioElementSpatialLayout_repository, update) {
  // create a default channel gains object w/ 0.0f as the gain value
  juce::ValueTree test{"test"};  // an empty tree
  AudioElementSpatialLayoutRepository repositoryInstance(test);

  juce::Uuid id = juce::Uuid();
  juce::String name = "testAudioElementSpatialLayout";
  juce::Uuid audioElement = juce::Uuid();
  juce::int32 firstChannel = 0;
  Speakers::AudioElementSpeakerLayout speakers = Speakers::kStereo;
  AudioElementSpatialLayout testSetup(
      id, name, audioElement, firstChannel, speakers, true,
      AudioElementSpatialLayout::Elevation::kFlat);

  repositoryInstance.update(testSetup);

  ASSERT_EQ(repositoryInstance.get().getId(), id);
  ASSERT_EQ(repositoryInstance.get().getName(), name);
  ASSERT_EQ(repositoryInstance.get().getAudioElementId(), audioElement);
  ASSERT_EQ(repositoryInstance.get().getFirstChannel(), firstChannel);
  ASSERT_EQ(repositoryInstance.get().getChannelLayout(), speakers);
  ASSERT_EQ(repositoryInstance.get().getElevation(),
            AudioElementSpatialLayout::Elevation::kFlat);
  ASSERT_EQ(repositoryInstance.get().isPanningEnabled(), true);

  AudioElementSpatialLayout toUpdate = repositoryInstance.get();
  toUpdate.setFirstChannel(1);
  toUpdate.setLayout(Speakers::k5Point1);
  toUpdate.setElevation(AudioElementSpatialLayout::Elevation::kArch);
  repositoryInstance.update(toUpdate);

  ASSERT_EQ(repositoryInstance.get().getFirstChannel(), 1);
  ASSERT_EQ(repositoryInstance.get().getChannelLayout(), Speakers::k5Point1);
  ASSERT_EQ(repositoryInstance.get().getElevation(),
            AudioElementSpatialLayout::Elevation::kArch);
}