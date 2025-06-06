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
#include "../rdr_factory/Renderer.h"
#include "ear/ear.hpp"

class BedToBedRdr final : public Renderer {
 public:
  BedToBedRdr() = delete;

  /**
   * @brief Create a BedToBed renderer. Returns nullptr if requested renderer
   * cannot be constructed.
   *
   * @param sourceBuffer Reference to buffer containing input channels.
   * @param inputLayout Layout from which to render.
   * @param playbackLayout Layout to render to.
   * @return std::unique_ptr<Renderer>
   */
  static std::unique_ptr<Renderer> createBedToBedRdr(
      const IAMFSpkrLayout inputLayout, const IAMFSpkrLayout playbackLayout);

  ~BedToBedRdr() = default;

  void render(const FBuffer& srcBuffer, FBuffer& outBuffer) override;

 private:
  BedToBedRdr(const float* renderMatrix,
              const Speakers::AudioElementSpeakerLayout inputLayout,
              const Speakers::AudioElementSpeakerLayout playbackLayout);
  inline void prepInterBuff(const int numSamples);
  inline void renderITU(const FBuffer& srcBuffer, FBuffer& outBuffer) const;

  const float* kRenderMatrix_;
  const Speakers::AudioElementSpeakerLayout kInputLayout_;
  const std::optional<std::vector<int>> kChannelMap_;
  const int kNumChIn_, kNumChOut_;
  juce::AudioBuffer<float> interBuffer_;
};