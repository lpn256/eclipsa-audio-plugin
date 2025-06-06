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

#include <gtest/gtest.h>

#include "data_structures/src/PlaybackMS.h"

TEST(test_mute_solo_type, from_tree) {
  juce::String mutedChannelString = "00000000001010";
  juce::String soloedChannelString = "00000000000101";
  const juce::ValueTree tree{
      PlaybackMS::kTreeType,
      {{PlaybackMS::kId, {}},
       {PlaybackMS::kMutedChannelsID, mutedChannelString},
       {PlaybackMS::kSoloedChannelsID, soloedChannelString}}};

  PlaybackMS data = PlaybackMS::fromTree(tree);

  auto mutedChannels = data.getMutedChannels();
  auto soloedChannels = data.getSoloedChannels();

  // Reverse the strings to match the bitset order.
  std::string mutedChannelsStdStr = mutedChannelString.toStdString();
  std::string soloedChannelsStdStr = soloedChannelString.toStdString();
  std::reverse(mutedChannelsStdStr.begin(), mutedChannelsStdStr.end());
  std::reverse(soloedChannelsStdStr.begin(), soloedChannelsStdStr.end());

  for (int i = mutedChannelsStdStr.length() - 1; i >= 0; --i) {
    if (mutedChannelsStdStr[i] == '1') {
      EXPECT_TRUE(mutedChannels[i]);
    } else if (mutedChannelsStdStr[i] == '0') {
      EXPECT_FALSE(mutedChannels[i]);
    }
  }

  for (int i = soloedChannelsStdStr.length() - 1; i >= 0; --i) {
    if (soloedChannelsStdStr[i] == '1') {
      EXPECT_TRUE(soloedChannels[i]);
    } else if (soloedChannelsStdStr[i] == '0') {
      EXPECT_FALSE(soloedChannels[i]);
    }
  }
}