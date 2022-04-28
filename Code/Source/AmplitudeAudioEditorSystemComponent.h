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

#include <AmplitudeAudioSystemComponent.h>

#include <AzToolsFramework/Entity/EditorEntityContextBus.h>

#include <IAudioSystemEditor.h>

namespace SparkyStudios::Audio::Amplitude
{
    /// System component for the Amplitude Audio Editor Gem.
    class AmplitudeAudioEditorSystemComponent
        : public AmplitudeAudioSystemComponent
        , protected AudioControlsEditor::EditorImplPluginEventBus::Handler
        , private AzToolsFramework::EditorEvents::Bus::Handler
    {
        using BaseSystemComponent = AmplitudeAudioSystemComponent;

    public:
        AZ_COMPONENT(AmplitudeAudioEditorSystemComponent, "{d8ff9eb7-b0f2-4a1c-b481-4308db435610}", BaseSystemComponent);
        static void Reflect(AZ::ReflectContext* context);

        AmplitudeAudioEditorSystemComponent();
        ~AmplitudeAudioEditorSystemComponent() override;

    protected:
        // AudioControlsEditor::EditorImplPluginEventBus interface implementation
        void InitializeEditorImplPlugin() override;
        void ReleaseEditorImplPlugin() override;
        AudioControls::IAudioSystemEditor* GetEditorImplPlugin() override;

    private:
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        // AZ::Component
        void Activate() override;
        void Deactivate() override;

        AZStd::unique_ptr<::AudioControls::IAudioSystemEditor> m_editorImplPlugin;
    };
} // namespace SparkyStudios::Audio::Amplitude
