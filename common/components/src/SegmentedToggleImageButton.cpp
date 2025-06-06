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

#include "SegmentedToggleImageButton.h"

STIBLookAndFeel::STIBLookAndFeel() {}

void STIBLookAndFeel::drawImageButton(juce::Graphics& g, juce::Image* image,
                                      int imageX, int imageY, int imageW,
                                      int imageH,
                                      const juce::Colour& overlayColour,
                                      float imageOpacity,
                                      juce::ImageButton& btn) {
  auto bounds = btn.getLocalBounds();
  // If the button is toggled on, shift text right.
  int offset = 0;
  if (btn.getToggleState()) {
    offset = 10;
  }
  // assign the background colour based on the button state
  juce::Colour backColour;
  if (btn.isMouseOver() || btn.getToggleState()) {
    backColour = EclipsaColours::onButtonGrey;
  } else {
    backColour = juce::Colours::transparentWhite;
  }

  auto imageRect = juce::Rectangle<float>(bounds.getCentreX() + offset - 10,
                                          bounds.getCentreY() - 8, 16, 16);
  g.drawImage(*image, imageRect);

  // Draw button as rounded if necessary.
  const float kCornerSize = bounds.getHeight() / 2.f;
  const float kStroke = 1.5f;
  // These two constants are to attempt to circumvent JUCE anti-aliasing alg.
  const float kYOffset = 0.24f;
  const float kHOffset = -0.54f;
  bool leftmost = !btn.isConnectedOnLeft();
  bool rightmost = !btn.isConnectedOnRight();
  if (rightmost) {
    juce::Path path;
    path.addRoundedRectangle(bounds.getX() - 1, bounds.getY() + kYOffset,
                             bounds.getWidth(), bounds.getHeight() + kHOffset,
                             kCornerSize, kCornerSize, false, true, false,
                             true);
    // fill the path
    g.setColour(backColour);
    g.fillPath(path);
    // draw the border
    g.setColour(EclipsaColours::selectionToggleBorderGrey);
    g.strokePath(path, juce::PathStrokeType(kStroke));
  } else if (leftmost) {
    juce::Path path;
    path.addRoundedRectangle(bounds.getX() + 1, bounds.getY() + kYOffset,
                             bounds.getWidth(), bounds.getHeight() + kHOffset,
                             kCornerSize, kCornerSize, true, false, true,
                             false);
    // fill the path
    g.setColour(backColour);
    g.fillPath(path);
    // draw the border
    g.setColour(EclipsaColours::selectionToggleBorderGrey);
    g.strokePath(path, juce::PathStrokeType(kStroke));
  } else {
    // draw the border
    g.setColour(backColour);
    g.fillRect(bounds);
    // fill the path
    g.setColour(EclipsaColours::selectionToggleBorderGrey);
    g.drawRect(bounds, 1.0f);
  }

  // redraw the image to ensure it appears
  g.drawImage(*image, imageRect);

  // If the button is toggled on draw a checkmark.
  if (btn.getToggleState()) {
    g.drawImageWithin(
        IconStore::getInstance().getCheckmarkIcon(), bounds.getX() + 10,
        bounds.getY(), bounds.getWidth(), bounds.getHeight(),
        juce::RectanglePlacement::xLeft | juce::RectanglePlacement::yMid |
            juce::RectanglePlacement::onlyReduceInSize);
  }
}

SegmentedToggleImageButton::SegmentedToggleImageButton(
    const std::initializer_list<juce::Image>& opts, const bool singularToggle)
    : kSingularToggle_(singularToggle) {
  for (const juce::Image& opt : opts) {
    auto button = std::make_unique<juce::ImageButton>();
    button->setImages(
        true, true, true, opt, 1.0f, juce::Colours::transparentBlack, opt, 0.5f,
        juce::Colours::grey, opt, 0.8f, EclipsaColours::iconWhite);
    buttons_.emplace_back(std::move(button));
  }
  configureButtons();
  setLookAndFeel(&lookAndFeel_);
}

void SegmentedToggleImageButton::paint(juce::Graphics& g) {
  auto bounds = getLocalBounds();
  // Draw buttons.
  int buttonWidth = bounds.getWidth() / buttons_.size();
  for (const auto& button : buttons_) {
    button->setBounds(bounds.removeFromLeft(buttonWidth));
  }
}

int SegmentedToggleImageButton::getToggled() {
  for (int i = 0; i < buttons_.size(); i++) {
    if (buttons_[i]->getToggleState()) {
      return i;
    }
  }
  // returns -1 when nothing is selection
  return -1;
}

std::vector<std::pair<juce::String, bool>>
SegmentedToggleImageButton::getState() {
  std::vector<std::pair<juce::String, bool>> state;
  for (const auto& button : buttons_) {
    state.push_back({button->getButtonText(), button->getToggleState()});
  }
  return state;
}

void SegmentedToggleImageButton::configureButtons() {
  for (const auto& button : buttons_) {
    button->setClickingTogglesState(true);
    button->onClick = [this, &button] { toggleButton(button.get()); };

    // Explicitly record which button edges are connected.
    if (button == buttons_.front()) {
      button->setConnectedEdges(
          juce::Button::ConnectedEdgeFlags::ConnectedOnRight);
    } else if (button == buttons_.back()) {
      button->setConnectedEdges(
          juce::Button::ConnectedEdgeFlags::ConnectedOnLeft);
    } else {
      button->setConnectedEdges(
          juce::Button::ConnectedEdgeFlags::ConnectedOnLeft |
          juce::Button::ConnectedEdgeFlags::ConnectedOnRight);
    }
    addAndMakeVisible(button.get());
  }
}

void SegmentedToggleImageButton::toggleButton(juce::Button* btn) {
  // Set toggle states if singular toggle.
  if (kSingularToggle_ && btn->getToggleState()) {
    for (const auto& button : buttons_) {
      bool toggleState = button.get() == btn ? true : false;
      button->setToggleState(toggleState, true);
    }
  }
  parentCallback_();
}