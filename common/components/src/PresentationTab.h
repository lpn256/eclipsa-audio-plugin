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

#include "AEStripComponent.h"
#include "substream_rdr/substream_rdr_utils/Speakers.h"

class PresentationTab : public juce::Component,
                        public juce::ValueTree::Listener {
 public:
  PresentationTab(
      MixPresentation* mixPresentation, AudioElementRepository* aeRepository,
      MultiChannelRepository* multichannelGainRepo,
      ChannelMonitorProcessor* channelMonitorProcessor)  // temporary hard-code
      : audioElementRepository_(aeRepository),
        mixPresentation_(mixPresentation),
        multichannelGainRepo_(multichannelGainRepo),
        channelMonitorProcessor_(channelMonitorProcessor) {
    // get the audio elements for this tab
    getAudioElements();

    // Create the AE Strips
    createAEStrips();
  }

  ~PresentationTab() override { setLookAndFeel(nullptr); }
  void paint(juce::Graphics& g) override {
    // nothing to paint
    if (aeStrips_.size() < 1) {
      return;
    }
    auto bounds = getLocalBounds();

    g.setColour(juce::Colours::transparentWhite);
    // Assign 90% of the top to aeStripComponentBounds
    auto aeStripComponentBounds =
        bounds.removeFromTop(bounds.getHeight() * 0.9);
    // Remove the top 10% for additional clearance
    aeStripComponentBounds.removeFromTop(aeStripComponentBounds.getHeight() *
                                         0.1);

    auto newbounds = aeStripComponentBounds;
    // Remove 10% from the left & right
    newbounds.removeFromLeft(aeStripComponentBounds.getWidth() * 0.08);
    newbounds.removeFromRight(aeStripComponentBounds.getWidth() * 0.08);

    // Allocate bounds for the top AE Strip
    auto topBounds =
        newbounds.removeFromTop(aeStripComponentBounds.getHeight() *
                                0.35f);  // will allocate 70% of the height to
                                         // both AE Strips (35% for each)
    // Calculate what fraction of the width aeStrip1 should take in terms of
    // channel indicators Add an additional 2.5f to the channel count to account
    // for the S/M buttons and container

    float fraction =
        (aeStrips_[0]->getChannelCount() + 2.5f) /
        (static_cast<float>(Speakers::kHOA3.getNumChannels()) +
         2.5f);  // third order ambisonics will have the widest strip

    aeStrips_[0]->setBounds(
        topBounds.removeFromLeft(newbounds.getWidth() * fraction));

    aeStrips_[0]->getBounds();

    // no more strips to paint
    if (aeStrips_.size() < 2) {
      return;
    }

    fraction = (aeStrips_[1]->getChannelCount() + 2.5f) /
               (static_cast<float>(Speakers::kHOA3.getNumChannels()) + 2.5f);

    // Remove 30% of the remaining height so AE Strip 2 only uses up 35%
    newbounds.removeFromBottom(aeStripComponentBounds.getHeight() * 0.28f);
    newbounds.removeFromTop(aeStripComponentBounds.getHeight() *
                            0.02f);  // for clearance between the two AE Strips
    aeStrips_[1]->setBounds(
        newbounds.removeFromLeft(newbounds.getWidth() * fraction));
    aeStrips_[1]->getBounds();
  }

 private:
  void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                const juce::Identifier& property) override {
    // get the audio elements for this tab
    getAudioElements();

    // Create the AE Strips
    createAEStrips();

    // repaint
    repaint();
  }

  void valueTreeChildAdded(juce::ValueTree& parentTree,
                           juce::ValueTree& childWhichHasBeenAdded) override {
    // add the new audio element to the list
    // get the audio elements for this tab
    getAudioElements();

    // Create the AE Strips
    createAEStrips();

    // repaint
    repaint();
  }

  void valueTreeChildRemoved(juce::ValueTree& parentTree,
                             juce::ValueTree& childWhichHasBeenRemoved,
                             int indexFromWhichChildWasRemoved) override {
    // remove the audio element from the list
    // get the audio elements for this tab
    getAudioElements();

    // Create the AE Strips
    createAEStrips();

    // repaint
    repaint();
  }

  void valueTreeChildOrderChanged(
      juce::ValueTree& parentTreeWhoseChildrenHaveMoved, int oldIndex,
      int newIndex) override {
    // update the order of the audio elements
    // get the audio elements for this tab
    getAudioElements();

    // Create the AE Strips
    createAEStrips();

    // repaint
    repaint();
  }

  void valueTreeParentChanged(
      juce::ValueTree& treeWhoseParentHasChanged) override {
    // update the parent of the audio element
    // get the audio elements for this tab
    getAudioElements();

    // Create the AE Strips
    createAEStrips();

    // repaint
    repaint();
  }

  void valueTreeRedirected(juce::ValueTree& treeWhichHasBeenChanged) override {
    // update the audio element
    // get the audio elements for this tab
    getAudioElements();

    // Create the AE Strips
    createAEStrips();

    // repaint
    repaint();
  }

  void getAudioElements() {
    audioElementRepository_->getAll(allAudioElementsArray_);

    std::vector<MixPresentationAudioElement> mixpresentationAudioElements =
        mixPresentation_->getAudioElements();

    // allAudioElementsArray_ contais all the audioelements in the repository
    // ensure that the number of AE's in mix, is not larger than the number of
    // AE's in the entire AE repository
    if (allAudioElementsArray_.size() < mixpresentationAudioElements.size()) {
      return;
    }
    for (int i = 0; i < mixpresentationAudioElements.size(); i++) {
      juce::Uuid audioElementId = mixpresentationAudioElements[i].getId();
      audioElements_.push_back(getAudioElement(audioElementId));
    }
  }

  AudioElement getAudioElement(const juce::Uuid& Id) {
    for (auto audioElement : allAudioElementsArray_) {
      if (audioElement->getId() == Id) {
        return *audioElement;
      }
    }
  }

  void createAEStrips() {
    aeStrips_.clear();
    for (auto audioelement : audioElements_) {
      int channel_count = audioelement.getChannelCount();
      int first_channel = audioelement.getFirstChannel();
      juce::String label_text = audioelement.getChannelConfig().toString();
      aeStrips_.add(std::make_unique<AEStripComponent>(
          channel_count, label_text, first_channel, multichannelGainRepo_,
          channelMonitorProcessor_));
      addAndMakeVisible(aeStrips_.getLast());
    }
  }

  void paintAEStrips() {
    for (auto aestrip : aeStrips_) {
      aestrip->repaint();
    }
  }

  MixPresentation* mixPresentation_;
  AudioElementRepository* audioElementRepository_;
  juce::OwnedArray<AudioElement> allAudioElementsArray_;  // all audio elements
  std::vector<AudioElement>
      audioElements_;  // the audio elements that belong to this mix

  MultiChannelRepository* multichannelGainRepo_;
  ChannelMonitorProcessor* channelMonitorProcessor_;
  juce::OwnedArray<AEStripComponent> aeStrips_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresentationTab)
};
