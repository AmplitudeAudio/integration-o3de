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

#include <SparkyStudios/Audio/Amplitude/Assets/RoomWallMaterialDescriptor.h>

#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/RTTI/RTTI.h>

namespace AZ
{
    class ReflectContext;
} // namespace AZ

namespace SparkyStudios::Audio::Amplitude
{
    //! Asset containing data about a navigation agent configuration.
    class RoomWallMaterialsAsset final : public AZ::Data::AssetData
    {
    public:
        AZ_RTTI(RoomWallMaterialsAsset, "{E69D4388-6DA7-429B-BF3D-08FF8D29BE51}", AZ::Data::AssetData)
        AZ_CLASS_ALLOCATOR(RoomWallMaterialsAsset, AZ::SystemAllocator, 0)

        static void Reflect(AZ::ReflectContext* rc);

        //! The list of material descriptors registered in the asset.
        RoomWallMaterialDescriptorList mMaterials;
    };
} // namespace SparkyStudios::Audio::Amplitude
