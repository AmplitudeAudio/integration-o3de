// Copyright (c) 2021-present Sparky Studios. All rights reserved.
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
#include <AzCore/Interface/Interface.h>

namespace SparkyStudios::Audio::Amplitude
{
    class AmplitudeAudioRequests
    {
    public:
        AZ_RTTI(AmplitudeAudioRequests, "{BA47A728-B462-4612-A2B0-CFA05117F47D}");
        virtual ~AmplitudeAudioRequests() = default;
        // Put your public methods here
    };

    class AmplitudeAudioBusTraits : public AZ::EBusTraits
    {
    public:
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
    };

    using AmplitudeAudioRequestBus = AZ::EBus<AmplitudeAudioRequests, AmplitudeAudioBusTraits>;
    using AmplitudeAudioInterface = AZ::Interface<AmplitudeAudioRequests>;
} // namespace SparkyStudios::Audio::Amplitude
