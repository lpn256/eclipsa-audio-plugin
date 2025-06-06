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
class RepositoryMultiBase : public RepositoryBase<T> {
 public:
  RepositoryMultiBase() : RepositoryBase<T>() {}
  RepositoryMultiBase(juce::ValueTree state) : RepositoryBase<T>(state) {}

  T getOrAdd(juce::Uuid id) {
    juce::ValueTree found = getChildWithId(id);
    if (!found.isValid()) {
      T newItem(id);
      jassert(add(newItem));
      return newItem;
    }

    return T::fromTree(found);
  }

  T updateOrAdd(const T& item) {
    if (update(item)) {
      return item;
    }
    add(item);
    return item;
  }

  std::optional<T> get(juce::Uuid id) const {
    juce::ValueTree found = getChildWithId(id);
    if (!found.isValid()) {
      return std::nullopt;
    }

    return T::fromTree(found);
  }

  std::optional<T> getFirst() const {
    if (getItemCount() == 0) {
      return std::nullopt;
    }

    return T::fromTree(this->state_.getChild(0));
  }

  bool add(const T& item) {
    // Don't add if there's already an element with the given ID
    if (getChildWithId(item.getId()).isValid()) {
      return false;
    };

    this->state_.appendChild(item.toValueTree(), nullptr);
    return true;
  }

  void getAll(juce::OwnedArray<T>& array) const {
    array.clear();
    array.ensureStorageAllocated(getItemCount());
    for (auto item : this->state_) {
      array.add(std::make_unique<T>(T::fromTree(item)));
    }
  }

  bool update(const T& item) {
    juce::ValueTree existingItem = getChildWithId(item.getId());
    if (!existingItem.isValid()) {
      return false;
    }

    existingItem.copyPropertiesAndChildrenFrom(item.toValueTree(), nullptr);
    return true;
  }

  bool remove(const T& item) {
    juce::ValueTree existingItem = getChildWithId(item.getId());
    if (!existingItem.isValid()) {
      return false;
    }

    this->state_.removeChild(existingItem, nullptr);
    return true;
  }

  int getItemCount() const { return this->state_.getNumChildren(); }

  bool clear() {
    this->state_.removeAllChildren(nullptr);
    return true;
  }

  juce::ValueTree getValueTree() const { return this->state_; }

 private:
  juce::ValueTree getChildWithId(juce::Uuid id) const {
    jassert(this->state_.isValid());
    return this->state_.getChildWithProperty(T::kId, id.toString());
  }
};
