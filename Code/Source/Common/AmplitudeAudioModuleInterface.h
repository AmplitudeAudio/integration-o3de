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

#include <AzCore/PlatformDef.h>
#include <IGem.h>

namespace SparkyStudios::Audio::Amplitude
{
    //! Base class for modules in this gem.
    class AmplitudeAudioModuleInterface : public CryHooksModule
    {
    public:
        AZ_TYPE_INFO_WITH_NAME_DECL(AmplitudeAudioModuleInterface)
        AZ_RTTI_NO_TYPE_INFO_DECL()
        AZ_CLASS_ALLOCATOR_DECL

        AmplitudeAudioModuleInterface();

        //! Add required SystemComponents to the SystemEntity.
        [[nodiscard]] AZ::ComponentTypeList GetRequiredSystemComponents() const override;
    };
} // namespace SparkyStudios::Audio::Amplitude
