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

#include "GainEditor.h"

#include "GainProcessor.h"

//==============================================================================
GainProcessorEditor::GainProcessorEditor(
    GainProcessor& p, juce::AudioProcessorValueTreeState& state)
    : AudioProcessorEditor(&p) {
  // Make sure that before the constructor has finished, you've set the
  // editor's size to whatever you need it to be.
  setSize(1, 1);
}

GainProcessorEditor::~GainProcessorEditor() {
  processor.editorBeingDeleted(this);
}

//==============================================================================
void GainProcessorEditor::paint(juce::Graphics& g) {
  g.fillAll(
      getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void GainProcessorEditor::resized() {
  auto area = getBounds();

  area.removeFromTop(editorMargin_);
  area.removeFromLeft(editorMargin_);
  area.removeFromRight(editorMargin_);
}
