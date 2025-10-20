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

#include <SparkyStudios/Audio/Amplitude/AmplitudeAudioBus.h>

#include <AzCore/std/smart_ptr/unique_ptr.h>

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>

#include <IAudioSystem.h>
#include <IAudioSystemImplementation.h>

namespace SparkyStudios::Audio::Amplitude
{
    class AmplitudeAudioSystemComponent
        : public AZ::Component
        , protected ::Audio::Gem::EngineRequestBus::Handler
        , protected AmplitudeAudioRequestBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(AmplitudeAudioSystemComponent);

        static void Reflect(AZ::ReflectContext* rc);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        AmplitudeAudioSystemComponent();
        ~AmplitudeAudioSystemComponent() override;

    protected:
        // AZ::Component
        void Init() override;
        void Activate() override;
        void Deactivate() override;

        // ::Audio::Gem::EngineRequestBus
        bool Initialize() override;
        void Release() override;

        // AmplitudeAudioRequestBus
        Engine* GetEngine() const override;

    private:
        AZStd::unique_ptr<::Audio::AudioSystemImplementation> _amplitudeEngine;
    };
} // namespace SparkyStudios::Audio::Amplitude
