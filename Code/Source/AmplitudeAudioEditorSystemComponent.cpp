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

#include <AmplitudeAudioEditorSystemComponent.h>
#include <AzCore/Serialization/SerializeContext.h>

#include <Editor/AmplitudeAudioSystemEditor.h>

namespace SparkyStudios::Audio::Amplitude
{
    void AmplitudeAudioEditorSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<AmplitudeAudioEditorSystemComponent, AmplitudeAudioSystemComponent>()->Version(0);
        }
    }

    AmplitudeAudioEditorSystemComponent::AmplitudeAudioEditorSystemComponent() = default;

    AmplitudeAudioEditorSystemComponent::~AmplitudeAudioEditorSystemComponent() = default;

    void AmplitudeAudioEditorSystemComponent::InitializeEditorImplPlugin()
    {
        m_editorImplPlugin.reset(new AmplitudeAudioSystemEditor());
    }

    void AmplitudeAudioEditorSystemComponent::ReleaseEditorImplPlugin()
    {
        m_editorImplPlugin.reset();
    }

    AudioControls::IAudioSystemEditor* AmplitudeAudioEditorSystemComponent::GetEditorImplPlugin()
    {
        return m_editorImplPlugin.get();
    }

    void AmplitudeAudioEditorSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        BaseSystemComponent::GetProvidedServices(provided);
        provided.push_back(AZ_CRC_CE("SSAmplitudeAudioEditorService"));
    }

    void AmplitudeAudioEditorSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        BaseSystemComponent::GetIncompatibleServices(incompatible);
        incompatible.push_back(AZ_CRC_CE("SSAmplitudeAudioEditorService"));
    }

    void AmplitudeAudioEditorSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        BaseSystemComponent::GetRequiredServices(required);
    }

    void AmplitudeAudioEditorSystemComponent::GetDependentServices(
        [[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        BaseSystemComponent::GetDependentServices(dependent);
    }

    void AmplitudeAudioEditorSystemComponent::Activate()
    {
        BaseSystemComponent::Activate();
        AzToolsFramework::EditorEvents::Bus::Handler::BusConnect();
        AudioControlsEditor::EditorImplPluginEventBus::Handler::BusConnect();
    }

    void AmplitudeAudioEditorSystemComponent::Deactivate()
    {
        AudioControlsEditor::EditorImplPluginEventBus::Handler::BusDisconnect();
        AzToolsFramework::EditorEvents::Bus::Handler::BusDisconnect();
        BaseSystemComponent::Deactivate();
    }
} // namespace SparkyStudios::Audio::Amplitude
