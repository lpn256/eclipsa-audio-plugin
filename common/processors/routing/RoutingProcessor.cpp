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

#include "RoutingProcessor.h"

#include <unistd.h>

#include "data_structures/src/AudioElementPluginSyncClient.h"
#include "logger/logger.h"

RoutingProcessor::RoutingProcessor(
    AudioElementSpatialLayoutRepository* audioElementSpatialLayoutRepository,
    AudioElementPluginSyncClient* syncClient, int totalChannelCount)
    : audioElementSpatialLayoutData_(audioElementSpatialLayoutRepository),
      syncClient_(syncClient),
      firstChannel_(0),
      totalChannels_(0),
      totalChannelCount_(totalChannelCount) {
  // Register ourselves to listen for updates to the AudioElementSpatialLayout
  // and/or audio element data
  audioElementSpatialLayoutData_->registerListener(this);
  syncClient->registerListener(this);
  initializeRouting();
}

RoutingProcessor::~RoutingProcessor() {
  // Deregister listeners
  audioElementSpatialLayoutData_->deregisterListener(this);
  syncClient_->removeListener(this);
}

void RoutingProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
  juce::ignoreUnused(sampleRate);
  copyBuffer_.setSize(totalChannelCount_, samplesPerBlock);

  // Ensure routing is properly initialized when preparing for playback
  // This is especially important for AAX state restoration scenarios and
  // when sync client connects between playback sessions
  LOG_INFO(0, "RoutingProcessor: prepareToPlay() called - refreshing routing");
  initializeRouting();
}

void RoutingProcessor::initializeRouting() {
  // Fetch the audio element to determine the first and total channel numbers
  juce::Uuid audioElementId =
      audioElementSpatialLayoutData_->get().getAudioElementId();

  LOG_INFO(0,
           "RoutingProcessor: initializeRouting() called - audioElementId: " +
               audioElementId.toString().toStdString() +
               ", syncClient connected: " +
               (syncClient_->isConnected() ? "true" : "false"));

  auto audioElement = syncClient_->getElement(audioElementId);
  if (!audioElement.has_value()) {
    // If sync client doesn't have the element data yet (e.g., during AAX state
    // restoration when Renderer Plugin isn't connected yet), use the saved
    // spatial layout data as fallback. This should be reliable since both
    // plugins save consistent state information.
    auto spatialLayout = audioElementSpatialLayoutData_->get();
    if (spatialLayout.isLayoutSelected() && !audioElementId.isNull()) {
      firstChannel_ = spatialLayout.getFirstChannel();
      totalChannels_ = spatialLayout.getChannelLayout().getNumChannels();
      LOG_INFO(0,
               "RoutingProcessor: Using saved routing from spatial layout "
               "(should match renderer state) - firstChannel: " +
                   std::to_string(firstChannel_) +
                   ", totalChannels: " + std::to_string(totalChannels_));
    } else {
      // Only fall back to stereo pass-through if no layout was ever selected
      firstChannel_ = 0;
      totalChannels_ = juce::jmin(
          2, totalChannelCount_);  // Stereo or mono if only 1 channel available
      LOG_INFO(0,
               "RoutingProcessor: No layout selected in saved state - using "
               "stereo pass-through routing");
    }
    return;
  }

  firstChannel_ = audioElement->getFirstChannel();
  totalChannels_ = audioElement->getChannelCount();

  LOG_INFO(0,
           "RoutingProcessor: Initialized routing from sync client - "
           "firstChannel: " +
               std::to_string(firstChannel_) +
               ", totalChannels: " + std::to_string(totalChannels_));
}

void RoutingProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                    juce::MidiBuffer& midiMessages) {
  copyBuffer_.clear();

  // Copy data from the input channels to copy buffer, shifting the
  // audio forward by the first channel
  for (int channel = 0; channel < totalChannels_; ++channel) {
    if (channel + firstChannel_ <= buffer.getNumChannels()) {
      copyBuffer_.copyFrom(channel + firstChannel_, 0, buffer, channel, 0,
                           buffer.getNumSamples());
    }
  }

  // Now copy the data back to the original buffer
  // We can't copy in place because JUCE doesn't let you copy between the same
  // buffer
  buffer.makeCopyOf(copyBuffer_);
}