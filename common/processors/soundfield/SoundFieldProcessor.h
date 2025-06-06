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

#include <processors/processor_base/ProcessorBase.h>

#include "SoundFieldReconstructor.h"
#include "data_repository/implementation/AudioElementSpatialLayoutRepository.h"
#include "data_structures/src/AudioElementPluginSyncClient.h"

class SoundFieldProcessor : public ProcessorBase,
                            public juce::ValueTree::Listener {
 public:
  SoundFieldProcessor(
      AudioElementSpatialLayoutRepository* audioElementSpatialLayoutRepo,
      AudioElementPluginSyncClient* syncClient, AmbisonicsData* ambisonicsData);

  ~SoundFieldProcessor();

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;

  void processBlock(juce::AudioBuffer<float>& buffer,
                    juce::MidiBuffer& midiMessages) override;

 private:
  void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                const juce::Identifier& property) override;

  std::shared_ptr<AudioElement> getAudioElementfromID(const juce::Uuid& id);
  RoomLayout getRoomLayout(std::shared_ptr<AudioElement>& element);

  AudioElementSpatialLayoutRepository* audioElementSpatialLayoutRepo_;
  AudioElementPluginSyncClient* syncClient_;
  AmbisonicsData* ambisonicsData_;
  std::unique_ptr<SoundField> soundField_;
  // Current playback layout.
  Speakers::AudioElementSpeakerLayout pbLayout_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SoundFieldProcessor)
};