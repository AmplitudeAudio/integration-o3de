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

#include <SSAmplitudeAudioSystemComponent.h>

#include <AzCore/Console/ILogger.h>
#include <AzCore/Memory/OSAllocator.h>
#include <AzCore/PlatformDef.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/EditContextConstants.inl>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Settings/SettingsRegistryMergeUtils.h>

#include <AzFramework/Platform/PlatformDefaults.h>

#include <AudioAllocators.h>

#include <Engine/AmplitudeAudioSystem.h>


namespace SparkyStudios::Audio::Amplitude
{
    void SSAmplitudeAudioSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<SSAmplitudeAudioSystemComponent, AZ::Component>()->Version(0);

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<SSAmplitudeAudioSystemComponent>(
                      "Sparky Studios - Audio - Amplitude Audio Gem", "Amplitude Audio implementation of the Audio Engine interfaces")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System"))
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true);
            }
        }
    }

    void SSAmplitudeAudioSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("AudioEngineService"));
    }

    void SSAmplitudeAudioSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("AudioEngineService"));
    }

    void SSAmplitudeAudioSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC("AudioSystemService"));
    }

    void SSAmplitudeAudioSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        dependent.push_back(AZ_CRC("AudioSystemService"));
    }

    SSAmplitudeAudioSystemComponent::SSAmplitudeAudioSystemComponent()
    {
        if (SSAmplitudeAudioInterface::Get() == nullptr)
        {
            SSAmplitudeAudioInterface::Register(this);
        }
    }

    SSAmplitudeAudioSystemComponent::~SSAmplitudeAudioSystemComponent()
    {
        if (SSAmplitudeAudioInterface::Get() == this)
        {
            SSAmplitudeAudioInterface::Unregister(this);
        }
    }

    bool SSAmplitudeAudioSystemComponent::Initialize()
    {
        bool result = false;

        // Check memory-related Wwise Cvars...
        //        const AZ::u64 memorySubpartitionSizes = Audio::Wwise::Cvars::s_StreamDeviceMemorySize
        //#if !defined(WWISE_RELEASE)
        //            + Audio::Wwise::Cvars::s_MonitorQueueMemorySize
        //#endif // !WWISE_RELEASE
        //            + Audio::Wwise::Cvars::s_CommandQueueMemorySize;
        //
        //        AZ_Assert(Audio::Wwise::Cvars::s_PrimaryMemorySize > memorySubpartitionSizes,
        //            "Wwise memory sizes of sub-categories add up to more than the primary memory pool size!")

        // Initialize memory block for Amplitude to use...
        if (!AZ::AllocatorInstance<::Audio::AudioImplAllocator>::IsReady())
        {
            // const size_t poolSize = Audio::Wwise::Cvars::s_PrimaryMemorySize << 10;

            ::Audio::AudioImplAllocator::Descriptor allocDesc;

            // Generic Allocator:
            allocDesc.m_allocationRecords = true;
            allocDesc.m_heap.m_numFixedMemoryBlocks = 1;
            allocDesc.m_heap.m_fixedMemoryBlocksByteSize[0] = 131072 << 10; // TODO

            allocDesc.m_heap.m_fixedMemoryBlocks[0] = AZ::AllocatorInstance<AZ::OSAllocator>::Get().Allocate(
                allocDesc.m_heap.m_fixedMemoryBlocksByteSize[0], allocDesc.m_heap.m_memoryBlockAlignment);

            AZ::AllocatorInstance<::Audio::AudioImplAllocator>::Create(allocDesc);
        }

        AZ::SettingsRegistryInterface::FixedValueString assetPlatform =
            AzFramework::OSPlatformToDefaultAssetPlatform(AZ_TRAIT_OS_PLATFORM_CODENAME);
        if (auto settingsRegistry = AZ::SettingsRegistry::Get(); settingsRegistry != nullptr)
        {
            AZ::SettingsRegistryMergeUtils::PlatformGet(
                *settingsRegistry, assetPlatform, AZ::SettingsRegistryMergeUtils::BootstrapSettingsRootKey, "assets");
        }

        _amplitudeEngine = AZStd::make_unique<::Audio::AmplitudeAudioSystem>(assetPlatform.c_str());
        if (_amplitudeEngine)
        {
            AZLOG_NOTICE("Amplitude AudioEngine created!");

            ::Audio::SystemRequest::Initialize initAudio;
            AZ::Interface<::Audio::IAudioSystem>::Get()->PushRequestBlocking(AZStd::move(initAudio));

            result = true;
        }
        else
        {
            AZLOG_NOTICE("Could not create Amplitude AudioEngine!");
        }

        return result;
    }

    void SSAmplitudeAudioSystemComponent::Release()
    {
        _amplitudeEngine.reset();

        if (AZ::AllocatorInstance<::Audio::AudioImplAllocator>::IsReady())
        {
            AZ::AllocatorInstance<::Audio::AudioImplAllocator>::Destroy();
        }
    }

    void SSAmplitudeAudioSystemComponent::Init()
    {
    }

    void SSAmplitudeAudioSystemComponent::Activate()
    {
        ::Audio::Gem::AudioEngineGemRequestBus::Handler::BusConnect();
        SSAmplitudeAudioRequestBus::Handler::BusConnect();
        AZ::TickBus::Handler::BusConnect();
    }

    void SSAmplitudeAudioSystemComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        SSAmplitudeAudioRequestBus::Handler::BusDisconnect();
        ::Audio::Gem::AudioEngineGemRequestBus::Handler::BusDisconnect();
    }

    void SSAmplitudeAudioSystemComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
    }
} // namespace SparkyStudios::Audio::Amplitude