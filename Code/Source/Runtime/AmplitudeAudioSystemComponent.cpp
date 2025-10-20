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

#include <Assets/RoomWallMaterialsAsset.h>
#include <Engine/AudioSystemImpl_Amplitude.h>
#include <Runtime/AmplitudeAudioSystemComponent.h>

#include <AzCore/Console/ILogger.h>
#include <AzCore/PlatformDef.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/EditContextConstants.inl>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Settings/SettingsRegistryMergeUtils.h>

#include <AzFramework/Platform/PlatformDefaults.h>

#include <AudioAllocators.h>

#include <Engine/Common.h>

namespace SparkyStudios::Audio::Amplitude
{
    AZ_COMPONENT_IMPL(
        AmplitudeAudioSystemComponent, "AmplitudeAudioSystemComponent", "{4CE34757-C09C-4E10-84AD-BB3341B11406}", AZ::Component)

    void AmplitudeAudioSystemComponent::Reflect(AZ::ReflectContext* rc)
    {
        RoomWallMaterialsAsset::Reflect(rc);

        if (auto* sc = azrtti_cast<AZ::SerializeContext*>(rc))
        {
            sc->Class<AmplitudeAudioSystemComponent, AZ::Component>()->Version(0);

            if (AZ::EditContext* ec = sc->GetEditContext())
            {
                ec->Class<AmplitudeAudioSystemComponent>("Amplitude Audio", "Amplitude Audio implementation of the Audio Engine interfaces")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true);
            }
        }
    }

    void AmplitudeAudioSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("AudioEngineService"));
    }

    void AmplitudeAudioSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("AudioEngineService"));
    }

    void AmplitudeAudioSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC("AudioSystemService"));
    }

    void AmplitudeAudioSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        dependent.push_back(AZ_CRC("AudioSystemService"));
    }

    AmplitudeAudioSystemComponent::AmplitudeAudioSystemComponent()
    {
        if (AmplitudeAudioInterface::Get() == nullptr)
            AmplitudeAudioInterface::Register(this);
    }

    AmplitudeAudioSystemComponent::~AmplitudeAudioSystemComponent()
    {
        if (AmplitudeAudioInterface::Get() == this)
            AmplitudeAudioInterface::Unregister(this);
    }

    bool AmplitudeAudioSystemComponent::Initialize()
    {
        bool result = false;

        AZ::SettingsRegistryInterface::FixedValueString projectBuildPath;
        AZ::SettingsRegistryInterface::FixedValueString engineConfig;
        AZ::SettingsRegistryInterface::FixedValueString assetPlatform =
            AzFramework::OSPlatformToDefaultAssetPlatform(AZ_TRAIT_OS_PLATFORM_CODENAME);

        if (auto* const settingsRegistry = AZ::SettingsRegistry::Get(); settingsRegistry != nullptr)
        {
            settingsRegistry->Get(projectBuildPath, kProjectBuildPathSettingsRegistryKey);
            settingsRegistry->Get(engineConfig, kEngineConfigSettingsRegistryKey);

            if (assetPlatform.empty())
            {
                AZ::SettingsRegistryMergeUtils::PlatformGet(
                    *settingsRegistry, assetPlatform, AZ::SettingsRegistryMergeUtils::BootstrapSettingsRootKey, "assets");
            }
        }

        if (projectBuildPath.empty())
        {
            // Defaults to predefined path
            projectBuildPath = kDefaultProjectBuildPath;
        }

        if (engineConfig.empty())
        {
            // Defaults to predefined config
            engineConfig = kDefaultEngineConfig;
        }

        _amplitudeEngine = AZStd::make_unique<::Audio::CAudioSystemImpl_Amplitude>(projectBuildPath.c_str(), engineConfig.c_str());

        if (_amplitudeEngine)
        {
            AZLOG_INFO("Amplitude Audio Engine created!")

            ::Audio::SystemRequest::Initialize initAudioRequest;
            AZ::Interface<::Audio::IAudioSystem>::Get()->PushRequestBlocking(AZStd::move(initAudioRequest));

            result = true;
        }
        else
        {
            AZLOG_ERROR("Could not create Amplitude Audio Engine!")
        }

        return result;
    }

    void AmplitudeAudioSystemComponent::Release()
    {
        _amplitudeEngine.reset();
    }

    Engine* AmplitudeAudioSystemComponent::GetEngine() const
    {
        return amEngine;
    }

    void AmplitudeAudioSystemComponent::Init()
    {}

    void AmplitudeAudioSystemComponent::Activate()
    {
        ::Audio::Gem::EngineRequestBus::Handler::BusConnect();
        AmplitudeAudioRequestBus::Handler::BusConnect();
    }

    void AmplitudeAudioSystemComponent::Deactivate()
    {
        AmplitudeAudioRequestBus::Handler::BusDisconnect();
        ::Audio::Gem::EngineRequestBus::Handler::BusDisconnect();
    }
} // namespace SparkyStudios::Audio::Amplitude
