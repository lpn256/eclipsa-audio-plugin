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

#include <juce_dsp/juce_dsp.h>

#include "../src/RepositoryItem.h"

class ChannelGains final : public RepositoryItemBase {
 public:
  ChannelGains();
  ChannelGains(juce::Uuid id, std::vector<float> gains,
               std::unordered_map<int, float> mutedChannels);
  ChannelGains(juce::Uuid id, juce::String name, juce::int32 numChannels);

  ~ChannelGains();

  void toggleChannelMute(const int& channel);

  static ChannelGains fromTree(juce::ValueTree tree);
  virtual juce::ValueTree toValueTree() const override;

  static std::vector<std::string> splitStringBySpace(const std::string& input);
  static std::vector<float> convertStringsToFloats(
      const std::vector<std::string>& stringVec);

  static std::vector<int> convertStringsToInts(
      const std::vector<std::string>& stringVec);

  void setTotalChannels(juce::int32 totalChannels);
  void setChannelGain(const int& channel, const float& gain);
  void setGains(std::vector<float> gains);

  juce::int32 getTotalChannels() const { return totalChannels_; }
  std::vector<float> getGains() const { return gains_; }
  std::unordered_map<int, float> getMutedChannels() const {
    return mutedChannels_;
  };

  inline static const juce::Identifier kTreeType{"multichannel_Gains"};
  inline static const juce::Identifier kTotalChannels{"total_channels"};
  inline static const juce::Identifier kGains{"gains"};
  inline static const juce::Identifier kMutedChannels{"muted_channels"};
  inline static const juce::Identifier kPrevGain{"previous_gain"};

 private:
  juce::int32 totalChannels_;
  std::vector<float> gains_;
  std::unordered_map<int, float> mutedChannels_;
};