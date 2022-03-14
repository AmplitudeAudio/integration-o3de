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

#include <AzCore/Component/Component.h>
#include <Builder/AmplitudeAudioControlBuilderWorker.h>

namespace SparkyStudios::Audio::Amplitude
{
    class AmplitudeAudioControlBuilderComponent
        : public AZ::Component
    {
    public:
        AZ_COMPONENT(AmplitudeAudioControlBuilderComponent, "{7F99877F-98E0-4607-BF1D-AC69C31E5170}");

        AmplitudeAudioControlBuilderComponent() = default;
        ~AmplitudeAudioControlBuilderComponent() override = default;

        static void Reflect(AZ::ReflectContext* context);
        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC_CE("AudioControlBuilderService"));
        }

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
        {
            incompatible.push_back(AZ_CRC_CE("AudioControlBuilderService"));
        }

        //////////////////////////////////////////////////////////////////////////
        // AZ::Component interface
        void Activate() override;
        void Deactivate() override;
        //////////////////////////////////////////////////////////////////////////

    private:
        AmplitudeAudioControlBuilderWorker m_audioControlBuilder;
    };

} // namespace AudioControlBuilder
