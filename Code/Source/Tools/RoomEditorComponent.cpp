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

#include <LmbrCentral/Shape/BoxShapeComponentBus.h>

#include <Runtime/RoomComponent.h>
#include <Tools/RoomEditorComponent.h>
#include <Tools/RoomWallMaterialProviderBus.h>
#include <SparkyStudios/Audio/Amplitude/Assets/RoomWallMaterialDescriptor.h>

#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Debug/Profiler.h>
#include <AzCore/Component/Entity.h>

namespace SparkyStudios::Audio::Amplitude
{
    void RoomEditorComponent::Reflect(AZ::ReflectContext* rc)
    {
        RoomComponent::Reflect(rc);

        if (auto* const sc = azrtti_cast<AZ::SerializeContext*>(rc))
        {
            sc->Class<RoomEditorComponent, EditorComponentBase>()
                ->Version(1)
                ->Field("FrontWallMaterialId", &RoomEditorComponent::_frontWallMaterialId)
                ->Field("BackWallMaterialId", &RoomEditorComponent::_backWallMaterialId)
                ->Field("LeftWallMaterialId", &RoomEditorComponent::_leftWallMaterialId)
                ->Field("RightWallMaterialId", &RoomEditorComponent::_rightWallMaterialId)
                ->Field("TopWallMaterialId", &RoomEditorComponent::_topWallMaterialId)
                ->Field("BottomWallMaterialId", &RoomEditorComponent::_bottomWallMaterialId)
                ->Field("Dimensions", &RoomEditorComponent::_roomDimensions)
                ->Field("Gain", &RoomEditorComponent::_roomGain);

            if (AZ::EditContext* ec = sc->GetEditContext())
            {
                ec->Class<RoomEditorComponent>(
                      "Audio Room",
                      "The Audio Room component setups a virtual room for enhanced audio reflections using customizable materials for each "
                      "wall.")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "Audio")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"))
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ->Attribute(AZ::Edit::Attributes::NameLabelOverride, "Audio Room")
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default, &RoomEditorComponent::_roomGain, "Room Gain",
                        "The gain (volume) of the room effects.")
                    ->Attribute(AZ::Edit::Attributes::Min, 0.0f)
                    ->Attribute(AZ::Edit::Attributes::Max, 1.0f)
                    ->DataElement(
                        AZ::Edit::UIHandlers::ComboBox, &RoomEditorComponent::_frontWallMaterialId, "Front Wall Material",
                        "The material for the front wall of the room.")
                    ->Attribute(AZ::Edit::Attributes::EnumValues, &RoomEditorComponent::BuildSelectableRoomWallMaterialsList)
                    ->DataElement(
                        AZ::Edit::UIHandlers::ComboBox, &RoomEditorComponent::_backWallMaterialId, "Back Wall Material",
                        "The material for the back wall of the room.")
                    ->Attribute(AZ::Edit::Attributes::EnumValues, &RoomEditorComponent::BuildSelectableRoomWallMaterialsList)
                    ->DataElement(
                        AZ::Edit::UIHandlers::ComboBox, &RoomEditorComponent::_leftWallMaterialId, "Left Wall Material",
                        "The material for the left wall of the room.")
                    ->Attribute(AZ::Edit::Attributes::EnumValues, &RoomEditorComponent::BuildSelectableRoomWallMaterialsList)
                    ->DataElement(
                        AZ::Edit::UIHandlers::ComboBox, &RoomEditorComponent::_rightWallMaterialId, "Right Wall Material",
                        "The material for the right wall of the room.")
                    ->Attribute(AZ::Edit::Attributes::EnumValues, &RoomEditorComponent::BuildSelectableRoomWallMaterialsList)
                    ->DataElement(
                        AZ::Edit::UIHandlers::ComboBox, &RoomEditorComponent::_topWallMaterialId, "Top Wall Material",
                        "The material for the top wall of the room.")
                    ->Attribute(AZ::Edit::Attributes::EnumValues, &RoomEditorComponent::BuildSelectableRoomWallMaterialsList)
                    ->DataElement(
                        AZ::Edit::UIHandlers::ComboBox, &RoomEditorComponent::_bottomWallMaterialId, "Bottom Wall Material",
                        "The material for the bottom wall of the room.")
                    ->Attribute(AZ::Edit::Attributes::EnumValues, &RoomEditorComponent::BuildSelectableRoomWallMaterialsList);
            }
        }
    }

    void RoomEditorComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("AmplitudeAudio_RoomComponentService"));
    }

    void RoomEditorComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("AmplitudeAudio_RoomComponentService"));
    }

    void RoomEditorComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC_CE("AxisAlignedBoxShapeService"));
    }

    void RoomEditorComponent::Activate()
    {
        LmbrCentral::ShapeComponentNotificationsBus::Handler::BusConnect(GetEntityId());

        EditorComponentBase::Activate();
    }

    void RoomEditorComponent::Deactivate()
    {
        EditorComponentBase::Deactivate();

        LmbrCentral::ShapeComponentNotificationsBus::Handler::BusDisconnect(GetEntityId());
    }

    void RoomEditorComponent::BuildGameEntity(AZ::Entity* gameEntity)
    {
        gameEntity->CreateComponent<RoomComponent>(
            GetWallCoefficients(_frontWallMaterialId), GetWallCoefficients(_backWallMaterialId), GetWallCoefficients(_leftWallMaterialId),
            GetWallCoefficients(_rightWallMaterialId), GetWallCoefficients(_topWallMaterialId), GetWallCoefficients(_bottomWallMaterialId),
            _roomDimensions, _roomGain);
    }

    void RoomEditorComponent::OnShapeChanged(ShapeChangeReasons changeReason)
    {
        if (changeReason == ShapeChangeReasons::ShapeChanged)
            UpdateRoomBounds();
    }

    AZStd::vector<AZStd::pair<AZ::u32, AZStd::string>> RoomEditorComponent::BuildSelectableRoomWallMaterialsList() const
    {
        AZ_PROFILE_FUNCTION(Entity);

        RoomWallMaterialDescriptorList materials;
        EBUS_EVENT(RoomWallMaterialProviderRequestBus, GetRegisteredMaterials, materials);

        AZStd::vector<AZStd::pair<AZ::u32, AZStd::string>> selectableMaterials;
        selectableMaterials.reserve(materials.size());

        for (const auto& material : materials)
            selectableMaterials.push_back({ material.GetId(), material.GetName() });

        AZStd::sort_heap(
            selectableMaterials.begin(), selectableMaterials.end(),
            [](const auto& lhs, const auto& rhs)
            {
                return lhs.second < rhs.second;
            });

        return selectableMaterials;
    }

    AZStd::vector<float> RoomEditorComponent::GetWallCoefficients(AZ::u32 materialId) const
    {
        RoomWallMaterialDescriptorList materials;
        EBUS_EVENT(RoomWallMaterialProviderRequestBus, GetRegisteredMaterials, materials);

        for (const auto& material : materials)
        {
            if (material.GetId() == materialId)
                return material.GetCoefficients();
        }

        return {};
    }

    void RoomEditorComponent::UpdateRoomBounds()
    {
        EBUS_EVENT_ID_RESULT(_roomDimensions, GetEntityId(), LmbrCentral::BoxShapeComponentRequestsBus, GetBoxDimensions);
    }
} // namespace SparkyStudios::Audio::Amplitude
