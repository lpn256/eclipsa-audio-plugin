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
#include <processors/gain/MSProcessor.h>

class TestPlaybackMSRepo : public MSPlaybackRepository {
 public:
  TestPlaybackMSRepo()
      : MSPlaybackRepository(juce::ValueTree{PlaybackMS::kTreeType}) {}
};

static void populateInput(juce::AudioBuffer<float>& buff) {
  for (int i = 0; i < buff.getNumChannels(); ++i) {
    for (int j = 0; j < buff.getNumSamples(); ++j) {
      buff.setSample(i, j, 1.0f);
    }
  }
}

class test_ms_processor : public testing::Test {
 protected:
  test_ms_processor() {
    muteSoloState.reset();
    buff = juce::AudioBuffer<float>(6, 1);
    populateInput(buff);
  }

  juce::AudioBuffer<float> buff;
  juce::MidiBuffer dummy;
  TestPlaybackMSRepo muteSoloStateRepo;
  PlaybackMS muteSoloState;
};

TEST_F(test_ms_processor, mute) {
  std::unique_ptr<MSProcessor> proc =
      std::make_unique<MSProcessor>(muteSoloStateRepo);
  // Mute channels 0, 4.
  muteSoloState = muteSoloStateRepo.get();
  muteSoloState.toggleMute(0);
  muteSoloState.toggleMute(4);
  muteSoloStateRepo.update(muteSoloState);

  // Expect non-zero before muting.
  EXPECT_EQ(buff.getSample(0, 0), 1.0f);
  EXPECT_EQ(buff.getSample(4, 0), 1.0f);

  proc->processBlock(buff, dummy);

  // Expect Ch.0 and Ch.4 muted.
  EXPECT_EQ(buff.getSample(0, 0), 0.0f);
  EXPECT_EQ(buff.getSample(4, 0), 0.0f);
}

TEST_F(test_ms_processor, toggle_mute) {
  std::unique_ptr<MSProcessor> proc =
      std::make_unique<MSProcessor>(muteSoloStateRepo);
  // Mute channels 0, 4.
  muteSoloState = muteSoloStateRepo.get();
  muteSoloState.toggleMute(0);
  muteSoloState.toggleMute(4);

  // Toggle mutes on channels 0, 4.
  muteSoloState.toggleMute(0);
  EXPECT_FALSE(muteSoloState.getMutedChannels()[0]);
  muteSoloState.toggleMute(4);
  EXPECT_FALSE(muteSoloState.getMutedChannels()[4]);
}

TEST_F(test_ms_processor, solo) {
  std::unique_ptr<MSProcessor> proc =
      std::make_unique<MSProcessor>(muteSoloStateRepo);
  // Solo channels 1, 2.
  muteSoloState = muteSoloStateRepo.get();
  muteSoloState.toggleSolo(1);
  muteSoloState.toggleSolo(2);
  muteSoloStateRepo.update(muteSoloState);

  // Expect all channels 1's before solo-ing.
  for (int i = 0; i < buff.getNumChannels(); ++i) {
    EXPECT_EQ(buff.getSample(i, 0), 1.0f);
  }

  proc->processBlock(buff, dummy);

  // Expect all channels except 1, 2 to be zeroed.
  for (int i = 0; i < buff.getNumChannels(); ++i) {
    if (i == 1 || i == 2) {
      EXPECT_EQ(buff.getSample(i, 0), 1.0f);
    } else {
      EXPECT_EQ(buff.getSample(i, 0), 0.0f);
    }
  }
}

TEST_F(test_ms_processor, mute_solo1) {
  std::unique_ptr<MSProcessor> proc =
      std::make_unique<MSProcessor>(muteSoloStateRepo);
  // Mute all channels.
  muteSoloState = muteSoloStateRepo.get();
  muteSoloState.setMutedChannels(muteSoloState.getMutedChannels().set());
  // Solo first channel.
  muteSoloState.toggleSolo(1);
  muteSoloStateRepo.update(muteSoloState);

  // Expect all channels 1's before solo-ing.
  for (int i = 0; i < buff.getNumChannels(); ++i) {
    EXPECT_EQ(buff.getSample(i, 0), 1.0f);
  }

  proc->processBlock(buff, dummy);

  // Expect all channels muted.
  for (int i = 0; i < buff.getNumChannels(); ++i) {
    EXPECT_EQ(buff.getSample(i, 0), 0.0f);
  }
}

TEST_F(test_ms_processor, mute_solo2) {
  std::unique_ptr<MSProcessor> proc =
      std::make_unique<MSProcessor>(muteSoloStateRepo);
  // Mute channels 0, 4.
  muteSoloState = muteSoloStateRepo.get();
  muteSoloState.toggleMute(0);
  muteSoloState.toggleMute(4);
  // Solo channel 1.
  muteSoloState.toggleSolo(1);
  muteSoloStateRepo.update(muteSoloState);

  // Expect all channels 1's before solo-ing.
  for (int i = 0; i < buff.getNumChannels(); ++i) {
    EXPECT_EQ(buff.getSample(i, 0), 1.0f);
  }

  proc->processBlock(buff, dummy);

  // Expect all channels muted except channel 1.
  for (int i = 0; i < buff.getNumChannels(); ++i) {
    if (i == 1) {
      EXPECT_EQ(buff.getSample(i, 0), 1.0f);
    } else {
      EXPECT_EQ(buff.getSample(i, 0), 0.0f);
    }
  }
}