/*
 * Copyright 2025 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <ambi_dec.h>  // SAF library
#include <juce_audio_basics/juce_audio_basics.h>

#include <Eigen/Dense>

#include "data_structures/src/AmbisonicsData.h"
#include "substream_rdr/substream_rdr_utils/Speakers.h"

// importing functions from ambi_dec.h from the SAF library
// these functions are used to initialize and configure the decoder
extern "C" {
void ambi_dec_create(void** const phAmbi);
void ambi_dec_destroy(void** const phAmbi);
void ambi_dec_init(
    void* const hAmbi,
    int samplerate);  // reinitializes the decoder w/  a new sample rate
void ambi_dec_process(void* const hAmbi, const float* const* inputs,
                      float* const* outputs, int nInputs, int nOutputs,
                      int nSamples);
int ambi_dec_getDAWsamplerate(
    void* const hAmbi);  // returns the sample rate of the decoder
void ambi_dec_setOutputConfigPreset(void* const hAmbi,
                                    int newPresetID);  // assigns speaker layout
float ambi_dec_getLoudspeakerAzi_deg(
    void* const hAmbi, int index);  // return azimuth of loudspeaker
float ambi_dec_getLoudspeakerElev_deg(
    void* const hAmbi, int index);  // return the elevation of loudspeaker
int ambi_dec_getNumLoudspeakers(void* const hAmbi);

void ambi_dec_initCodec(void* const hAmbi);

int ambi_dec_getMasterDecOrder(void* const hAmbi);

void ambi_dec_setMasterDecOrder(void* const hAmbi, int newValue);

CODEC_STATUS ambi_dec_getCodecStatus(void* const hAmbi);

PROC_STATUS ambi_dec_getProcStatus(void* const hAmbi);
}

// The instance of this class is managed by the SoundFieldProcessor
class SoundField {
 public:
  SoundField(const Speakers::AudioElementSpeakerLayout layout,
             AmbisonicsData& ambisonicsData);
  ~SoundField();

  void createDecoder();

  void reinitDecoder(const int& sampleRate = 48e3);

  void destroyDecoder() { ambi_dec_destroy(&phAmbi_); }

  void processDecoder(juce::AudioBuffer<float>& buffer);

  int getDecoderSampleRate() const {
    return ambi_dec_getDAWsamplerate(phAmbi_);
  }

  float getLoudspeakerAzi_deg(const int& index) const {
    return ambi_dec_getLoudspeakerAzi_deg(phAmbi_, index);
  }

  float getLoudspeakerElev_deg(const int& index) const {
    return ambi_dec_getLoudspeakerElev_deg(phAmbi_, index);
  }

  int getNumLoudspeakers() const {
    return ambi_dec_getNumLoudspeakers(phAmbi_);
  }

  bool isCodecInitialized() const {
    CODEC_STATUS status = ambi_dec_getCodecStatus(phAmbi_);
    if (status == CODEC_STATUS::CODEC_STATUS_INITIALISED) {
      return true;
    } else {
      return false;
    }
  }

  bool getProcessingStatus() const {
    PROC_STATUS status = ambi_dec_getProcStatus(phAmbi_);
    if (status == PROC_STATUS::PROC_STATUS_NOT_ONGOING) {
      return false;
    } else {
      return true;
    }
  }

  void reinitCodec() { ambi_dec_initCodec(phAmbi_); }

  // rows corresponds to number of channels
  float** allocate2DArray(const int& rows);

  void deallocate2DArray(float**& array, const int& rows);

  const int samplesPerBuffer_ = 128;  // STFT in decoder requires 128 samples

 private:
  const Speakers::AudioElementSpeakerLayout layout_;
  AmbisonicsData& ambisonicsData_;
  int numLoudSpeakers_;
  void* phAmbi_;        // decoder handle
  float** inputData_;   // input data
  float** outputData_;  // output data
};
