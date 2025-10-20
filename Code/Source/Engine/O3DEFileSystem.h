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

#include <Engine/O3DEFile.h>

namespace SparkyStudios::Audio::Amplitude
{
    //! O3DE implementation for the Amplitude Audio file system.
    class O3DEFileSystem final : public FileSystem
    {
    public:
        //! @inherit
        void SetBasePath(const AmOsString& basePath) override;

        //! @inherit
        [[nodiscard]] const AmOsString& GetBasePath() const override;

        //! @inherit
        [[nodiscard]] AmOsString ResolvePath(const AmOsString& path) const override;

        //! @inherit
        [[nodiscard]] bool Exists(const AmOsString& path) const override;

        //! @inherit
        [[nodiscard]] bool IsDirectory(const AmOsString& path) const override;

        //! @inherit
        [[nodiscard]] AmOsString Join(const std::vector<AmOsString>& parts) const override;

        //! @inherit
        [[nodiscard]] std::shared_ptr<File> OpenFile(const AmOsString& path, eFileOpenMode mode = eFileOpenMode_Read) const override;

        //! @inherit
        void StartOpenFileSystem() override;

        //! @inherit
        bool TryFinalizeOpenFileSystem() override;

        //! @inherit
        void StartCloseFileSystem() override;

        //! @inherit
        bool TryFinalizeCloseFileSystem() override;

    private:
        AmOsString _basePath;
    };
} // namespace SparkyStudios::Audio::Amplitude
