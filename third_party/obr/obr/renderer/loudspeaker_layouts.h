/*
 * Copyright (c) 2025 Google LLC
 *
 * This source code is subject to the terms of the BSD 3-Clause Clear License,
 * which you can find in the LICENSE file, and the Open Binaural Renderer
 * Patent License 1.0, which you can find in the PATENTS file.
 */

#ifndef OBR_LOUDSPEAKER_LAYOUTS_H_
#define OBR_LOUDSPEAKER_LAYOUTS_H_

#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/log/check.h"
#include "absl/log/log.h"
#include "obr/renderer/audio_element_type.h"
#include "obr/renderer/input_channel_config.h"

namespace obr {

/*!\brief Defines loudspeaker layouts which can be rendered using
 * obr.
 */
class LoudspeakerLayouts {
 public:
  LoudspeakerLayouts() = default;

  /*!\brief Returns the loudspeaker layout for a given layout name.
   *
   * \param name Name of the layout.
   * \return Vector of LoudspeakerLayoutInputChannel.
   */
  std::vector<LoudspeakerLayoutInputChannel> getLoudspeakerLayout(
      AudioElementType type) {
    // Check if the layout is defined.
    auto it = layout_map.find(type);
    if (it == layout_map.end()) {
      LOG(ERROR) << "Unknown loudspeaker layout: \""
                 << GetAudioElementTypeStr(type) << "\".";
      return {};
    }

    std::vector<LoudspeakerLayoutInputChannel> loudspeaker_layout;

    for (auto loudspeaker : layout_map.at(it->first)) {
      loudspeaker_layout.push_back(loudspeaker_map.at(loudspeaker));
    }

    return loudspeaker_layout;
  }

 private:
  /*!\brief Defines labels for virtual loudspeakers used. */
  enum VirtualLoudspeaker {
    kC,
    kLFE,
    kL30,
    kR30,
    kL45,
    kR45,
    kL60,
    kR60,
    kL90,
    kR90,
    kL110,
    kR110,
    kL135,
    kR135,
    kTL30,
    kTR30,
    kTL45,
    kTR45,
    kTL90,
    kTR90,
    kTL135,
    kTR135,
    kTL150,
    kTR150,
  };

  /*!\brief Loudspeaker label - loudspeaker source configuration mapping. */
  const absl::flat_hash_map<VirtualLoudspeaker, LoudspeakerLayoutInputChannel>
      loudspeaker_map = {
          {kC, LoudspeakerLayoutInputChannel("kC", 0.0f, 0.0f, 1.0f, false)},
          {kLFE,
           LoudspeakerLayoutInputChannel("kLFE", 0.0f, -30.0f, 1.0f, true)},
          {kL30,
           LoudspeakerLayoutInputChannel("kL30", 30.0f, 0.0f, 1.0f, false)},
          {kR30,
           LoudspeakerLayoutInputChannel("kR30", -30.0f, 0.0f, 1.0f, false)},
          {kL45,
           LoudspeakerLayoutInputChannel("kL45", 45.0f, 0.0f, 1.0f, false)},
          {kR45,
           LoudspeakerLayoutInputChannel("kR45", -45.0f, 0.0f, 1.0f, false)},
          {kL60,
           LoudspeakerLayoutInputChannel("kL60", 60.0f, 0.0f, 1.0f, false)},
          {kR60,
           LoudspeakerLayoutInputChannel("kR60", -60.0f, 0.0f, 1.0f, false)},
          {kL90,
           LoudspeakerLayoutInputChannel("kL90", 90.0f, 0.0f, 1.0f, false)},
          {kR90,
           LoudspeakerLayoutInputChannel("kR90", -90.0f, 0.0f, 1.0f, false)},
          {kL110,
           LoudspeakerLayoutInputChannel("kL110", 110.0f, 0.0f, 1.0f, false)},
          {kR110,
           LoudspeakerLayoutInputChannel("kR110", -110.0f, 0.0f, 1.0f, false)},
          {kL135,
           LoudspeakerLayoutInputChannel("kL135", 135.0f, 0.0f, 1.0f, false)},
          {kR135,
           LoudspeakerLayoutInputChannel("kR135", -135.0f, 0.0f, 1.0f, false)},
          {kTL30,
           LoudspeakerLayoutInputChannel("kTL30", 30.0f, 45.0f, 1.0f, false)},
          {kTR30,
           LoudspeakerLayoutInputChannel("kTR30", -30.0f, 45.0f, 1.0f, false)},
          {kTL45,
           LoudspeakerLayoutInputChannel("kTL45", 45.0f, 45.0f, 1.0f, false)},
          {kTR45,
           LoudspeakerLayoutInputChannel("kTR45", -45.0f, 45.0f, 1.0f, false)},
          {kTL90,
           LoudspeakerLayoutInputChannel("kTL90", 90.0f, 45.0f, 1.0f, false)},
          {kTR90,
           LoudspeakerLayoutInputChannel("kTR90", -90.0f, 45.0f, 1.0f, false)},
          {kTL135,
           LoudspeakerLayoutInputChannel("kTL135", 135.0f, 45.0f, 1.0f, false)},
          {kTR135, LoudspeakerLayoutInputChannel("kTR135", -135.0f, 45.0f, 1.0f,
                                                 false)},
          {kTL150,
           LoudspeakerLayoutInputChannel("kTL150", 150.0f, 45.0f, 1.0f, false)},
          {kTR150, LoudspeakerLayoutInputChannel("kTR150", -150.0f, 45.0f, 1.0f,
                                                 false)},
  };

  /*!\brief Loudspeaker layout - loudspeaker label mapping. */
  const absl::flat_hash_map<AudioElementType, std::vector<VirtualLoudspeaker>>
      layout_map = {
          {AudioElementType::kLayoutMono, {kC}},
          {AudioElementType::kLayoutStereo, {kL30, kR30}},
          {AudioElementType::kLayout3_1_2_ch,
           {kL45, kR45, kC, kLFE, kTL30, kTR30}},
          {AudioElementType::kLayout5_1_0_ch,
           {kL30, kR30, kC, kLFE, kL110, kR110}},
          {AudioElementType::kLayout5_1_2_ch,
           {kL30, kR30, kC, kLFE, kL110, kR110, kTL90, kTR90}},
          {AudioElementType::kLayout5_1_4_ch,
           {kL30, kR30, kC, kLFE, kL110, kR110, kTL45, kTR45, kTL135, kTR135}},
          {AudioElementType::kLayout7_1_0_ch,
           {kL30, kR30, kC, kLFE, kL90, kR90, kL135, kR135}},
          {AudioElementType::kLayout7_1_2_ch,
           {kL30, kR30, kC, kLFE, kL90, kR90, kL135, kR135, kTL90, kTR90}},
          {AudioElementType::kLayout7_1_4_ch,
           {kL30, kR30, kC, kLFE, kL90, kR90, kL135, kR135, kTL45, kTR45,
            kTL135, kTR135}},
          {AudioElementType::kLayout9_1_0_ch,
           {kL30, kR30, kC, kLFE, kL60, kR60, kL90, kR90, kL135, kR135}},
          {AudioElementType::kLayout9_1_2_ch,
           {kL30, kR30, kC, kLFE, kL60, kR60, kL90, kR90, kL135, kR135, kTL90,
            kTR90}},
          {AudioElementType::kLayout9_1_4_ch,
           {kL30, kR30, kC, kLFE, kL60, kR60, kL90, kR90, kL135, kR135, kTL45,
            kTR45, kTL135, kTR135}},
          {AudioElementType::kLayout9_1_6_ch,
           {kL30, kR30, kC, kLFE, kL60, kR60, kL90, kR90, kL135, kR135, kTL30,
            kTR30, kTL90, kTR90, kTL150, kTR150}},
  };
};

};  // namespace obr

#endif  // OBR_LOUDSPEAKER_LAYOUTS_H_
