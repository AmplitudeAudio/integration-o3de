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

#include <SparkyStudios/Audio/Amplitude/Amplitude.h>

#include <AudioAllocators.h>

namespace SparkyStudios::Audio::Amplitude
{
    //! O3DE implementation for the Amplitude Audio memory allocator.
    class O3DEMemoryAllocator final : public MemoryAllocator
    {
    public:
        //! @inheritdoc
        AmVoidPtr Malloc(eMemoryPoolKind pool, AmSize size) override;

        //! @inheritdoc
        AmVoidPtr Malign(eMemoryPoolKind pool, AmSize size, AmUInt32 alignment) override;

        //! @inheritdoc
        AmVoidPtr Realloc([[maybe_unused]] eMemoryPoolKind pool, AmVoidPtr address, AmSize size) override;

        //! @inheritdoc
        AmVoidPtr Realign([[maybe_unused]] eMemoryPoolKind pool, AmVoidPtr address, AmSize size, AmUInt32 alignment) override;

        //! @inheritdoc
        void Free([[maybe_unused]] eMemoryPoolKind pool, AmVoidPtr address) override;

        //! @inheritdoc
        AmSize SizeOf([[maybe_unused]] eMemoryPoolKind pool, AmVoidPtr address) override;
    };
} // namespace SparkyStudios::Audio::Amplitude
