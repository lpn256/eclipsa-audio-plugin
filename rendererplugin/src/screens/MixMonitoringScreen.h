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
#include <components/components.h>
#include <substream_rdr/substream_rdr_utils/Speakers.h>

#include "../RendererProcessor.h"
#include "components/src/loudness_meter/LoudnessScale.h"
#include "data_structures/src/PlaybackMS.h"

class MixMonitoringScreen : public juce::Component, juce::ValueTree::Listener {
 public:
  MixMonitoringScreen(RepositoryCollection repos, SpeakerMonitorData& data)
      : repos_(repos), rtData_(data), stats_(data) {
    // Configure component as a listener to the room repo for playback layout.
    repos_.roomSetupRepo_.registerListener(this);
    repos_.playbackMSRepo_.registerListener(this);

    // Create loudness meters for the current playback layout.
    pbLayout_ =
        repos_.roomSetupRepo_.get().getSpeakerLayout().getRoomSpeakerLayout();
    std::vector<juce::String> chLabels = pbLayout_.getSpeakerLabels();
    createLoudnessMeters(chLabels);

    resetButton_.setImages(false, false, true, kResetImg_, 1.0f,
                           EclipsaColours::tabTextGrey, kResetImg_, 1.0f,
                           EclipsaColours::tabTextGrey, kResetImg_, 1.0f,
                           EclipsaColours::tabTextGrey);
    resetButton_.onClick = [this] { resetSoloMutes(); };
    addAndMakeVisible(resetButton_);

    addAndMakeVisible(rightScale_);
    addAndMakeVisible(leftScale_);
    addAndMakeVisible(hmeter_);
    addAndMakeVisible(stats_);
  }

  ~MixMonitoringScreen() {
    repos_.roomSetupRepo_.deregisterListener(this);
    repos_.playbackMSRepo_.deregisterListener(this);
  }

  void createLoudnessMeters(const std::vector<juce::String>& chLabels) {
    // Delete old meters and construct new ones for the given labels.
    meters_.clear();

    // Construct meters with channel labels.
    for (int i = 0; i < chLabels.size(); ++i) {
      meters_.emplace_back(std::make_unique<LoudnessMeter>(
          chLabels[i], i, repos_.playbackMSRepo_));
      addAndMakeVisible(meters_[i].get());
    }
  }

  void paint(juce::Graphics& g) {
    auto bounds = getLocalBounds();
    const auto boundsRef = bounds;

    auto leftScaleBounds = bounds.removeFromLeft(boundsRef.getWidth() * 0.05f)
                               .removeFromTop(boundsRef.getHeight() * 0.66);
    leftScale_.setBounds(leftScaleBounds);

    //  3/4 of horizontal space allocated to metering bars.
    bounds.removeFromLeft(kMeterOffset);
    auto meterBounds = bounds.removeFromLeft(boundsRef.getWidth() * 0.75f);

    // Draw meters.
    int meterWidth =
        meterBounds.getWidth() /
        (kMaxChannels_ + 3);  // 12Ch. + scale bar + headphone meters.
    std::vector<float> loudnesses;
    rtData_.playbackLoudness.read(loudnesses);
    for (int i = 0; i < meters_.size(); ++i) {
      meterBounds.removeFromLeft(kMeterOffset);
      if (i < loudnesses.size()) {
        meters_[i]->setloudness(loudnesses[i]);
      }
      meters_[i]->setBounds(
          meterBounds.removeFromLeft(meterWidth - kMeterOffset));
    }

    // Draw headphones loudness bars and image.
    auto headphoneLoudnessBounds =
        meterBounds.removeFromRight(2 * meterWidth + kMeterOffset);
    std::array<float, 2> binauralLoudnesses;
    rtData_.binauralLoudness.read(binauralLoudnesses);
    hmeter_.setLoudness(binauralLoudnesses[0], binauralLoudnesses[1]);
    hmeter_.setBarWidth(meters_.back()->getWidth());
    hmeter_.setBounds(headphoneLoudnessBounds);

    // Configure meter loudness scale bounds and draw loudness meter scale.
    auto meterAndResetBounds =
        meterBounds.removeFromRight(leftScaleBounds.getWidth());
    auto meterScaleBounds =
        meterAndResetBounds.removeFromTop(boundsRef.getHeight() * 0.66);
    auto rstButtonBounds = meterAndResetBounds.removeFromBottom(
        meterScaleBounds.getHeight() * (0.3f));

    // Draw global S/M reset button near the S/M buttons.
    if (!meters_.empty()) {
      const auto* lastMeter = meters_.back().get();

      // Get the S/M buttons area in local coordinates of the lastMeter.
      const auto smLocalBounds = lastMeter->getSMButtonsBounds();

      // Translate these coordinates to MixMonitoringScreen coordinates
      // since getSMButtonsBounds() is relative to the lastMeter.
      const auto smButtonsBoundsInParent =
          smLocalBounds.translated(lastMeter->getX(), lastMeter->getY());

      const int kGap = 10;
      const int kResetButtonWidth = 20;
      const int kResetButtonHeight = 20;

      // Position the reset button just to the right of the S/M buttons.
      const int resetButtonX = smButtonsBoundsInParent.getRight() + kGap;
      const int resetButtonY =
          smButtonsBoundsInParent.getCentreY() - (kResetButtonHeight / 2);

      resetButton_.setBounds(resetButtonX, resetButtonY, kResetButtonWidth,
                             kResetButtonHeight);
    }
    rightScale_.setBounds(meterScaleBounds);

    // Draw loudness stats.
    auto statsBounds = bounds;
    stats_.setBounds(statsBounds);
  };

  // Update playback layout on change and repaint.
  void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                const juce::Identifier& property) override {
    if (property == RoomSetup::kSpeakerLayout) {
      pbLayout_ =
          repos_.roomSetupRepo_.get().getSpeakerLayout().getRoomSpeakerLayout();

      // All speakers should start unmuted and unsoloed on playback layout
      // change.
      PlaybackMS muteSoloState;
      repos_.playbackMSRepo_.update(muteSoloState);
      createLoudnessMeters(pbLayout_.getSpeakerLabels());

      repaint();
    }

    // When a channel is Solo'd, repaint meters to indicate the implicit muting
    // taking place.
    if (property == PlaybackMS::kSoloedChannelsID) {
      for (const auto& meter : meters_) {
        meter->repaint();
      }
    }
  }

  void resetSoloMutes() {
    repos_.playbackMSRepo_.update(PlaybackMS());
    for (const auto& meter : meters_) {
      meter->resetSoloMute();
    }
  }

 private:
  RepositoryCollection repos_;
  SpeakerMonitorData& rtData_;
  const juce::Image kResetImg_ = IconStore::getInstance().getResetIcon();
  const int kMaxChannels_ = 12;
  const int kMeterOffset = 4;  // Provide some offset between meters.

  // Current playback layout.
  Speakers::AudioElementSpeakerLayout pbLayout_;

  // Child components.
  std::vector<std::unique_ptr<LoudnessMeter>> meters_;
  juce::ImageButton resetButton_;
  LoudnessScale rightScale_;
  LoudnessScale leftScale_;
  HeadphonesLoudnessMeter hmeter_;
  LoudnessStats stats_;
};