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

#include <AzCore/IO/FileIO.h>

namespace SparkyStudios::Audio::Amplitude
{
    //! O3DE implementation for the Amplitude Audio file.
    class O3DEFile final : public File
    {
    public:
        O3DEFile(AZ::IO::HandleType fileHandle, AmOsString filePath);

        ~O3DEFile() override;

        //! @inherit
        [[nodiscard]] AmOsString GetPath() const override;

        //! @inherit
        [[nodiscard]] bool Eof() const override;

        //! @inherit
        AmSize Read(AmUInt8Buffer dst, AmSize bytes) const override;

        //! @inherit
        AmSize Write(AmConstUInt8Buffer src, AmSize bytes) override;

        //! @inherit
        [[nodiscard]] AmSize Length() const override;

        //! @inherit
        void Seek(AmInt64 offset, eFileSeekOrigin origin) override;

        //! @inherit
        [[nodiscard]] AmSize Position() const override;

        //! @inherit
        [[nodiscard]] AmVoidPtr GetPtr() const override;

        //! @inherit
        [[nodiscard]] bool IsValid() const override;

        //! @inherit.
        void Close() override;

    private:
        AZ::IO::HandleType _fileHandle;
        AmOsString _filePath;
    };
} // namespace SparkyStudios::Audio::Amplitude
