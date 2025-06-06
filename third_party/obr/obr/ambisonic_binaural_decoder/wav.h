/*
 * Copyright (c) 2025 Google LLC
 *
 * This source code is subject to the terms of the BSD 3-Clause Clear License,
 * which you can find in the LICENSE file, and the Open Binaural Renderer
 * Patent License 1.0, which you can find in the PATENTS file.
 */
// TODO(b/400661672): Use a common library to handle WAV files.
#ifndef OBR_WAV_H_
#define OBR_WAV_H_

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

// This code is forked from Resonance Audio's `wav.h`.
namespace obr {

/*!\brief Wraps WavReader class to decode a wav file into memory. */
class Wav {
 public:
  ~Wav();

  // Reads a RIFF WAVE from an opened binary stream.
  static std::unique_ptr<const Wav> CreateOrNull(std::istream* binary_stream);

  // Returns reference to interleaved samples.
  const std::vector<int16_t>& interleaved_samples() const {
    return interleaved_samples_;
  }

  // Returns number of channels.
  size_t GetNumChannels() const { return num_channels_; }

  // Returns sample rate.
  int GetSampleRateHz() const { return sample_rate_; }

 private:
  /*!\brief Private constructor used by static factory methods.
   *
   * \param num_channels Number of audio channels.
   * \param sample_rate Sample rate.
   * \param interleaved_samples Decoded interleaved samples.
   */
  Wav(size_t num_channels, int sample_rate,
      std::vector<int16_t>&& interleaved_samples);

  // Number of channels.
  size_t num_channels_;

  // Sample rate.
  int sample_rate_;

  // Interleaved samples.
  std::vector<int16_t> interleaved_samples_;
};

}  // namespace obr

#endif  // OBR_WAV_H_
