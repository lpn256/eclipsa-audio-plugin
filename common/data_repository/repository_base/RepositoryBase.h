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

#include <data_structures/data_structures.h>
#include <juce_data_structures/juce_data_structures.h>

template <RepositoryItem T>
class RepositoryBase {
 public:
  void setStateTree(juce::ValueTree state) { state_ = state; }
  void writeToStream(juce::MemoryOutputStream& stream) const {
    state_.writeToStream(stream);
  }
  void registerListener(juce::ValueTree::Listener* listener) {
    state_.addListener(listener);
  }
  void deregisterListener(juce::ValueTree::Listener* listener) {
    state_.removeListener(listener);
  }

 protected:
  RepositoryBase() = default;
  RepositoryBase(juce::ValueTree state) : state_(state) {}
  juce::ValueTree state_;
};