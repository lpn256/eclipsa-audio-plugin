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

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class SliderLabelAttachment final : public juce::Component {
 public:
  SliderLabelAttachment(juce::AudioProcessorValueTreeState& state,
                        const juce::AudioProcessorParameterWithID* parameter);
  ~SliderLabelAttachment() override = default;

  void resized() override;
  void setSliderStyle(juce::Slider::SliderStyle newStyle);

 private:
  juce::AudioProcessorValueTreeState::SliderAttachment initializeAttachment(
      juce::AudioProcessorValueTreeState& state,
      const juce::AudioProcessorParameterWithID* parameter);

  void initializeSliderAndLabel(
      const juce::AudioProcessorParameterWithID* parameter);

  juce::Slider slider_;
  juce::Label label_;
  juce::AudioProcessorValueTreeState::SliderAttachment attachment_;

  const int labelWidth_ = 135;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SliderLabelAttachment)
};