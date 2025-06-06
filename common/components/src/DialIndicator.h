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

#include <juce_gui_basics/juce_gui_basics.h>

#include "EclipsaColours.h"
#include "data_repository/implementation/MixPresentationRepository.h"

enum class DialColourIds {
  dialFill = 0,
  dialOutline = 1,
  blueArc = 2,
};

class DialIndicatorLookAndFeel : public juce::LookAndFeel_V4 {
 public:
  DialIndicatorLookAndFeel() {
    setColour((int)DialColourIds::dialFill, EclipsaColours::inactiveGrey);
    setColour((int)DialColourIds::dialOutline, EclipsaColours::headingGrey);
    setColour((int)DialColourIds::blueArc, EclipsaColours::controlBlue);
  }
};

class DimmedDialIndicatorLookAndFeel : public juce::LookAndFeel_V4 {
 public:
  DimmedDialIndicatorLookAndFeel() {
    float alpha = 0.4f;
    setColour((int)DialColourIds::dialFill,
              EclipsaColours::inactiveGrey.withAlpha(alpha));
    setColour((int)DialColourIds::dialOutline,
              EclipsaColours::headingGrey.withAlpha(alpha));
    setColour((int)DialColourIds::blueArc,
              EclipsaColours::controlBlue.withAlpha(alpha));
  }
};

class DialIndicator : public juce::Component {
 public:
  DialIndicator(const int& value, const int& min, const int& max,
                bool centered = true)
      : centered_(centered), value_(value), min_(min), max_(max) {
    setLookAndFeel(&lookAndFeel_);
  }
  ~DialIndicator() { setLookAndFeel(nullptr); }

  void dimLookAndFeel() { setLookAndFeel(&dimmedLookAndFeel_); }

  void resetLookAndFeel() { setLookAndFeel(&lookAndFeel_); }

  float getDialAngleCentered(
      const int& value) {  // this assume 12 O'Clock is 0 Rad
    float sensitivity;
    if (value >= 0) {
      sensitivity = (juce::MathConstants<float>::pi - startAngle_) /
                    max_;  // max dB returns 7*pi/8
    } else {
      sensitivity = -(juce::MathConstants<float>::pi - startAngle_) /
                    min_;  //-100dB returns -7*pi/8
    }
    return sensitivity * value_;
  }

  float getDialAngleOffcentered(
      const int& value) {  // this assume 12 O'Clock is 0 Rad

    float average = 0.5f * (max_ + min_);
    float sensitivity;

    // must draw the arm on the right side of the dial
    if (value >= average) {
      sensitivity = (juce::MathConstants<float>::pi - startAngle_) /
                    (max_ - average);  // max value returns 7*pi/8
      return sensitivity *
             (value_ - average);  // the average returns an angle of 0
    } else {
      float zero_pos = -(juce::MathConstants<float>::pi - startAngle_);
      sensitivity = -zero_pos / (average - min_);  // min value returns -7*pi/8
      return (sensitivity * (value_ - min_)) +
             zero_pos;  // the average returns an angle of 0
    }
  }

  std::pair<int, int> getDialXY(const float& angle) {
    // ensure dial does not intersect the arc
    float multiple = 0.8f;
    // Calculate the point on the arc
    float xOnArc = centerX_ + (multiple * radius_ *
                               std::sin(angle));  // when angle < 0 x < centreX_
    float yOnArc =
        centerY_ - (multiple * radius_ *
                    std::cos(angle));  // when angle > pi/2, y < centreY_
    return {xOnArc, yOnArc};
  }

  void paint(juce::Graphics& g) override {
    auto bounds = getLocalBounds().toFloat();

    radius_ = 0.95f * bounds.getWidth() / 2.0f;
    centerX_ = bounds.getCentreX();
    centerY_ = bounds.getCentreY();

    auto lineThickness = 2.0f;

    g.setColour(findColour((int)DialColourIds::dialFill));
    g.fillEllipse(bounds);

    // Define the path for the arc
    juce::Path arcPath;
    arcPath.addCentredArc(centerX_, centerY_, radius_, radius_,
                          juce::MathConstants<float>::pi, startAngle_,
                          endAngle_, true);

    g.setColour(findColour((int)DialColourIds::dialOutline));
    g.strokePath(arcPath, juce::PathStrokeType(lineThickness,
                                               juce::PathStrokeType::curved,
                                               juce::PathStrokeType::rounded));

    // Draw the dial arm
    float dialAngle;
    if (centered_) {
      dialAngle = getDialAngleCentered(value_);
    } else {
      dialAngle = getDialAngleOffcentered(value_);
    }

    std::pair<int, int> dialXY = getDialXY(dialAngle);
    g.drawLine(centerX_, centerY_, dialXY.first, dialXY.second, lineThickness);

    // Draw the blue arc
    g.setColour(findColour((int)DialColourIds::blueArc));
    juce::Path blueArcPath;
    if (centered_) {
      blueArcPath.addCentredArc(centerX_, centerY_, radius_, radius_, 0.f, 0.f,
                                dialAngle, true);
    } else {
      blueArcPath.addCentredArc(centerX_, centerY_, radius_, radius_, 0,
                                -7.f * startAngle_, dialAngle, true);
    }

    g.strokePath(blueArcPath,
                 juce::PathStrokeType(lineThickness * 1.75f,
                                      juce::PathStrokeType::curved,
                                      juce::PathStrokeType::rounded));
  }

  void setDialValue(const int value) {
    if (value < min_) {
      value_ = min_;
    } else if (value > max_) {
      value_ = max_;
    } else {
      value_ = value;
    }

    repaint();
  }

  juce::String mixName_;

 private:
  DialIndicatorLookAndFeel lookAndFeel_;
  DimmedDialIndicatorLookAndFeel dimmedLookAndFeel_;
  bool centered_;
  int value_;
  int min_;
  int max_;
  const float startAngle_ = juce::MathConstants<float>::pi / 8.f;
  const float endAngle_ = 15.f * juce::MathConstants<float>::pi / 8.f;
  float radius_;
  float centerX_;
  float centerY_;
};