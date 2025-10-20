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

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TransformBus.h>

#include <SparkyStudios/Audio/Amplitude/Amplitude.h>
#include <SparkyStudios/Audio/Amplitude/Components/RoomComponentBus.h>

namespace SparkyStudios::Audio::Amplitude
{
    class RoomComponent
        : public AZ::Component
        , public RoomComponentRequestBus::Handler
        , private AZ::TransformNotificationBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(RoomComponent)

        static void Reflect(AZ::ReflectContext* rc);

        RoomComponent() = default;

        explicit RoomComponent(
            AZStd::vector<float> frontWallCoefficients,
            AZStd::vector<float> backWallCoefficients,
            AZStd::vector<float> leftWallCoefficients,
            AZStd::vector<float> rightWallCoefficients,
            AZStd::vector<float> topWallCoefficients,
            AZStd::vector<float> bottomWallCoefficients,
            const AZ::Vector3& roomDimensions,
            AmReal32 roomGain);

    protected:
        // AZ::Component
        void Init() override;
        void Activate() override;
        void Deactivate() override;

        // RoomComponentRequestBus
        void SetRoomGain(float gain) override;

        // AZ::TransformNotificationBus
        void OnTransformChanged(const AZ::Transform& /*local*/, const AZ::Transform& /*world*/) override;

    private:
        bool EnsureRoom();
        [[nodiscard]] RoomWallMaterial ToMaterial(AZStd::vector<float> coefficients) const;
        void UpdateRoomTransform(const AZ::Transform& transform);

        AZStd::vector<float> _frontWallCoefficients;
        AZStd::vector<float> _backWallCoefficients;
        AZStd::vector<float> _leftWallCoefficients;
        AZStd::vector<float> _rightWallCoefficients;
        AZStd::vector<float> _topWallCoefficients;
        AZStd::vector<float> _bottomWallCoefficients;

        AZ::Vector3 _roomDimensions;

        AmReal32 _roomGain;

        Room _room;
    };
} // namespace SparkyStudios::Audio::Amplitude
