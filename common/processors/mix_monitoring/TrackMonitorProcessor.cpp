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

#include "TrackMonitorProcessor.h"

#include "data_structures/src/AudioElementSpatialLayout.h"

TrackMonitorProcessor::TrackMonitorProcessor(
    SpeakerMonitorData& data, AudioElementSpatialLayoutRepository* repo)
    : audioElementSpatialLayoutRepository_(repo),
      rtData_(data),
      playbackLayout_(juce::AudioChannelSet::mono()),
      inputLayout_(Speakers::kMono),
      samplesPerBlock_(1),
      sampleRate_(48000) {
  audioElementSpatialLayoutRepository_->registerListener(this);
}

void TrackMonitorProcessor::prepareToPlay(double sampleRate,
                                          int samplesPerBlock) {
  samplesPerBlock_ = samplesPerBlock;
  sampleRate_ = sampleRate;

  // Construct a measurement object if necessary.
  if (loudnessImpl_ == nullptr) {
    loudnessImpl_ = std::make_unique<MeasureEBU128>(sampleRate);
  }

  // Construct a binaural renderer
  binauralRendererLock_.enter();
  binauralLoudnessRenderer_ = createRenderer(inputLayout_, Speakers::kBinaural,
                                             samplesPerBlock, sampleRate);
  binauralBuffer_ = juce::AudioBuffer<float>(
      Speakers::kBinaural.getNumChannels(), samplesPerBlock);
  binauralRendererLock_.exit();
}

void TrackMonitorProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                         juce::MidiBuffer& midiMessages) {
  // Get portion of buffer containing rendered channels.
  rdrBuffer_ = getRenderedBuffer(buffer);

  // UI triggered a stats update (rare).
  if (rtData_.resetStats.load()) {
    loudnessImpl_->reset(playbackLayout_, rdrBuffer_);
    rtData_.resetStats.store(false);
  }
  // Measure EBU128 loudness statistics.
  loudnessStats_ = loudnessImpl_->measureLoudness(playbackLayout_, rdrBuffer_);
  rtData_.loudnessEBU128.update(loudnessStats_);

  // Measure per-channel loudness in dB.
  std::vector<float> loudnesses(rdrBuffer_.getNumChannels());
  for (int i = 0; i < rdrBuffer_.getNumChannels(); ++i) {
    float loudness = 20.0f * std::log10(rdrBuffer_.getRMSLevel(
                                 i, 0, rdrBuffer_.getNumSamples()));
    loudnesses[i] = loudness;
  }
  rtData_.playbackLoudness.update(loudnesses);

  // Measure binaural loudness by performing a binaural render
  if (binauralLoudnessRenderer_ != nullptr) {
    binauralRendererLock_.enter();
    // Create a buffer to render the binaural audio to
    binauralLoudnessRenderer_->render(rdrBuffer_, binauralBuffer_);
    binauralRendererLock_.exit();

    std::array<float, 2> loudnesses = {-10, -10};
    for (int i = 0; i < 2; ++i) {
      loudnesses[i] = 20.0f * std::log10(binauralBuffer_.getRMSLevel(
                                  i, 0, binauralBuffer_.getNumSamples()));
    }
    rtData_.binauralLoudness.update(loudnesses);
  }
}

const juce::AudioBuffer<float> TrackMonitorProcessor::getRenderedBuffer(
    juce::AudioBuffer<float>& busBuff) {
  auto dataPtrs = busBuff.getArrayOfWritePointers();
  int numRdrCh = playbackLayout_.size();
  return juce::AudioBuffer<float>(dataPtrs, numRdrCh, busBuff.getNumSamples());
}

void TrackMonitorProcessor::valueTreePropertyChanged(
    juce::ValueTree& treeWhosePropertyHasChanged,
    const juce::Identifier& property) {
  // An update to the audio element spatial layout repository has occurred

  juce::ignoreUnused(treeWhosePropertyHasChanged);
  juce::ignoreUnused(property);

  if (property != AudioElementSpatialLayout::kLayout) {
    return;
  }

  AudioElementSpatialLayout audioElementSpatialLayout =
      audioElementSpatialLayoutRepository_->get();
  playbackLayout_ =
      audioElementSpatialLayout.getChannelLayout().getChannelSet();
  inputLayout_ = audioElementSpatialLayout.getChannelLayout();
  binauralRendererLock_.enter();
  binauralLoudnessRenderer_ = createRenderer(inputLayout_, Speakers::kBinaural,
                                             samplesPerBlock_, sampleRate_);
  binauralRendererLock_.exit();
}
