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
#include <juce_gui_basics/juce_gui_basics.h>

#include <array>

class LoudnessScale : public juce::Component {
 public:
  LoudnessScale();

  void paint(juce::Graphics& g) override;

  std::array<int, 10> getLoudnessLevels() const { return kLoudnessLevels_; }

 private:
  const int kMaxLabelStrLen_ = 3;
  const std::array<int, 10> kLoudnessLevels_ = {
      {-60, -50, -40, -30, -25, -20, -15, -10, -5, 0}};

  juce::OwnedArray<juce::Label> loudnessLevelLabels_;
};