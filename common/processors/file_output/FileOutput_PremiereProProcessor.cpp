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

#include "FileOutput_PremiereProProcessor.h"

#include "iamf_export_utils/IAMFExportUtil.h"
#include "logger/logger.h"
#include "processors/file_output/FileOutputProcessor.h"

FileOutput_PremiereProProcessor::FileOutput_PremiereProProcessor(
    FileExportRepository& fileExportRepository,
    AudioElementRepository& audioElementRepository,
    MixPresentationRepository& mixPresentationRepository,
    MixPresentationLoudnessRepository& mixPresentationLoudnessRepository)
    : FileOutputProcessor(fileExportRepository, audioElementRepository,
                          mixPresentationRepository,
                          mixPresentationLoudnessRepository),
      estimatedSamplesToProcess_(0),
      processedSamples_(0) {
  LOG_ANALYTICS(0, "FileOutput_PremiereProProcessor instantiated.");
}

FileOutput_PremiereProProcessor::~FileOutput_PremiereProProcessor() {
  LOG_ANALYTICS(0, "FileOutput_PremiereProProcessor destroyed.");
}

void FileOutput_PremiereProProcessor::setNonRealtime(
    bool isNonRealtime) noexcept {
  LOG_ANALYTICS(0, std::string("File Output Premiere Pro Set Non-Realtime ") +
                       (isNonRealtime ? "true" : "false"));
  // Initialize the writer if we are rendering in offline mode
  if (isNonRealtime && !performingRender_) {
    FileExport config = fileExportRepository_.get();
    if ((config.getAudioFileFormat() == AudioFileFormat::IAMF) &&
        (config.getExportAudio())) {
      performingRender_ = true;
      startTime_ = config.getStartTime();
      endTime_ = config.getEndTime();

      // To create the IAMF file, create a list of all the audio element wav
      // files to be created
      LOG_ANALYTICS(0, "FileOutput PremierePro, Beginning .iamf file export");
      juce::OwnedArray<AudioElement> audioElements;
      audioElementRepository_.getAll(audioElements);
      iamfWavFileWriters_.clear();
      iamfWavFileWriters_.reserve(audioElements.size());
      for (int i = 0; i < audioElements.size(); i++) {
        juce::String wavFilePath = config.getExportFile() + "_audio_element_ " +
                                   juce::String(i) + ".wav";
        sampleRate_ = config.getSampleRate();

        iamfWavFileWriters_.emplace_back(new AudioElementFileWriter(
            wavFilePath, config.getSampleRate(), config.getBitDepth(),
            config.getAudioCodec(), *audioElements[i]));
      }
      sampleTally_ = 0;
    }
  }
}

void FileOutput_PremiereProProcessor::prepareToPlay(double sampleRate,
                                                    int samplesPerBlock) {
  LOG_ANALYTICS(0, "FileOutput_PremiereProProcessor prepareToPlay");
  FileOutputProcessor::prepareToPlay(sampleRate, samplesPerBlock);
  FileExport config = fileExportRepository_.get();

  processedSamples_ = 0;

  int totalDuration = config.getEndTime() - config.getStartTime();

  estimatedSamplesToProcess_ = static_cast<int>(totalDuration * sampleRate);
  LOG_ANALYTICS(0, "FileOutput PremierePro, totalDuration: " +
                       std::to_string(totalDuration) +
                       ", Estimated samples to process: " +
                       std::to_string(estimatedSamplesToProcess_) + "\n");
}

void FileOutput_PremiereProProcessor::processBlock(
    juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
  juce::ignoreUnused(midiMessages);

  if (!performingRender_ || buffer.getNumSamples() < 1) {
    return;
  }

  long currentTime = sampleTally_ / sampleRate_;
  long nextTime = (sampleTally_ + buffer.getNumSamples()) / sampleRate_;

  if (currentTime < startTime_ || nextTime > endTime_) {
    // Do not write to the wav file if we are outside the start and end time
    return;
  }

  if (processedSamples_ <= estimatedSamplesToProcess_) {
    LOG_ANALYTICS(
        0, std::string(
               "PremierePro FileOutput process block is performRendering"));
    LOG_ANALYTICS(0, "Processing an additional " +
                         std::to_string(buffer.getNumSamples()) +
                         " samples. Already processed " +
                         std::to_string(processedSamples_) + " of " +
                         std::to_string(estimatedSamplesToProcess_));
    processedSamples_ += buffer.getNumSamples();

    // Write the audio data to the wav file writers
    for (auto& writer : iamfWavFileWriters_) {
      writer->write(buffer);
    }
  } else {
    // Stop rendering if we are switching back to online mode
    // copy loudness values from the map to the repository
    LOG_ANALYTICS(
        0, "FileOutput PremierePro Setting performRendering_ to false \n");
    suspendProcessing(true);
    performingRender_ = false;

    FileExport config = fileExportRepository_.get();

    // close the output file, since rendering is completed
    for (auto& writer : iamfWavFileWriters_) {
      writer->close();
    }
    juce::File outputFile = juce::File(config.getExportFile());
    outputFile.deleteFile();

    bool exportIAMFSuccess =
        exportIamfFile(config.getExportFolder(), config.getExportFolder());

    // If muxing is enabled and audio export was successful, mux the audio and
    // video files.
    if (exportIAMFSuccess && fileExportRepository_.get().getExportVideo()) {
      bool muxIAMFSuccess = IAMFExportHelper::muxIAMF(
          audioElementRepository_, mixPresentationRepository_,
          fileExportRepository_.get());

      if (!muxIAMFSuccess) {
        LOG_INFO(0,
                 "IAMF Muxing: Failed to mux IAMF file with provided video.");
      }
    }

    if (!config.getExportAudioElements()) {
      // Delete the extraneuos audio element files
      for (auto& writer : iamfWavFileWriters_) {
        juce::File audioElementFile(writer->getFilePath());
        audioElementFile.deleteFile();
      }
    }
    iamfWavFileWriters_.clear();
  }
}
