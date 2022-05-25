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

#include <AmplitudeAudioEditorSystemComponent.h>
#include <AmplitudeAudioModuleInterface.h>

#include <Builder/AmplitudeAudioControlBuilderComponent.h>

namespace SparkyStudios::Audio::Amplitude
{
    class AmplitudeAudioEditorModule : public AmplitudeAudioModuleInterface
    {
    public:
        AZ_RTTI(AmplitudeAudioEditorModule, "{8A8A1807-B917-4DC7-9001-F3F156EA2180}", AmplitudeAudioModuleInterface);
        AZ_CLASS_ALLOCATOR(AmplitudeAudioEditorModule, AZ::SystemAllocator, 0);

        AmplitudeAudioEditorModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            // Add ALL components descriptors associated with this gem to m_descriptors.
            // This will associate the AzTypeInfo information for the components with the SerializeContext, BehaviorContext and
            // EditContext. This happens through the [MyComponent]::Reflect() function.
            m_descriptors.insert(
                m_descriptors.end(),
                {
                    AmplitudeAudioEditorSystemComponent::CreateDescriptor(),
                    AmplitudeAudioControlBuilderComponent::CreateDescriptor(),
                });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         * Non-SystemComponents should not be added here
         */
        [[nodiscard]] AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<AmplitudeAudioEditorSystemComponent>(),
            };
        }
    };
} // namespace SparkyStudios::Audio::Amplitude

AZ_DECLARE_MODULE_CLASS(Gem_Amplitude_Editor, SparkyStudios::Audio::Amplitude::AmplitudeAudioEditorModule)
