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

#include <AzCore/EBus/EBus.h>
#include <SparkyStudios/Audio/Amplitude/Assets/RoomWallMaterialDescriptor.h>

namespace SparkyStudios::Audio::Amplitude
{
    //! EBus is used to request registered room wall materials.
    class RoomWallMaterialProviderRequests
    {
    public:
        AZ_RTTI(RoomWallMaterialProviderRequests, "{DB064F4F-E618-4167-A686-BBCFC2B97EE6}")

        virtual ~RoomWallMaterialProviderRequests() = default;

        //! Gets the names of all registered wall materials.
        //!
        //! @param[out] names The names of the registered wall materials.
        virtual void GetRegisteredMaterialNames(RoomWallMaterialNameSet& names) const = 0;

        //! Gets all the registered wall materials.
        //!
        //! @param[out] materials The registered wall materials.
        virtual void GetRegisteredMaterials(RoomWallMaterialDescriptorList& materials) const = 0;

        //! Gets the wall material with the given name.
        //!
        //! @param[in] name The name of the wall material to get. If no wall material with the provided name is found,
        //! an invalid one will be returned.
        //!
        //! @return The found wall material, or an invalid one if not found.
        [[nodiscard]] virtual RoomWallMaterial GetMaterial(const AZStd::string& name) const = 0;
    };

    class RoomWallMaterialProviderBusTraits : public AZ::EBusTraits
    {
    public:
        // EBusTraits
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Multiple;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;

        //! allows multiple threads to call
        using MutexType = AZStd::recursive_mutex;
    };

    using RoomWallMaterialProviderRequestBus = AZ::EBus<RoomWallMaterialProviderRequests, RoomWallMaterialProviderBusTraits>;
    using RoomWallMaterialProviderInterface = AZ::Interface<RoomWallMaterialProviderRequests>;
} // namespace SparkyStudios::Audio::Amplitude
