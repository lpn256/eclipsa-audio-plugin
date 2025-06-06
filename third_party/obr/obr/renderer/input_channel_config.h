/*
 * Copyright (c) 2025 Google LLC
 *
 * This source code is subject to the terms of the BSD 3-Clause Clear License,
 * which you can find in the LICENSE file, and the Open Binaural Renderer
 * Patent License 1.0, which you can find in the PATENTS file.
 */

#ifndef OBR_INPUT_CHANNEL_CONFIG_H_
#define OBR_INPUT_CHANNEL_CONFIG_H_

#include <cstddef>
#include <string>

namespace obr {

/*!\brief Base class for input channel configurations of different types of
 * audio elements. */
class InputChannelConfigBase {
 public:
  /*!\brief Constructor.
   *
   * \param id Identifier of the input channel.
   */
  explicit InputChannelConfigBase(const std::string& id)
      : id_(id), input_channel_index_(0) {}

  /*!\brief Destructor. */
  virtual ~InputChannelConfigBase() = default;

  /*!\brief Returns the identifier of the input channel.
   *
   * \return Identifier of the input channel.
   */
  std::string GetID() { return id_; }

  /*!\brief Sets the index of the input channel.
   *
   * \param index Index of the input channel.
   */
  void SetChannelIndex(size_t index) { input_channel_index_ = index; }

  /*!\brief Returns the index of the input channel.
   *
   * \return Index of the input channel.
   */
  size_t GetChannelIndex() const { return input_channel_index_; }

 private:
  std::string id_;
  size_t input_channel_index_;
};

/*!\brief Configuration of an ambisonic scene input channel. */
class AmbisonicSceneInputChannel : public InputChannelConfigBase {
 public:
  /*!\brief Constructor.
   *
   * \param id Identifier of the input channel.
   */
  explicit AmbisonicSceneInputChannel(const std::string& id)
      : InputChannelConfigBase(id) {}

 private:
};

/*!\brief Configuration of a loudspeaker layout input channel. */
class LoudspeakerLayoutInputChannel : public InputChannelConfigBase {
 public:
  /*!\brief Constructor.
   *
   * \param id Name of the input channel.
   * \param azimuth Azimuth in degrees.
   * \param elevation Elevation in degrees.
   * \param distance Distance in meters.
   * \param is_LFE Flag indicating if the channel is an LFE channel.
   */
  LoudspeakerLayoutInputChannel(const std::string& id, float azimuth,
                                float elevation, float distance, bool is_LFE)
      : InputChannelConfigBase(id),
        azimuth(azimuth),
        elevation(elevation),
        distance(distance),
        is_LFE(is_LFE) {}

  float GetAzimuth() const { return azimuth; }
  float GetElevation() const { return elevation; }
  float GetDistance() const { return distance; }
  bool GetIsLFE() const { return is_LFE; }

 private:
  const float azimuth;
  const float elevation;
  const float distance;
  const bool is_LFE;
};

/*!\brief Configuration of an audio object input channel. */
class AudioObjectInputChannel : public InputChannelConfigBase {
 public:
  /*!\brief Constructor.
   *
   * \param id Identifier of the input channel.
   * \param azimuth Azimuth in degrees.
   * \param elevation Elevation in degrees.
   * \param distance Distance in meters.
   */
  AudioObjectInputChannel(const std::string& id, float azimuth, float elevation,
                          float distance)
      : InputChannelConfigBase(id),
        gain_(1.0f),
        azimuth_(azimuth),
        elevation_(elevation),
        distance_(distance) {}

  float GetGain() const { return gain_; }
  float GetAzimuth() const { return azimuth_; }
  float GetElevation() const { return elevation_; }
  float GetDistance() const { return distance_; }

  void SetGain(float gain) { gain_ = gain; }
  void SetAzimuth(float azimuth) { azimuth_ = azimuth; }
  void SetElevation(float elevation) { elevation_ = elevation; }
  void SetDistance(float distance) { distance_ = distance; }

 private:
  float gain_;
  float azimuth_;
  float elevation_;
  float distance_;
};

}  // namespace obr

#endif  // OBR_INPUT_CHANNEL_CONFIG_H_
