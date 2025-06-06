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

#include "RepositoryBase.h"

template <RepositoryItem T>
class RepositorySingleBase : public RepositoryBase<T> {
 public:
  RepositorySingleBase() : RepositoryBase<T>() {}

  RepositorySingleBase(juce::ValueTree state) : RepositoryBase<T>(state) {
    if (this->state_.getNumProperties() == 0) {
      this->state_.copyPropertiesFrom(T().toValueTree(), nullptr);
    }
  }

  T get() const { return T::fromTree(this->state_); }

  juce::ValueTree getTree() { return this->state_; }

  void update(const T& item) {
    this->state_.copyPropertiesFrom(item.toValueTree(), nullptr);
  }
};