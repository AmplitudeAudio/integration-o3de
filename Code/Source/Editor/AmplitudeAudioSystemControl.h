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

#include <IAudioInterfacesCommonData.h>

#include <IAudioSystemControl.h>

#include <SparkyStudios/Audio/Amplitude/Amplitude.h>
using namespace SparkyStudios::Audio::Amplitude;

namespace AudioControls
{
    enum EAmplitudeControlTypes
    {
        eAMCT_INVALID = 0,
        eAMCT_AMPLITUDE_EVENT = AUDIO_BIT(0),
        eAMCT_AMPLITUDE_RTPC = AUDIO_BIT(1),
        eAMCT_AMPLITUDE_SOUND_BANK = AUDIO_BIT(2),
        eAMCT_AMPLITUDE_SWITCH = AUDIO_BIT(3),
        eAMCT_AMPLITUDE_SWITCH_STATE = AUDIO_BIT(4),
        eAMCT_AMPLITUDE_BUS = AUDIO_BIT(5),
        eAMCT_AMPLITUDE_EFFECT = AUDIO_BIT(6),
    };

    //-------------------------------------------------------------------------------------------//
    class AmplitudeAudioSystemControl : public IAudioSystemControl
    {
    public:
        AmplitudeAudioSystemControl() = default;

        AmplitudeAudioSystemControl(const AZStd::string& name, CID id, TImplControlType type);

        ~AmplitudeAudioSystemControl() override = default;

        AmObjectID GetAmplitudeId() const;
        void SetAmplitudeId(AmObjectID id);

    private:
        AmObjectID _id;
    };
} // namespace AudioControls
