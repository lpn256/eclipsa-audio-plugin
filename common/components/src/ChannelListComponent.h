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

class RoutingScreen;

class ChannelListComponent final : public juce::ListBoxModel,
                                   public juce::Component {
 public:
  friend RoutingScreen;
  ChannelListComponent();
  void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height,
                        bool rowIsSelected) override;
  int getNumRows() override;
  void resized() override;

 private:
  juce::Array<juce::String> channelList_;
  juce::ListBox listBox_;
};