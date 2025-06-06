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
#include <data_structures/src/RepositoryCollection.h>
#include <data_structures/src/SpeakerMonitorData.h>
#include <processors/processor_base/ProcessorBase.h>

#include "data_repository/implementation/MSPlaybackRepository.h"

class MSProcessor : public ProcessorBase, public juce::ValueTree::Listener {
 public:
  MSProcessor(RepositoryCollection repos);
  MSProcessor(MSPlaybackRepository& msPlaybackRepository);
  ~MSProcessor();

  // Update local Solo / Mute state on value change.
  void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                const juce::Identifier& property) override;
  // Update local Solo / Mute state on value change.
  void valueTreeRedirected(juce::ValueTree& tree) override;

  void processBlock(juce::AudioBuffer<float>& buffer,
                    juce::MidiBuffer& midiMessages) override;

 private:
  MSPlaybackRepository& msPlaybackRepository_;
  std::bitset<PlaybackMS::kMaxNumPlaybackCh> soloedChs_;
  std::bitset<PlaybackMS::kMaxNumPlaybackCh> mutedChs_;
};