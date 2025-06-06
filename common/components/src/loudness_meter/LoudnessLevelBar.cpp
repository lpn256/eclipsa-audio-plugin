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

#include "LoudnessLevelBar.h"

LoudnessLevelBar::LoudnessLevelBar() { startTimerHz(kRefreshRate_); }

void LoudnessLevelBar::paint(juce::Graphics& g) {
  // Centre-justify and fill level bar with grey background.
  auto bounds = getLocalBounds();
  int width = bounds.getWidth();
  bounds.removeFromLeft(width * 0.3f);
  bounds.removeFromRight(width * 0.3f);
  barWidth_ = bounds.getWidth();
  g.setColour(kGray_);
  g.fillRect(bounds);

  // Calculate bounds for coloured sections.
  auto greenBounds = bounds.removeFromBottom(bounds.getHeight() * 0.5f);
  auto yellowBounds = bounds.removeFromBottom(bounds.getHeight() * 0.7f);
  auto orangeBounds = bounds.removeFromBottom(bounds.getHeight() * 0.9f);
  auto redBounds = bounds;

  // Invalid loudnesses should not be drawn.
  int level = 60;
  if (isValidLoudness(loudness_)) {
    level = std::abs(loudness_);
  }

  updateResidualPeak(level);

  // Fill coloured regions depending on loudness level.
  g.setColour(kGreen_);
  level = fillBar(level, {kGreenStart_, kGreenEnd_}, greenBounds, g);

  g.setColour(kYellow_);
  level = fillBar(level, {kGreenEnd_, kYellowEnd_}, yellowBounds, g);

  g.setColour(kOrange_);
  level = fillBar(level, {kYellowEnd_, kOrangeEnd_}, orangeBounds, g);

  g.setColour(kRed_);
  level = fillBar(level, {kOrangeEnd_, kRedEnd_}, redBounds, g);
}

void LoudnessLevelBar::updateResidualPeak(const int level) {
  // Update peak level and reset decay counter.
  if (level < resPeak_.level_) {
    resPeak_.level_ = level;
    resPeak_.counterToDecay_ = kRefreshRate_ * kDecayPeriod_;
  } else {
    --resPeak_.counterToDecay_;
    // When counter expired begin decaying level.
    if (resPeak_.counterToDecay_ <= 0) {
      resPeak_.level_ += 1;
    }
  }
}

void LoudnessLevelBar::drawResidualPeak(const std::pair<int, int> range,
                                        juce::Rectangle<int>& bounds,
                                        juce::Graphics& g) {
  if (resPeak_.level_ <= range.first && resPeak_.level_ >= range.second) {
    float barHeightPct = static_cast<float>(range.first - resPeak_.level_) /
                         (range.first - range.second);
    int barHeight = static_cast<int>(barHeightPct * bounds.getHeight());
    int yPos = bounds.getHeight() - barHeight;
    g.fillRect(bounds.getX(), bounds.getY() + yPos, bounds.getWidth(), 2);
  }
}

int LoudnessLevelBar::fillBar(const int level, const std::pair<int, int> range,
                              juce::Rectangle<int>& bounds, juce::Graphics& g) {
  // Attempt to draw residual peak.
  drawResidualPeak(range, bounds, g);
  // Nothing left to draw.
  if (level == -1) {
    return level;
  }
  // Draw the portion of the loudness level that falls within range.
  else {
    // Fill amount = min(amount that falls within range, total range).
    int totalRge = range.first - range.second;
    float rgeToFill = std::min(range.first - level, totalRge);
    // Fraction of bar to fill = BarHeight * rgeToFill / totalRge.
    g.fillRect(
        bounds.removeFromBottom(bounds.getHeight() * (rgeToFill / totalRge)));
    // Update remaining level to draw.
    return rgeToFill < totalRge ? -1 : level;
  }
}

void LoudnessLevelBar::timerCallback() { repaint(); }