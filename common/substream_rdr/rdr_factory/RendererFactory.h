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
#include <data_structures/src/AudioElement.h>

#include "substream_rdr/substream_rdr_utils/Speakers.h"

class Renderer;

/**
 * @brief Create a renderer object to correctly handle substream rendering.
 *
 * @param inputLayout Input channel positioning within buffer.
 * @param playbackLayout Playback layout the input stream is to be rendered to.
 * @return std::unique_ptr<Renderer>
 */
std::unique_ptr<Renderer> createRenderer(
    const Speakers::AudioElementSpeakerLayout inputLayout,
    const Speakers::AudioElementSpeakerLayout playbackLayout,
    const int numSamples = 0, const int sampleRate = 48e3);