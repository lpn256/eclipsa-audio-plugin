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

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_data_structures/juce_data_structures.h>

#include <string>

#include "../src/RepositoryItem.h"
#include "substream_rdr/substream_rdr_utils/Speakers.h"

struct LayoutLoudness : public RepositoryItemBase {
 public:
  LayoutLoudness() : RepositoryItemBase(juce::Uuid::null()) {};
  LayoutLoudness(Speakers::AudioElementSpeakerLayout layout,
                 const float& integratedLoudness = 0.0f,
                 const float& digitalPeak = 0.0f, const float& truePeak = 0.0f)
      : RepositoryItemBase(juce::Uuid::null()),
        layout_(layout),
        integratedLoudness_(integratedLoudness),
        digitalPeak_(digitalPeak),
        truePeak_(truePeak) {}

  bool operator==(const LayoutLoudness& other) const {
    return other.layout_ == layout_;
  }
  bool operator!=(const LayoutLoudness& other) const {
    return !(other == *this);
  }

  void setIntegratedLoudness(const float integratedLoudness) {
    integratedLoudness_ = integratedLoudness;
  }

  void setDigitalPeak(const float digitalPeak) { digitalPeak_ = digitalPeak; }

  void setTruePeak(const float truePeak) { truePeak_ = truePeak; }

  static LayoutLoudness fromTree(const juce::ValueTree tree) {
    return LayoutLoudness(
        static_cast<Speakers::AudioElementSpeakerLayout>(tree[kLayout]),
        tree[kIntegratedLoudness], tree[kDigitalPeak], tree[kTruePeak]);
  }

  virtual juce::ValueTree toValueTree() const override {
    return {kTreeType,
            {{kId, id_.toString()},
             {kLayout, static_cast<int>(layout_)},
             {kIntegratedLoudness, integratedLoudness_},
             {kDigitalPeak, digitalPeak_},
             {kTruePeak, truePeak_}}};
  }

  Speakers::AudioElementSpeakerLayout getLayout() const { return layout_; }
  juce::Uuid getId() const { return id_; }
  float getIntegratedLoudness() const { return integratedLoudness_; }
  float getDigitalPeak() const { return digitalPeak_; }
  float getTruePeak() const { return truePeak_; }

  inline static const juce::Identifier kTreeType{"layout_loudness"};
  inline static const juce::Identifier kLayout{"audio_element_layout"};
  inline static const juce::Identifier kIntegratedLoudness{
      "Integrated_Loudness"};
  inline static const juce::Identifier kDigitalPeak{"Digital_Peak"};
  inline static const juce::Identifier kTruePeak{"True_Peak"};

 private:
  Speakers::AudioElementSpeakerLayout layout_;
  float integratedLoudness_;
  float digitalPeak_;
  float truePeak_;
};

class MixPresentationLoudness final : public RepositoryItemBase {
 public:
  MixPresentationLoudness();
  MixPresentationLoudness(
      juce::Uuid id,
      Speakers::AudioElementSpeakerLayout largestLayout = Speakers::kStereo);

  bool operator==(const MixPresentationLoudness& other) const;
  bool operator!=(const MixPresentationLoudness& other) const {
    return !(other == *this);
  }

  static MixPresentationLoudness fromTree(const juce::ValueTree tree);
  virtual juce::ValueTree toValueTree() const override;

  void replaceLargestLayout(const Speakers::AudioElementSpeakerLayout layout,
                            const float integratedLoudness = 0.0f,
                            const float digitalPeak = 0.0f,
                            const float truePeak = 0.0f);

  void setLayoutIntegratedLoudness(
      const Speakers::AudioElementSpeakerLayout& layout,
      const float integratedLoudness);

  void setLayoutDigitalPeak(const Speakers::AudioElementSpeakerLayout& layout,
                            const float digitalPeak);

  void setLayoutTruePeak(const Speakers::AudioElementSpeakerLayout& layout,
                         const float truePeak);

  bool includesLayout(const Speakers::AudioElementSpeakerLayout& layout) const;

  std::array<LayoutLoudness, 2> getLayouts() const { return layouts_; }

  float getLayoutIntegratedLoudness(
      const Speakers::AudioElementSpeakerLayout& layout) const;

  float getLayoutDigitalPeak(
      const Speakers::AudioElementSpeakerLayout& layout) const;

  float getLayoutTruePeak(
      const Speakers::AudioElementSpeakerLayout& layout) const;

  Speakers::AudioElementSpeakerLayout getLargestLayout() const {
    return largestLayout_;
  }

  inline static const juce::Identifier kTreeType{"mix_presentation_loudness"};
  inline static const juce::Identifier kLayouts{"layout_loudnesses"};
  inline static const juce::Identifier kLargestLayout{"largest_layout"};

 private:
  std::array<LayoutLoudness, 2> layouts_;  // only export upto two layouts
  Speakers::AudioElementSpeakerLayout largestLayout_;
};