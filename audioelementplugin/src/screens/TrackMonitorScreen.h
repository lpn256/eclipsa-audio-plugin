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

#include <cstddef>

#include "../AudioElementPluginProcessor.h"
#include "components/src/loudness_meter/LoudnessScale.h"
#include "components/src/track_monitor_visuals/TrackMonitorViewPort.h"

class TrackMonitorScreen : public juce::Component,
                           public juce::ValueTree::Listener {
 public:
  TrackMonitorScreen(AudioElementPluginSyncClient* syncClient,
                     AudioElementPluginRepositoryCollection repos);

  ~TrackMonitorScreen();

  void paint(juce::Graphics& g) override;

 private:
  void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                const juce::Identifier& property) override;

  void initializeUI();

  void drawRightScaleandHeadphoneMeters(juce::Rectangle<int>& viewPortBounds);
  void changeComponentVisibility(const bool& visibility);

  // UI components
  const juce::Image kResetImg_ = IconStore::getInstance().getResetIcon();
  AudioElementSpatialLayoutRepository* audioElementSpatialLayoutRepository_;

  // Current playback layout.
  Speakers::AudioElementSpeakerLayout pbLayout_;

  LoudnessStats stats_;
  SpeakerMonitorData& rtData_;

  juce::ImageButton resetButton_;
  LoudnessScale leftScale_;
  LoudnessScale rightScale_;
  HeadphonesLoudnessMeter hmeter_;
  TrackMonitorViewPort viewPort_;
};