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

#include <Editor/AmplitudeAudioLoader.h>

#include <AudioFileUtils.h>
#include <Config.h>
#include <Editor/AmplitudeAudioSystemEditor.h>
#include <IAudioSystemControl.h>
#include <IAudioSystemEditor.h>

#include <AzCore/IO/FileIO.h>
#include <AzCore/IO/IOUtils.h>

namespace SparkyStudios::Audio::Amplitude
{
    using namespace AudioControls;

    //-------------------------------------------------------------------------------------------//
    void AmplitudeAudioControlLoader::Load(AmplitudeAudioSystemEditor* audioSystemImpl)
    {
        m_audioSystemImpl = audioSystemImpl;
        const AZ::IO::FixedMaxPath projectFullPath{ m_audioSystemImpl->GetDataPath() };
        LoadControlsInFolder(AZ::IO::FixedMaxPath{ projectFullPath / EventsFolder }.Native(), eAMCT_AMPLITUDE_EVENT);
        LoadControlsInFolder(AZ::IO::FixedMaxPath{ projectFullPath / RtpcFolder }.Native(), eAMCT_AMPLITUDE_RTPC);
        LoadControlsInFolder(AZ::IO::FixedMaxPath{ projectFullPath / SwitchesFolder }.Native(), eAMCT_AMPLITUDE_SWITCH);
        LoadControlsInFolder(AZ::IO::FixedMaxPath{ projectFullPath / EffectsFolder }.Native(), eAMCT_AMPLITUDE_EFFECT);
        LoadSoundBanks(projectFullPath.Native(), SoundBanksFolder, false);
        LoadBuses(projectFullPath.Native());
    }

    //-------------------------------------------------------------------------------------------//
    void AmplitudeAudioControlLoader::LoadSoundBanks(
        const AZStd::string_view rootFolder, const AZStd::string_view subPath, bool isLocalized)
    {
        AZ::IO::FixedMaxPath searchPath(rootFolder);
        searchPath /= subPath;
        auto foundFiles = ::Audio::FindFilesInPath(searchPath.Native(), "*");

        for (const auto& filePath : foundFiles)
        {
            AZ_Assert(
                AZ::IO::FileIOBase::GetInstance()->Exists(filePath.c_str()), "FindFiles found file '%s' but FileIO says it doesn't exist!",
                filePath.c_str());

            // Read the asset into a memory buffer
            AZ::IO::FileIOStream ioStream;
            if (!ioStream.Open(filePath.Native().c_str(), AZ::IO::OpenMode::ModeRead))
            {
                AZ_Warning(
                    "SSAmplitudeAudio", false, "Asset loading for \"%s\" failed because the source file could not be opened.",
                    filePath.Native().c_str());

                return;
            }

            const AZ::IO::SizeType fileSize = ioStream.GetLength();
            AZStd::vector<char> fileBuffer(fileSize + 1);
            fileBuffer[fileSize] = '\0';
            size_t bytesRead = ioStream.Read(fileSize, fileBuffer.data());
            if (bytesRead != fileSize)
            {
                AZ_Warning("SSAmplitudeAudio", false, "File failed to read completely: %s", filePath.Native().c_str());
                return;
            }

            const AZStd::string controlData = AZStd::string(fileBuffer.begin(), fileBuffer.end());
            rapidjson::Document json;
            json.Parse(controlData.c_str());

            if (!json.HasParseError() && json.IsObject())
            {
                auto* control = LoadControl(json, eAMCT_AMPLITUDE_SOUND_BANK);
                if (control)
                {
                    control->SetLocalized(isLocalized);
                }
            }
        }
    }

    //-------------------------------------------------------------------------------------------//
    void AmplitudeAudioControlLoader::LoadBuses(const AZStd::string_view rootFolder)
    {
        AZ::IO::FixedMaxPath filePath(rootFolder);
        filePath /= Audio::Amplitude::BusesConfigFile;
        AZ_Assert(AZ::IO::FileIOBase::GetInstance()->Exists(filePath.c_str()), "File '%s' doesn't exist!", filePath.c_str());
        AZ::IO::PathView fileName = filePath.Filename();

        // Read the asset into a memory buffer
        AZ::IO::FileIOStream ioStream;
        if (!ioStream.Open(filePath.Native().c_str(), AZ::IO::OpenMode::ModeRead))
        {
            AZ_Warning(
                "SSAmplitudeAudio", false, "Asset loading for \"%s\" failed because the source file could not be opened.",
                filePath.Native().c_str());

            return;
        }

        const AZ::IO::SizeType fileSize = ioStream.GetLength();
        AZStd::vector<char> fileBuffer(fileSize + 1);
        fileBuffer[fileSize] = '\0';
        size_t bytesRead = ioStream.Read(fileSize, fileBuffer.data());
        if (bytesRead != fileSize)
        {
            AZ_Warning("SSAmplitudeAudio", false, "File failed to read completely: %s", filePath.Native().c_str());
            return;
        }

        const AZStd::string controlData = AZStd::string(fileBuffer.begin(), fileBuffer.end());
        rapidjson::Document json;
        json.Parse(controlData.c_str());

        if (!json.HasParseError() && json.HasMember("buses"))
        {
            for (const auto& bus : json["buses"].GetArray())
            {
                LoadControl(bus, eAMCT_AMPLITUDE_BUS);
            }
        }
        else
        {
            AZ_Error("SSAmplitudeAudio", false, AZStd::string::format("File failed to parse file: %s", filePath.Native().c_str()).c_str());
        }
    }

    //-------------------------------------------------------------------------------------------//
    void AmplitudeAudioControlLoader::LoadControlsInFolder(const AZStd::string_view folderPath, EAmplitudeControlTypes type)
    {
        auto foundFiles = ::Audio::FindFilesInPath(folderPath, "*");

        for (const auto& filePath : foundFiles)
        {
            AZ_Assert(
                AZ::IO::FileIOBase::GetInstance()->Exists(filePath.c_str()), "FindFiles found file '%s' but FileIO says it doesn't exist!",
                filePath.c_str());

            if (AZ::IO::FileIOBase::GetInstance()->IsDirectory(filePath.c_str()))
            {
                LoadControlsInFolder(filePath.Native(), type);
            }
            else
            {
                // Open the file, read, and call LoadControls with the JSON content...
                AZ_TracePrintf("AmplitudeAudioLoader", "Loading JSON from '%s'", filePath.c_str());

                // Read the asset into a memory buffer
                AZ::IO::FileIOStream ioStream;
                if (!ioStream.Open(filePath.Native().c_str(), AZ::IO::OpenMode::ModeRead))
                {
                    AZ_Warning(
                        "SSAmplitudeAudio", false, "Asset loading for \"%s\" failed because the source file could not be opened.",
                        filePath.Native().c_str());

                    return;
                }

                const AZ::IO::SizeType fileSize = ioStream.GetLength();
                AZStd::vector<char> fileBuffer(fileSize + 1);
                fileBuffer[fileSize] = '\0';
                size_t bytesRead = ioStream.Read(fileSize, fileBuffer.data());
                if (bytesRead != fileSize)
                {
                    AZ_Warning("SSAmplitudeAudio", false, "File failed to read completely: %s", filePath.Native().c_str());
                    return;
                }

                const AZStd::string controlData = AZStd::string(fileBuffer.begin(), fileBuffer.end());
                rapidjson::Document json;
                json.Parse(controlData.c_str());

                if (!json.HasParseError())
                {
                    LoadControl(json, type);
                }
                else
                {
                    AZ_Error("SSAmplitudeAudio", false, "File failed to parse file: %s", filePath.Native().c_str());
                }
            }
        }
    }

    //-------------------------------------------------------------------------------------------//
    AmplitudeAudioSystemControl* AmplitudeAudioControlLoader::LoadControl(
        const rapidjson::Value& json, EAmplitudeControlTypes type, IAudioSystemControl* parent)
    {
        if (json.HasMember("name"))
        {
            AZStd::string controlName(json["name"].GetString());
            if (type == eAMCT_AMPLITUDE_SOUND_BANK)
            {
                controlName += SoundBankFileExtension;
            }

            auto* control = static_cast<AmplitudeAudioSystemControl*>(m_audioSystemImpl->GetControlByName(controlName));
            if (!control)
            {
                control = static_cast<AmplitudeAudioSystemControl*>(
                    m_audioSystemImpl->CreateControl(SControlDef(controlName, type, false, parent)));
                control->SetAmplitudeId(json["id"].GetUint64());
            }

            control->SetPlaceholder(false);

            if (type == eAMCT_AMPLITUDE_SWITCH && json.HasMember("states"))
            {
                const auto& states = json["states"].GetArray();
                for (const auto& state : states)
                {
                    LoadControl(state, eAMCT_AMPLITUDE_SWITCH_STATE, control);
                }
            }

            return control;
        }

        return nullptr;
    }

    //-------------------------------------------------------------------------------------------//
    const AZStd::string& AmplitudeAudioControlLoader::GetLocalizationFolder() const
    {
        return m_localizationFolder;
    }
} // namespace SparkyStudios::Audio::Amplitude