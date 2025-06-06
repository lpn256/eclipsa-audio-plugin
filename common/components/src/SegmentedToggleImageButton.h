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
#include <juce_gui_basics/juce_gui_basics.h>

#include "components/src/EclipsaColours.h"
#include "components/src/Icons.h"

class STIBLookAndFeel : public juce::LookAndFeel_V4 {
 public:
  STIBLookAndFeel();

  void drawImageButton(juce::Graphics& g, juce::Image* image, int imageX,
                       int imageY, int imageW, int imageH,
                       const juce::Colour& overlayColour, float imageOpacity,
                       juce::ImageButton& btn) override;
};

class SegmentedToggleImageButton : public juce::Component {
 public:
  SegmentedToggleImageButton(const std::initializer_list<juce::Image>& opts,
                             const bool singularToggle);

  ~SegmentedToggleImageButton() { setLookAndFeel(nullptr); }

  void paint(juce::Graphics& g);

  std::vector<std::pair<juce::String, bool>> getState();
  void setToggled(int idx) {
    // Catch invalid index/enum selection case.
    if (idx < 0 || idx >= buttons_.size()) {
      return;
    }
    buttons_[idx]->setToggleState(true, juce::sendNotification);
  }
  int getToggled();

  void onChange(std::function<void()> func) { parentCallback_ = func; }

 private:
  void configureButtons();
  // Toggle button and alert listener.
  void toggleButton(juce::Button* btn);

  const juce::Image kCheckImg_ = IconStore::getInstance().getCheckmarkIcon();
  const bool kSingularToggle_ = false;
  std::vector<std::unique_ptr<juce::ImageButton>> buttons_;
  std::function<void()> parentCallback_;
  STIBLookAndFeel lookAndFeel_;
};