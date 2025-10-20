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

#include <Assets/RoomWallMaterialsAsset.h>
#include <Runtime/AmplitudeAudioSystemComponent.h>
#include <Tools/RoomWallMaterialProviderBus.h>

#include <AzFramework/Asset/AssetCatalogBus.h>
#include <AzToolsFramework/Entity/EditorEntityContextBus.h>

#include <IAudioSystemEditor.h>

namespace SparkyStudios::Audio::Amplitude
{
    /// System component for AmplitudeAudio editor
    class AmplitudeAudioEditorSystemComponent
        : public AmplitudeAudioSystemComponent
        , protected AudioControlsEditor::EditorImplPluginEventBus::Handler
        , protected AzToolsFramework::EditorEvents::Bus::Handler
        , private RoomWallMaterialProviderRequestBus::Handler
        , private AZ::Data::AssetBus::MultiHandler
        , private AzFramework::AssetCatalogEventBus::Handler
    {
        using BaseSystemComponent = AmplitudeAudioSystemComponent;

    public:
        AZ_COMPONENT_DECL(AmplitudeAudioEditorSystemComponent);

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

        static AZStd::optional<AZ::Data::Asset<AZ::Data::AssetData>> RetrieveRoomWallMaterials();

        // AmplitudeAudioEditorSystemComponent
        void AddAsset(const AZ::Data::Asset<AZ::Data::AssetData>& asset);

        // AZ::Component
        void Activate() override;
        void Deactivate() override;

        // AzToolsFramework::EditorEventsBus
        void NotifyRegisterViews() override;

        // RoomWallMaterialProviderRequestBus
        void GetRegisteredMaterialNames(RoomWallMaterialNameSet& names) const override;
        void GetRegisteredMaterials(RoomWallMaterialDescriptorList& materials) const override;
        RoomWallMaterial GetMaterial(const AZStd::string& name) const override;

        // AZ::Data::AssetBus::MultiHandler
        void OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset) override;
        void OnAssetReloaded(AZ::Data::Asset<AZ::Data::AssetData> asset) override;

        // AzFramework::AssetCatalogEventBus::Handler
        void OnCatalogLoaded(const char*) override;

        AZStd::unique_ptr<::AudioControls::IAudioSystemEditor> _editorImplPlugin;
        AZ::Data::Asset<RoomWallMaterialsAsset> _roomWallMaterialsAsset;
    };
} // namespace SparkyStudios::Audio::Amplitude
