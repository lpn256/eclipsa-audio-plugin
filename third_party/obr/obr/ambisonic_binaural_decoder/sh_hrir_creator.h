/*
 * Copyright (c) 2025 Google LLC
 *
 * This source code is subject to the terms of the BSD 3-Clause Clear License,
 * which you can find in the LICENSE file, and the Open Binaural Renderer
 * Patent License 1.0, which you can find in the PATENTS file.
 */

#ifndef OBR_SH_HRIR_CREATOR_H_
#define OBR_SH_HRIR_CREATOR_H_

#include <memory>
#include <string>

#include "obr/ambisonic_binaural_decoder/resampler.h"
#include "obr/ambisonic_binaural_decoder/wav.h"
#include "obr/audio_buffer/audio_buffer.h"

// This code is forked from Resonance Audio's `sh_hrir_creator.h`.
namespace obr {

/*!\brief Creates a multichannel audio buffer of Spherical Harmonic-encoded Head
 * Related Impulse Responses from Wav HRIR assets. It also checks if the channel
 * count of the HRIR file is correct and resamples if necessary to match the
 * system (target) sampling rate.
 *
 * \param wav `Wav` instance that contains HRIRs.
 * \param target_sample_rate_hz Target sampling rate in Hertz.
 * \param resampler Pointer to a resampler used to convert HRIRs to the system
 *        rate (the resampler's internal state will be reset on each function
 *        call).
 * \return Unique pointer to `AudioBuffer` where the HRIRs will
 *         be written.
 */
std::unique_ptr<AudioBuffer> CreateShHrirsFromWav(const Wav& wav,
                                                  int target_sample_rate_hz,
                                                  Resampler* resampler);

/*!\brief Creates HRIR multichannel audio buffer from assets.
 *
 * \param filename Name of the asset that contains HRIRs.
 * \param target_sample_rate_hz Target sampling rate in Hertz.
 * \param resampler Pointer to a resampler used to convert HRIRs to the system
 *        rate.
 * \return Unique pointer to `AudioBuffer` where the HRIRs will be written.
 */
std::unique_ptr<AudioBuffer> CreateShHrirsFromAssets(
    const std::string& filename, int target_sample_rate_hz,
    Resampler* resampler);

}  // namespace obr

#endif  // OBR_SH_HRIR_CREATOR_H_
