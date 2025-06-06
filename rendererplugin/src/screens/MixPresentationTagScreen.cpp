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

#include "MixPresentationTagScreen.h"

#include <string>
#include <unordered_map>

#include "../rendererplugin/src/RendererProcessor.h"
#include "components/src/EclipsaColours.h"
#include "components/src/ImageTextButton.h"
#include "data_repository/implementation/MixPresentationRepository.h"
#include "data_structures/src/MixPresentation.h"
#include "logger/logger.h"

MixPresentationTagScreen::MixPresentationTagScreen(
    MixPresentationRepository* mixPresentationRepository,
    juce::Uuid initialMixPresentationId)
    : mixPresentationRepository_(mixPresentationRepository),
      currentMixPresId_(initialMixPresentationId),
      addTagButton_(IconStore::getInstance().getPlusIcon()),
      contentLanguageBox_("Content Language"),
      languageChanged_([this]() { languageChangedCallback(); }),
      tagNameBox_("Name"),
      tagValueBox_("Value") {
  addAndMakeVisible(addTagButton_);
  addTagButton_.setCyanLookAndFeel();
  addTagButton_.setButtonText("Add Tag");
  addTagButton_.setButtonOnClick([this]() {
    // do nothing if the name field is empty
    // do nothing if the name already exists in the map
    // do nothing if the map is full
    // do nothing if the value field is empty
    if (tagNameBox_.getText().isEmpty() ||
        existingTagsMap_.size() >= kmaxTags_ ||
        tagValueBox_.getText().isEmpty()) {
      return;
    }
    // do nothing if the mixPresentation does not exist
    if (!mixPresentationRepository_->get(currentMixPresId_).has_value()) {
      LOG_ERROR(RendererProcessor::instanceId_,
                "addTagButton.onClicked: "
                "MixPresentation not found in "
                "repository");
      return;
    }
    MixPresentation mixPres =
        mixPresentationRepository_->get(currentMixPresId_).value();

    std::string tagName = tagNameBox_.getText().toStdString();
    std::string tagValue = tagValueBox_.getText().toStdString();

    tagNameBox_.setText("");
    tagValueBox_.setText("");
    mixPres.addTagPair(tagName, tagValue);
    mixPresentationRepository_->update(mixPres);
    updateTagButtons();
    repaint();
  });
  addAndMakeVisible(contentLanguageBox_);
  configureLanguageDropDownBox();
  addAndMakeVisible(tagNameBox_);
  tagNameBox_.setInputRestrictions(kMaxChars_, permittedChars);
  addAndMakeVisible(tagValueBox_);
  tagValueBox_.setInputRestrictions(kMaxChars_, permittedChars);
  addAndMakeVisible(existingTagsLabel_);
  existingTagsLabel_.setText("Existing Tags", juce::dontSendNotification);
  existingTagsLabel_.setFont(juce::Font(18.0f));
  existingTagsLabel_.setColour(juce::Label::textColourId,
                               EclipsaColours::headingGrey);
}

MixPresentationTagScreen::~MixPresentationTagScreen() {
  setLookAndFeel(nullptr);
}
void MixPresentationTagScreen::paint(juce::Graphics& g) {
  // create padding at the top
  // store as a reference
  const auto screenBounds =
      getLocalBounds()
          .removeFromBottom(getLocalBounds().proportionOfHeight(0.95f))
          .removeFromLeft(getLocalBounds().proportionOfWidth(0.99f));
  auto bounds = screenBounds;

  const float topPortion = 0.45f;
  // top half of the screen
  // store for reference
  const auto inputBoundsRef =
      bounds.removeFromTop(screenBounds.proportionOfHeight(topPortion));

  auto inputBounds = inputBoundsRef;
  auto languageDropDownBounds =
      inputBounds.removeFromTop(inputBoundsRef.proportionOfHeight(0.333f));

  const float fieldHeight = 0.12f;
  const float fieldWidth = 0.49f;

  languageDropDownBounds.removeFromRight(
      inputBoundsRef.proportionOfWidth(1 - fieldWidth));
  languageDropDownBounds.setHeight(
      screenBounds.proportionOfHeight(fieldHeight));
  contentLanguageBox_.setBounds(languageDropDownBounds);

  // calculate bounds for the input fields
  // assign bounds for the input fields
  auto textFieldBounds =
      inputBounds.removeFromTop(screenBounds.proportionOfHeight(fieldHeight));

  tagNameBox_.setBounds(textFieldBounds.removeFromLeft(
      inputBoundsRef.proportionOfWidth(fieldWidth)));

  textFieldBounds.removeFromLeft(
      inputBoundsRef.proportionOfWidth(1 - (2 * fieldWidth)));

  tagValueBox_.setBounds(textFieldBounds.removeFromLeft(
      inputBoundsRef.proportionOfWidth(fieldWidth)));

  // calculate and assign bounds for the add tag button
  inputBounds.reduce(inputBoundsRef.proportionOfWidth(0.3f),
                     inputBoundsRef.proportionOfHeight(0.125f));
  addTagButton_.setBounds(inputBounds);

  // bottom half of the screen
  const auto existingTagBounds = bounds;
  auto existingTagsLabelBounds =
      bounds.removeFromTop(existingTagBounds.proportionOfHeight(0.1f));

  existingTagsLabel_.setBounds(existingTagsLabelBounds);

  const int verticalPadding = screenBounds.proportionOfHeight(0.01f);

  drawTagButtons(bounds);
}

void MixPresentationTagScreen::updateTagButtons() {
  std::optional<MixPresentation> mixPresOpt =
      mixPresentationRepository_->get(currentMixPresId_);
  if (!mixPresOpt.has_value()) {
    LOG_ERROR(RendererProcessor::instanceId_,
              "MixPresentationTagScreen::updateTagButtons: "
              "MixPresentation not found in "
              "repository");
    return;
  }
  updateMapFromRepo();
  tagButtons_.clear();
  for (const auto& tag : existingTagsMap_) {
    auto tagButton = std::make_unique<ImageTextButton>(
        IconStore::getInstance().getRemoveAEIcon());
    tagButton->setButtonText(tag.first + ": " + tag.second);
    tagButton->setGreyLookAndFeel();
    tagButton->setButtonListener(this);
    addAndMakeVisible(tagButton.get());
    tagButtons_.add(tagButton.release());
  }
}

void MixPresentationTagScreen::drawTagButtons(juce::Rectangle<int>& bounds) {
  const float padding = 0.02f;
  const float maxTagHeight = 1.f / (float)kmaxTags_ - padding;
  const auto boundsRef = bounds;
  bounds.removeFromRight(bounds.proportionOfWidth(0.05f));

  for (auto& tagButton : tagButtons_) {
    tagButton->setBounds(
        bounds.removeFromTop(boundsRef.proportionOfHeight(maxTagHeight)));
    bounds.removeFromTop(boundsRef.proportionOfHeight(
        padding));  // remove padding between tag buttons
  }
}

void MixPresentationTagScreen::changeMixPresentation(
    const juce::Uuid mixPresentationId) {
  //  get the tags for the mix presentation
  if (!mixPresentationRepository_->get(mixPresentationId).has_value()) {
    LOG_ERROR(RendererProcessor::instanceId_,
              "MixPresentationTagScreen::changeMixPresentation "
              "MixPresentation not found in "
              "repository");
    return;
  }
  currentMixPresId_ = mixPresentationId;
  updateTagButtons();
  if (existingTagsMap_.contains(kContentLanguageTag_)) {
    LanguageData::MixLanguages language(MixPresentation::stringToLanguage(
        existingTagsMap_[kContentLanguageTag_]));
    int boxIndex = static_cast<int>(language);
    contentLanguageBox_.setSelectedIndex(boxIndex - 1,
                                         juce::dontSendNotification);
  } else {
    contentLanguageBox_.setSelectedIndex(-1, juce::dontSendNotification);
    contentLanguageBox_.setText("Select Content Language");
  }
  repaint();
}

void MixPresentationTagScreen::updateMapFromRepo() {
  existingTagsMap_.clear();
  MixPresentation mixPres = mixPresentationRepository_->get(currentMixPresId_)
                                .value_or(MixPresentation());
  std::unordered_map<std::string, std::string> tags = mixPres.getTags();

  for (const auto& tag : tags) {
    existingTagsMap_.insert(tag);
  }
}

void MixPresentationTagScreen::buttonClicked(juce::Button* button) {
  for (auto tagButton : tagButtons_) {
    if (button == tagButton->getButton()) {
      std::string buttonText = tagButton->getButtonText();
      MixPresentation mixPres =
          mixPresentationRepository_->get(currentMixPresId_)
              .value_or(MixPresentation());
      mixPres.removeTag(buttonText);
      mixPresentationRepository_->update(mixPres);
      // if the content language tag is removed, update the content language box
      if (buttonText.find(kContentLanguageTag_) != std::string::npos) {
        contentLanguageBox_.setSelectedIndex(-1, juce::dontSendNotification);
        contentLanguageBox_.setText("Select Content Language");
      }
      updateTagButtons();
      repaint();
      break;
    }
  }
}

void MixPresentationTagScreen::configureLanguageDropDownBox() {
  contentLanguageBox_.onChange(languageChanged_);
  contentLanguageBox_.setTextWhenNothingSelected("Select Content Language");
  for (int i = 1; i < static_cast<int>(LanguageData::MixLanguages::COUNT);
       i++) {
    LanguageData::MixLanguages language =
        static_cast<LanguageData::MixLanguages>(i);
    contentLanguageBox_.addOption(MixPresentation::languageToString(language));
  }
  MixPresentation mixPres = mixPresentationRepository_->get(currentMixPresId_)
                                .value_or(MixPresentation());

  // retrieve the content language from the mix presentation
  // if the content language tag, does not exist, ensure the box
  // has an index of -1
  std::unordered_map<std::string, std::string> tags = mixPres.getTags();
  if (!tags.contains(kContentLanguageTag_)) {
    contentLanguageBox_.setSelectedIndex(-1, juce::dontSendNotification);
    contentLanguageBox_.setText("Select Content Language");
  } else {
    LanguageData::MixLanguages language(
        MixPresentation::stringToLanguage(tags[kContentLanguageTag_]));
    int boxIndex = static_cast<int>(language);
    contentLanguageBox_.setSelectedIndex(boxIndex - 1,
                                         juce::dontSendNotification);
  }
}

void MixPresentationTagScreen::languageChangedCallback() {
  int index = contentLanguageBox_.getSelectedIndex();
  MixPresentation mixPres = mixPresentationRepository_->get(currentMixPresId_)
                                .value_or(MixPresentation());
  int currentLanguageIndex =
      static_cast<int>(mixPres.getMixPresentationLanguage()) - 1;
  // no need to do anything
  if (index == -1) {
    contentLanguageBox_.setText("Select Content Language");
    return;
  } else if (index == currentLanguageIndex) {
    return;
  }

  index++;
  LanguageData::MixLanguages language =
      static_cast<LanguageData::MixLanguages>(index);

  mixPres.addTagPair(kContentLanguageTag_,
                     MixPresentation::languageToString(language).toStdString());
  mixPresentationRepository_->update(mixPres);
  updateTagButtons();
  repaint();
  LOG_ANALYTICS(RendererProcessor::instanceId_,
                std::string("Language changed for MixPresentation to: ") +
                    MixPresentation::languageToString(language).toStdString());
}