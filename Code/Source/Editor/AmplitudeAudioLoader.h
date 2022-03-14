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

#include <ACETypes.h>
#include <AzCore/std/string/string_view.h>

#include <Editor/AmplitudeAudioSystemControl.h>

#include <AzCore/JSON/document.h>

#include <SparkyStudios/Audio/Amplitude/Amplitude.h>

namespace SparkyStudios::Audio::Amplitude
{
    using namespace AudioControls;

    class AmplitudeAudioSystemEditor;

    //-------------------------------------------------------------------------------------------//
    class AmplitudeAudioControlLoader
    {
    public:
        AmplitudeAudioControlLoader() = default;
        void Load(AmplitudeAudioSystemEditor* audioSystemImpl);
        const AZStd::string& GetLocalizationFolder() const;

    private:
        void LoadSoundBanks(const AZStd::string_view rootFolder, const AZStd::string_view subPath, bool isLocalized);
        void LoadBuses(const AZStd::string_view rootFolder);
        void LoadControlsInFolder(const AZStd::string_view folderPath, EAmplitudeControlTypes type);
        AmplitudeAudioSystemControl* LoadControl(
            const rapidjson::Value& json, EAmplitudeControlTypes type, IAudioSystemControl* parent = nullptr);

    private:
        AZStd::string m_localizationFolder;
        AmplitudeAudioSystemEditor* m_audioSystemImpl = nullptr;
    };
} // namespace SparkyStudios::Audio::Amplitude
