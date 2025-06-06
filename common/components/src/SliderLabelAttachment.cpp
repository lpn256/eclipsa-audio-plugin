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

#include "SliderLabelAttachment.h"

SliderLabelAttachment::SliderLabelAttachment(
    juce::AudioProcessorValueTreeState& state,
    const juce::AudioProcessorParameterWithID* parameter)

    : attachment_(initializeAttachment(state, parameter)) {}

juce::AudioProcessorValueTreeState::SliderAttachment
SliderLabelAttachment::initializeAttachment(
    juce::AudioProcessorValueTreeState& state,
    const juce::AudioProcessorParameterWithID* parameter) {
  initializeSliderAndLabel(parameter);
  return juce::AudioProcessorValueTreeState::SliderAttachment(
      state, parameter->getParameterID(), slider_);
}

void SliderLabelAttachment::resized() {
  auto area = getLocalBounds();
  area.removeFromLeft(labelWidth_);
  slider_.setBounds(area);
}

void SliderLabelAttachment::setSliderStyle(juce::Slider::SliderStyle newStyle) {
  slider_.setSliderStyle(newStyle);
}

void SliderLabelAttachment::initializeSliderAndLabel(
    const juce::AudioProcessorParameterWithID* parameter) {
  addAndMakeVisible(slider_);
  slider_.setTextValueSuffix(parameter->getLabel());
  addAndMakeVisible(label_);
  label_.setText(parameter->getName(100), juce::sendNotification);
  label_.attachToComponent(&slider_, true);
}