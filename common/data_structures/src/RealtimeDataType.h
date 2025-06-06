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
#include <juce_core/juce_core.h>

template <typename T>
class RealtimeDataType {
 public:
  // Read data from queue into dest. Returns false if the queue was empty.
  bool read(T& dest) const {
    lock.enterRead();
    dest = data;
    lock.exitRead();
    return true;
  }

  // Update the queue with the most recent data.
  void update(T const& val) {
    lock.enterWrite();
    data = val;
    lock.exitWrite();
  }

 private:
  T data{};
  juce::ReadWriteLock lock;
};