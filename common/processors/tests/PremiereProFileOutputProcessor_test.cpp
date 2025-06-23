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

#include "gtest/gtest.h"
#include "processors/file_output/FileOutputProcessor_PremierePro.h"

TEST(test_channel_based, output_iamf_file) {
  juce::ValueTree testState("test_state");

  FileExportRepository fileExportRepository(
      testState.getOrCreateChildWithName("file", nullptr));
  AudioElementRepository audioElementRepository(
      testState.getOrCreateChildWithName("element", nullptr));
  MixPresentationRepository mixRepository(
      testState.getOrCreateChildWithName("mix", nullptr));
  MixPresentationLoudnessRepository mixPresentationLoudnessRepository(
      testState.getOrCreateChildWithName("mixLoudness", nullptr));

  iamf_tools_cli_proto::UserMetadata iamfMD;

  // Set up an output filepath
  juce::String iamfPathStr(juce::File::getCurrentWorkingDirectory()
                               .getChildFile("test.iamf")
                               .getFullPathName());
  std::filesystem::path iamfPath(iamfPathStr.toStdString());
  std::filesystem::remove(iamfPath);
  FileExport ex = fileExportRepository.get();
  ex.setExportFolder(
      juce::File::getCurrentWorkingDirectory().getFullPathName());
  ex.setExportFile(juce::File::getCurrentWorkingDirectory()
                       .getChildFile("test")
                       .getFullPathName());
  ex.setExportAudio(true);
  ex.setAudioFileFormat(AudioFileFormat::IAMF);
  fileExportRepository.update(ex);

  // Create some AudioElements to fill the repository with.
  AudioElement ae1(juce::Uuid(), "Audio Element 1", "Description 1",
                   Speakers::kStereo, 0);
  audioElementRepository.add(ae1);

  // Create some MixPresentations to fill the repository with.
  const juce::Uuid mixId = juce::Uuid();
  MixPresentation mp1(mixId, "Mix Presentation 1", 1,
                      LanguageData::MixLanguages::English, {});
  MixPresentationLoudness mixLoudness = MixPresentationLoudness(mixId);
  mp1.addAudioElement(ae1.getId(), 0, ae1.getName());

  mixLoudness.replaceLargestLayout(Speakers::k5Point1);

  mp1.addTagPair("artist", "Rockstars");
  mp1.addTagPair("album", "Eclipsa");
  mixRepository.add(mp1);
  mixPresentationLoudnessRepository.add(mixLoudness);

  // Create instance of FIO processor
  PremiereProFileOutputProcessor premierepro_fio_proc(
      fileExportRepository, audioElementRepository, mixRepository,
      mixPresentationLoudnessRepository);

  // Start a bounce
  premierepro_fio_proc.prepareToPlay(16000, 128);
  premierepro_fio_proc.setNonRealtime(true);

  // Pass 8 channels worth of data for the two audio elements
  juce::AudioBuffer<float> buffer(10, 10);
  juce::MidiBuffer midiBuffer;
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      buffer.setSample(j, i, 0.5f);
    }
  }

  for (int i = 0; i < 10; ++i) {
    premierepro_fio_proc.processBlock(buffer, midiBuffer);
  }

  // Validate the IAMF file was created.
  EXPECT_TRUE(std::filesystem::exists(iamfPath));

  // Clean up the IAMF file.
  std::filesystem::remove(iamfPath);
}

TEST(test_ambisonics, output_iamf_file) {
  juce::ValueTree testState("test_state");

  FileExportRepository fileExportRepository(
      testState.getOrCreateChildWithName("file", nullptr));
  AudioElementRepository audioElementRepository(
      testState.getOrCreateChildWithName("element", nullptr));
  MixPresentationRepository mixRepository(
      testState.getOrCreateChildWithName("mix", nullptr));
  MixPresentationLoudnessRepository mixPresentationLoudnessRepository(
      testState.getOrCreateChildWithName("mixLoudness", nullptr));

  iamf_tools_cli_proto::UserMetadata iamfMD;

  // Set up an output filepath
  juce::String iamfPathStr(juce::File::getCurrentWorkingDirectory()
                               .getChildFile("test.iamf")
                               .getFullPathName());
  std::filesystem::path iamfPath(iamfPathStr.toStdString());
  std::filesystem::remove(iamfPath);
  FileExport ex = fileExportRepository.get();
  ex.setExportFolder(
      juce::File::getCurrentWorkingDirectory().getFullPathName());
  ex.setExportFile(juce::File::getCurrentWorkingDirectory()
                       .getChildFile("test")
                       .getFullPathName());
  ex.setExportAudio(true);
  ex.setAudioFileFormat(AudioFileFormat::IAMF);
  fileExportRepository.update(ex);

  // Create some AudioElements to fill the repository with.
  AudioElement ae1(juce::Uuid(), "Audio Element 1", "Description 1",
                   Speakers::kHOA2, 0);
  audioElementRepository.add(ae1);

  // Create some MixPresentations to fill the repository with.
  MixPresentation mp1(juce::Uuid(), "Mix Presentation 1", 1,
                      LanguageData::MixLanguages::English, {});
  MixPresentationLoudness mixLoudness(mp1.getId());
  mp1.addAudioElement(ae1.getId(), 0, ae1.getName());
  // use Speakers::k5Point1 as the largest layout
  mixLoudness.replaceLargestLayout(Speakers::k5Point1);
  mixRepository.add(mp1);
  mixPresentationLoudnessRepository.add(mixLoudness);

  // Create instance of FIO processor
  PremiereProFileOutputProcessor premierepro_fio_proc(
      fileExportRepository, audioElementRepository, mixRepository,
      mixPresentationLoudnessRepository);

  // Start a bounce
  premierepro_fio_proc.prepareToPlay(16000, 128);
  premierepro_fio_proc.setNonRealtime(true);

  // Pass 8 channels worth of data for the two audio elements
  juce::AudioBuffer<float> buffer(10, 10);
  juce::MidiBuffer midiBuffer;
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      buffer.setSample(j, i, 0.5f);
    }
  }

  for (int i = 0; i < 10; ++i) {
    premierepro_fio_proc.processBlock(buffer, midiBuffer);
  }

  // Validate the IAMF file was created.
  EXPECT_TRUE(std::filesystem::exists(iamfPath));

  // Clean up the IAMF file.
  std::filesystem::remove(iamfPath);
}