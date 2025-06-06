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

#include "RoomSetupScreen.h"

#include <data_structures/data_structures.h>

#include "BinaryData.h"
#include "data_structures/src/RoomSetup.h"
#include "processors/processor_base/ProcessorBase.h"

RoomSetupScreen::RoomSetupScreen(RoomSetupRepository& repository,
                                 ProcessorBase* fiProc)
    : juce::Component("Room Setup"),
      roomSetupData_(repository),
      fileOutputProcessor_(fiProc),
      isRendering_(false),
      roomVisImage_("RoomSetupScreenVisualizationImage"),
      speakerLayoutLabel_("RoomSetupScreenSpeakerLayoutLabel"),
      speakerLayoutOptions_("RoomSetupScreenSpeakerLayoutOptions") {
  const juce::Image roomVisImage =
      juce::ImageFileFormat::loadFrom(BinaryData::room_vis_placeholder_png,
                                      BinaryData::room_vis_placeholder_pngSize);
  roomVisImage_.setImage(roomVisImage);
  speakerLayoutLabel_.setText("Select your speaker layout",
                              juce::dontSendNotification);
  speakerLayoutLabel_.setJustificationType(juce::Justification::centred);

  initializeComboBox();

  speakerLayoutOptions_.onChange = [this]() {
    const size_t optionIndex =
        static_cast<size_t>(speakerLayoutOptions_.getSelectedItemIndex());
    RoomLayout item = speakerLayoutConfigurationOptions[optionIndex];
    this->roomSetupData_.update(item);
  };

  addAndMakeVisible(roomVisImage_);
  addAndMakeVisible(speakerLayoutLabel_);
  addAndMakeVisible(speakerLayoutOptions_);
  addAndMakeVisible(startStopBounce_);
  startStopBounce_.setButtonText("Start/Stop Bounce");

  // Simulate bouncing for now
  startStopBounce_.onClick = [this]() {
    juce::Logger::writeToLog("Start/Stop Bounce button clicked");
    if (!isRendering_) {
      fileOutputProcessor_->setNonRealtime(true);
      isRendering_ = true;
    } else {
      fileOutputProcessor_->setNonRealtime(false);
      isRendering_ = false;
    }
  };

  repository.registerListener(this);

  setSize(2, 2);
}

void RoomSetupScreen::initializeComboBox() {
  for (size_t i = 0; i < speakerLayoutConfigurationOptions.size(); ++i) {
    int id = static_cast<int>(i) + 1;
    const auto& channelSet = speakerLayoutConfigurationOptions[i];
    speakerLayoutOptions_.addItem(channelSet.getDescription(), id);
    if (channelSet == roomSetupData_.get().getSpeakerLayout()) {
      speakerLayoutOptions_.setSelectedId(id);
    }
  }
}

void RoomSetupScreen::resized() {
  auto bounds = getLocalBounds();
  bounds = bounds.reduced(margin_);
  const int halfWidth = bounds.getWidth() / 2;
  const juce::Image& img = roomVisImage_.getImage();
  roomVisImage_.setSize(img.getWidth(), img.getHeight());
  roomVisImage_.setBoundsToFit(bounds.removeFromLeft(halfWidth),
                               juce::Justification::centred, true);
  const int contentsHeight = margin_ + layoutDropdownHeight_ + labelHeight_;
  const int halfWhitespace = (bounds.getHeight() - contentsHeight) / 2;
  speakerLayoutLabel_.setBounds(bounds.removeFromTop(labelHeight_));
  bounds.removeFromTop(margin_);
  speakerLayoutOptions_.setBounds(bounds.removeFromTop(layoutDropdownHeight_));
  startStopBounce_.setBounds(bounds.removeFromBottom(20));
}

void RoomSetupScreen::paint(juce::Graphics& g) {
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void RoomSetupScreen::valueTreeRedirected(
    juce::ValueTree& treeWhichHasBeenChanged) {
  juce::ignoreUnused(treeWhichHasBeenChanged);
  initializeComboBox();
}