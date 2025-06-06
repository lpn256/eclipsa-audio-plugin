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

#include "HOAToBedRdr.h"

static void calculateAmbiData(const int numChIn, ear::HOATypeMetadata& md) {
  // Compute HOA Order and Degree per channel.
  std::vector<int> chOrders(numChIn, 0), chDegrees(numChIn, 0);
  for (int i = 0; i < numChIn; ++i) {
    chOrders[i] = std::sqrt(i);
    chDegrees[i] = i - chOrders[i] * (chOrders[i] + 1);
  }
  md.orders = chOrders;
  md.degrees = chDegrees;
}

static Speakers::AudioElementSpeakerLayout getIntermediateLayout(
    const Speakers::AudioElementSpeakerLayout playbackLayout) {
  // Expanded layouts get rendered to their base layouts, from which relevant
  // channels are extracted.
  if (playbackLayout.isExpandedLayout()) {
    return playbackLayout.getExplBaseLayout() == Speakers::kExpl9Point1Point6
               ? Speakers::k22p2
               : playbackLayout.getExplBaseLayout();
  }
  // Other layouts that are not BS2051 layouts are first rendered to a slightly
  // higher layout then downmixed.
  switch (playbackLayout) {
    case Speakers::k3Point1Point2:
      return Speakers::k5Point1Point2;
    case Speakers::k7Point1Point2:
      return Speakers::k7Point1Point4;
    case Speakers::kMono:
      return Speakers::kStereo;
    default:
      return Speakers::kUnknown;
  }
}

std::unique_ptr<Renderer> HOAToBedRdr::createHOAToBedRdr(
    const Speakers::AudioElementSpeakerLayout inputLayout,
    const Speakers::AudioElementSpeakerLayout playbackLayout) {
  // Only render from HOA to non-HOA layouts.
  if (!inputLayout.isAmbisonics() || playbackLayout.isAmbisonics() ||
      playbackLayout == Speakers::kBinaural) {
    return nullptr;
  }

  const int numChIn = inputLayout.getNumChannels();

  // Compute HOA Order and Degree per channel.
  ear::HOATypeMetadata md;
  calculateAmbiData(numChIn, md);

  // Determine if we need an intermediate layout for rendering.
  Speakers::AudioElementSpeakerLayout interLayout =
      getIntermediateLayout(playbackLayout);
  if (interLayout == Speakers::kUnknown) {
    interLayout = playbackLayout;
  }

  std::string ituLayoutStr = interLayout.getItuString();
  if (ituLayoutStr == "Unknown") {
    return nullptr;
  }

  // Calculate gain matrix for rendering.
  std::vector<std::vector<float>> hoaDecodeMat(
      numChIn, std::vector<float>(interLayout.getNumChannels()));
  auto gcLayout = ear::getLayout(ituLayoutStr);
  ear::GainCalculatorHOA gc(gcLayout);
  gc.calculate(md, hoaDecodeMat);

  // Construct a renderer for the given playback layout.
  return std::unique_ptr<Renderer>(
      new HOAToBedRdr(interLayout, playbackLayout, std::move(hoaDecodeMat)));
}

HOAToBedRdr::HOAToBedRdr(const IAMFSpkrLayout interLayout,
                         const IAMFSpkrLayout playbackLayout,
                         const std::vector<std::vector<float>>&& decodeMat)
    : kInterLayout_(interLayout),
      kOutputLayout_(playbackLayout),
      kChMap_(playbackLayout.getChGainMap()),
      kDecodeMat_(decodeMat) {}

void HOAToBedRdr::render(const FBuffer& srcBuffer, FBuffer& outBuffer) {
  if (kInterLayout_ != kOutputLayout_) {
    prepInterBuff(kInterLayout_, srcBuffer.getNumSamples());
    renderITU(srcBuffer, interBuffer_);
    mixChannels(interBuffer_, outBuffer);
  } else {
    renderITU(srcBuffer, outBuffer);
    mixChannels(outBuffer, outBuffer);
  }
}

inline void HOAToBedRdr::prepInterBuff(
    const Speakers::AudioElementSpeakerLayout interLayout,
    const int numSamples) {
  // Allocate a new buffer if necessary.
  if (interBuffer_.getNumChannels() != kInterLayout_.getNumChannels() ||
      interBuffer_.getNumSamples() != numSamples) {
    interBuffer_.setSize(kInterLayout_.getNumChannels(), numSamples);
  }
  interBuffer_.clear();
}

inline void HOAToBedRdr::renderITU(const FBuffer& srcBuffer,
                                   FBuffer& outBuffer) const {
  // Output channel generation: Each col. 'outChIdx' of decodeMat gives gain to
  // be applied to each input channel 'inChIdx' to produce output ch# 'outChIdx'
  for (int outChIdx = 0; outChIdx < outBuffer.getNumChannels(); ++outChIdx) {
    for (int inChIdx = 0; inChIdx < srcBuffer.getNumChannels(); ++inChIdx) {
      float gainToApply = kDecodeMat_[inChIdx][outChIdx];
      outBuffer.addFrom(outChIdx, 0, srcBuffer.getReadPointer(inChIdx),
                        srcBuffer.getNumSamples(), gainToApply);
    }
  }
}

inline void HOAToBedRdr::mixChannels(const FBuffer& srcBuffer,
                                     FBuffer& outBuffer) const {
  for (const auto [destCh, srcCh, gain] : kChMap_) {
    outBuffer.addFrom(destCh, 0, srcBuffer.getReadPointer(srcCh),
                      srcBuffer.getNumSamples(), gain);
  }
}