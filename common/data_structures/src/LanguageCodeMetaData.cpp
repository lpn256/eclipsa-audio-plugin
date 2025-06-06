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

#include "LanguageCodeMetaData.h"

std::string LanguageData::getLanguageCode(const MixLanguages& language) {
  return langcodes_map.at(language).first;
}

LanguageData::MixLanguages LanguageData::getLanguageEnum(
    const std::string& language) {
  return LanguageData::langvector_map.at(language);
}

std::string LanguageData::getLanguageName(const MixLanguages& language) {
  return langcodes_map.at(language).second[0];
}