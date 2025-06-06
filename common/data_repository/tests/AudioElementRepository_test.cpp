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

#include "../implementation/AudioElementRepository.h"

#include <data_structures/data_structures.h>
#include <gtest/gtest.h>

#include "substream_rdr/substream_rdr_utils/Speakers.h"

class TestAudioElementRepository : public AudioElementRepository {
 public:
  TestAudioElementRepository()
      : AudioElementRepository(juce::ValueTree{"test"}) {}
};

// Just a sanity check -- the test_base_repository suite should cover all
// meaningful repository logic

TEST(test_audio_element_repository, update_element) {
  TestAudioElementRepository repositoryInstance;
  AudioElement testElement({}, "test_name", Speakers::kStereo, 0);
  repositoryInstance.add(testElement);
  AudioElement initialState =
      repositoryInstance.get(testElement.getId()).value();
  ASSERT_EQ(testElement, initialState);

  testElement.setName("new_name");
  testElement.setChannelConfig(Speakers::k5Point1);
  ASSERT_TRUE(repositoryInstance.update(testElement));

  AudioElement updatedState =
      repositoryInstance.get(testElement.getId()).value();
  ASSERT_NE(updatedState, initialState);
  ASSERT_EQ(updatedState, testElement);
}

TEST(test_audio_element_repository, get_all) {
  TestAudioElementRepository repositoryInstance;
  AudioElement testElement({}, "test_name", Speakers::kStereo, 0);
  AudioElement testElement2({}, "test_name_2", Speakers::kMono, 2);
  repositoryInstance.add(testElement);
  repositoryInstance.add(testElement2);

  juce::OwnedArray<AudioElement> array;
  repositoryInstance.getAll(array);
  ASSERT_EQ(array.size(), 2);
  ASSERT_NE(*array.getUnchecked(0), *array.getUnchecked(1));
}