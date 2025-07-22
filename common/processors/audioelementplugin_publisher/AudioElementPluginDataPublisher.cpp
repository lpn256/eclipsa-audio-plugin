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

#include "AudioElementPluginDataPublisher.h"

#include <cmath>
#include <memory>

#include "data_structures/src/AudioElementCommunication.h"
#include "data_structures/src/SpeakerMonitorData.h"

AudioElementPluginDataPublisher::AudioElementPluginDataPublisher(
    AudioElementSpatialLayoutRepository* audioElementSpatialLayoutRepository,
    AudioElementParameterTree* automationParameterTree,
    SpeakerMonitorData& monitorData)
    : audioElementSpatialLayoutData_(audioElementSpatialLayoutRepository),
      automationParameterTree_(automationParameterTree),
      monitorData_(monitorData) {
  // Set up the initial data
  localData_.x = automationParameterTree_->getXPosition();
  localData_.y = automationParameterTree_->getYPosition();
  localData_.z = automationParameterTree_->getZPosition();
  dataChanged_ = true;

  // Update any information from the repository
  updateData();

  automationParameterTree_->addXPositionListener(this);
  automationParameterTree_->addYPositionListener(this);
  automationParameterTree_->addZPositionListener(this);

  audioElementSpatialLayoutRepository->registerListener(this);
  startTimerHz(60);
}

AudioElementPluginDataPublisher::~AudioElementPluginDataPublisher() {}

void AudioElementPluginDataPublisher::prepareToPlay(double sampleRate,
                                                    int samplesPerBlock) {
  dataChanged_ = true;
}

void AudioElementPluginDataPublisher::updateData() {
  dataChanged_ = true;
  // Fetch the audio element plugin name from the repository
  strncpy(localData_.name,
          audioElementSpatialLayoutData_->get().getName().toRawUTF8(),
          sizeof(localData_.name));
  localData_.name[sizeof(localData_.name) - 1] =
      '\0';  // Ensure null-terminated
  channels_ =
      audioElementSpatialLayoutData_->get().getChannelLayout().getNumChannels();

  std::memcpy(localData_.uuid.data(),
              audioElementSpatialLayoutData_->get().getId().getRawData(),
              localData_.uuid.size());

  monitorData_.avgLoudness.update(-70.f);  // Reset the average loudness
}

void AudioElementPluginDataPublisher::processBlock(
    juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {
  float loudness = 0;
  for (int i = 0; i < channels_; ++i) {
    float chLoud =
        20.0f * std::log10(buffer.getRMSLevel(i, 0, buffer.getNumSamples()));
    // Clamp the loudness to -70 dB since some tracks will be -Inf
    loudness += std::max(chLoud, -70.0f);
  }
  loudness = loudness / channels_;
  monitorData_.avgLoudness.update(loudness);
}

void AudioElementPluginDataPublisher::timerCallback() {
  if (publisher_ == nullptr) {
    publisher_ =
        std::unique_ptr<AudioElementPublisher>(new AudioElementPublisher());
  }
  float loudness(-70.f);
  monitorData_.avgLoudness.read(loudness);
  if (localData_.loudness != loudness) {
    localData_.loudness = loudness;
    dataChanged_ = true;
  }

  if (dataChanged_) {
    publisher_->publishData(localData_);
    dataChanged_ = false;
  }
}
