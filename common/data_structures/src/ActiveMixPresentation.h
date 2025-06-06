/*
 * Copyright 2025 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include "data_structures/src/RepositoryItem.h"

class ActiveMixPresentation final : public RepositoryItemBase {
 public:
  ActiveMixPresentation();
  ActiveMixPresentation(const juce::Uuid newActiveMixId);

  void updateActiveMixId(const juce::Uuid newId) { activeMixId_ = newId; }
  juce::Uuid getActiveMixId(void) { return activeMixId_; }

  static ActiveMixPresentation fromTree(const juce::ValueTree tree);
  virtual juce::ValueTree toValueTree() const override;

  inline static const juce::Identifier kTreeType{"active_mix"};
  inline static const juce::Identifier kActiveMixID{"active_mix_id"};

 private:
  juce::Uuid activeMixId_ = juce::Uuid::null();
};