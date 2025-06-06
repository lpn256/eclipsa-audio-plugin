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

#include "../implementation/RoomSetupRepository.h"

#include <data_structures/data_structures.h>
#include <gtest/gtest.h>

#include "data_structures/src/RoomSetup.h"
#include "substream_rdr/substream_rdr_utils/Speakers.h"

class TestRoomSetupRepository : public RoomSetupRepository {
 public:
  TestRoomSetupRepository() : RoomSetupRepository(juce::ValueTree{"test"}) {}
};

// Just a sanity check -- the test_base_repository suite should cover all
// meaningful repository logic

TEST(test_room_setup_repository, update) {
  TestRoomSetupRepository repositoryInstance;
  RoomLayout layout = RoomLayout(Speakers::k5Point1, "5.1");
  RoomSetup testSetup(layout);
  RoomSetup defaultSetup = repositoryInstance.get();
  // Once room setup is more than just one member, it should define some
  // meaningful == operator that we can use to compare here.
  ASSERT_NE(testSetup.getSpeakerLayout(), defaultSetup.getSpeakerLayout());

  repositoryInstance.update(testSetup);
  RoomSetup updatedSetup = repositoryInstance.get();
  ASSERT_EQ(testSetup.getSpeakerLayout(), updatedSetup.getSpeakerLayout());
}
