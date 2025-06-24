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

#include "../src/RendererProcessor.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>
#include <memory>
#include <utility>

#include "data_repository/implementation/AudioElementRepository.h"
#include "data_repository/implementation/FileExportRepository.h"
#include "data_repository/implementation/RoomSetupRepository.h"
#include "data_structures/src/ActiveMixPresentation.h"
#include "data_structures/src/AudioElement.h"
#include "data_structures/src/RoomSetup.h"
#include "juce_core/system/juce_PlatformDefs.h"
#include "juce_cryptography/juce_cryptography.h"
#include "substream_rdr/substream_rdr_utils/Speakers.h"

void manuallyConfigureRepositories(
    RendererProcessor& rendererProcessor,
    const Speakers::AudioElementSpeakerLayout& layout = Speakers::kStereo) {
  // add a stereo audio element
  AudioElement audioElement;
  audioElement.setName(layout.toString());
  audioElement.setChannelConfig(layout);
  audioElement.setDescription(layout.toString());
  audioElement.setFirstChannel(0);
  AudioElementRepository& aeRepo = rendererProcessor.getRepositories().aeRepo_;
  aeRepo.add(audioElement);

  // create a mix presentation
  // add the audio element to the mix presentation
  MixPresentationRepository& mixPresentationRepository =
      rendererProcessor.getRepositories().mpRepo_;
  MixPresentation mixPresentation;

  const juce::Uuid mixPresID = mixPresentation.getId();
  mixPresentation.addAudioElement(audioElement.getId(), 1.f, layout.toString());
  mixPresentationRepository.updateOrAdd(mixPresentation);

  // update the activeMixPresentationRepository
  ActiveMixRepository& activeMixRepository =
      rendererProcessor.getRepositories().activeMPRepo_;
  ActiveMixPresentation activeMix(mixPresID);
  activeMixRepository.update(activeMix);

  RoomSetupRepository& roomSetupRepository =
      rendererProcessor.getRepositories().roomSetupRepo_;
  RoomSetup roomSetup;
  roomSetup.setSpeakerLayout(speakerLayoutConfigurationOptions[0]);
  roomSetupRepository.update(roomSetup);
}

TEST(test_renderer_processor, processor_chain) {
  // Data used by all test fixtures:
  // Constants
  const int kSampleRate = 48e3;
  const int kSamplesPerFrame = 128;
  // Set the duration of the input video file.
  const float kAudioDuration_s = 0.2f;
  const int kTotalSamples = kAudioDuration_s * kSampleRate;

  RendererProcessor rendererProcessor;
  manuallyConfigureRepositories(rendererProcessor);

  // Generate a 440Hz tone
  // will pass to the renderer processor
  const float kAmplitude_ = 0.1f;
  juce::AudioBuffer<float> sineWaveAudio(1, kSamplesPerFrame);
  for (int i = 0; i < kSamplesPerFrame; ++i) {
    sineWaveAudio.setSample(
        0, i, kAmplitude_ * std::sin(2 * M_PI * 440 * (float)i / kSampleRate));
  }

  // Copy the the sine wave audio to each buffer channel
  const int kNumChannels_ = Speakers::kHOA5.getNumChannels();
  const Speakers::AudioElementSpeakerLayout layout(
      rendererProcessor.getRepositories()
          .aeRepo_.getFirst()
          .value()
          .getChannelConfig());

  juce::AudioBuffer<float> audioBuffer(kNumChannels_, kSamplesPerFrame);
  juce::MidiBuffer midiBuffer;
  for (int i = 0; i < layout.getNumChannels(); ++i) {
    audioBuffer.copyFrom(i, 0, sineWaveAudio, 0, 0, kSamplesPerFrame);
  }

  // Apply arbitrary gains to the first 2 channels
  const int numChannelsModified = 2;
  const std::array<float, numChannelsModified> gains = {2.f, 0.5f};

  MultiChannelRepository& multiChannelRepository =
      rendererProcessor.getRepositories().chGainRepo_;
  // update the repository with the gains
  for (int i = 0; i < gains.size(); ++i) {
    ChannelGains channelGains = multiChannelRepository.get();
    channelGains.setChannelGain(i, gains[i]);
    multiChannelRepository.update(channelGains);
  }

  // Process the audio buffer
  rendererProcessor.prepareToPlay(kSampleRate, kSamplesPerFrame);

  rendererProcessor.processBlock(audioBuffer, midiBuffer);

  // confirm the gains were applied
  for (int i = 0; i < kNumChannels_; ++i) {
    for (int j = 0; j < kSamplesPerFrame; ++j) {
      if (i < numChannelsModified) {
        ASSERT_EQ(audioBuffer.getSample(i, j),
                  sineWaveAudio.getSample(0, j) * gains[i]);
      }
    }
  }
}

std::filesystem::path manuallyConfigureFileExport(
    RendererProcessor& rendererProcessor, const juce::String& fileName,
    const float kAudioDuration_s, const int& kSampleRate) {
  // Set up an output filepath for the file export
  // Set up an output filepath for the fir
  const juce::String extension = ".iamf";
  jassert(fileName.contains(extension));
  juce::String iamfPathStr(juce::File::getCurrentWorkingDirectory()
                               .getChildFile(fileName)
                               .getFullPathName());
  std::filesystem::path iamfPath(iamfPathStr.toStdString());
  std::filesystem::remove(iamfPath);
  FileExportRepository& fileExportRepository =
      rendererProcessor.getRepositories().fioRepo_;

  const juce::String fileWithOutExtension =
      fileName.substring(0, fileName.length() - extension.length());
  FileExport ex = fileExportRepository.get();
  ex.setExportFolder(
      juce::File::getCurrentWorkingDirectory().getFullPathName());
  ex.setExportFile(juce::File::getCurrentWorkingDirectory()
                       .getChildFile(fileWithOutExtension)
                       .getFullPathName());
  ex.setEndTime(kAudioDuration_s);
  ex.setSampleRate(kSampleRate);
  ex.setExportAudio(true);
  ex.setAudioFileFormat(AudioFileFormat::IAMF);
  fileExportRepository.update(ex);
  return iamfPath;
}

TEST(test_renderer_processor, validate_file_checksum) {
  // Data used by all test fixtures:
  // Constants
  const int kSampleRate = 48e3;
  const int kSamplesPerFrame = 128;
  const int kNumChannels_ = Speakers::kHOA5.getNumChannels();
  // Set the duration of the input video file.
  const float kAudioDuration_s = 0.2f;
  const int kTotalSamples = kAudioDuration_s * kSampleRate;

  RendererProcessor rendererProcessor;
  manuallyConfigureRepositories(rendererProcessor);
  std::filesystem::path path = manuallyConfigureFileExport(
      rendererProcessor, juce::String("HashSourceFile.iamf"), kAudioDuration_s,
      kSampleRate);

  // Generate a 440Hz tone & 220Hz tone
  // will pass to the renderer processor
  const float kAmplitude_ = 0.1f;
  juce::AudioBuffer<float> sineWaveAudio(1, kSamplesPerFrame);
  for (int i = 0; i < kSamplesPerFrame; ++i) {
    sineWaveAudio.setSample(
        0, i, kAmplitude_ * std::sin(2 * M_PI * 440 * (float)i / kSampleRate));
  }

  // attempt file export
  rendererProcessor.prepareToPlay(kSampleRate, kSamplesPerFrame);
  rendererProcessor.setNonRealtime(true);
  // Copy the the sine wave audio to each buffer channel and process the
  // frame.
  juce::AudioBuffer<float> exportAudioBuffer(kNumChannels_, kSamplesPerFrame);
  juce::MidiBuffer exportMidiBuffer;
  for (int sampsProcd = 0; sampsProcd < kTotalSamples;
       sampsProcd += kSamplesPerFrame) {
    // Copy audio data to each channel of exportAudioBuffer.
    for (int i = 0; i < kNumChannels_; ++i) {
      exportAudioBuffer.copyFrom(i, 0, sineWaveAudio, 0, 0, kSamplesPerFrame);
    }
    rendererProcessor.processBlock(exportAudioBuffer, exportMidiBuffer);
  }
  rendererProcessor.setNonRealtime(false);

  // confirm that the  .iamf file was created
  juce::File iamfFile(path.string());
  assert(iamfFile.existsAsFile());

  std::unique_ptr<juce::FileInputStream> fileStream(
      iamfFile.createInputStream());
  juce::MemoryBlock fileData;
  fileData.setSize(iamfFile.getSize());
  fileStream->read(fileData.getData(), fileData.getSize());

  // Clean up the IAMF file.
  std::filesystem::remove(path);

  // generate a checksum for the file
  juce::SHA256 newChecksum(fileData.getData(), fileData.getSize());
  const juce::String newChecksumString = newChecksum.toHexString();

  // Since Debug vs Release builds produce different checksums due to
  // different optimization levels, try both checksums for resilience
  std::string debugChecksumFileName = "HashSourceFile.debug.iamf.checksum";
  std::string releaseChecksumFileName = "HashSourceFile.release.iamf.checksum";

  std::cout << "DEBUG: Build details:" << std::endl;
  std::cout << "DEBUG: NDEBUG defined: " <<
#ifdef NDEBUG
      "yes"
#else
      "no"
#endif
            << std::endl;
  std::cout << "DEBUG: CMAKE_BUILD_TYPE_RELEASE defined: " <<
#ifdef CMAKE_BUILD_TYPE_RELEASE
      "yes"
#else
      "no"
#endif
            << std::endl;
  std::cout << "DEBUG: FORCE_RELEASE_CHECKSUM defined: " <<
#ifdef FORCE_RELEASE_CHECKSUM
      "yes"
#else
      "no"
#endif
            << std::endl;

  // Try both debug and release checksums
  bool checksumMatched = false;
  juce::String expectedChecksum;

  // Helper function to get checksum path and content
  auto getChecksumContent = [](const std::string& fileName) -> juce::String {
    // determine the checksum path
    std::filesystem::path checksumPath;
    // check if 'rendererplugin/test' is in the current path
    // if not, add it
    if (std::filesystem::current_path().string().find("rendererplugin/test") !=
        std::string::npos) {
      checksumPath =
          std::filesystem::current_path() / "testresources" / fileName;
    } else {
      checksumPath = std::filesystem::current_path() /
                     "rendererplugin/test/testresources" / fileName;
    }

    // remove the 'build' segment from the path
    std::filesystem::path correctedChecksumPath;
    for (const auto& part : checksumPath) {
      if (part == "build") {
        continue;  // Skip the 'build' segment
      }
      correctedChecksumPath /= part;
    }
    checksumPath = correctedChecksumPath;

    juce::File existingChecksumFile(checksumPath.string());
    if (!existingChecksumFile.existsAsFile()) {
      std::cerr << "Checksum file not found: " << checksumPath.string()
                << std::endl;
      return "";
    }

    return existingChecksumFile.loadFileAsString().trimCharactersAtEnd("\n");
  };

  // Try debug checksum
  juce::String debugChecksum = getChecksumContent(debugChecksumFileName);
  if (debugChecksum == newChecksumString) {
    checksumMatched = true;
    expectedChecksum = debugChecksum;
    std::cout << "DEBUG: Checksum matched debug build checksum" << std::endl;
  }

  // Try release checksum if debug didn't match
  if (!checksumMatched) {
    juce::String releaseChecksum = getChecksumContent(releaseChecksumFileName);
    if (releaseChecksum == newChecksumString) {
      checksumMatched = true;
      expectedChecksum = releaseChecksum;
      std::cout << "DEBUG: Checksum matched release build checksum"
                << std::endl;
    }
  }

  // If neither matched, we'll assert with better error message
  if (!checksumMatched) {
    expectedChecksum =
        debugChecksum;  // Use debug checksum for the error message
    std::cout << "DEBUG: Checksum didn't match either debug or release checksum"
              << std::endl;
    std::cout << "Generated: " << newChecksumString << std::endl;
    std::cout << "Debug expected: " << debugChecksum << std::endl;
    std::cout << "Release expected: "
              << getChecksumContent(releaseChecksumFileName) << std::endl;
  }

  // Use this assertion to check if either checksum matched
  ASSERT_TRUE(checksumMatched) << "Generated checksum doesn't match either "
                                  "debug or release expected checksum";

  // This maintains compatibility with existing test logic
  ASSERT_EQ(expectedChecksum, newChecksumString);
}

TEST(test_renderer_processor, validate_up_mixing) {
  // Data used by all test fixtures:
  // Constants
  const int kSampleRate = 48e3;
  const int kSamplesPerFrame = 128;
  // Set the duration of the input video file.
  const float kAudioDuration_s = 0.2f;
  const int kTotalSamples = kAudioDuration_s * kSampleRate;

  RendererProcessor rendererProcessor;
  manuallyConfigureRepositories(rendererProcessor, Speakers::kMono);

  // Generate a 440Hz tone
  // will pass to the renderer processor
  juce::AudioBuffer<float> sineWaveAudio(1, kSamplesPerFrame);
  for (int i = 0; i < kSamplesPerFrame; ++i) {
    sineWaveAudio.setSample(0, i,
                            std::sin(2 * M_PI * 440 * (float)i / kSampleRate));
  }

  // Copy the the sine wave audio to each buffer channel
  const int kNumChannels_ = Speakers::kHOA5.getNumChannels();
  const Speakers::AudioElementSpeakerLayout layout(
      rendererProcessor.getRepositories()
          .aeRepo_.getFirst()
          .value()
          .getChannelConfig());

  const int numChannelsModified = layout.getNumChannels();

  juce::AudioBuffer<float> audioBuffer(kNumChannels_, kSamplesPerFrame);
  juce::MidiBuffer midiBuffer;
  for (int i = 0; i < layout.getNumChannels(); ++i) {
    audioBuffer.copyFrom(i, 0, sineWaveAudio, 0, 0, kSamplesPerFrame);
  }

  // Process the audio buffer
  rendererProcessor.prepareToPlay(kSampleRate, kSamplesPerFrame);

  rendererProcessor.processBlock(audioBuffer, midiBuffer);

  // confirm the gains were applied
  for (int i = 0; i < kNumChannels_; ++i) {
    for (int j = 0; j < kSamplesPerFrame; ++j) {
      if (i < 2) {
        ASSERT_NEAR(audioBuffer.getSample(i, j),
                    0.707f * sineWaveAudio.getSample(0, j), 0.01f);
      }
    }
  }
}