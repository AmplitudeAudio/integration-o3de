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

#include <SparkyStudios/Audio/Amplitude/Assets/RoomWallMaterialDescriptor.h>

#include <AzCore/Serialization/EditContext.h>

namespace SparkyStudios::Audio::Amplitude
{
    void RoomWallMaterialDescriptor::Reflect(AZ::ReflectContext* rc)
    {
        if (auto* const sc = azrtti_cast<AZ::SerializeContext*>(rc))
        {
            sc->Class<RoomWallMaterialDescriptor>()
                ->Version(1)
                ->Field("Id", &RoomWallMaterialDescriptor::_id)
                ->Field("Name", &RoomWallMaterialDescriptor::_name)
                ->Field("Coefficients", &RoomWallMaterialDescriptor::_coefficients);

            if (auto* const ec = sc->GetEditContext())
            {
                ec->Class<RoomWallMaterialDescriptor>("Room Wall Material", "Configures absorption coefficients for a room wall.")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &RoomWallMaterialDescriptor::_name, "Name", "The material name.")
                    ->Attribute(AZ::Edit::Attributes::ChangeNotify, &RoomWallMaterialDescriptor::OnNameChanged)
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default, &RoomWallMaterialDescriptor::_coefficients, "Coefficients",
                        "The absorption coefficients for the material.")
                    ->Attribute(AZ::Edit::Attributes::MaxLength, 9);
            }
        }
    }

    RoomWallMaterialDescriptor::RoomWallMaterialDescriptor()
        : _id(0)
        , _name()
        , _coefficients()
    {
        _coefficients.resize(9);
    }

    RoomWallMaterialDescriptor::RoomWallMaterialDescriptor(AZStd::string name, Coefficients coefficients)
        : _name(AZStd::move(name))
        , _coefficients(AZStd::move(coefficients))
    {
        _id = AZ::Crc32(_name);
    }

    RoomWallMaterialDescriptor::RoomWallMaterialDescriptor(eRoomWallMaterialType materialType)
    {
        const RoomWallMaterial material(materialType);

        switch (materialType)
        {
        case eRoomWallMaterialType_AcousticTile:
            _name = "Acoustic Tile";
            break;
        case eRoomWallMaterialType_BrickPainted:
            _name = "Brick Painted";
            break;
        case eRoomWallMaterialType_CarpetOnConcrete:
            _name = "Carpet on Concrete";
            break;
        case eRoomWallMaterialType_ConcreteUnpainted:
            _name = "Concrete Unpainted";
            break;
        case eRoomWallMaterialType_FoamPanel:
            _name = "Foam Panel";
            break;
        case eRoomWallMaterialType_Glass:
            _name = "Glass";
            break;
        case eRoomWallMaterialType_GypsumBoard:
            _name = "Gypsum Board";
            break;
        case eRoomWallMaterialType_HeavyDrapes:
            _name = "Heavy Drapes";
            break;
        case eRoomWallMaterialType_IceSurface:
            _name = "Ice Surface";
            break;
        case eRoomWallMaterialType_Marble:
            _name = "Marble";
            break;
        case eRoomWallMaterialType_Metal:
            _name = "Metal";
            break;
        case eRoomWallMaterialType_PlasterSmooth:
            _name = "Plaster Smooth";
            break;
        case eRoomWallMaterialType_Transparent:
            _name = "Transparent";
            break;
        case eRoomWallMaterialType_WaterSurface:
            _name = "Water Surface";
            break;
        case eRoomWallMaterialType_Wood:
            _name = "Wood";
            break;
        case eRoomWallMaterialType_Custom:
            _name = "Custom";
            break;
        }

        _id = AZ::Crc32(_name);

        _coefficients.resize(9);
        for (int i = 0; i < 9; i++)
            _coefficients[i] = material.m_absorptionCoefficients[i];
    }

    RoomWallMaterial RoomWallMaterialDescriptor::ToMaterial() const
    {
        RoomWallMaterial material(eRoomWallMaterialType_Custom);

        for (int i = 0; i < 9; i++)
            material.m_absorptionCoefficients[i] = _coefficients[i];

        return material;
    }

    AZ::Crc32 RoomWallMaterialDescriptor::OnNameChanged()
    {
        _id = AZ::Crc32(_name);
        return AZ::Edit::PropertyRefreshLevels::EntireTree;
    }
} // namespace SparkyStudios::Audio::Amplitude
