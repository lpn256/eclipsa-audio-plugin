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

#include "processors/mix_monitoring/MixMonitorProcessor.h"

#include <gtest/gtest.h>

#include <fstream>

#include "processors/mix_monitoring/loudness_standards/MeasureEBU128.h"

// Optional additional information for local debugging.
#define VERBOSE_FILTER_DEBUG 0

// Dummy repo.
class TestRoomSetupRepo : public RoomSetupRepository {
 public:
  TestRoomSetupRepo() : RoomSetupRepository(juce::ValueTree{"test"}) {}
};

// Test type to access interface filter method.
class TestEBU128Filter : public MeasureEBU128 {
 public:
  TestEBU128Filter(const double sampleRate, const juce::AudioChannelSet& chData)
      : MeasureEBU128(sampleRate, chData) {}

  void filterBuffer(const juce::AudioBuffer<float>& buffer,
                    juce::AudioBuffer<float>& filterOutputBuffer) {
    MeasureEBU128::filterBuffer(buffer, filterOutputBuffer);
  }
};

// Fill a buffer with a sine wave.
void genSinWave(juce::AudioBuffer<float>& buffer, const double sampleRate,
                const float frequency, const float amplitude) {
  for (int i = 0; i < buffer.getNumChannels(); ++i)
    for (int j = 0; j < buffer.getNumSamples(); ++j) {
      buffer.setSample(
          i, j, amplitude * std::sin(2 * M_PI * frequency * j / sampleRate));
    }
}

const double kSampleRate = 48000.0f;

TEST(test_ebu128_measure, test_shelf_filter) {
  const juce::AudioChannelSet chLayout = juce::AudioChannelSet::mono();

  // Instantiate a measurement object.
  TestEBU128Filter ebu128Impl(kSampleRate, chLayout);

  // Generate a sine wave at  6kHz.
  const int kNumChannels = chLayout.size();
  const int kNumSamples = 100;
  const int kfreq = 6000;
  juce::AudioBuffer<float> buffer(kNumChannels, kNumSamples);
  genSinWave(buffer, kSampleRate, kfreq, 1.0f);

  // Apply the filter.
  juce::AudioBuffer<float> outputBuffer(kNumChannels, kNumSamples);
  ebu128Impl.filterBuffer(buffer, outputBuffer);

  // Rudimentary check for applied gain.
  // Expect that as kFreq > 1kHz (shelf cut-off freq) peak amplitude > 1.
  EXPECT_GT(outputBuffer.getMagnitude(0, kNumSamples), 1.0f);

#if VERBOSE_FILTER_DEBUG
  std::ofstream out("/tmp/shelf_filter.txt");
  for (int i = 0; i < kNumSamples; ++i) {
    out << filterOutputBuffer.getSample(0, i) << std::endl;
  }
  out.close();
#endif
}

TEST(test_ebu128_measure, test_highpass_filter) {
  const juce::AudioChannelSet chLayout = juce::AudioChannelSet::mono();

  // Instantiate a measurement object.
  TestEBU128Filter ebu128Impl(kSampleRate, chLayout);

  // Generate a sine wave at  50Hz.
  const int kNumChannels = chLayout.size();
  const int kNumSamples = 1000;
  const int kfreq = 50;
  juce::AudioBuffer<float> buffer(kNumChannels, kNumSamples);
  genSinWave(buffer, kSampleRate, kfreq, 1.0f);

  // Apply the filter.
  juce::AudioBuffer<float> outputBuffer(kNumChannels, kNumSamples);
  ebu128Impl.filterBuffer(buffer, outputBuffer);

  // Rudimentary check for filter attenuation.
  // Expect that as kFreq < 100Hz (high-pass cut-off freq) peak amplitude < 1.
  EXPECT_LT(outputBuffer.getMagnitude(0, kNumSamples), 1.0f);

#if VERBOSE_FILTER_DEBUG
  std::ofstream out("/tmp/hp_filter.txt");
  for (int i = 0; i < kNumSamples; ++i) {
    out << filterOutputBuffer.getSample(0, i) << std::endl;
  }
  out.close();
#endif
}

TEST(test_ebu128_measure, measure_integrated_loudness) {
  // Instantiate a measurement object.
  const juce::AudioChannelSet chLayout = juce::AudioChannelSet::mono();
  MeasureEBU128 ebu128Impl(kSampleRate, chLayout);

  // Generate a sine wave at 1kHz.
  const int kNumChannels = chLayout.size();
  const int kNumSamples = 528;
  const int kfreq = 1000;
  juce::AudioBuffer<float> buffer(kNumChannels, kNumSamples);
  genSinWave(buffer, kSampleRate, kfreq, 1.0f);

  // As the gating period is 400ms, for a sample rate of 48kHz, we expect 19200
  // samples before a gating block is processed. This equates to 36.3 (37)
  // buffers processed before the first valid measurement.
  MeasureEBU128::LoudnessStats loudnessStats{};
  for (int i = 0; i < 36; ++i) {
    loudnessStats = ebu128Impl.measureLoudness(chLayout, buffer);
    EXPECT_EQ(loudnessStats.loudnessIntegrated, 0.0f);
  }

  // Measure loudness. For a single channel 1kHz sinewave, we expect loudness
  // to be -3.01 LKFS per ITU-R BS.1770-5.
  loudnessStats = ebu128Impl.measureLoudness(chLayout, buffer);
  EXPECT_NEAR(loudnessStats.loudnessIntegrated, -3.01f, 0.06f);
}

TEST(test_ebu_128, measure_true_peak) {
  // Instantiate a measurement object.
  const juce::AudioChannelSet chLayout = juce::AudioChannelSet::create5point1();
  MeasureEBU128 ebu128Impl(kSampleRate, chLayout);

  // Generate a sine wave at 1kHz.
  const int kNumChannels = chLayout.size();
  const int kNumSamples = 528;
  const int kfreq = 1000;
  juce::AudioBuffer<float> buffer(kNumChannels, kNumSamples);
  genSinWave(buffer, kSampleRate, kfreq, 1.0f);

  // Expect the true peak level to still be ~1.0, or 0 dB TP
  MeasureEBU128::LoudnessStats loudnessStats{};
  loudnessStats = ebu128Impl.measureLoudness(chLayout, buffer);
  EXPECT_NEAR(loudnessStats.loudnessTruePeak, 0.0f, 0.1f);
}

TEST(test_ebu_128, measure_all) {
  // Instantiate a measurement object.
  const juce::AudioChannelSet chLayout = juce::AudioChannelSet::mono();
  MeasureEBU128 ebu128Impl(kSampleRate, chLayout);

  // Generate an arbitrary sinewave.
  const int kNumChannels = chLayout.size();
  const int kNumSamples = 528;
  const int kfreq = 1000;
  juce::AudioBuffer<float> buffer(kNumChannels, kNumSamples);
  genSinWave(buffer, kSampleRate, kfreq, 1.0f);

  // Run the measurement over 273 * 528 = 144,144 samples / 48,000
  // samples/second = ~3 seconds.
  MeasureEBU128::LoudnessStats loudnessStats{};
  for (int i = 0; i < 273; ++i) {
    loudnessStats = ebu128Impl.measureLoudness(chLayout, buffer);
  }

  // Expect all loudnesses to be ~equal, as it's the same signal periodically
  // repeating over 3s.
  // momentary, shortterm, integrated, idk what to expect for loudness range...
  EXPECT_NEAR(loudnessStats.loudnessMomentary, -3.1f,
              0.2f);  // I'll allow more variation on this one as it has the
                      // shortest window.
  EXPECT_NEAR(loudnessStats.loudnessShortTerm, -3.1f, 0.1f);
  EXPECT_NEAR(loudnessStats.loudnessIntegrated, -3.1f, 0.1f);
  EXPECT_NEAR(loudnessStats.loudnessRange, 7.0f,
              0.1f);  // Arbitrarily chosen accuracy as unsure what
  // measurement should be over the window.
  EXPECT_NEAR(loudnessStats.loudnessTruePeak, 0.0f, 0.1f);
}

TEST(test_mix_monitor_proc, process_block) {
  // Configure repository with a valid playback layout.
  TestRoomSetupRepo repo;
  RoomSetup room({RendererTypes::IAMFSpkrLayout::kITU_A_0_2_0, "Nope"});
  repo.update(room);

  // Instantiate processor.
  MixMonitorProcessor proc(repo);

  // Set playback details and fill buffer with arbitrary sinewave.
  juce::AudioChannelSet playbackLayout = juce::AudioChannelSet::stereo();
  const int kNumChannels = playbackLayout.size();
  const int kNumSamples = 528;
  const int kfreq = 1000;
  // Configuring the processor for a stereo buffer. Need to zero one channel for
  // valid calculation comparisons.
  juce::AudioBuffer<float> buffer(kNumChannels, kNumSamples);
  genSinWave(buffer, kSampleRate, kfreq, 1.0f);
  buffer.clear(1, 0, kNumSamples);
  proc.setPlayConfigDetails(playbackLayout.size(), playbackLayout.size(),
                            kSampleRate, playbackLayout.size());

  proc.prepareToPlay(kSampleRate, kNumSamples);

  juce::MidiBuffer dummy;
  // Run the measurement over 273 * 528 = 144,144 samples / 48,000
  // samples/second = ~3 seconds.
  for (int i = 0; i < 273; ++i) {
    proc.processBlock(buffer, dummy);
  }

  // Check validity of measurements.
  MeasureEBU128::LoudnessStats loudnessStats = proc.getEBU128Stats();
  EXPECT_NEAR(loudnessStats.loudnessMomentary, -3.1f,
              0.2f);  // I'll allow more variation on this one as it has the
                      // shortest window.
  EXPECT_NEAR(loudnessStats.loudnessShortTerm, -3.1f, 0.1f);
  EXPECT_NEAR(loudnessStats.loudnessIntegrated, -3.1f, 0.1f);
  EXPECT_NEAR(loudnessStats.loudnessRange, 7.0f, 0.1f);
  EXPECT_NEAR(loudnessStats.loudnessTruePeak, 0.0f, 0.1f);
}

// Test changing playback layout resets measurements
TEST(test_mix_monitor_proc, process_block_layout_changed) {
  // Configure repository with a valid playback layout.
  TestRoomSetupRepo repo;
  RoomSetup room = repo.get();
  room.setSpeakerLayout({RendererTypes::IAMFSpkrLayout::kITU_A_0_2_0, "Nope"});
  repo.update(room);

  // Instantiate processor.
  MixMonitorProcessor proc(repo);

  // Set playback details and fill buffer with arbitrary sinewave.
  juce::AudioChannelSet playbackLayout = juce::AudioChannelSet::stereo();
  const int kNumChannels = playbackLayout.size();
  const int kNumSamples = 528;
  const int kfreq = 1000;
  // Configuring the processor for a stereo buffer. Need to zero one channel for
  // valid calculation comparisons.
  juce::AudioBuffer<float> buffer(kNumChannels, kNumSamples);
  genSinWave(buffer, kSampleRate, kfreq, 1.0f);
  buffer.clear(1, 0, kNumSamples);
  proc.setPlayConfigDetails(playbackLayout.size(), playbackLayout.size(),
                            kSampleRate, playbackLayout.size());

  proc.prepareToPlay(kSampleRate, kNumSamples);

  juce::MidiBuffer dummy;
  for (int i = 0; i < 36; ++i) {
    proc.processBlock(buffer, dummy);
    MeasureEBU128::LoudnessStats loudnessStats = proc.getEBU128Stats();
    EXPECT_EQ(loudnessStats.loudnessIntegrated, 0.0f);
  }

  // Modify the playback layout one frame before a valid measurement would have
  // been produced.
  room = repo.get();
  room.setSpeakerLayout({RendererTypes::IAMFSpkrLayout::kITU_I_0_7_0, "7.1"});
  repo.update(room);
  proc.prepareToPlay(kNumSamples, kNumSamples);

  proc.processBlock(buffer, dummy);
  MeasureEBU128::LoudnessStats loudnessStats = proc.getEBU128Stats();
  EXPECT_EQ(loudnessStats.loudnessIntegrated, 0.0f);
}