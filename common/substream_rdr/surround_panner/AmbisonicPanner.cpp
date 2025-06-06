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

#include "AmbisonicPanner.h"

AmbisonicPanner::AmbisonicPanner(
    const Speakers::AudioElementSpeakerLayout inputLayout,
    const Speakers::AudioElementSpeakerLayout pannedLayout,
    const int samplesPerBlock, const int sampleRate)
    : kNumChIn_(inputLayout.getNumChannels()),
      encoder_(nullptr),
      AudioPanner(inputLayout, pannedLayout, samplesPerBlock, sampleRate) {
  // Create the encoder for encoding to the desired ambisonic order.
  encoder_ = std::make_unique<obr::AmbisonicEncoder>(
      kInputLayout_.getNumChannels(),
      std::sqrt(pannedLayout.getNumChannels()) - 1);
  int numSamplesOutPlanar = pannedLayout.getNumChannels() * kSamplesPerBlock_;

  // Resize internal buffers for API calls.
  inputBufferPlanar_ =
      obr::AudioBuffer(inputLayout.getNumChannels(), kSamplesPerBlock_);
  outputBufferPlanar_ =
      obr::AudioBuffer(pannedLayout.getNumChannels(), kSamplesPerBlock_);
}

AmbisonicPanner::~AmbisonicPanner() {}

void AmbisonicPanner::positionUpdated() {
  // NOTE: As the audio source is of arbitrary channel format, we will assume
  // all channels originate from the same point.
  for (int i = 0; i < kNumChIn_; ++i) {
    encoder_->SetSource(i, 1.f, currPos_.azimuth, currPos_.elevation,
                        currPos_.distance);
  }
}

void AmbisonicPanner::process(juce::AudioBuffer<float>& inputBuffer,
                              juce::AudioBuffer<float>& outputBuffer) {
  outputBuffer.clear();

  for (int i = 0; i < kNumChIn_; ++i) {
    const float* rPtr = inputBuffer.getReadPointer(i);
    for (int j = 0; j < kSamplesPerBlock_; ++j) {
      inputBufferPlanar_[i][j] = rPtr[j];
    }
  }

  // Convert input buffer to planar vector and add spatial information.
  encoder_->ProcessPlanarAudioData(inputBufferPlanar_, &outputBufferPlanar_);

  // Write the processed planar output data to the intermediate buffer.
  for (int i = 0; i < kPannedLayout_.getNumChannels(); i++) {
    for (int j = 0; j < kSamplesPerBlock_; j++) {
      outputBuffer.setSample(i, j, outputBufferPlanar_[i][j]);
    }
  }
}