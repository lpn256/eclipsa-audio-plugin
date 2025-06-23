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
#include "processors/loudness_export/LoudnessExportProcessor_PremierePro.h"

void configureMixPresentations(
    const std::vector<juce::Uuid>& mixIds,
    const std::vector<juce::String>& mixNames,
    const std::vector<float>& mixGains,
    const std::vector<std::vector<AudioElement>>& audioElements,
    MixPresentationRepository& mixPresRepo) {
  jassert(mixIds.size() == mixNames.size() &&
          mixIds.size() == mixGains.size() &&
          audioElements.size() == mixIds.size());
  for (int i = 0; i < mixIds.size(); i++) {
    MixPresentation mixPresentation(mixIds[i], mixNames[i], mixGains[i]);
    for (int j = 0; j < audioElements[i].size(); j++) {
      mixPresentation.addAudioElement(audioElements[i][j].getId(), 1.f,
                                      audioElements[i][j].getName());
    }
    mixPresRepo.updateOrAdd(mixPresentation);
  }
  return;
}

void configureMixPresentationLoudness(
    MixPresentationLoudness& mixLoudness,
    const Speakers::AudioElementSpeakerLayout& layout) {
  // assign obscure initial values to the mix loudness
  const float obscureInitialValue = -500.f;
  mixLoudness.replaceLargestLayout(layout);
  mixLoudness.setLayoutIntegratedLoudness(layout, obscureInitialValue);
  mixLoudness.setLayoutDigitalPeak(layout, obscureInitialValue);
  mixLoudness.setLayoutTruePeak(layout, obscureInitialValue);

  mixLoudness.setLayoutIntegratedLoudness(Speakers::kStereo,
                                          obscureInitialValue);
  mixLoudness.setLayoutDigitalPeak(Speakers::kStereo, obscureInitialValue);
  mixLoudness.setLayoutTruePeak(Speakers::kStereo, obscureInitialValue);
}

juce::AudioBuffer<float> createSinWaveAudio(const int kSamplesPerFrame,
                                            const int kSampleRate) {
  // Generate a 440Hz tone
  juce::AudioBuffer<float> sineWaveAudio(1, kSamplesPerFrame);
  for (int i = 0; i < kSamplesPerFrame; ++i) {
    sineWaveAudio.setSample(
        0, i, 0.1f * std::sin(2 * M_PI * 440 * (float)i / kSampleRate));
  }
  return sineWaveAudio;
}
// this tests ensures that the loudness values are copied to the
// repository when the processor is toggled between non-realtime and realtime
TEST(test_loudness_proc, copyExportContainerDataToRepo) {
  juce::ValueTree testState("test_state");

  FileExportRepository fileExportRepository(
      testState.getOrCreateChildWithName("file", nullptr));
  MixPresentationLoudnessRepository mixPresentationLoudnessRepository(
      testState.getOrCreateChildWithName("mixLoudness", nullptr));

  MixPresentationRepository mixPresentationRepository(
      testState.getOrCreateChildWithName("mixPres", nullptr));

  AudioElementRepository audioElementRepository(
      testState.getOrCreateChildWithName("audioElement", nullptr));

  // Data used by all test fixtures:
  // Constants
  const int kSampleRate = 48e3;
  const int kSamplesPerFrame = 128;
  // Set the duration of the input video file.
  const float kAudioDuration_s = 0.2;
  const int kTotalSamples = kAudioDuration_s * kSampleRate;

  // Update the file export config
  // loudness_proc only cares about AudioFileFormat and setExportAudio(true)
  FileExport ex = fileExportRepository.get();
  ex.setExportAudio(true);
  ex.setAudioFileFormat(AudioFileFormat::IAMF);
  ex.setSampleRate(kSampleRate);
  fileExportRepository.update(ex);

  // Specify the audio element layouts.
  // Largest layout will be 5.1
  const Speakers::AudioElementSpeakerLayout kAudioElementLayout1 =
      Speakers::kStereo;
  const Speakers::AudioElementSpeakerLayout kAudioElementLayout2 =
      Speakers::k5Point1;

  const int numChannels = kAudioElementLayout1.getNumChannels() +
                          kAudioElementLayout2.getNumChannels();

  // Create a mix presentation with two audio elements.
  const std::vector<juce::Uuid> mixIds{juce::Uuid()};
  const std::vector<juce::String> mixNames{"Mix 1"};
  // create 3 audio elements
  const AudioElement audioElement1(juce::Uuid(), "AE 1", Speakers::kStereo, 0);
  const AudioElement audioElement2(
      juce::Uuid(), "AE 2", Speakers::k5Point1,
      audioElement1.getChannelCount() + audioElement1.getFirstChannel());

  audioElementRepository.updateOrAdd(audioElement1);
  audioElementRepository.updateOrAdd(audioElement2);

  // create a vector of audio elements, that will be assigned to the mix
  // presentation
  const std::vector<std::vector<AudioElement>> audioElements = {
      {audioElement1, audioElement2}};

  const std::vector<float> mixGains = {1.f};

  configureMixPresentations(mixIds, mixNames, mixGains, audioElements,
                            mixPresentationRepository);

  MixPresentationLoudness mixLoudness(mixIds[0]);
  // ensure the largest layout is 5.1
  configureMixPresentationLoudness(mixLoudness, kAudioElementLayout2);

  mixPresentationLoudnessRepository.updateOrAdd(
      mixLoudness);  // update the repository

  // Create instance of processor
  LoudnessExportProcessor loudness_proc(
      fileExportRepository, mixPresentationRepository,
      mixPresentationLoudnessRepository, audioElementRepository);

  // Generate a 440Hz tones.
  // the sine wave and will be assigned to a different
  // audio element
  juce::AudioBuffer<float> sineWaveAudio =
      createSinWaveAudio(kSamplesPerFrame, kSampleRate);

  // Start cacluclaing loudness values
  loudness_proc.prepareToPlay(kSampleRate, kSamplesPerFrame);
  // ensure there is 1 loudness implementation, for the non-stero layout
  loudness_proc.setNonRealtime(true);

  // Copy the the sine wave audio to each buffer channel and process the
  // frame.
  juce::AudioBuffer<float> audioBuffer(numChannels, kSamplesPerFrame);
  juce::MidiBuffer midiBuffer;
  for (int sampsProcd = 0; sampsProcd < kTotalSamples;
       sampsProcd += kSamplesPerFrame) {
    // Copy audio data to each channel of audioBuffer.
    for (int i = 0; i < numChannels; ++i) {
      audioBuffer.copyFrom(i, 0, sineWaveAudio, 0, 0, kSamplesPerFrame);
    }
    loudness_proc.processBlock(audioBuffer, midiBuffer);
  }

  // should copy the loudness values to the repository
  loudness_proc.setNonRealtime(false);

  // compare values from the RT Data Struct to the repository
  using EBU128Stats = MeasureEBU128::LoudnessStats;
  EBU128Stats stereoLoudnessStats;
  EBU128Stats layoutLoudnessStats;
  const MixPresentationLoudnessExportContainer* exportContainer =
      loudness_proc.getExportContainers()[0];
  exportContainer->loudnessExportData->stereoEBU128.read(stereoLoudnessStats);
  exportContainer->loudnessExportData->layoutEBU128.read(layoutLoudnessStats);
  MixPresentationLoudness results =
      mixPresentationLoudnessRepository.get(mixIds[0]).value();
  EXPECT_EQ(results.getLargestLayout(), Speakers::k5Point1);

  EXPECT_EQ(results.getLayoutIntegratedLoudness(Speakers::kStereo),
            stereoLoudnessStats.loudnessIntegrated);
  EXPECT_EQ(results.getLayoutIntegratedLoudness(Speakers::k5Point1),
            layoutLoudnessStats.loudnessIntegrated);

  EXPECT_EQ(results.getLayoutDigitalPeak(Speakers::kStereo),
            stereoLoudnessStats.loudnessDigitalPeak);
  EXPECT_EQ(results.getLayoutDigitalPeak(Speakers::k5Point1),
            layoutLoudnessStats.loudnessDigitalPeak);

  EXPECT_EQ(results.getLayoutTruePeak(Speakers::kStereo),
            stereoLoudnessStats.loudnessTruePeak);
  EXPECT_EQ(results.getLayoutTruePeak(Speakers::k5Point1),
            layoutLoudnessStats.loudnessTruePeak);
}