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

#include <SSAmplitudeAudioEditorSystemComponent.h>
#include <SSAmplitudeAudioModuleInterface.h>

#include <Builder/AmplitudeAudioControlBuilderComponent.h>

namespace SparkyStudios::Audio::Amplitude
{
    class SSAmplitudeAudioEditorModule : public SSAmplitudeAudioModuleInterface
    {
    public:
        AZ_RTTI(SSAmplitudeAudioEditorModule, "{2d70ff2e-7444-4f1c-a67c-13dd81b980cb}", SSAmplitudeAudioModuleInterface);
        AZ_CLASS_ALLOCATOR(SSAmplitudeAudioEditorModule, AZ::SystemAllocator, 0);

        SSAmplitudeAudioEditorModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            // Add ALL components descriptors associated with this gem to m_descriptors.
            // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and
            // EditContext. This happens through the [MyComponent]::Reflect() function.
            m_descriptors.insert(
                m_descriptors.end(),
                {
                    SSAmplitudeAudioEditorSystemComponent::CreateDescriptor(),
                    AmplitudeAudioControlBuilderComponent::CreateDescriptor(),
                });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         * Non-SystemComponents should not be added here
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<SSAmplitudeAudioEditorSystemComponent>(),
            };
        }
    };
} // namespace SparkyStudios::Audio::Amplitude

AZ_DECLARE_MODULE_CLASS(Gem_SSAmplitudeAudio, SparkyStudios::Audio::Amplitude::SSAmplitudeAudioEditorModule)
