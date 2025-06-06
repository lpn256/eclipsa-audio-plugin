/*
 * Copyright 2025 Google LLC
 *
 * Licensed under the Apache License,
 * Version 2.0 (the "License");
 * you may not use this file except in
 * compliance with the License.
 * You may obtain a copy of the License at
 *
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by
 * applicable law or agreed to in writing, software
 * distributed under the
 * License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the
 * specific language governing permissions and
 * limitations under the
 * License.
 */

#pragma once

#include <components/components.h>

class GainProcessor;

//==============================================================================
class GainProcessorEditor final : public juce::AudioProcessorEditor {
 public:
  explicit GainProcessorEditor(GainProcessor& p,
                               juce::AudioProcessorValueTreeState& state);
  ~GainProcessorEditor() override;

  //==============================================================================
  void paint(juce::Graphics&) override;
  void resized() override;

 private:
  const int editorMargin_ = 15;
  const int sliderHeight_ = 150;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainProcessorEditor)
};
