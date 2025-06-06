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

#include "SoundFieldReconstructor.h"

#include "ambi_dec.h"

SoundField::SoundField(const Speakers::AudioElementSpeakerLayout layout,
                       AmbisonicsData& ambisonicsData)
    : ambisonicsData_(ambisonicsData),
      layout_(layout),
      inputData_(allocate2DArray(layout.getNumChannels())) {
  createDecoder();
  // virtual speakers
  numLoudSpeakers_ = getNumLoudspeakers();
  outputData_ = allocate2DArray(numLoudSpeakers_);

  std::vector<float> speakerLoudnesses(
      numLoudSpeakers_, -80.f);  // ensure initial loudnesses are near silent
  std::vector<float> speakerAzimuths(numLoudSpeakers_);
  std::vector<float> speakerElevations(numLoudSpeakers_);
  // write the speaker azimuth and elevation angles
  for (int i = 0; i < numLoudSpeakers_; i++) {
    // convert degrees to radians
    speakerAzimuths[i] =
        getLoudspeakerAzi_deg(i) * juce::MathConstants<float>::pi / 180.f;
    speakerElevations[i] =
        getLoudspeakerElev_deg(i) * juce::MathConstants<float>::pi / 180.f;
  }
  ambisonicsData_.speakerLoudnesses.update(speakerLoudnesses);
  ambisonicsData_.speakerAzimuths = speakerAzimuths;
  ambisonicsData_.speakerElevations = speakerElevations;
}

SoundField::~SoundField() {
  destroyDecoder();
  deallocate2DArray(inputData_, layout_.getNumChannels());
  deallocate2DArray(outputData_, numLoudSpeakers_);
}

void SoundField::createDecoder() {
  ambi_dec_create(&phAmbi_);
  reinitDecoder();
}

void SoundField::reinitDecoder(const int& sampleRate) {
  ambi_dec_init(phAmbi_, sampleRate);
  // set the source config to be an ideal microphone
  // ensures the highest SH order is used for decoding
  // across all frequencies
  // sets MIC_PRESETS to MIC_PRESET_IDEAL
  ambi_dec_setSourcePreset(phAmbi_, 1);
  // assigns the LOUDSPEAKER_ARRAY_PRESET of 49 spherically arranged speakers
  ambi_dec_setOutputConfigPreset(phAmbi_, 28);
  // disable the pre-processing for HRTFs
  ambi_dec_setEnableHRIRsPreProc(phAmbi_, 0);
  // set the decoding method to All-Around
  ambi_dec_setDecMethod(phAmbi_, 0, 4);
  ambi_dec_setDecMethod(phAmbi_, 1, 4);
  // set the decoding order for all frequency bands according to the layout
  // 3rd order ambisonics corresponds to an input of 3, sqrt(16) - 1 = 3
  // 2nd order ambisonics corresponds to an input of 2, sqrt(9) -1 = 2
  // 1st order ambisonics corresponds to an input of 1, sqrt(4) - 1 = 1
  ambi_dec_setMasterDecOrder(phAmbi_, std::sqrt(layout_.getNumChannels()) - 1);
  ambi_dec_setDecOrderAllBands(phAmbi_,
                               std::sqrt(layout_.getNumChannels()) - 1);
  ambi_dec_initCodec(phAmbi_);  // updates codec parameters
}

void SoundField::processDecoder(juce::AudioBuffer<float>& buffer) {
  const float* const* readPointers = buffer.getArrayOfReadPointers();
  const int numChannels = layout_.getNumChannels();
  const int downSampleFactor = buffer.getNumSamples() / samplesPerBuffer_;
  if (buffer.getNumChannels() < numChannels) {
    return;
  }

  for (int i = 0; i < numChannels; i++) {  // for each channel, take 128
                                           // samples
    // Attempt to get 128 samples from each channel for the STFT in
    // ambi_dec_process.
    for (int j = 0, k = 0; j < samplesPerBuffer_; ++j, k += downSampleFactor) {
      // Downsample the input buffer to 128 samples.
      inputData_[i][j] = readPointers[i][k];
    }
  }
  // decode the signal of k channels
  // writes the decoded signal to outputData_ which represents the virtual loud
  // speakers
  ambi_dec_process(phAmbi_, inputData_, outputData_, numChannels,
                   numLoudSpeakers_, samplesPerBuffer_);
  std::vector<float> rmsValues(numLoudSpeakers_);
  // calculate RMS for each loudspeaker
  for (int i = 0; i < numLoudSpeakers_; i++) {
    float sum = 0;
    for (int j = 0; j < samplesPerBuffer_; j++) {
      sum += outputData_[i][j] * outputData_[i][j];
    }
    float rms = sqrt(sum / samplesPerBuffer_);
    rmsValues[i] = 20.f * std::log10(rms);  // calulate in dB
  }

  // update real-time data struct
  ambisonicsData_.speakerLoudnesses.update(rmsValues);
};

float** SoundField::allocate2DArray(const int& rows) {
  float** array = new float*[rows];
  for (int i = 0; i < rows; i++) {
    array[i] = new float[samplesPerBuffer_];
  }
  return array;
}

void SoundField::deallocate2DArray(float**& array, const int& rows) {
  for (int i = 0; i < rows; i++) {
    delete[] array[i];
  }
  delete[] array;
}
