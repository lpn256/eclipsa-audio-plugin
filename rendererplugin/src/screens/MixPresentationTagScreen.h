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

#include <components/components.h>

#include <functional>
#include <string>
#include <type_traits>
#include <unordered_map>

#include "components/src/ImageTextButton.h"
#include "components/src/SelectionBox.h"
#include "components/src/TitledTextBox.h"
#include "data_repository/implementation/MixPresentationRepository.h"

class MixPresentationTagScreen : public juce::Component,
                                 public juce::Button::Listener {
 public:
  MixPresentationTagScreen(MixPresentationRepository* mixPresentationRepository,
                           juce::Uuid initialMixPresentationId);
  ~MixPresentationTagScreen();

  void paint(juce::Graphics& g) override;

  void changeMixPresentation(const juce::Uuid mixPresentationId);

 private:
  void updateTagButtons();

  void drawTagButtons(juce::Rectangle<int>& bounds);

  void updateMapFromRepo();

  // responds to mix presentation tag being removed
  void buttonClicked(juce::Button* button) override;

  void configureLanguageDropDownBox();

  std::function<void()> languageChanged_;

  void languageChangedCallback();

  const int kmaxTags_ = 10;

  const int kMaxChars_ = 41;

  const std::string kContentLanguageTag_ = "Content Language";

  const std::string permittedChars =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-,<.>/"
      "?;'\"[{]}\\|`~!@#$%&*()+= ";

  MixPresentationRepository* mixPresentationRepository_;
  juce::Uuid currentMixPresId_;
  ImageTextButton addTagButton_;
  SelectionBox contentLanguageBox_;
  TitledTextBox tagNameBox_;
  TitledTextBox tagValueBox_;
  juce::Label existingTagsLabel_;
  std::map<std::string, std::string> existingTagsMap_;

  // an array of the existing tags assigned to the mix presentation
  juce::OwnedArray<ImageTextButton> tagButtons_;
};
