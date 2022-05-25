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
        , public AZ::TickBus::Handler
        , protected ::Audio::Gem::AudioEngineGemRequestBus::Handler
        , protected AmplitudeAudioRequestBus::Handler
    {
    public:
        AZ_COMPONENT(AmplitudeAudioSystemComponent, "{C8566CAF-B250-4687-A470-57DC9F881971}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        AmplitudeAudioSystemComponent();
        ~AmplitudeAudioSystemComponent() override;

    protected:
        // AmplitudeAudioRequestBus interface implementation


        // Audio::Gem::AudioEngineGemRequestBus interface implementation
        bool Initialize() override;
        void Release() override;

        // AZ::Component interface implementation
        void Init() override;
        void Activate() override;
        void Deactivate() override;

        // AZTickBus interface implementation
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

    private:
        AZStd::unique_ptr<::Audio::AudioSystemImplementation> _amplitudeEngine;
    };
} // namespace SparkyStudios::Audio::Amplitude
