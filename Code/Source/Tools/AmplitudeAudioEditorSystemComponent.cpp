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

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Settings/SettingsRegistry.h>
#include <AzCore/Settings/SettingsRegistryMergeUtils.h>

#include <AzFramework/Asset/GenericAssetHandler.h>
#include <AzToolsFramework/API/ViewPaneOptions.h>

#include <UI/PropertyEditor/PropertyEditorAPI.h>

#include <Assets/RoomWallMaterialsAsset.h>
#include <Tools/AmplitudeAudioEditorSystemComponent.h>
#include <Tools/AmplitudeAudioWidget.h>
#include <Tools/Editor/AmplitudeAudioSystemEditor.h>

namespace SparkyStudios::Audio::Amplitude
{
    namespace
    {
        constexpr char kAssetCategoryName[] = "Amplitude Audio";

#pragma region RoomWallMaterialsAsset
        AzFramework::GenericAssetHandler<RoomWallMaterialsAsset>* sRoomWallMaterialsAssetHandler = nullptr;

        constexpr char kDefaultRoomWallMaterialsAssetFilePath[] = "Assets/AmplitudeAudio/RoomWallMaterials";

        constexpr char kRoomWallMaterialsAssetDisplayName[] = "Room Wall Materials";
        constexpr char kRoomWallMaterialsAssetExtension[] = "amwallmaterials";
#pragma endregion

        void RegisterAssetHandlers()
        {
            sRoomWallMaterialsAssetHandler = aznew AzFramework::GenericAssetHandler<RoomWallMaterialsAsset>(
                kRoomWallMaterialsAssetDisplayName, kAssetCategoryName, kRoomWallMaterialsAssetExtension);

            sRoomWallMaterialsAssetHandler->Register();
        }

        void UnregisterAssetHandlers()
        {
            if (sRoomWallMaterialsAssetHandler)
            {
                sRoomWallMaterialsAssetHandler->Unregister();
                delete sRoomWallMaterialsAssetHandler;
                sRoomWallMaterialsAssetHandler = nullptr;
            }
        }

        AZStd::optional<AZ::Data::Asset<AZ::Data::AssetData>> CreateDefaultRoomWallMaterialsAsset(
            const AZStd::string& fullTargetFilePath, const AZStd::string& relativePath)
        {
            AZ::IO::FileIOStream fileStream(fullTargetFilePath.c_str(), AZ::IO::OpenMode::ModeWrite);
            if (fileStream.IsOpen())
            {
                AZ::Data::AssetId assetId;
                EBUS_EVENT_RESULT(
                    assetId, AZ::Data::AssetCatalogRequestBus, GetAssetIdByPath, relativePath.c_str(),
                    azrtti_typeid<RoomWallMaterialsAsset>(), true);

                AZ::Data::Asset<AZ::Data::AssetData> newAsset =
                    AZ::Data::AssetManager::Instance().FindOrCreateAsset<RoomWallMaterialsAsset>(
                        assetId, AZ::Data::AssetLoadBehavior::Default);

                if (auto* asset = azrtti_cast<RoomWallMaterialsAsset*>(newAsset.GetData()))
                {
                    asset->mMaterials.push_back(RoomWallMaterialDescriptor(eRoomWallMaterialType_AcousticTile));
                    asset->mMaterials.push_back(RoomWallMaterialDescriptor(eRoomWallMaterialType_BrickPainted));
                    asset->mMaterials.push_back(RoomWallMaterialDescriptor(eRoomWallMaterialType_CarpetOnConcrete));
                    asset->mMaterials.push_back(RoomWallMaterialDescriptor(eRoomWallMaterialType_ConcreteUnpainted));
                    asset->mMaterials.push_back(RoomWallMaterialDescriptor(eRoomWallMaterialType_FoamPanel));
                    asset->mMaterials.push_back(RoomWallMaterialDescriptor(eRoomWallMaterialType_Glass));
                    asset->mMaterials.push_back(RoomWallMaterialDescriptor(eRoomWallMaterialType_GypsumBoard));
                    asset->mMaterials.push_back(RoomWallMaterialDescriptor(eRoomWallMaterialType_HeavyDrapes));
                    asset->mMaterials.push_back(RoomWallMaterialDescriptor(eRoomWallMaterialType_IceSurface));
                    asset->mMaterials.push_back(RoomWallMaterialDescriptor(eRoomWallMaterialType_Marble));
                    asset->mMaterials.push_back(RoomWallMaterialDescriptor(eRoomWallMaterialType_Metal));
                    asset->mMaterials.push_back(RoomWallMaterialDescriptor(eRoomWallMaterialType_PlasterSmooth));
                    asset->mMaterials.push_back(RoomWallMaterialDescriptor(eRoomWallMaterialType_Transparent));
                    asset->mMaterials.push_back(RoomWallMaterialDescriptor(eRoomWallMaterialType_WaterSurface));
                    asset->mMaterials.push_back(RoomWallMaterialDescriptor(eRoomWallMaterialType_Wood));

                    // Check it out in the source control system
                    EBUS_EVENT(
                        AzToolsFramework::SourceControlCommandBus, RequestEdit, fullTargetFilePath.c_str(), true,
                        [](bool /*success*/, const AzToolsFramework::SourceControlFileInfo& /*info*/)
                        {
                        });

                    // Save the asset into a file
                    if (sRoomWallMaterialsAssetHandler != nullptr && sRoomWallMaterialsAssetHandler->SaveAssetData(newAsset, &fileStream))
                        return newAsset;

                    AZ_Error("Amplitude", false, "Unable to save default wall materials to %s", fullTargetFilePath.c_str())
                }
            }

            return AZStd::nullopt;
        }
    } // namespace

    AZ_COMPONENT_IMPL(
        AmplitudeAudioEditorSystemComponent,
        "AmplitudeAudioEditorSystemComponent",
        "{473C6BE4-FD46-4CA5-AC46-4A583F6954ED}",
        BaseSystemComponent);

    void AmplitudeAudioEditorSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto* const serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<AmplitudeAudioEditorSystemComponent, AmplitudeAudioSystemComponent>()->Version(0);
        }
    }

    AmplitudeAudioEditorSystemComponent::AmplitudeAudioEditorSystemComponent() = default;

    AmplitudeAudioEditorSystemComponent::~AmplitudeAudioEditorSystemComponent() = default;

    void AmplitudeAudioEditorSystemComponent::InitializeEditorImplPlugin()
    {
        _editorImplPlugin.reset(new AmplitudeAudioSystemEditor());
    }

    void AmplitudeAudioEditorSystemComponent::ReleaseEditorImplPlugin()
    {
        _editorImplPlugin.reset();
    }

    AudioControls::IAudioSystemEditor* AmplitudeAudioEditorSystemComponent::GetEditorImplPlugin()
    {
        return _editorImplPlugin.get();
    }

    void AmplitudeAudioEditorSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        BaseSystemComponent::GetProvidedServices(provided);
        provided.push_back(AZ_CRC_CE("AmplitudeAudioEditorService"));
    }

    void AmplitudeAudioEditorSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        BaseSystemComponent::GetIncompatibleServices(incompatible);
        incompatible.push_back(AZ_CRC_CE("AmplitudeAudioEditorService"));
    }

    void AmplitudeAudioEditorSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        BaseSystemComponent::GetRequiredServices(required);
    }

    void AmplitudeAudioEditorSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        BaseSystemComponent::GetDependentServices(dependent);
    }

    AZStd::optional<AZ::Data::Asset<AZ::Data::AssetData>> AmplitudeAudioEditorSystemComponent::RetrieveRoomWallMaterials()
    {
        AZ::Data::AssetId resultAssetId;

        // Constructing the path to the asset
        const AZStd::string& assetExtension = kRoomWallMaterialsAssetExtension;

        // Use path relative to the asset root to avoid hardcoding full path in the configuration
        AZStd::string relativePath = kDefaultRoomWallMaterialsAssetFilePath;
        AzFramework::StringFunc::Path::ReplaceExtension(relativePath, assetExtension.c_str());

        // Try to find an already existing asset
        EBUS_EVENT_RESULT(
            resultAssetId, AZ::Data::AssetCatalogRequestBus, GetAssetIdByPath, relativePath.c_str(),
            azrtti_typeid<RoomWallMaterialsAsset>(), false);

        if (!resultAssetId.IsValid())
        {
            // No file for the asset was found, create a new one with default materials
            AZ::IO::Path fullPath;
            if (const auto* settingsRegistry = AZ::SettingsRegistry::Get(); settingsRegistry != nullptr)
                settingsRegistry->Get(fullPath.Native(), AZ::SettingsRegistryMergeUtils::FilePathKey_ProjectPath);

            fullPath /= kDefaultRoomWallMaterialsAssetFilePath;
            fullPath.ReplaceExtension(AZ::IO::PathView(assetExtension));

            if (auto newAsset = CreateDefaultRoomWallMaterialsAsset(fullPath.Native(), relativePath))
                return newAsset;

            AZ_Warning("Amplitude", false, "Failed to create default room wall materials asset at %s", fullPath.c_str())
        }
        else
        {
            AZ::Data::Asset<AZ::Data::AssetData> asset =
                AZ::Data::AssetManager::Instance().GetAsset<RoomWallMaterialsAsset>(resultAssetId, AZ::Data::NoLoad);
            return asset;
        }

        return AZStd::nullopt;
    }

    void AmplitudeAudioEditorSystemComponent::AddAsset(const AZ::Data::Asset<AZ::Data::AssetData>& asset)
    {
        if (asset.GetType() == azrtti_typeid<RoomWallMaterialsAsset>())
        {
            _roomWallMaterialsAsset = asset;
            AZ::Data::AssetBus::MultiHandler::BusConnect(asset.GetId());
        }

        EBUS_EVENT(AzToolsFramework::PropertyEditorGUIMessagesBus, RequestRefresh, AzToolsFramework::Refresh_AttributesAndValues);
    }

    void AmplitudeAudioEditorSystemComponent::Activate()
    {
        AzFramework::AssetCatalogEventBus::Handler::BusConnect();

        RegisterAssetHandlers();

        BaseSystemComponent::Activate();
        AzToolsFramework::EditorEvents::Bus::Handler::BusConnect();
        AudioControlsEditor::EditorImplPluginEventBus::Handler::BusConnect();
        RoomWallMaterialProviderRequestBus::Handler::BusConnect();
    }

    void AmplitudeAudioEditorSystemComponent::Deactivate()
    {
        RoomWallMaterialProviderRequestBus::Handler::BusDisconnect();
        AudioControlsEditor::EditorImplPluginEventBus::Handler::BusDisconnect();
        AzToolsFramework::EditorEvents::Bus::Handler::BusDisconnect();
        BaseSystemComponent::Deactivate();

        UnregisterAssetHandlers();

        AZ::Data::AssetBus::MultiHandler::BusDisconnect();
        AzFramework::AssetCatalogEventBus::Handler::BusDisconnect();
    }

    void AmplitudeAudioEditorSystemComponent::NotifyRegisterViews()
    {
        AzToolsFramework::ViewPaneOptions options;
        options.paneRect = QRect(100, 100, 500, 400);
        options.showOnToolsToolbar = true;
        options.toolbarIcon = ":/AmplitudeAudio/toolbar_icon.svg";

        // Register our custom widget as a dockable tool with the Editor under an Examples sub-menu
        AzToolsFramework::RegisterViewPane<AmplitudeAudioWidget>("AmplitudeAudio", "Examples", options);
    }

    void AmplitudeAudioEditorSystemComponent::GetRegisteredMaterialNames(RoomWallMaterialNameSet& names) const
    {
        names.clear();

        if (!_roomWallMaterialsAsset.IsReady())
            return;

        for (const auto& material : _roomWallMaterialsAsset->mMaterials)
            names.insert(material.GetName());
    }

    void AmplitudeAudioEditorSystemComponent::GetRegisteredMaterials(RoomWallMaterialDescriptorList& materials) const
    {
        materials.clear();

        if (!_roomWallMaterialsAsset.IsReady())
            return;

        for (const auto& material : _roomWallMaterialsAsset->mMaterials)
            materials.push_back(material);
    }

    RoomWallMaterial AmplitudeAudioEditorSystemComponent::GetMaterial(const AZStd::string& name) const
    {
        if (_roomWallMaterialsAsset.IsReady())
            for (const auto& material : _roomWallMaterialsAsset->mMaterials)
                if (material.GetName() == name)
                    return material.ToMaterial();

        return {};
    }

    void AmplitudeAudioEditorSystemComponent::OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset)
    {
        AddAsset(asset);
    }

    void AmplitudeAudioEditorSystemComponent::OnAssetReloaded(AZ::Data::Asset<AZ::Data::AssetData> asset)
    {
        AddAsset(asset);
    }

    void AmplitudeAudioEditorSystemComponent::OnCatalogLoaded(const char* string)
    {
        AZ_UNUSED(string);

        // Create or retrieve room wall materials from library
        if (const auto asset = RetrieveRoomWallMaterials(); asset.has_value())
        {
            _roomWallMaterialsAsset = asset.value();
            AZ::Data::AssetBus::MultiHandler::BusConnect(asset->GetId());
        }
    }
} // namespace SparkyStudios::Audio::Amplitude
