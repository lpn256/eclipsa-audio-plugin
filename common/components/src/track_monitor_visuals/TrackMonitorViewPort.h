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
#include "AmbisonicsVisualizerSet.h"
#include "LoudnessMetersSet.h"

class TrackMonitorViewPort : public juce::Component {
 public:
  TrackMonitorViewPort(
      AudioElementPluginSyncClient* syncClient,
      AudioElementPluginRepositoryCollection audioElementPluginRepo,
      Speakers::AudioElementSpeakerLayout& pbLayout);
  ~TrackMonitorViewPort();

  void paint(juce::Graphics& g) override;

  void switchedToAmbisonics();

  void switchedtoLoudnessMeters();

  int getMeterWidth() { return set_.getMeterWidth(); }

  int getMeterOffset() { return set_.getMeterOffset(); }

  void resetSoloMutes() { set_.resetSoloMutes(); }

 private:
  juce::Viewport viewPort_;
  LoudnessMetersSet set_;
  AmbisonicsVisualizerSet ambisonicsSet_;

  // Current playback layout.
  Speakers::AudioElementSpeakerLayout& pbLayout_;
};