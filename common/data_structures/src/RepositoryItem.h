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

#include <juce_data_structures/juce_data_structures.h>

#include <type_traits>

class RepositoryItemBase {
 public:
  virtual juce::ValueTree toValueTree() const = 0;
  juce::Uuid getId() const { return id_; }

  inline static const juce::Identifier kId{"id"};

 protected:
  RepositoryItemBase(juce::Uuid id) : id_(id) {}
  juce::Uuid id_;
};

template <typename T>
concept RepositoryItem = requires(T item) {
  // static T T::fromTree(juce::ValueTree)
  { T::fromTree(juce::ValueTree()) } -> std::same_as<T>;

  // juce::ValueTree T::toValueTree() const
  { std::as_const(item).toValueTree() } -> std::same_as<juce::ValueTree>;

  // static const juce::Identifier kTreeType
  { T::kTreeType } -> std::same_as<const juce::Identifier&>;

  std::is_default_constructible<T>();
  std::is_base_of<RepositoryItemBase, T>();
};