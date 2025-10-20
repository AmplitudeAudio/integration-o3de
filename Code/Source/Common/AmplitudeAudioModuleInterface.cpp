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

#include <Common/AmplitudeAudioModuleInterface.h>
#include <Runtime/AmplitudeAudioSystemComponent.h>
#include <Runtime/RoomComponent.h>

#include <AzCore/Memory/Memory.h>

namespace SparkyStudios::Audio::Amplitude
{
    AZ_TYPE_INFO_WITH_NAME_IMPL(AmplitudeAudioModuleInterface, "AmplitudeAudioModuleInterface", "{B8EB2413-5B1B-49B3-A8B1-D9E65AAA69E9}")
    AZ_RTTI_NO_TYPE_INFO_IMPL(AmplitudeAudioModuleInterface, CryHooksModule)
    AZ_CLASS_ALLOCATOR_IMPL(AmplitudeAudioModuleInterface, AZ::SystemAllocator)

    AmplitudeAudioModuleInterface::AmplitudeAudioModuleInterface()
        : CryHooksModule()
    {
        m_descriptors.insert(
            m_descriptors.end(),
            {
                AmplitudeAudioSystemComponent::CreateDescriptor(),

                RoomComponent::CreateDescriptor(),
            });
    }

    AZ::ComponentTypeList AmplitudeAudioModuleInterface::GetRequiredSystemComponents() const
    {
        return AZ::ComponentTypeList{
            azrtti_typeid<AmplitudeAudioSystemComponent>(),
        };
    }
} // namespace SparkyStudios::Audio::Amplitude
