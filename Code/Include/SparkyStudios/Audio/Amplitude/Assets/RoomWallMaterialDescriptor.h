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

#include <AzCore/Serialization/SerializeContext.h>
#include <SparkyStudios/Audio/Amplitude/Core/Room.h>

namespace SparkyStudios::Audio::Amplitude
{
    //! Stores data about a single wall material (name and coefficients).
    //!
    //! This class is mainly used by the RoomWallMaterialsAsset to describe
    //! each entry of the collection in the asset.
    class RoomWallMaterialDescriptor final
    {
    public:
        using List = AZStd::vector<AZStd::pair<AZ::u32, AZStd::string>>;
        using Coefficients = AZStd::vector<float>;

        AZ_RTTI(RoomWallMaterialDescriptor, "{60986BF9-0D24-4CB9-BE6D-510BB040DD89}")
        AZ_CLASS_ALLOCATOR(RoomWallMaterialDescriptor, AZ::SystemAllocator, 0)

        static void Reflect(AZ::ReflectContext* rc);

        //! Creates an empty descriptor.
        RoomWallMaterialDescriptor();

        //! Creates a new room wall material descriptor with a custom name and coefficients.
        //! @param name The name of the room wall material.
        //! @param coefficients The material absorption coefficients.
        explicit RoomWallMaterialDescriptor(AZStd::string name, Coefficients coefficients);

        //! Creates a new room material descriptor from a predefined material type.
        //! @param materialType The predefined material type from the SDK.
        explicit RoomWallMaterialDescriptor(eRoomWallMaterialType materialType);

        bool operator==(const RoomWallMaterialDescriptor& rhs) const;

        bool operator!=(const RoomWallMaterialDescriptor& rhs) const;

        bool operator<(const RoomWallMaterialDescriptor& rhs) const;

        //! Converts this descriptor to an Amplitude material.
        [[nodiscard]] RoomWallMaterial ToMaterial() const;

        //! Gets the ID of this descriptor.
        [[nodiscard]] AZ_FORCE_INLINE const AZ::Crc32& GetId() const
        {
            return _id;
        }

        //! Gets the name of the material.
        [[nodiscard]] AZ_FORCE_INLINE const AZStd::string& GetName() const
        {
            return _name;
        }

        //! Get the absorption coefficients of this material.
        AZ_FORCE_INLINE const Coefficients& GetCoefficients() const
        {
            return _coefficients;
        }

    private:
        AZ::Crc32 OnNameChanged();

        AZ::Crc32 _id;
        AZStd::string _name;
        Coefficients _coefficients;
    };

    typedef AZStd::unordered_set<AZStd::string> RoomWallMaterialNameSet;
    typedef AZStd::vector<RoomWallMaterialDescriptor> RoomWallMaterialDescriptorList;

    AZ_INLINE bool RoomWallMaterialDescriptor::operator==(const RoomWallMaterialDescriptor& rhs) const
    {
        return _id == rhs._id;
    }

    AZ_INLINE bool RoomWallMaterialDescriptor::operator!=(const RoomWallMaterialDescriptor& rhs) const
    {
        return !(*this == rhs);
    }

    AZ_INLINE bool RoomWallMaterialDescriptor::operator<(const RoomWallMaterialDescriptor& rhs) const
    {
        return rhs._id < _id;
    }
} // namespace SparkyStudios::Audio::Amplitude
