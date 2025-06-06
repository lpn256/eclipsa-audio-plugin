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
#include <data_structures/src/SpeakerMonitorData.h>
#include <processors/processor_base/ProcessorBase.h>

#include "data_repository/implementation/AudioElementSpatialLayoutRepository.h"
#include "loudness_standards/MeasureEBU128.h"
#include "substream_rdr/rdr_factory/Renderer.h"
#include "substream_rdr/rdr_factory/RendererFactory.h"

class TrackMonitorProcessor : public ProcessorBase, juce::ValueTree::Listener {
 public:
  using EBU128Stats = MeasureEBU128::LoudnessStats;

  TrackMonitorProcessor(SpeakerMonitorData& data,
                        AudioElementSpatialLayoutRepository* repo);

  ~TrackMonitorProcessor() = default;

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;

  void processBlock(juce::AudioBuffer<float>& buffer,
                    juce::MidiBuffer& midiMessages) override;

  EBU128Stats getEBU128Stats() { return loudnessStats_; }

 private:
  // Create a buffer of rendered channels from the renderer processor buffer.
  const juce::AudioBuffer<float> getRenderedBuffer(
      juce::AudioBuffer<float>& busBuff);

  void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                const juce::Identifier& property) override;
  AudioElementSpatialLayoutRepository* audioElementSpatialLayoutRepository_;
  SpeakerMonitorData& rtData_;

  juce::SpinLock binauralRendererLock_;
  Speakers::AudioElementSpeakerLayout inputLayout_;
  std::unique_ptr<Renderer> binauralLoudnessRenderer_;
  int samplesPerBlock_, sampleRate_;

  // Recent copy of the current playback layout.
  juce::AudioChannelSet playbackLayout_;

  // Buffer containing playback rendered audio.
  juce::AudioBuffer<float> rdrBuffer_;
  juce::AudioBuffer<float> binauralBuffer_;

  std::unique_ptr<MeasureEBU128> loudnessImpl_;
  EBU128Stats loudnessStats_{};
};