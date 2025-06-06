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
#include "substream_rdr/rdr_factory/Renderer.h"

class HOAToBedRdr final : public Renderer {
 public:
  /**
   * @brief Create a HOAToBed renderer. Returns nullptr if requested renderer
   * cannot be constructed.
   *
   * @param sourceBuffer Reference to buffer containing input channels.
   * @param inputLayout HOA layout from which to render.
   * @param playbackLayout Channel layout to render to.
   * @return std::unique_ptr<Renderer>
   */
  static std::unique_ptr<Renderer> createHOAToBedRdr(
      const Speakers::AudioElementSpeakerLayout inputLayout,
      const Speakers::AudioElementSpeakerLayout playbackLayout);

  ~HOAToBedRdr() = default;

  void render(const FBuffer& srcBuffer, FBuffer& outBuffer) override;

 private:
  HOAToBedRdr(const IAMFSpkrLayout interLayout,
              const IAMFSpkrLayout playbackLayout,
              const std::vector<std::vector<float>>&& decodeMat);
  inline void prepInterBuff(
      const Speakers::AudioElementSpeakerLayout interLayout,
      const int numSamples);
  inline void renderITU(const FBuffer& srcBuffer, FBuffer& outBuffer) const;
  inline void mixChannels(const FBuffer& srcBuffer, FBuffer& outBuffer) const;

  const Speakers::AudioElementSpeakerLayout kInterLayout_, kOutputLayout_;
  const std::vector<Speakers::ChGainMap> kChMap_;
  const std::vector<std::vector<float>> kDecodeMat_;
  juce::AudioBuffer<float> interBuffer_;
};