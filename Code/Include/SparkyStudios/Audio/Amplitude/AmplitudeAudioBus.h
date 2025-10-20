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
#include <AzCore/Interface/Interface.h>

#include <SparkyStudios/Audio/Amplitude/Amplitude.h>

namespace SparkyStudios::Audio::Amplitude
{
    //! EBUS requests for the Amplitude Audio Gem
    class AmplitudeAudioRequests
    {
    public:
        AZ_RTTI(AmplitudeAudioRequests, "{446993A5-8876-492B-9F0E-D65739162B99}")
        virtual ~AmplitudeAudioRequests() = default;

        //! Gets the instance of the Amplitude Audio engine.
        virtual Engine* GetEngine() const = 0;
    };

    class AmplitudeAudioBusTraits : public AZ::EBusTraits
    {
    public:
        // AZ::EBusTraits
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
    };

    using AmplitudeAudioRequestBus = AZ::EBus<AmplitudeAudioRequests, AmplitudeAudioBusTraits>;
    using AmplitudeAudioInterface = AZ::Interface<AmplitudeAudioRequests>;
} // namespace SparkyStudios::Audio::Amplitude
