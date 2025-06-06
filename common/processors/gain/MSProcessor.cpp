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

#include "MSProcessor.h"

#include "data_repository/implementation/MSPlaybackRepository.h"
#include "data_structures/src/PlaybackMS.h"

MSProcessor::MSProcessor(RepositoryCollection repos)
    : msPlaybackRepository_(repos.playbackMSRepo_) {
  // NOTE: Deregistering of this processor has been problematic and removed for
  // now.
  msPlaybackRepository_.registerListener(this);
}

MSProcessor::MSProcessor(MSPlaybackRepository& msPlaybackRepository)
    : msPlaybackRepository_(msPlaybackRepository) {
  msPlaybackRepository_.registerListener(this);
}

MSProcessor::~MSProcessor() { msPlaybackRepository_.deregisterListener(this); }

void MSProcessor::valueTreePropertyChanged(
    juce::ValueTree& treeWhosePropertyHasChanged,
    const juce::Identifier& property) {
  PlaybackMS muteSoloState = msPlaybackRepository_.get();
  mutedChs_ = muteSoloState.getMutedChannels();
  soloedChs_ = muteSoloState.getSoloedChannels();
}

void MSProcessor::valueTreeRedirected(juce::ValueTree& tree) {
  PlaybackMS muteSoloState = msPlaybackRepository_.get();
  mutedChs_ = muteSoloState.getMutedChannels();
  soloedChs_ = muteSoloState.getSoloedChannels();
}

void MSProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                               juce::MidiBuffer& midiMessages) {
  // Determine gains for muted / soloed channels.
  std::bitset<PlaybackMS::kMaxNumPlaybackCh> chGains;
  chGains.set();          // Initially all channels are on.
  chGains &= ~mutedChs_;  // Muting disables a channel.
  if (soloedChs_.any()) {
    chGains &= soloedChs_;  // Disable all channels except soloed(s).
  }

  // Compute a safe number of channels to iterate through. In practice this
  // should always be the size of channel gains
  int channelCount = std::min((int)chGains.size(), buffer.getNumChannels());

  // Perform channel muting.
  for (int i = 0; i < channelCount; ++i) {
    buffer.applyGain(i, 0, buffer.getNumSamples(), chGains[i]);
  }
}