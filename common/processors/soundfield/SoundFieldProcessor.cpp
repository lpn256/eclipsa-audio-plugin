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

#pragma once
#include "SoundFieldProcessor.h"

SoundFieldProcessor::SoundFieldProcessor(
    AudioElementSpatialLayoutRepository* audioElementSpatialLayoutRepo,
    AudioElementPluginSyncClient* syncClient, AmbisonicsData* ambisonicsData)
    : audioElementSpatialLayoutRepo_(audioElementSpatialLayoutRepo),
      syncClient_(syncClient),
      ambisonicsData_(ambisonicsData) {
  audioElementSpatialLayoutRepo_->registerListener(this);

  if (audioElementSpatialLayoutRepo_->get().getAudioElementId() !=
      juce::Uuid::null()) {
    pbLayout_ = audioElementSpatialLayoutRepo_->get().getChannelLayout();
  } else {
    pbLayout_ = Speakers::kUnknown;
  }
}

SoundFieldProcessor::~SoundFieldProcessor() {
  audioElementSpatialLayoutRepo_->deregisterListener(this);
}

void SoundFieldProcessor::prepareToPlay(double sampleRate,
                                        int samplesPerBlock) {
  // if the decoder sample rate is different from the current sample rate,
  // reinit
  if (soundField_ &&
      static_cast<int>(sampleRate) != soundField_->getDecoderSampleRate()) {
    soundField_->reinitDecoder(static_cast<int>(sampleRate));
  }
}

void SoundFieldProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                       juce::MidiBuffer& midiMessages) {
  if (soundField_) {
    soundField_->processDecoder(buffer);
  }
}

void SoundFieldProcessor::valueTreePropertyChanged(
    juce::ValueTree& treeWhosePropertyHasChanged,
    const juce::Identifier& property) {
  if (property == AudioElementSpatialLayout::kLayout) {
    pbLayout_ = audioElementSpatialLayoutRepo_->get().getChannelLayout();

    // If the layout is an ambisonics layout, instantiate the soundfield
    // measurement implementation.
    if (pbLayout_.isAmbisonics()) {
      soundField_ = std::make_unique<SoundField>(pbLayout_, *ambisonicsData_);
    } else {
      soundField_.reset();
    }
  }
}

std::shared_ptr<AudioElement> SoundFieldProcessor::getAudioElementfromID(
    const juce::Uuid& id) {
  juce::OwnedArray<AudioElement> elements;
  syncClient_->getAudioElements(elements);
  for (int i = 0; i < elements.size(); i++) {
    if (elements[i]->getId() == id) {
      return std::make_shared<AudioElement>(*elements[i]);
    }
  }
  return nullptr;
}

RoomLayout SoundFieldProcessor::getRoomLayout(
    std::shared_ptr<AudioElement>& element) {
  return RoomLayout(element->getChannelConfig(),
                    element->getDescription().toStdString());
}