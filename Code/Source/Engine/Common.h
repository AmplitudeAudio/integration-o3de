// Copyright (c) 2025-present Sparky Studios. All rights reserved.
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

#include <SparkyStudios/Audio/Amplitude/Math/CartesianCoordinateSystem.h>

#include <AzCore/Math/Quaternion.h>
#include <AzCore/Math/Vector3.h>
#include <AzCore/std/string/string_view.h>

namespace SparkyStudios::Audio::Amplitude
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Amplitude Audio Xml Element and Attributes Names
    namespace XmlTags
    {
        static constexpr char kEventTag[] = "AmplitudeEvent";
        static constexpr char kRtpcTag[] = "AmplitudeRtpc";
        static constexpr char kSwitchTag[] = "AmplitudeSwitch";
        static constexpr char kSwitchStateTag[] = "AmplitudeSwitchState";
        static constexpr char kFileTag[] = "AmplitudeFile";
        static constexpr char kEnvironmentTag[] = "AmplitudeEnvironment";

        static constexpr char kLocalizedAttribute[] = "amplitude_localized";
        static constexpr char kIdAttribute[] = "amplitude_id";
        static constexpr char kNameAttribute[] = "amplitude_name";
        static constexpr char kValueAttribute[] = "amplitude_value";
        static constexpr char kMultiplierAttribute[] = "atl_multiplier";
        static constexpr char kShiftAttribute[] = "atl_shift";
    } // namespace XmlTags
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    static constexpr char kInitBankFile[] = "init.ambank";
    static constexpr char kDefaultProjectBuildPath[] = "sounds/ambuild";
    static constexpr char kDefaultProjectSourcesPath[] = "sounds/amproject/sources";
    static constexpr char kDefaultEngineConfig[] = "pc.config.amconfig";

    static constexpr AZStd::string_view kProjectBuildPathSettingsRegistryKey = "/SparkyStudios/Audio/Amplitude/Settings/Project/build_path";
    static constexpr AZStd::string_view kProjectSourcesPathSettingsRegistryKey = "/SparkyStudios/Audio/Amplitude/Settings/Project/sources_path";
    static constexpr AZStd::string_view kEngineConfigSettingsRegistryKey = "/SparkyStudios/Audio/Amplitude/Settings/Runtime/engine_config";

    AZ_INLINE static const char* gMemoryManagerPools[]{ "Engine", "Amplimix", "SoundData", "Filtering", "Codec", "IO", "Default" };

    AZ_INLINE AmVector3 ATLVec3ToAmVec3(const AZ::Vector3& vec)
    {
        static const CartesianCoordinateSystem o3de(
            CartesianCoordinateSystem::Axis::PositiveX, CartesianCoordinateSystem::Axis::PositiveY,
            CartesianCoordinateSystem::Axis::PositiveZ);

        return CartesianCoordinateSystem::ConvertToDefault(AmVector3{ { vec.GetX(), vec.GetY(), vec.GetZ() } }, o3de);
    }

    AZ_INLINE AmQuaternion ATLQuatToAmQuat(const AZ::Quaternion& quat)
    {
        static const CartesianCoordinateSystem o3de(
            CartesianCoordinateSystem::Axis::PositiveX, CartesianCoordinateSystem::Axis::PositiveY,
            CartesianCoordinateSystem::Axis::PositiveZ);

        return CartesianCoordinateSystem::ConvertToDefault(AmQuaternion{ { quat.GetW(), quat.GetX(), quat.GetY(), quat.GetZ() } }, o3de);
    }

    AZ_INLINE Orientation ATLFwdUpToAmOrientation(const AZ::Vector3& fwd, const AZ::Vector3& up)
    {
        return { ATLVec3ToAmVec3(fwd), ATLVec3ToAmVec3(up) };
    }
} // namespace SparkyStudios::Audio::Amplitude
