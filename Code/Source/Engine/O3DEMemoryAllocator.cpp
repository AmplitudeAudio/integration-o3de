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

#include <Engine/Common.h>
#include <Engine/O3DEMemoryAllocator.h>

namespace SparkyStudios::Audio::Amplitude
{
    AmVoidPtr O3DEMemoryAllocator::Malloc(eMemoryPoolKind pool, AmSize size)
    {
        return AZ::AllocatorInstance<::Audio::AudioImplAllocator>::Get().Allocate(
            size, 0, 0, gMemoryManagerPools[static_cast<AmUInt8>(pool)]);
    }

    AmVoidPtr O3DEMemoryAllocator::Malign(eMemoryPoolKind pool, AmSize size, AmUInt32 alignment)
    {
        return AZ::AllocatorInstance<::Audio::AudioImplAllocator>::Get().Allocate(
            size, alignment, 0, gMemoryManagerPools[static_cast<AmUInt8>(pool)]);
    }

    AmVoidPtr O3DEMemoryAllocator::Realloc([[maybe_unused]] eMemoryPoolKind pool, AmVoidPtr address, AmSize size)
    {
        return AZ::AllocatorInstance<::Audio::AudioImplAllocator>::Get().ReAllocate(address, size, 0);
    }

    AmVoidPtr O3DEMemoryAllocator::Realign([[maybe_unused]] eMemoryPoolKind pool, AmVoidPtr address, AmSize size, AmUInt32 alignment)
    {
        return AZ::AllocatorInstance<::Audio::AudioImplAllocator>::Get().ReAllocate(address, size, alignment);
    }

    void O3DEMemoryAllocator::Free([[maybe_unused]] eMemoryPoolKind pool, AmVoidPtr address)
    {
        AZ::AllocatorInstance<::Audio::AudioImplAllocator>::Get().DeAllocate(address);
    }

    AmSize O3DEMemoryAllocator::SizeOf([[maybe_unused]] eMemoryPoolKind pool, AmVoidPtr address)
    {
        return AZ::AllocatorInstance<::Audio::AudioImplAllocator>::Get().AllocationSize(address);
    }
} // namespace SparkyStudios::Audio::Amplitude
