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
#include "LoudnessExport_PremiereProProcessor.h"

#include "data_structures/src/FileExport.h"
#include "logger/logger.h"
#include "processors/loudness_export/LoudnessExportProcessor.h"

LoudnessExport_PremiereProProcessor::LoudnessExport_PremiereProProcessor(
    FileExportRepository& fileExportRepo,
    MixPresentationRepository& mixPresentationRepo,
    MixPresentationLoudnessRepository& loudnessRepo,
    AudioElementRepository& audioElementRepo)
    : LoudnessExportProcessor(fileExportRepo, mixPresentationRepo, loudnessRepo,
                              audioElementRepo),
      estimatedSamplesToProcess_(0),
      processedSamples_(0) {
  LOG_ANALYTICS(0, "LoudnessExport_PremiereProProcessor instantiated.");
}

LoudnessExport_PremiereProProcessor::~LoudnessExport_PremiereProProcessor() {
  LOG_ANALYTICS(0, "LoudnessExport_PremiereProProcessor destroyed.");
  LoudnessExportProcessor::~LoudnessExportProcessor();
}

void LoudnessExport_PremiereProProcessor::setNonRealtime(
    bool isNonRealtime) noexcept {
  LOG_ANALYTICS(0,
                std::string("LoudnessExport Premiere Pro Set Non-Realtime ") +
                    (isNonRealtime ? "true" : "false"));
  // Initialize the writer if we are rendering in offline mode
  if (isNonRealtime && !performingRender_) {
    FileExport config = fileExportRepository_.get();
    if ((config.getAudioFileFormat() == AudioFileFormat::IAMF) &&
        (config.getExportAudio())) {
      performingRender_ = true;

      sampleRate_ = config.getSampleRate();
      sampleTally_ = 0;
      startTime_ = config.getStartTime();
      endTime_ = config.getEndTime();

      // Get all mix presentation loudnesses from the repository
      loudnessRepo_.getAll(mixPresentationLoudnesses_);
      LOG_ANALYTICS(0,
                    "PremierePro, Initializing Export Containers for loudness "
                    "metadata calculations\n");
      intializeExportContainers();
    }
    return;
  }
}

void LoudnessExport_PremiereProProcessor::prepareToPlay(double sampleRate,
                                                        int samplesPerBlock) {
  LOG_ANALYTICS(0, "LoudnessExport_PremiereProProcessor prepareToPlay");
  LoudnessExportProcessor::prepareToPlay(sampleRate, samplesPerBlock);
  FileExport config = fileExportRepository_.get();

  processedSamples_ = 0;

  int totalDuration = config.getEndTime() - config.getStartTime();

  estimatedSamplesToProcess_ = static_cast<int>(totalDuration * sampleRate);
  LOG_ANALYTICS(0, "LoudnessExport PremierePro, totalDuration: " +
                       std::to_string(totalDuration) +
                       ", Estimated samples to process: " +
                       std::to_string(estimatedSamplesToProcess_) + "\n");
}

void LoudnessExport_PremiereProProcessor::processBlock(
    juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
  // If we are not performing a render, return
  if (!performingRender_ || buffer.getNumSamples() < 1) {
    return;
  }

  // calculate the current and next time based on the sample tally
  double currentTime = static_cast<double>(sampleTally_) / sampleRate_;
  sampleTally_ += buffer.getNumSamples();
  double nextTime = static_cast<double>(sampleTally_) / sampleRate_;

  // do not render
  if (currentTime < startTime_ || nextTime > endTime_) {
    return;
  }

  if (processedSamples_ <= estimatedSamplesToProcess_) {
    LOG_ANALYTICS(
        0, std::string(
               "PremierePro LoudnessExport process block is performRendering"));
    LOG_ANALYTICS(0, "Processing an additional " +
                         std::to_string(buffer.getNumSamples()) +
                         " samples. Already processed " +
                         std::to_string(processedSamples_) + " of " +
                         std::to_string(estimatedSamplesToProcess_));
    processedSamples_ += buffer.getNumSamples();

    for (auto& exportContainer : exportContainers_) {
      exportContainer.process(buffer);
    }
  } else {
    // Stop rendering if we are switching back to online mode
    // copy loudness values from the map to the repository
    LOG_ANALYTICS(0, "Setting performRendering_ to false \n");
    suspendProcessing(true);
    performingRender_ = false;
    for (auto& exportContainer : exportContainers_) {
      copyExportContainerDataToRepo(exportContainer);
    }
    LOG_ANALYTICS(0, "Copied loudness metadata to repository \n");
  }
}