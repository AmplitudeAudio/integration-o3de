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

#include <Common/AmplitudeAudioModuleInterface.h>

namespace SparkyStudios::Audio::Amplitude
{
    class AmplitudeAudioModule : public AmplitudeAudioModuleInterface
    {
    public:
        AZ_RTTI(AmplitudeAudioModule, "{DF9CDD90-F992-4628-B222-660D1CF8AB8A}", AmplitudeAudioModuleInterface)
        AZ_CLASS_ALLOCATOR(AmplitudeAudioModule, AZ::SystemAllocator)
    };
} // namespace SparkyStudios::Audio::Amplitude

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME), SparkyStudios::Audio::Amplitude::AmplitudeAudioModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_AmplitudeAudio, SparkyStudios::Audio::Amplitude::AmplitudeAudioModule)
#endif
