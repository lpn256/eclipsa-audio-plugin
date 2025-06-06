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
#include "components/src/ambisonics_visualizers/ColourLegend.h"

class AmbisonicsVisualizerSet : public juce::Component {
 public:
  AmbisonicsVisualizerSet(
      AudioElementPluginSyncClient* syncClient,
      AudioElementPluginRepositoryCollection audioElementPluginRepo,
      Speakers::AudioElementSpeakerLayout& pbLayout);
  ~AmbisonicsVisualizerSet();

  void paint(juce::Graphics& g) override;

  void updateVisualizers();

 private:
  void createAmbisonicsVisualizers();

  // UI components
  const juce::Image kResetImg_ = IconStore::getInstance().getResetIcon();
  const int kMaxChannels_ = 16;
  const int kMeterOffset = 4;  // Provide some offset between meters.
  const int kMeterWidth = 36;

  const std::vector<std::pair<AmbisonicsVisualizer::VisualizerView,
                              AmbisonicsVisualizer::VisualizerView>>
      viewPairs_ = {{AmbisonicsVisualizer::VisualizerView::kLeft,
                     AmbisonicsVisualizer::VisualizerView::kRight},
                    {AmbisonicsVisualizer::VisualizerView::kFront,
                     AmbisonicsVisualizer::VisualizerView::kRear},
                    {AmbisonicsVisualizer::VisualizerView::kTop,
                     AmbisonicsVisualizer::VisualizerView::kBottom}};

  AudioElementPluginRepositoryCollection& repos_;
  AmbisonicsData& ambisonicsData_;
  AudioElementPluginSyncClient* syncClient_;
  AudioElementSpatialLayoutRepository* audioElementSpatialLayoutRepository_;
  ColourLegend colourLegend_;

  // Current playback layout.
  Speakers::AudioElementSpeakerLayout& pbLayout_;

  std::vector<std::unique_ptr<VisualizerPair>> ambisonicsVisualizers_;
  juce::Label ambisonicsLabel_;
};