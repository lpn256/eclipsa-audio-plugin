/*
 * Copyright (c) 2025 Google LLC
 *
 * This source code is subject to the terms of the BSD 3-Clause Clear License,
 * which you can find in the LICENSE file, and the Open Binaural Renderer
 * Patent License 1.0, which you can find in the PATENTS file.
 */

#ifndef OBR_COMMON_TEST_UTIL_H_
#define OBR_COMMON_TEST_UTIL_H_

#include <cstddef>

#include "obr/audio_buffer/audio_buffer.h"

namespace obr {

// Silences an audio channel.
void GenerateSilence(AudioBuffer::Channel* output);

// Generates a sine wave at the specified frequency in hertz at the given
// sampling rate in hertz.
void GenerateSineWave(float frequency_hz, int sample_rate,
                      AudioBuffer::Channel* output);

// Generates a saw tooth signal between -1 and 1 for the given wave form length.
void GenerateSawToothSignal(size_t tooth_length_samples,
                            AudioBuffer::Channel* output);

// Generates a Dirac impulse filter kernel, which delays filtered signals by
// the given delay.
void GenerateDiracImpulseFilter(size_t delay_samples,
                                AudioBuffer::Channel* output);

// Generates a linear ramp signal between -1 and 1.
void GenerateIncreasingSignal(AudioBuffer::Channel* output);

// Returns the index of the first non-zero element.
size_t ZeroCompare(const AudioBuffer::Channel& signal, float epsilon);

// Compares the content of two audio channels. Returns true if the absolute
// difference between all samples is below epsilon.
bool CompareAudioBuffers(const AudioBuffer::Channel& buffer_a,
                         const AudioBuffer::Channel& buffer_b, float epsilon);

// Returns delayed_signal.size() in output if delayed_signal is approximately
// equal to original_signal delayed by the given amount; otherwise, returns the
// index of the first unequal element in delayed_signal.
size_t DelayCompare(const AudioBuffer::Channel& original_signal,
                    const AudioBuffer::Channel& delayed_signal, size_t delay,
                    float epsilon);

// Test if two signals are shifted by a fixed number of samples with zero
// padding.
bool TestZeroPaddedDelay(const AudioBuffer::Channel& original_signal,
                         const AudioBuffer::Channel& delayed_signal,
                         size_t delay_samples, float epsilon);

// Returns absolute peak amplitude of a signal.
double CalculateSignalPeak(const AudioBuffer::Channel& channel);

// Returns energy of a signal.
double CalculateSignalEnergy(const AudioBuffer::Channel& channel);

// Returns Root Mean Square (RMS) of a signal.
double CalculateSignalRms(const AudioBuffer::Channel& channel);

// Expresses a magnitude measurement in dB.
double DbFromMagnitude(double magnitude);

// Expresses a power measurement in dB.
double DbFromPower(double power);

// Returns the maximum cross correlation value between two signals.
// To be used only with signals of the same length.
float MaxCrossCorrelation(const AudioBuffer::Channel& signal_a,
                          const AudioBuffer::Channel& signal_b);

}  // namespace obr

#endif  // OBR_COMMON_TEST_UTIL_H_
