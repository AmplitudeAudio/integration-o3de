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
#include <Tools/Builder/AudioControlBuilderWorker.h>

namespace SparkyStudios::Audio::Amplitude
{
    class AudioControlBuilderComponent : public AZ::Component
    {
    public:
        AZ_COMPONENT(AudioControlBuilderComponent, "{5F5CEE21-6A9A-4050-AA2F-68EFE1D8EAAA}")

        AudioControlBuilderComponent() = default;
        ~AudioControlBuilderComponent() override = default;

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC_CE("AudioControlBuilderService"));
        }

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
        {
            incompatible.push_back(AZ_CRC_CE("AudioControlBuilderService"));
        }

        // AZ::Component
        void Activate() override;
        void Deactivate() override;

    private:
        AudioControlBuilderWorker m_audioControlBuilder;
    };
} // namespace SparkyStudios::Audio::Amplitude
