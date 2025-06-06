# Copyright 2025 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

eclipsa_add_test(test_base_repository RepositoryBase_test.cpp "data_structures;data_repository")
eclipsa_add_test(test_audio_element_repository AudioElementRepository_test.cpp "data_structures;data_repository")
eclipsa_add_test(test_room_setup_repository RoomSetupRepository_test.cpp "data_structures;data_repository")
eclipsa_add_test(test_file_export_repository FileExportRepository_test.cpp "data_structures;data_repository")
eclipsa_add_test(test_mix_presentation_repository MixPresentationRepository_test.cpp "data_structures;data_repository")
eclipsa_add_test(test_gain_repository MultiChannelGainRepository_test.cpp "data_structures;data_repository")
eclipsa_add_test(test_msplayback_repository MSPlaybackRepository_test.cpp "data_structures;data_repository")
eclipsa_add_test(test_audioelementspatiallayout_repository AudioElementSpatialLayoutRepository_test.cpp "data_structures;data_repository")
eclipsa_add_test(test_active_mp_repo ActiveMixPresentationRepository_test.cpp "data_structures;data_repository")
eclipsa_add_test(test_mix_presentation_solo_mute_repository MixPresentationSoloMuteRepository_test.cpp "data_structures;data_repository")
eclipsa_add_test(test_mix_presentation_loudness_repository MixPresentationLoudnessRepository_test.cpp "data_structures;data_repository")