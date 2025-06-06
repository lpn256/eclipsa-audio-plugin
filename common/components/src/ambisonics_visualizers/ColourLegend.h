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
#include "components/src/EclipsaColours.h"
#include "components/src/loudness_meter/LoudnessScale.h"

class ColourLegend : public juce::Component {
 public:
  ColourLegend();

  ~ColourLegend() override { setLookAndFeel(nullptr); }

  static juce::Colour assignColour(const float& decibels);

  static int interpolateColourChannel(const float& loudness, const int& color1,
                                      const int& color2, const float& loudness1,
                                      const float& loudness2);

  static constexpr std::array<float, 6> kColourTransitions = {
      -60.f, -40.f, -25.f, -15.f, -5.f, 0.f};

 private:
  void paint(juce::Graphics& g) override;

  std::vector<float> makeRange(
      const float& start, const float& end,
      const float& increment);  // make a range of floats from start to end
  std::vector<juce::Colour> calculateLegendColours();

  void paintColourLegend(juce::Graphics& g, juce::Rectangle<int>& bounds);

  LoudnessScale loudnessScale_;

  // lines per TickInterval is 26
  const int kApproxHeight_ = 240;
  const int kLinesPerColour_ = 2;

  const float k5dBIncrementPortion_ =
      0.66666f;  // the portion of the colour legend that
                 // represents a 5dB increment

  // memoize the colours
  const std::vector<juce::Colour> ksegmentColours_;
};
