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

#include <memory>

#include "components/components.h"
#include "components/src/AEContainerSet.h"
#include "components/src/MixAEContainer.h"
#include "data_structures/src/MixPresentation.h"

class PresentationEditorViewPort : public juce::Component {
 public:
  PresentationEditorViewPort(
      std::map<juce::Uuid, std::unique_ptr<MixAEContainer>>* containers);
  ~PresentationEditorViewPort() {};

  void paint(juce::Graphics& g) override;

  int getRequiredHeight() { return set_.calculateContainerHeight(); }

  const int kMaxHeight = 260;

 private:
  MixPresentation* mixPresentation_;
  juce::Viewport viewPort_;
  AEContainerSet set_;
};