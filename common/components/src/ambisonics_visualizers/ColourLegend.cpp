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

#include "ColourLegend.h"

#include <array>
#include <cmath>
#include <vector>

ColourLegend::ColourLegend() : ksegmentColours_(calculateLegendColours()) {
  addAndMakeVisible(loudnessScale_);
}

void ColourLegend::paint(juce::Graphics& g) {
  auto bounds = getLocalBounds();

  // trim the bottom of the bounds to ensure they
  // are an integer multiple of the number of colours
  // initial bounds value is 295, so reduce to 240
  bounds.removeFromTop(27);
  bounds.removeFromBottom(28);

  const auto boundsRef = bounds;

  // draw the loudness scale on the left side of the colour legend
  auto loudnessScaleBounds =
      bounds.removeFromLeft(boundsRef.proportionOfWidth(0.45f));
  loudnessScaleBounds.translate(0, 4);
  loudnessScale_.setBounds(loudnessScaleBounds);

  bounds.removeFromLeft(boundsRef.proportionOfWidth(.1f));

  paintColourLegend(g, bounds);
}

void ColourLegend::paintColourLegend(juce::Graphics& g,
                                     juce::Rectangle<int>& bounds) {
  bounds.removeFromRight(bounds.proportionOfWidth(0.33f));
  // hold a reference for the bounds
  const juce::Rectangle<int> boundsRef = bounds;

  for (int i = ksegmentColours_.size() - 1; i >= 0; i--) {
    g.setColour(ksegmentColours_[i]);
    g.fillRect(bounds.removeFromTop(kLinesPerColour_));
  }
}

juce::Colour ColourLegend::assignColour(const float& decibel) {
  juce::Colour leftColour;
  juce::Colour rightColour;
  float leftLoudness;
  float rightLoudness;
  // determine what colours you want to interpolate between
  if (decibel < kColourTransitions[0]) {
    return EclipsaColours::inactiveGrey;
  } else if (decibel < kColourTransitions[1]) {
    leftColour = EclipsaColours::inactiveGrey;
    rightColour = EclipsaColours::controlBlue;
    leftLoudness = kColourTransitions[0];
    rightLoudness = kColourTransitions[1];
  } else if (decibel < kColourTransitions[2]) {
    leftColour = EclipsaColours::controlBlue;
    rightColour = EclipsaColours::green;
    leftLoudness = kColourTransitions[1];
    rightLoudness = kColourTransitions[2];
  } else if (decibel < kColourTransitions[3]) {
    leftColour = EclipsaColours::green;
    rightColour = EclipsaColours::yellow;
    leftLoudness = kColourTransitions[2];
    rightLoudness = kColourTransitions[3];
  } else if (decibel < kColourTransitions[4]) {
    leftColour = EclipsaColours::yellow;
    rightColour = EclipsaColours::orange;
    leftLoudness = kColourTransitions[3];
    rightLoudness = kColourTransitions[4];
  } else if (decibel < kColourTransitions[5]) {
    leftColour = EclipsaColours::orange;
    rightColour = EclipsaColours::red;
    leftLoudness = kColourTransitions[4];
    rightLoudness = kColourTransitions[5];
  } else {
    return EclipsaColours::red;
  }
  const int blue = interpolateColourChannel(decibel, leftColour.getBlue(),
                                            rightColour.getBlue(), leftLoudness,
                                            rightLoudness);
  const int green = interpolateColourChannel(decibel, leftColour.getGreen(),
                                             rightColour.getGreen(),
                                             leftLoudness, rightLoudness);
  const int red = interpolateColourChannel(decibel, leftColour.getRed(),
                                           rightColour.getRed(), leftLoudness,
                                           rightLoudness);
  return juce::Colour(red, green, blue);
}

int ColourLegend::interpolateColourChannel(const float& loudness,
                                           const int& color1, const int& color2,
                                           const float& loudness1,
                                           const float& loudness2) {
  // calculate the slope
  const float m = (color2 - color1) / (loudness2 - loudness1);
  // calculate the new colour channel value
  return color1 + m * (loudness - loudness1);
}

std::vector<juce::Colour> ColourLegend::calculateLegendColours() {
  std::vector<juce::Colour> segmentColours;
  // the first two thirds of the colour legend are divided into 5dB increments
  // 0 -> -5dB, -5dB -> -10dB, -10dB -> -20dB, -20dB -> -25dB, -25dB -> -30dB
  const std::array<int, 10> loudnessLevels = loudnessScale_.getLoudnessLevels();

  const int numTickIntervals = loudnessLevels.size() - 1;
  const int numLinesPerTickInterval = kApproxHeight_ / numTickIntervals;
  const int numColoursPerTickInterval =
      numLinesPerTickInterval / kLinesPerColour_;

  for (int i = 1; i < loudnessLevels.size(); i++) {
    const float start = loudnessLevels[i - 1];
    const float end = loudnessLevels[i];
    const float increment = (end - start) / numColoursPerTickInterval;
    const std::vector<float> dBRange = makeRange(
        start, end, increment);  // make a range of floats from start to end>
    for (int j = 0; j < dBRange.size(); j++) {
      segmentColours.emplace_back(assignColour(dBRange[j]));
    }
  }
  return segmentColours;
}

std::vector<float> ColourLegend::makeRange(
    const float& start, const float& end,
    const float& increment) {  // make a range of floats from start to end
  std::vector<float> range;

  if (increment == 0.f || (increment < 0.f && start < end) ||
      (increment > 0.f && start > end)) {
    return range;  // Return an empty vector
  }

  if (increment < 0.f) {
    for (float i = start; i > end; i += increment) {
      range.push_back(i);
    }
  } else {
    for (float i = start; i < end; i += increment) {
      range.push_back(i);
    }
  }

  return range;
}