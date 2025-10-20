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

#include <AzToolsFramework/ToolsComponents/EditorComponentBase.h>
#include <LmbrCentral/Shape/ShapeComponentBus.h>

#include <Runtime/RoomComponent.h>

namespace SparkyStudios::Audio::Amplitude
{
    class RoomEditorComponent
        : public AzToolsFramework::Components::EditorComponentBase
        , private LmbrCentral::ShapeComponentNotificationsBus::Handler
    {
    public:
        AZ_EDITOR_COMPONENT(RoomEditorComponent, "{965894D5-F691-449C-8C37-9C4B39B5458F}");

        static void Reflect(AZ::ReflectContext* rc);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);

        // AZ::Component
        void Activate() override;
        void Deactivate() override;

        // AzToolsFramework::Components::EditorComponentBase
        void BuildGameEntity(AZ::Entity* gameEntity) override;

        // LmbrCentral::ShapeComponentNotificationsBus
        void OnShapeChanged(ShapeChangeReasons changeReason) override;

    private:
        // RoomEditorComponent
        [[nodiscard]] AZStd::vector<AZStd::pair<AZ::u32, AZStd::string>> BuildSelectableRoomWallMaterialsList() const;
        [[nodiscard]] AZStd::vector<float> GetWallCoefficients(AZ::u32 materialId) const;
        void UpdateRoomBounds();

        AZ::u32 _frontWallMaterialId;
        AZ::u32 _backWallMaterialId;
        AZ::u32 _leftWallMaterialId;
        AZ::u32 _rightWallMaterialId;
        AZ::u32 _topWallMaterialId;
        AZ::u32 _bottomWallMaterialId;

        AZ::Vector3 _roomDimensions;

        AmReal32 _roomGain;
    };
} // namespace SparkyStudios::Audio::Amplitude
