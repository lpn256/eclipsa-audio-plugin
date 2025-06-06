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

#include "MixPresentationLoudness.h"

#include "juce_core/system/juce_PlatformDefs.h"
#include "substream_rdr/substream_rdr_utils/Speakers.h"

MixPresentationLoudness::MixPresentationLoudness()
    : RepositoryItemBase(juce::Uuid::null()),
      layouts_(
          {LayoutLoudness(Speakers::kStereo), LayoutLoudness(Speakers::kMono)}),
      largestLayout_(Speakers::kStereo) {}

MixPresentationLoudness::MixPresentationLoudness(
    juce::Uuid id, Speakers::AudioElementSpeakerLayout largestLayout)
    : RepositoryItemBase(id), largestLayout_(largestLayout) {
  layouts_[0] = LayoutLoudness(Speakers::kStereo);
  replaceLargestLayout(largestLayout);
}

void MixPresentationLoudness::replaceLargestLayout(
    const Speakers::AudioElementSpeakerLayout layout,
    const float integratedLoudness, const float digitalPeak,
    const float truePeak) {
  // ensure that the layout is not ambisonics
  // and not binaural
  jassert(!layout.isAmbisonics());
  jassert(layout != Speakers::kBinaural);

  // if stereo or mono was passed
  // ensure the 2nd index is set to mono
  // ensure that the largest layout is set to stereo
  if (layout == Speakers::kMono || layout == Speakers::kStereo) {
    largestLayout_ = Speakers::kStereo;
    layouts_[1] = LayoutLoudness(Speakers::kMono);
  } else if (!layout.isExpandedLayout() &&
             (layout != Speakers::kMono ||
              layout != Speakers::kStereo)) {  // if the layout is not expanded
    largestLayout_ = layout;
    layouts_[1] =
        LayoutLoudness(layout, integratedLoudness, digitalPeak, truePeak);
  } else {  // handle expanded layouts
    largestLayout_ = layout.getExplBaseLayout();
    layouts_[1] = LayoutLoudness(largestLayout_);
  }
}

void MixPresentationLoudness::setLayoutIntegratedLoudness(
    const Speakers::AudioElementSpeakerLayout& layout,
    const float integratedLoudness) {
  // if the layout is not included in the layouts, do nothing
  if (layout == Speakers::kStereo) {
    layouts_[0].setIntegratedLoudness(integratedLoudness);
  } else if (layout != Speakers::kStereo && layout == largestLayout_) {
    layouts_[1].setIntegratedLoudness(integratedLoudness);
  }
  return;
}

void MixPresentationLoudness::setLayoutDigitalPeak(
    const Speakers::AudioElementSpeakerLayout& layout,
    const float digitalPeak) {
  // if the layout is not included in the layouts, do nothing
  if (layout == Speakers::kStereo) {
    layouts_[0].setDigitalPeak(digitalPeak);
  } else if (layout != Speakers::kStereo && layout == largestLayout_) {
    layouts_[1].setDigitalPeak(digitalPeak);
  }
  return;
}

void MixPresentationLoudness::setLayoutTruePeak(
    const Speakers::AudioElementSpeakerLayout& layout, const float truePeak) {
  // if the layout is not included in the layouts, do nothing
  if (layout == Speakers::kStereo) {
    layouts_[0].setTruePeak(truePeak);
  } else if (layout != Speakers::kStereo && layout == largestLayout_) {
    layouts_[1].setTruePeak(truePeak);
  }
  return;
}

MixPresentationLoudness MixPresentationLoudness::fromTree(
    const juce::ValueTree tree) {
  jassert(tree.hasProperty(kId));
  MixPresentationLoudness mixPres(
      juce::Uuid(tree[kId]),
      static_cast<Speakers::AudioElementSpeakerLayout>(tree[kLargestLayout]));

  juce::ValueTree layoutsTree = tree.getChildWithName(kLayouts);
  for (int i = 0; i < 2; i++) {
    mixPres.layouts_[i] = LayoutLoudness::fromTree(layoutsTree.getChild(i));
  }
  return mixPres;
}

juce::ValueTree MixPresentationLoudness::toValueTree() const {
  juce::ValueTree tree(kTreeType,
                       {{kId, getId().toString()},
                        {kLargestLayout, static_cast<int>(largestLayout_)}});

  juce::ValueTree layoutsTree =
      tree.getOrCreateChildWithName(kLayouts, nullptr);

  for (const auto& layoutLoudness : layouts_) {
    layoutsTree.appendChild(layoutLoudness.toValueTree(), nullptr);
  }
  tree.appendChild(layoutsTree, nullptr);

  return tree;
}

bool MixPresentationLoudness::operator==(
    const MixPresentationLoudness& other) const {
  if (other.id_ != id_ || other.layouts_ != layouts_ ||
      other.largestLayout_ != largestLayout_) {
    return false;
  }
  for (int i = 0; i < 2; i++) {
    if (layouts_[i] != other.layouts_[i]) {
      return false;
    }
  }
  return true;
}

float MixPresentationLoudness::getLayoutIntegratedLoudness(
    const Speakers::AudioElementSpeakerLayout& layout) const {
  if (layout == Speakers::kStereo) {
    return layouts_[0].getIntegratedLoudness();
  } else if (layout == largestLayout_) {
    return layouts_[1].getIntegratedLoudness();
  }
  return 0.0f;
}

float MixPresentationLoudness::getLayoutDigitalPeak(
    const Speakers::AudioElementSpeakerLayout& layout) const {
  if (layout == Speakers::kStereo) {
    return layouts_[0].getDigitalPeak();
  } else if (layout == largestLayout_) {
    return layouts_[1].getDigitalPeak();
  }
  return 0.0f;
}

float MixPresentationLoudness::getLayoutTruePeak(
    const Speakers::AudioElementSpeakerLayout& layout) const {
  if (layout == Speakers::kStereo) {
    return layouts_[0].getTruePeak();
  } else if (layout == largestLayout_) {
    return layouts_[1].getTruePeak();
  }
  return 0.0f;
}