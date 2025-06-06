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

#include "../implementation/MultiChannelGainRepository.h"

#include <data_structures/data_structures.h>
#include <gtest/gtest.h>

TEST(test_gain_repository, update) {
  // create a default channel gains object w/ 0.0f as the gain value
  juce::ValueTree test{"test"};  // an empty tree
  MultiChannelRepository repositoryInstance(test);

  juce::Uuid id = juce::Uuid();
  std::vector<float> gains(28, 1.5f);
  ChannelGains testSetup(id, gains, std::unordered_map<int, float>());

  repositoryInstance.update(testSetup);
  testSetup.setChannelGain(0, 5.0f);  // set channel 0 to 5.0f
  repositoryInstance.update(testSetup);

  ASSERT_EQ(repositoryInstance.get().getId(), id);
  ASSERT_EQ(repositoryInstance.get().getTotalChannels(), gains.size());
  ASSERT_EQ(repositoryInstance.get().getGains()[0], 5.0f);
  for (int i = 2; i < repositoryInstance.get().getTotalChannels(); i++) {
    ASSERT_EQ(repositoryInstance.get().getGains()[i], 1.5f);
  }

  testSetup = repositoryInstance.get();
  testSetup.toggleChannelMute(1);  // set channel 1 to 0.0f
  repositoryInstance.update(testSetup);
  std::vector<float> temp = repositoryInstance.get().getGains();
  ASSERT_EQ(repositoryInstance.get().getGains()[1], 0.f);
}
