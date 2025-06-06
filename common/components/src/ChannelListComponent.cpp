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

#include "ChannelListComponent.h"

ChannelListComponent::ChannelListComponent() : listBox_("Channel List", this) {
  addAndMakeVisible(listBox_);
}

void ChannelListComponent::paintListBoxItem(int rowNumber, juce::Graphics& g,
                                            int width, int height,
                                            bool rowIsSelected) {
  juce::ignoreUnused(rowIsSelected);
  if (rowNumber < channelList_.size()) {
    g.setColour(juce::LookAndFeel::getDefaultLookAndFeel().findColour(
        juce::Label::textColourId));
    g.setFont((float)height * 0.7f);

    g.drawText(channelList_[rowNumber], 5, 0, width, height,
               juce::Justification::centredLeft, true);
  }
}

int ChannelListComponent::getNumRows() { return channelList_.size(); }

void ChannelListComponent::resized() { listBox_.setBounds(getLocalBounds()); }