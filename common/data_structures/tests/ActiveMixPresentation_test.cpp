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

#include <data_structures/data_structures.h>
#include <gtest/gtest.h>

TEST(test_active_mp, to_from_tree) {
  ActiveMixPresentation activeMix;
  const juce::Uuid activeMixId = juce::Uuid();
  activeMix.updateActiveMixId(activeMixId);

  juce::ValueTree tree = activeMix.toValueTree();
  ActiveMixPresentation fromTreeActiveMix =
      ActiveMixPresentation::fromTree(tree);
  EXPECT_EQ(fromTreeActiveMix.getActiveMixId(), activeMix.getActiveMixId());
}