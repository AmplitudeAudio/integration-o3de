// Copyright (c) 2021-present Sparky Studios. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#define AMPLITUDE_ASSETS_DIR_NAME "amplitude_assets"
#define AMPLITUDE_PROJECT_DIR_NAME "amplitude_project"

namespace SparkyStudios::Audio::Amplitude
{
    static constexpr char kProjectRootPath[] = "sounds/" AMPLITUDE_PROJECT_DIR_NAME "/";
    static constexpr char kAssetsRootPath[] = "sounds/" AMPLITUDE_ASSETS_DIR_NAME "/";
    static constexpr char kDefaultBanksPath[] = "sounds/" AMPLITUDE_ASSETS_DIR_NAME "/soundbanks/";

    static constexpr char kEngineConfigFile[] = "audio_config.json";
    static constexpr char kBusesConfigFile[] = "buses.json";
    static constexpr char kInitBankFile[] = "init.ambank";

    static constexpr char kProjectFileExtension[] = ".json";
    static constexpr char kSoundBankFileExtension[] = ".ambank";
    static constexpr char kAssetBusFileExtension[] = ".ambus";
    static constexpr char kAssetCollectionFileExtension[] = ".amcollection";
    static constexpr char kAssetSoundFileExtension[] = ".amsound";
    static constexpr char kAssetEventFileExtension[] = ".amevent";
    static constexpr char kAssetEnvironmentFileExtension[] = ".amenv";
    static constexpr char kAssetAttenuationFileExtension[] = ".amattenuation";
    static constexpr char kAssetSwitchFileExtension[] = ".amswitch";
    static constexpr char kAssetSwitchContainerFileExtension[] = ".amswitchcontainer";
    static constexpr char kAssetRtpcFileExtension[] = ".amrtpc";
    static constexpr char kAssetMediaFileExtension[] = ".ams";

    // Project Folders
    static constexpr char kEventsFolder[] = "events";
    static constexpr char kRtpcFolder[] = "rtpc";
    static constexpr char kSoundBanksFolder[] = "soundbanks";
    static constexpr char kSoundsFolder[] = "sounds";
    static constexpr char kSwitchesFolder[] = "switches";
    static constexpr char kEffectsFolder[] = "effects";
} // namespace SparkyStudios::Audio::Amplitude
