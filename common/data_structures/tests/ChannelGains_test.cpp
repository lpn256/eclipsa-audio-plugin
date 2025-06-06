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

#include "../src/ChannelGains.h"

#include <gtest/gtest.h>
#include <juce_data_structures/juce_data_structures.h>

// test the default constructor
TEST(test_channel_gains, default_value) {
  // Test the default constructor
  ChannelGains channelGains;
  ASSERT_EQ(channelGains.getGains()[0],
            1.f);  // the default value should be 1.f
  ASSERT_EQ(channelGains.getTotalChannels(), 1);  // there should be one channel

  ASSERT_EQ(channelGains.getMutedChannels().size(),
            0);  // there should be no muted channels
}

// test the from_value_tree method
TEST(test_channel_gains, from_value_tree) {
  // create a ChannelGains object calling the constructor
  juce::Uuid id = juce::Uuid();
  ChannelGains channelGains(id, "", 3);
  channelGains.setGains({1.0f, 2.0f, 3.0f});

  std::vector<float> test_gains = {1.0f, 2.0f, 3.0f};

  // Manually create a tree object
  // muted channels are optional
  juce::ValueTree tree{ChannelGains::kTreeType,
                       {{ChannelGains::kId, id.toString()},
                        {ChannelGains::kTotalChannels, 3},
                        {ChannelGains::kGains, juce::String("1.0 2.0 3.0")}}};

  // create a ChannelGains object from the ValueTree object
  const ChannelGains channelGains2 = ChannelGains::fromTree(tree);

  // validate that the name and totalChannels are the same
  ASSERT_EQ(channelGains2.getTotalChannels(), channelGains.getTotalChannels());

  // validate that the single channel gains are the same
  for (int i = 0; i < channelGains.getTotalChannels(); i++) {
    ASSERT_EQ(channelGains.getGains()[i], channelGains2.getGains()[i]);
  }

  channelGains.toggleChannelMute(0);
  channelGains.toggleChannelMute(2);

  juce::ValueTree tree2{ChannelGains::kTreeType,
                        {{ChannelGains::kId, id.toString()},
                         {ChannelGains::kTotalChannels, 3},
                         {ChannelGains::kGains, juce::String("0.0 2.0 0.0")},
                         {ChannelGains::kMutedChannels, juce::String("0 2")},
                         {ChannelGains::kPrevGain, juce::String("1.0 3.0")}}};

  const ChannelGains channelGains3 = ChannelGains::fromTree(tree2);
  for (int i = 0; i < channelGains3.getTotalChannels(); i++) {
    ASSERT_EQ(channelGains.getGains()[i], channelGains3.getGains()[i]);
  }

  std::unordered_map<int, float> mutedChannels =
      channelGains.getMutedChannels();
  for (auto pair : mutedChannels) {
    ASSERT_EQ(mutedChannels[pair.first],
              channelGains3.getMutedChannels()[pair.first]);
  }
}

// test the to_value_tree method
TEST(test_channel_gains, to_value_tree) {
  // create a ChannelGains object calling the constructor
  juce::Uuid id = juce::Uuid();

  const ChannelGains channelGains(id, "multiChannelGains", 3);

  // create a ValueTree object
  const juce::ValueTree tree(channelGains.toValueTree());

  // validate that the tree types, number of channels and names are the same
  ASSERT_EQ(tree.getType(), ChannelGains::kTreeType);
  ASSERT_EQ(static_cast<int>(tree[ChannelGains::kTotalChannels]),
            static_cast<int>(channelGains.getTotalChannels()));

  // validate that the single channel gains are the same
  std::vector<float> gains_fromtree =
      ChannelGains::convertStringsToFloats(ChannelGains::splitStringBySpace(
          tree[ChannelGains::kGains].toString().toStdString()));
  ASSERT_EQ(gains_fromtree, channelGains.getGains());
}

// test the from_vector constructor
TEST(test_channel_gains, from_vector) {
  std::vector<float> test_gains = {1.0f, 2.0f, 3.0f};
  juce::Uuid id = juce::Uuid();
  // create test vectors for gains and channel identifiers
  std::vector<float> gains(
      {1.0f, 2.0f, 3.0f});  // these will be copied into the ChannelGains object
  ChannelGains channelGains(id, gains, std::unordered_map<int, float>());
  ASSERT_EQ(channelGains.getGains(), test_gains);
}

TEST(test_channel_gains, set_gains) {
  // create a default channel gains object w/ 0.0f as the gain value
  juce::Uuid id = juce::Uuid();
  ChannelGains channelGains(id, "multiChannelGains", 3);

  // Apply new gains to all the channel gains object
  std::vector<float> test_gains({1.0f, 2.0f, 3.0f});
  channelGains.setGains(test_gains);
  ASSERT_EQ(channelGains.getGains(), test_gains);

  // update individual channel gains
  channelGains.setChannelGain(0, 5.0f);  // set channel 1 to 2.0f
  channelGains.setChannelGain(1, 5.0f);  // set channel 2 to 3.0f
  channelGains.setChannelGain(2, 5.0f);  // set channel 3 to 4.0f
  ASSERT_EQ(channelGains.getGains(), std::vector<float>(3, 5.0f));
}

TEST(test_channel_gains, mute_channels) {
  juce::Uuid id = juce::Uuid();
  ChannelGains channelGains(id, "multiChannelGains", 3);

  // Apply new gains to all the channel gains object
  std::vector<float> test_gains({1.0f, 2.0f, 3.0f});
  channelGains.setGains(test_gains);

  // mute channels 1 and 3
  channelGains.toggleChannelMute(0);
  channelGains.toggleChannelMute(2);

  // validate that the muted channels are stored in the mutedChannels_ map
  std::unordered_map<int, float> mutedChannels =
      channelGains.getMutedChannels();
  ASSERT_EQ(mutedChannels.size(),
            2);  // validate that only 2 channels have been muted
  ASSERT_EQ(mutedChannels[0],
            test_gains[0]);  // validate that channel 1 has been muted, and it's
                             // original gain was stored
  ASSERT_EQ(mutedChannels[2],
            test_gains[2]);  // validate that channel 3 has been muted, and it's
                             // original gain was stored

  // validate that channel 1 and 3 have a gain of 0.0f and channel 2 has a gain
  // of 2.0f
  for (int i = 0; i < channelGains.getGains().size(); i++) {
    if (i == 0 || i == 2) {
      ASSERT_EQ(channelGains.getGains()[i], 0.0f);
    } else {
      ASSERT_EQ(channelGains.getGains()[i], test_gains[i]);
    }
  }

  // validate that the channels can be unmuted
  channelGains.toggleChannelMute(0);
  channelGains.toggleChannelMute(2);

  // validate that the muted_channels map is empty
  mutedChannels = channelGains.getMutedChannels();
  ASSERT_EQ(mutedChannels.size(), 0);  // validate that no channels are muted

  // validate that the gains have been restored to the test values
  for (int i = 0; i < channelGains.getGains().size(); i++) {
    ASSERT_EQ(channelGains.getGains()[i], test_gains[i]);
  }
}