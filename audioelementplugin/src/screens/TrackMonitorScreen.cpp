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

#include "TrackMonitorScreen.h"

TrackMonitorScreen::TrackMonitorScreen(
    AudioElementPluginSyncClient* syncClient,
    AudioElementPluginRepositoryCollection audioElementPluginRepo)
    : audioElementSpatialLayoutRepository_(
          &audioElementPluginRepo.audioElementSpatialLayoutRepository_),
      stats_(audioElementPluginRepo.monitorData_),
      rtData_(audioElementPluginRepo.monitorData_),
      viewPort_(syncClient, audioElementPluginRepo, pbLayout_) {
  audioElementSpatialLayoutRepository_->registerListener(this);

  initializeUI();
}

TrackMonitorScreen::~TrackMonitorScreen() {
  setLookAndFeel(nullptr);
  audioElementSpatialLayoutRepository_->deregisterListener(this);
}

void TrackMonitorScreen::valueTreePropertyChanged(
    juce::ValueTree& treeWhosePropertyHasChanged,
    const juce::Identifier& property) {
  if (property != AudioElementSpatialLayout::kLayout) {
    return;
  }

  pbLayout_ = audioElementSpatialLayoutRepository_->get().getChannelLayout();

  if (!pbLayout_.isAmbisonics()) {
    LOG_ANALYTICS(
        AudioElementPluginProcessor::instanceId_,
        "TrackMonitorScreen Listener::Non-Ambisonics layout selected.");
    changeComponentVisibility(true);
    viewPort_.switchedtoLoudnessMeters();
  } else {
    LOG_ANALYTICS(AudioElementPluginProcessor::instanceId_,
                  "TrackMonitorScreen Listener::Ambisonics layout selected.");
    changeComponentVisibility(false);
    // draw ambisonics visualizer
    viewPort_.switchedToAmbisonics();
  }
  viewPort_.setVisible(true);
  repaint();
}

void TrackMonitorScreen::paint(juce::Graphics& g) {
  if (!audioElementSpatialLayoutRepository_->get().isLayoutSelected()) {
    return;
  }
  viewPort_.setVisible(true);
  if (!pbLayout_.isAmbisonics()) {
    auto statsBounds = getLocalBounds();
    const auto boundsRef = statsBounds;

    auto leftScaleBounds =
        statsBounds.removeFromLeft(boundsRef.getWidth() * 0.05f)
            .removeFromTop(boundsRef.getHeight() * 0.66);
    leftScale_.setBounds(leftScaleBounds);

    //  3/4 of horizontal space allocated to metering bars.
    statsBounds.removeFromLeft(4);
    auto viewPortBounds =
        statsBounds.removeFromLeft(statsBounds.getWidth() * 0.75f);

    changeComponentVisibility(true);

    statsBounds.removeFromLeft(4);
    stats_.setBounds(statsBounds);

    drawRightScaleandHeadphoneMeters(viewPortBounds);
    viewPort_.setBounds(viewPortBounds);
  } else {
    changeComponentVisibility(false);

    viewPort_.setBounds(getLocalBounds());
  }
}

void TrackMonitorScreen::initializeUI() {
  resetButton_.setImages(false, true, true, kResetImg_, 1.0f,
                         juce::Colours::grey, kResetImg_, 1.0f,
                         juce::Colours::lightgrey, kResetImg_, 1.0f,
                         juce::Colours::whitesmoke);
  resetButton_.onClick = [this] { viewPort_.resetSoloMutes(); };
  addAndMakeVisible(resetButton_);
  addAndMakeVisible(rightScale_);
  addAndMakeVisible(leftScale_);
  addAndMakeVisible(hmeter_);
  addAndMakeVisible(stats_);

  changeComponentVisibility(false);

  addAndMakeVisible(viewPort_);
  viewPort_.setVisible(false);
}

// this functions accepts a reference to initial viewPort bounds
// and then calculates and assigns the bounds for the
// scale, headphone meters, and reset button
void TrackMonitorScreen::drawRightScaleandHeadphoneMeters(
    juce::Rectangle<int>& viewPortBounds) {
  // Configure meter loudness scale bounds and draw loudness meter scale.
  const auto meterAndResetBoundsRef = viewPortBounds.removeFromRight(
      3 * viewPort_.getMeterWidth() + 2 * viewPort_.getMeterOffset());
  auto meterAndResetBounds = meterAndResetBoundsRef;

  meterAndResetBounds.removeFromRight(
      meterAndResetBounds.proportionOfWidth(0.66f));
  auto meterScaleBounds =
      meterAndResetBounds.removeFromTop(meterAndResetBounds.getHeight() * 0.66);
  auto rstButtonBounds = meterAndResetBounds.removeFromBottom(
      meterScaleBounds.getHeight() * (0.4f));

  rightScale_.setBounds(meterScaleBounds);

  auto tempBounds = meterAndResetBoundsRef;
  tempBounds.removeFromRight(tempBounds.proportionOfWidth(0.33f));
  tempBounds.removeFromTop(tempBounds.proportionOfHeight(0.66f));
  // Draw global S/M reset button.
  resetButton_.setBounds(rstButtonBounds);

  // Draw headphones loudness bars and image.
  auto bounds = meterAndResetBoundsRef;
  auto headphoneLoudnessBounds = bounds.removeFromRight(
      2 * viewPort_.getMeterWidth() + viewPort_.getMeterOffset());
  hmeter_.setBarWidth(viewPort_.getMeterWidth());
  hmeter_.setBounds(headphoneLoudnessBounds);

  std::array<float, 2> binauralLoudnesses;
  rtData_.binauralLoudness.read(binauralLoudnesses);
  hmeter_.setLoudness(binauralLoudnesses[0], binauralLoudnesses[1]);
}

void TrackMonitorScreen::changeComponentVisibility(const bool& visibility) {
  resetButton_.setVisible(visibility);
  rightScale_.setVisible(visibility);
  leftScale_.setVisible(visibility);
  hmeter_.setVisible(visibility);
  stats_.setVisible(visibility);
}
