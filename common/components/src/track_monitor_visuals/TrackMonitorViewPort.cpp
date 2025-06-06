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

#include "TrackMonitorViewPort.h"

TrackMonitorViewPort::TrackMonitorViewPort(
    AudioElementPluginSyncClient* syncClient,
    AudioElementPluginRepositoryCollection audioElementPluginRepo,
    Speakers::AudioElementSpeakerLayout& pbLayout)
    : set_(syncClient, audioElementPluginRepo, pbLayout),
      ambisonicsSet_(syncClient, audioElementPluginRepo, pbLayout),
      pbLayout_(pbLayout) {
  addAndMakeVisible(viewPort_);
  viewPort_.setScrollBarsShown(false, true);
  if (pbLayout_.isAmbisonics()) {
    viewPort_.setViewedComponent(&ambisonicsSet_, false);
  } else {
    viewPort_.setViewedComponent(&set_, false);
  }
}

TrackMonitorViewPort::~TrackMonitorViewPort() { setLookAndFeel(nullptr); }

void TrackMonitorViewPort::paint(juce::Graphics& g) {
  const auto bounds = getLocalBounds();
  viewPort_.setSize(bounds.getWidth(), bounds.getHeight());
  int width = bounds.getWidth();

  if (!pbLayout_.isAmbisonics() &&
      set_.getNumMeters() > set_.kMaxMeterThreshold) {
    width = set_.calculateRequiredSetWidth();
  }
  viewPort_.getViewedComponent()->setSize(width, bounds.getHeight() * .95f);
}

void TrackMonitorViewPort::switchedToAmbisonics() {
  ambisonicsSet_.updateVisualizers();
  viewPort_.setViewedComponent(&ambisonicsSet_, false);
}

void TrackMonitorViewPort::switchedtoLoudnessMeters() {
  set_.updateMeters();
  viewPort_.setViewedComponent(&set_, false);
}