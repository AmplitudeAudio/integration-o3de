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

#include <Runtime/RoomComponent.h>

#include <AzCore/Serialization/SerializeContext.h>

#include <Engine/Common.h>

namespace SparkyStudios::Audio::Amplitude
{
    AZ_COMPONENT_IMPL(RoomComponent, "RoomComponent", "{8224ED53-259D-471B-B0B9-642C0EFF5C45}", AZ::Component);

    void RoomComponent::Reflect(AZ::ReflectContext* rc)
    {
        if (auto* const sc = azrtti_cast<AZ::SerializeContext*>(rc))
        {
            if (sc->FindClassData(azrtti_typeid<RoomComponent>()) != nullptr)
                return;

            sc->Class<RoomComponent, AZ::Component>()
                ->Version(1)
                ->Field("FrontWallCoefficients", &RoomComponent::_frontWallCoefficients)
                ->Field("BackWallCoefficients", &RoomComponent::_backWallCoefficients)
                ->Field("LeftWallCoefficients", &RoomComponent::_leftWallCoefficients)
                ->Field("RightWallCoefficients", &RoomComponent::_rightWallCoefficients)
                ->Field("TopWallCoefficients", &RoomComponent::_topWallCoefficients)
                ->Field("BottomWallCoefficients", &RoomComponent::_bottomWallCoefficients)
                ->Field("Dimensions", &RoomComponent::_roomDimensions)
                ->Field("Gain", &RoomComponent::_roomGain);
        }
    }

    RoomComponent::RoomComponent(
        AZStd::vector<float> frontWallCoefficients,
        AZStd::vector<float> backWallCoefficients,
        AZStd::vector<float> leftWallCoefficients,
        AZStd::vector<float> rightWallCoefficients,
        AZStd::vector<float> topWallCoefficients,
        AZStd::vector<float> bottomWallCoefficients,
        const AZ::Vector3& roomDimensions,
        AmReal32 roomGain)
        : _frontWallCoefficients(AZStd::move(frontWallCoefficients))
        , _backWallCoefficients(AZStd::move(backWallCoefficients))
        , _leftWallCoefficients(AZStd::move(leftWallCoefficients))
        , _rightWallCoefficients(AZStd::move(rightWallCoefficients))
        , _topWallCoefficients(AZStd::move(topWallCoefficients))
        , _bottomWallCoefficients(AZStd::move(bottomWallCoefficients))
        , _roomDimensions(roomDimensions)
        , _roomGain(roomGain)
    {}

    void RoomComponent::Init()
    {
        EnsureRoom();
    }

    void RoomComponent::Activate()
    {
        RoomComponentRequestBus::Handler::BusConnect(GetEntityId());
        AZ::TransformNotificationBus::Handler::BusConnect(GetEntityId());
    }

    void RoomComponent::Deactivate()
    {
        AZ::TransformNotificationBus::Handler::BusDisconnect(GetEntityId());
        RoomComponentRequestBus::Handler::BusDisconnect(GetEntityId());
    }

    void RoomComponent::SetRoomGain(float gain)
    {
        _roomGain = gain;

        if (EnsureRoom())
            _room.SetGain(_roomGain);
    }

    void RoomComponent::OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world)
    {
        AZ_UNUSED(local)

        if (EnsureRoom())
            UpdateRoomTransform(world);
    }

    bool RoomComponent::EnsureRoom()
    {
        if (!_room.Valid() && amEngine->IsInitialized())
        {
            _room = amEngine->AddRoom(static_cast<AmRoomID>(GetEntityId()));
            _room.SetDimensions(ATLVec3ToAmVec3(_roomDimensions));
            _room.SetGain(_roomGain);
            _room.SetWallMaterials(
                ToMaterial(_leftWallCoefficients), ToMaterial(_rightWallCoefficients), ToMaterial(_bottomWallCoefficients),
                ToMaterial(_topWallCoefficients), ToMaterial(_frontWallCoefficients), ToMaterial(_backWallCoefficients));

            AZ::Transform transform = AZ::Transform::CreateIdentity();
            EBUS_EVENT_ID_RESULT(transform, GetEntityId(), AZ::TransformBus, GetWorldTM);

            UpdateRoomTransform(transform);
        }

        return _room.Valid();
    }

    RoomWallMaterial RoomComponent::ToMaterial(AZStd::vector<float> coefficients) const
    {
        RoomWallMaterial material(eRoomWallMaterialType_Custom);
        for (int i = 0; i < 9; i++)
            material.m_absorptionCoefficients[i] = coefficients[i];

        return material;
    }

    void RoomComponent::UpdateRoomTransform(const AZ::Transform& transform)
    {
        _room.SetLocation(ATLVec3ToAmVec3(transform.GetTranslation()));
        _room.SetOrientation(Orientation(ATLQuatToAmQuat(transform.GetRotation())));
    }
} // namespace SparkyStudios::Audio::Amplitude
