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

#include "data_repository/implementation/ActiveMixPresentationRepository.h"

#include <data_structures/data_structures.h>
#include <gtest/gtest.h>

class TestActiveMixPresRepo : public ActiveMixRepository {
 public:
  TestActiveMixPresRepo() : ActiveMixRepository(juce::ValueTree{"test"}) {}
};

TEST(test_active_mp_repo, get_and_set) {
  TestActiveMixPresRepo activeMixRepo;
  const juce::Uuid activeMixId = juce::Uuid();

  // Update the active mix presentation. Pull it back down and confirm it's
  // still the same.
  ActiveMixPresentation activeMix = activeMixRepo.get();
  activeMix.updateActiveMixId(activeMixId);
  activeMixRepo.update(activeMix);
  activeMix = activeMixRepo.get();
  EXPECT_EQ(activeMix.getActiveMixId(), activeMixId);
}
