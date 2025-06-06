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

#include "LoudnessScale.h"

#include "../EclipsaColours.h"

LoudnessScale::LoudnessScale() {
  for (const int level : kLoudnessLevels_) {
    loudnessLevelLabels_.add(new juce::Label("", juce::String(level)));
    loudnessLevelLabels_.getLast()->setJustificationType(
        juce::Justification::topRight);
    loudnessLevelLabels_.getLast()->setMinimumHorizontalScale(0.2f);
    loudnessLevelLabels_.getLast()->setColour(juce::Label::textColourId,
                                              EclipsaColours::tabTextGrey);
    addAndMakeVisible(loudnessLevelLabels_.getLast());
  }
}

void LoudnessScale::paint(juce::Graphics& g) {
  auto bounds = getLocalBounds();
  int labelHeight = bounds.getHeight() / loudnessLevelLabels_.size();
  for (juce::Label* label : loudnessLevelLabels_) {
    label->setBounds(bounds.removeFromBottom(labelHeight));
  }
}