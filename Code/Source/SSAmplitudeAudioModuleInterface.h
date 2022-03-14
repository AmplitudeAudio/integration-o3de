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

#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>

#include <IGem.h>

#include <SSAmplitudeAudioSystemComponent.h>

namespace SparkyStudios::Audio::Amplitude
{
    class SSAmplitudeAudioModuleInterface : public CryHooksModule
    {
    public:
        AZ_RTTI(SSAmplitudeAudioModuleInterface, "{ff431931-73d9-4112-b26a-0e3a7dc34d05}", CryHooksModule);
        AZ_CLASS_ALLOCATOR(SSAmplitudeAudioModuleInterface, AZ::SystemAllocator, 0);

        SSAmplitudeAudioModuleInterface()
            : CryHooksModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            // Add ALL components descriptors associated with this gem to m_descriptors.
            // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and
            // EditContext. This happens through the [MyComponent]::Reflect() function.
            m_descriptors.insert(
                m_descriptors.end(),
                {
                    SSAmplitudeAudioSystemComponent::CreateDescriptor(),
                });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<SSAmplitudeAudioSystemComponent>(),
            };
        }
    };
} // namespace SparkyStudios::Audio::Amplitude
