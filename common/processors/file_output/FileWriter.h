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
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

#include "data_structures/src/FileExport.h"

class FileWriter {
 public:
  FileWriter(const juce::String& filename, double sampleRate, int numChannels,
             int firstChannel, int bitDepth, AudioCodec codec)
      : numChannels_(numChannels),
        firstChannel_(firstChannel),
        outputFile_(filename),
        bitDepth_(bitDepth) {
    outputFile_.deleteFile();
    juce::WavAudioFormat format;
    writer_ = format.createWriterFor(new juce::FileOutputStream(outputFile_),
                                     sampleRate,    // Sample Rate
                                     numChannels_,  // Number of channels
                                     bitDepth_,     // Bits per sample
                                     {},
                                     0  // Quality option index
    );
  }

  ~FileWriter() { close(); };

  void write(juce::AudioBuffer<float>& buffer) {
    if (writer_ != nullptr) {
      juce::AudioBuffer<float> toWrite;
      toWrite.setDataToReferTo(&buffer.getArrayOfWritePointers()[firstChannel_],
                               numChannels_, buffer.getNumSamples());
      writer_->writeFromAudioSampleBuffer(toWrite, 0, buffer.getNumSamples());
      framesWritten += buffer.getNumSamples();
    }
  }

  void close() {
    if (writer_ != nullptr) {
      writer_->flush();
      delete writer_;
      writer_ = nullptr;
    }
  }

  std::string getFilePath() {
    return outputFile_.getFullPathName().toStdString();
  }

  int getFramesWritten() { return framesWritten; }

 private:
  juce::AudioFormatWriter* writer_;
  juce::File outputFile_;
  int framesWritten;
  int numChannels_;
  int firstChannel_;
  int bitDepth_;
};