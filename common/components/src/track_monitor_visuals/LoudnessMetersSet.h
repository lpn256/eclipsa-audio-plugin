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

#include "../../../audioelementplugin/src/AudioElementPluginProcessor.h"

class LoudnessMetersSet : public juce::Component {
 public:
  LoudnessMetersSet(
      AudioElementPluginSyncClient* syncClient,
      AudioElementPluginRepositoryCollection audioElementPluginRepo,
      Speakers::AudioElementSpeakerLayout& pbLayout);
  ~LoudnessMetersSet();

  void paint(juce::Graphics& g) override;

  void drawLoudnessMeters(juce::Graphics& g);

  void updateMeters();

  int getNumMeters() { return meters_.size(); }

  int calculateRequiredSetWidth() {
    return (meters_.size()) * (kMeterWidth + kMeterOffset);
  }

  int getMeterWidth() { return kMeterWidth; }

  int getMeterOffset() { return kMeterOffset; }

  void resetSoloMutes();

  const int kMaxMeterThreshold = 11;

 private:
  void createLoudnessMeters(const std::vector<juce::String>& chLabels);

  // UI components
  const juce::Image kResetImg_ = IconStore::getInstance().getResetIcon();
  const int kMaxChannels_ = 16;
  const int kMeterOffset = 4;  // Provide some offset between meters.
  const int kMeterWidth = 36;

  AudioElementPluginRepositoryCollection& repos_;
  SpeakerMonitorData& spkrData_;
  MSPlaybackRepository& playbackMSRepo_;
  AudioElementPluginSyncClient* syncClient_;
  AudioElementSpatialLayoutRepository* audioElementSpatialLayoutRepository_;

  // Current playback layout.
  Speakers::AudioElementSpeakerLayout pbLayout_;

  std::vector<std::unique_ptr<LoudnessMeter>> meters_;
};