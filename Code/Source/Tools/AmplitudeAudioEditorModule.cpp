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

#include <Tools/AmplitudeAudioEditorSystemComponent.h>
#include <Tools/RoomEditorComponent.h>

static void InitAmplitudeAudioResources()
{
    // We must register our Qt resources (.qrc file) since this is being loaded from a separate module (gem)
    Q_INIT_RESOURCE(AmplitudeAudio);
}

namespace SparkyStudios::Audio::Amplitude
{
    class AmplitudeAudioEditorModule : public AmplitudeAudioModuleInterface
    {
    public:
        AZ_RTTI(AmplitudeAudioEditorModule, "{DF9CDD90-F992-4628-B222-660D1CF8AB8A}", AmplitudeAudioModuleInterface);
        AZ_CLASS_ALLOCATOR(AmplitudeAudioEditorModule, AZ::SystemAllocator);

        AmplitudeAudioEditorModule()
        {
            InitAmplitudeAudioResources();

            m_descriptors.insert(
                m_descriptors.end(),
                {
                    AmplitudeAudioEditorSystemComponent::CreateDescriptor(),

                    RoomEditorComponent::CreateDescriptor(),
                });
        }

        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<AmplitudeAudioEditorSystemComponent>(),
            };
        }
    };
} // namespace SparkyStudios::Audio::Amplitude

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME, _Editor), SparkyStudios::Audio::Amplitude::AmplitudeAudioEditorModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_AmplitudeAudio_Editor, SparkyStudios::Audio::Amplitude::AmplitudeAudioEditorModule)
#endif
