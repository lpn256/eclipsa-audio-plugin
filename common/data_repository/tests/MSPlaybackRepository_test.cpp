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

#include "data_repository/implementation/MSPlaybackRepository.h"

#include <gtest/gtest.h>

class TestMSPlaybackRepository : public MSPlaybackRepository {
 public:
  TestMSPlaybackRepository() : MSPlaybackRepository(juce::ValueTree{"test"}) {}
};

TEST(test_msplayback_repository, empty) {
  TestMSPlaybackRepository repositoryInstance;
  PlaybackMS muteSoloState;
  PlaybackMS defaultMuteSoloState = repositoryInstance.get();
  EXPECT_EQ(muteSoloState.getMutedChannels(),
            defaultMuteSoloState.getMutedChannels());
}

TEST(test_msplayback_repository, update) {
  TestMSPlaybackRepository repositoryInstance;
  PlaybackMS defaultMuteSoloState = repositoryInstance.get();
  defaultMuteSoloState.toggleMute(0);
  repositoryInstance.update(defaultMuteSoloState);
  PlaybackMS muteSoloState = repositoryInstance.get();
  EXPECT_EQ(muteSoloState.getMutedChannels(),
            defaultMuteSoloState.getMutedChannels());
}

TEST(test_msplayback_repository, to_from_tree) {
  TestMSPlaybackRepository repositoryInstance;
  PlaybackMS defaultMuteSoloState = repositoryInstance.get();
  defaultMuteSoloState.toggleMute(0);
  defaultMuteSoloState.toggleSolo(1);

  auto tree = defaultMuteSoloState.toValueTree();
  PlaybackMS muteSoloState = PlaybackMS::fromTree(tree);

  EXPECT_EQ(muteSoloState.getMutedChannels(),
            defaultMuteSoloState.getMutedChannels());
  EXPECT_EQ(muteSoloState.getSoloedChannels(),
            defaultMuteSoloState.getSoloedChannels());
}