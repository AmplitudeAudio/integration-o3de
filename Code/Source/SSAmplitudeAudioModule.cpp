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

#include <SSAmplitudeAudioModuleInterface.h>
#include <SSAmplitudeAudioSystemComponent.h>

namespace SparkyStudios::Audio::Amplitude
{
    class SSAmplitudeAudioModule : public SSAmplitudeAudioModuleInterface
    {
    public:
        AZ_RTTI(SSAmplitudeAudioModule, "{2D70FF2E-7444-4F1C-A67C-13DD81B980CB}", SSAmplitudeAudioModuleInterface);
        AZ_CLASS_ALLOCATOR(SSAmplitudeAudioModule, AZ::SystemAllocator, 0);
    };
} // namespace SparkyStudios::Audio::Amplitude

AZ_DECLARE_MODULE_CLASS(Gem_SSAmplitudeAudio, SparkyStudios::Audio::Amplitude::SSAmplitudeAudioModule)
