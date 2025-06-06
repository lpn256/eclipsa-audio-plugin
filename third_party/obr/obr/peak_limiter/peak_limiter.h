/*
 * Copyright (c) 2025 Google LLC
 *
 * This source code is subject to the terms of the BSD 3-Clause Clear License,
 * which you can find in the LICENSE file, and the Open Binaural Renderer
 * Patent License 1.0, which you can find in the PATENTS file.
 */

#ifndef OBR_PEAK_LIMITER_H_
#define OBR_PEAK_LIMITER_H_

#include "obr/audio_buffer/audio_buffer.h"

namespace obr {

class PeakLimiter {
 public:
  /*!\brief Constructor for PeakLimiter.
   *
   * \param sampling_rate Sampling rate of the audio data.
   * \param release_ms Release time in milliseconds.
   * \param ceiling_db Ceiling level in decibels.
   */
  PeakLimiter(int sampling_rate, double release_ms, double ceiling_db);

  /*!\brief Default destructor. */
  ~PeakLimiter() = default;

  /*!\brief Processes the input audio buffer and applies peak limiting.
   *
   * \param input Input audio buffer.
   * \param output Output audio buffer.
   */
  void Process(const AudioBuffer& input, AudioBuffer* output);

 private:
  const int sampling_rate_;
  const double ceiling_;
  const double release_time_constant_;
  double env_;

  /*!\brief Returns the maximum gain required to limit the peak.
   *
   * \param sample Input sample.
   * \return Maximum gain required to limit the peak.
   */
  double GetMaximumRequiredGain(double sample) const;
};

}  // namespace obr

#endif  // OBR_PEAK_LIMITER_H_
