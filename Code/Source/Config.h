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
    static constexpr const char ProjectRootPath[] = "sounds/" AMPLITUDE_PROJECT_DIR_NAME "/";
    static constexpr const char AssetsRootPath[] = "sounds/" AMPLITUDE_ASSETS_DIR_NAME "/";
    static constexpr const char DefaultBanksPath[] = "sounds/" AMPLITUDE_ASSETS_DIR_NAME "/soundbanks/";

    static constexpr const char EngineConfigFile[] = "audio_config.json";
    static constexpr const char BusesConfigFile[] = "buses.json";
    static constexpr const char InitBankFile[] = "init.ambank";

    static constexpr const char ProjectFileExtension[] = ".json";
    static constexpr const char SoundBankFileExtension[] = ".ambank";
    static constexpr const char AssetBusFileExtension[] = ".ambus";
    static constexpr const char AssetCollectionFileExtension[] = ".amcollection";
    static constexpr const char AssetSoundFileExtension[] = ".amsound";
    static constexpr const char AssetEventFileExtension[] = ".amevent";
    static constexpr const char AssetEnvironmentFileExtension[] = ".amenv";
    static constexpr const char AssetAttenuationFileExtension[] = ".amattenuation";
    static constexpr const char AssetSwitchFileExtension[] = ".amswitch";
    static constexpr const char AssetSwitchContainerFileExtension[] = ".amswitchcontainer";
    static constexpr const char AssetRtpcFileExtension[] = ".amrtpc";
    static constexpr const char AssetMediaFileExtension[] = ".ams";

    // Project Folders
    static constexpr const char EventsFolder[] = "events";
    static constexpr const char RtpcFolder[] = "rtpc";
    static constexpr const char SoundBanksFolder[] = "soundbanks";
    static constexpr const char SoundsFolder[] = "sounds";
    static constexpr const char SwitchesFolder[] = "switches";
    static constexpr const char EffectsFolder[] = "effects";
} // namespace SparkyStudios::Audio::Amplitude
