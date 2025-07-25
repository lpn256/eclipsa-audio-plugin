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

#include "RendererEditor.h"

#include "components/src/DAWWarningBanner.h"
#include "components/src/EclipsaColours.h"
#include "screens/MonitorScreen.h"

CustomLookAndFeel::CustomLookAndFeel() {
  setColour(juce::ResizableWindow::backgroundColourId,
            EclipsaColours::backgroundOffBlack);
  setColour(juce::Label::textColourId, EclipsaColours::textWhite);
  setColour(juce::Label::backgroundColourId,
            findColour(juce::ResizableWindow::backgroundColourId));
  setColour(juce::TextButton::ColourIds::buttonColourId,
            EclipsaColours::backgroundOffBlack);
  setColour(juce::TextButton::ColourIds::buttonOnColourId,
            EclipsaColours::buttonRolloverColour);
  setColour(juce::TextButton::textColourOffId, EclipsaColours::textWhite);
  setColour(juce::TextButton::ColourIds::textColourOnId,
            EclipsaColours::buttonRolloverTextColour);
}

void CustomLookAndFeel::drawButtonBackground(
    juce::Graphics& g, juce::Button& button,
    const juce::Colour& backgroundColour, bool isMouseOverButton,
    bool isButtonDown) {
  auto buttonArea = button.getLocalBounds();

  juce::Colour backColour;
  if (isMouseOverButton) {
    backColour = findColour(juce::TextButton::buttonOnColourId);
  } else {
    backColour = findColour(juce::TextButton::buttonColourId);
  }

  if (isButtonDown) {
    // Darken the background colour
    backColour = backColour.darker(0.5f);
  }

  g.setColour(backColour);
  float cornerSize = buttonArea.getHeight() / 2.0f;
  g.fillRoundedRectangle(buttonArea.toFloat(), cornerSize);
  g.setColour(juce::Colour(136, 147, 146));
  g.drawRoundedRectangle(buttonArea.toFloat(), cornerSize, 2.0f);
}

//==============================================================================

RendererEditor::RendererEditor(RendererProcessor& p)
    : MainEditor(p),
      dawWarningBanner_(&p.getRoomSetupRepository()),
      monitorScreen_(p.getRepositories(), p.getSpeakerMonitorData(),
                     p.getChannelMonitorData(), *this,
                     p.getMainBusNumInputChannels()),
      currentScreen_(&monitorScreen_) {
  setResizable(true, true);

  // Get screen dimensions and calculate appropriate size
  auto displays = juce::Desktop::getInstance().getDisplays();
  auto mainDisplay = displays.getPrimaryDisplay();

  if (mainDisplay != nullptr) {
    auto screenArea = mainDisplay->userArea;

    int maxWidth =
        static_cast<int>(screenArea.getWidth() * 1.0f);  // 100% of screen width
    int maxHeight = static_cast<int>(screenArea.getHeight() *
                                     1.0f);  // 100% of screen height

    // Preferred size but constrained by screen
    int preferredWidth = 1600;
    int preferredHeight = 752;

    int actualWidth = juce::jmin(preferredWidth, maxWidth);
    int actualHeight = juce::jmin(preferredHeight, maxHeight);

    // Round to even numbers to avoid rendering artifacts
    actualWidth = (actualWidth + 1) & ~1;    // Round up to even number
    actualHeight = (actualHeight + 1) & ~1;  // Round up to even number

    setSize(actualWidth, actualHeight);

    // Set resize limits that force even dimensions to prevent artifacts
    int minWidth =
        juce::jmin(800, static_cast<int>(screenArea.getWidth() * 0.5f));
    int minHeight =
        juce::jmin(500, static_cast<int>(screenArea.getHeight() * 0.35f));

    // Ensure all resize limits are even numbers
    minWidth = (minWidth + 1) & ~1;
    minHeight = (minHeight + 1) & ~1;
    maxWidth = (maxWidth + 1) & ~1;
    maxHeight = (maxHeight + 1) & ~1;

    setResizeLimits(minWidth, minHeight, maxWidth, maxHeight);
  } else {
    // Fallback if screen detection fails
    setSize(1200, 650);
    setResizeLimits(800, 500, 1600, 900);
  }

  // Set up the look and feel information here
  setLookAndFeel(&customLookAndFeel_);

  // Add the DAW warning banner and let it determine its own visibility.
  addChildComponent(dawWarningBanner_);
  dawWarningBanner_.refreshVisibility();
}

RendererEditor::~RendererEditor() { setLookAndFeel(nullptr); }

void RendererEditor::paint(juce::Graphics& g) {
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

  // First, create some padding around our widgets
  auto bounds = getLocalBounds();
  bounds.removeFromTop(20);
  bounds.removeFromBottom(20);
  bounds.removeFromLeft(40);
  bounds.removeFromRight(40);

  // Add the title label
  addAndMakeVisible(titleLabel_);
  titleLabel_.setText("Eclipsa Audio Renderer", juce::dontSendNotification);
  titleLabel_.setFont(juce::Font("Audiowide", 30.0f, juce::Font::plain));
  titleLabel_.setBounds(bounds.removeFromTop(40));

  // Add some spacing between title and warning banner (if shown)
  bounds.removeFromTop(5);

  // Position the DAW warning banner above the separator line
  // It's already added as a child in the constructor and its visibility set.
  if (dawWarningBanner_.isVisible()) {
    dawWarningBanner_.updatePosition(titleLabel_.getBottom() + 5, getWidth());
    bounds.removeFromTop(35);
  }

  // Add some spacing before the separator line
  bounds.removeFromTop(5);

  // Add the title separator line
  auto separatorBounds = bounds.removeFromTop(2);
  juce::Colour gradientWhite =
      getLookAndFeel().findColour(juce::Label::textColourId);
  juce::Colour gradientBrown(140, 78, 41);
  g.setGradientFill(juce::ColourGradient(gradientWhite, separatorBounds.getX(),
                                         separatorBounds.getY(), gradientBrown,
                                         separatorBounds.getWidth(),
                                         separatorBounds.getY(), false));
  g.fillRect(separatorBounds);

  bounds.removeFromTop(20);  // Add padding under the banner/separator

  // Now draw in the current screen
  addAndMakeVisible(currentScreen_);
  currentScreen_->setBounds(bounds);
}

void RendererEditor::resized() {
  // Snap to even dimensions to prevent rendering artifacts
  auto currentBounds = getBounds();
  int width = currentBounds.getWidth();
  int height = currentBounds.getHeight();

  // Round to even numbers
  int evenWidth = (width + 1) & ~1;
  int evenHeight = (height + 1) & ~1;

  // Only update if dimensions changed
  if (width != evenWidth || height != evenHeight) {
    setBounds(currentBounds.getX(), currentBounds.getY(), evenWidth,
              evenHeight);
    return;  // Avoid infinite recursion
  }

  // Continue with normal layout logic
  repaint();
}

void RendererEditor::setScreen(juce::Component& screen) {
  removeAllChildren();
  addAndMakeVisible(titleLabel_);
  addChildComponent(dawWarningBanner_);

  dawWarningBanner_.refreshVisibility();

  currentScreen_ = &screen;
  addAndMakeVisible(currentScreen_);
  repaint();
}

void RendererEditor::resetScreen() {
  removeAllChildren();
  addAndMakeVisible(titleLabel_);
  addChildComponent(dawWarningBanner_);

  // Refresh visibility to ensure the banner state is correct
  dawWarningBanner_.refreshVisibility();

  currentScreen_ = &monitorScreen_;
  addAndMakeVisible(currentScreen_);
  repaint();
}
