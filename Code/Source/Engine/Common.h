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

#include <SparkyStudios/Audio/Amplitude/Core/Memory.h>
#include <SparkyStudios/Audio/Amplitude/Core/Version.h>

namespace SparkyStudios::Audio::Amplitude
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Amplitude Xml Element Names
    namespace XmlTags
    {
        static constexpr const char* EventTag = "AmplitudeEvent";
        static constexpr const char* RtpcTag = "AmplitudeRtpc";
        static constexpr const char* SwitchTag = "AmplitudeSwitch";
        static constexpr const char* SwitchStateTag = "AmplitudeSwitchState";
        static constexpr const char* StateTag = "AmplitudeState";
        static constexpr const char* FileTag = "AmplitudeFile";
        static constexpr const char* BusTag = "AmplitudeBus";
        static constexpr const char* EnvironmentTag = "AmplitudeEnvironment";

        static constexpr const char* LocalizedAttribute = "amplitude_localized";
        static constexpr const char* IdAttribute = "amplitude_id";
        static constexpr const char* NameAttribute = "amplitude_name";
        static constexpr const char* ValueAttribute = "amplitude_value";
        static constexpr const char* MultiplierAttribute = "atl_multiplier";
        static constexpr const char* ShiftAttribute = "atl_shift";
    } // namespace XmlTags

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    inline static const char* MemoryManagerPools[]{ "Engine", "Amplimix", "SoundData", "Filtering", "Codec" };

    inline hmm_vec3 ATLVec3ToAmVec3(AZ::Vector3 vec)
    {
        return AM_Vec3(vec.GetX(), vec.GetY(), vec.GetZ());
    }
} // namespace SparkyStudios::Audio::Amplitude