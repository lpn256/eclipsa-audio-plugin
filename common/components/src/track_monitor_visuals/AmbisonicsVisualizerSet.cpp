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

#include "AmbisonicsVisualizerSet.h"

AmbisonicsVisualizerSet::AmbisonicsVisualizerSet(
    AudioElementPluginSyncClient* syncClient,
    AudioElementPluginRepositoryCollection audioElementPluginRepo,
    Speakers::AudioElementSpeakerLayout& pbLayout)
    : repos_(audioElementPluginRepo),
      syncClient_(syncClient),
      audioElementSpatialLayoutRepository_(
          &audioElementPluginRepo.audioElementSpatialLayoutRepository_),
      ambisonicsData_(audioElementPluginRepo.ambisonicsData_),
      pbLayout_(pbLayout) {
  ambisonicsLabel_.setText("Position", juce::dontSendNotification);
  ambisonicsLabel_.setFont(juce::Font(18.0f));
  ambisonicsLabel_.setColour(juce::Label::textColourId,
                             EclipsaColours::headingGrey);
  ambisonicsLabel_.setColour(juce::Label::backgroundColourId,
                             juce::Colours::transparentBlack);
  addAndMakeVisible(ambisonicsLabel_);
  addAndMakeVisible(colourLegend_);

  updateVisualizers();
}

AmbisonicsVisualizerSet::~AmbisonicsVisualizerSet() { setLookAndFeel(nullptr); }

void AmbisonicsVisualizerSet::paint(juce::Graphics& g) {
  // Draw ambisonics visualizer
  auto bounds = getLocalBounds();
  const auto visualizerBounds = bounds;
  auto labelBounds =
      bounds.removeFromLeft(visualizerBounds.proportionOfWidth(0.125f));
  labelBounds.removeFromBottom(visualizerBounds.proportionOfHeight(0.8f));
  ambisonicsLabel_.setBounds(labelBounds);

  auto scaleBounds =
      bounds.removeFromRight(visualizerBounds.proportionOfWidth(0.125f));

  for (int i = 0; i < ambisonicsVisualizers_.size(); i++) {
    ambisonicsVisualizers_[i]->setBounds(
        bounds.removeFromLeft(2.f * labelBounds.getWidth()));
  }

  scaleBounds.translate(-40, 0);
  scaleBounds.removeFromRight(scaleBounds.proportionOfWidth(0.2f));
  colourLegend_.setBounds(scaleBounds);
}

void AmbisonicsVisualizerSet::createAmbisonicsVisualizers() {
  ambisonicsVisualizers_.clear();
  for (int i = 0; i < viewPairs_.size(); i++) {
    ambisonicsVisualizers_.emplace_back(std::make_unique<VisualizerPair>(
        &ambisonicsData_, viewPairs_[i].first, viewPairs_[i].second));
    addAndMakeVisible(ambisonicsVisualizers_.back().get());
  }
}

void AmbisonicsVisualizerSet::updateVisualizers() {
  pbLayout_ = audioElementSpatialLayoutRepository_->get().getChannelLayout();
  createAmbisonicsVisualizers();
  repaint();
}
