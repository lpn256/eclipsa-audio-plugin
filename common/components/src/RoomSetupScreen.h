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

#include <data_repository/data_repository.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "processors/processor_base/ProcessorBase.h"

class RoomSetupScreen final : public juce::Component,
                              public juce::ValueTree::Listener {
 public:
  RoomSetupScreen(RoomSetupRepository& repository, ProcessorBase* fiProc);

  void initializeComboBox();

  void resized() override;
  void paint(juce::Graphics& g) override;

  virtual void valueTreeRedirected(
      juce::ValueTree& treeWhichHasBeenChanged) override;

 private:
  RoomSetupRepository& roomSetupData_;
  ProcessorBase* fileOutputProcessor_;
  bool isRendering_;

  juce::ImageComponent roomVisImage_;
  juce::Label speakerLayoutLabel_;
  juce::ComboBox speakerLayoutOptions_;
  juce::TextButton startStopBounce_;

  const int margin_ = 3;
  const int layoutDropdownHeight_ = 20;
  const int layoutDropdownWidth_ = 140;
  const int labelHeight_ = 20;
};