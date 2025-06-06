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
#include "AmbisonicsVisualizer.h"

class VisualizerPair : public juce::Component {
 private:
  AmbisonicsData* ambisonicsData_;

 public:
  VisualizerPair(AmbisonicsData* ambisonicsData,
                 const AmbisonicsVisualizer::VisualizerView& upperView,
                 const AmbisonicsVisualizer::VisualizerView& lowerView)
      : ambisonicsData_(ambisonicsData),
        upperVisualizer_(
            std::make_unique<AmbisonicsVisualizer>(ambisonicsData_, upperView)),
        lowerVisualizer_(std::make_unique<AmbisonicsVisualizer>(ambisonicsData_,
                                                                lowerView)) {
    addAndMakeVisible(upperVisualizer_.get());
    addAndMakeVisible(lowerVisualizer_.get());
  }

  ~VisualizerPair() { setLookAndFeel(nullptr); }

  void paint(juce::Graphics& g) override {
    auto bounds = getLocalBounds();
    const auto allBounds = bounds;
    auto upperBounds =
        bounds.removeFromTop(allBounds.proportionOfHeight(0.49f));
    auto lowerBounds =
        bounds.removeFromBottom(allBounds.proportionOfHeight(0.49f));

    upperVisualizer_->setBounds(upperBounds);
    lowerVisualizer_->setBounds(lowerBounds);
  }

  void paintOverChildren(juce::Graphics& g) override {
    g.setColour(EclipsaColours::tabTextGrey.withAlpha(0.3f));

    const float dashLength[] = {4.f, 4.f};
    const int size = sizeof(dashLength) / sizeof(dashLength[0]);
    juce::Point<int> start = upperVisualizer_->upperCirclePoint();
    juce::Point<int> end = upperVisualizer_->upperLabelPoint();
    g.drawDashedLine(juce::Line<float>(start.toFloat(), end.toFloat()),
                     dashLength, size, 1.f, 0);

    start = upperVisualizer_->lowerLabelPoint();
    end = lowerVisualizer_->upperLabelPoint();
    end.setY(end.getY() + upperVisualizer_->getHeight());
    g.drawDashedLine(juce::Line<float>(start.toFloat(), end.toFloat()),
                     dashLength, size, 1.f, 0);
  }
  std::unique_ptr<AmbisonicsVisualizer> upperVisualizer_;
  std::unique_ptr<AmbisonicsVisualizer> lowerVisualizer_;
};