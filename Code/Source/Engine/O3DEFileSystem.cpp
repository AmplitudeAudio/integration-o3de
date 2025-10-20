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

#include <Engine/O3DEFileSystem.h>

namespace SparkyStudios::Audio::Amplitude
{
    void O3DEFileSystem::SetBasePath(const AmOsString& basePath)
    {
        _basePath = basePath;
    }

    const AmOsString& O3DEFileSystem::GetBasePath() const
    {
        return _basePath;
    }

    AmOsString O3DEFileSystem::ResolvePath(const AmOsString& path) const
    {
        AmOsString resolvedPath = path;

        if (!path.starts_with(_basePath))
            resolvedPath = _basePath + AM_OS_STRING("/") + path;

        // Normalize path separators and resolve relative components manually
        // Replace all backslashes with forward slashes
        std::ranges::replace(resolvedPath, '\\', '/');

        // Remove duplicate slashes
        for (size_t i = 0; i < resolvedPath.length() - 1;)
        {
            if (resolvedPath[i] == '/' && resolvedPath[i + 1] == '/')
                resolvedPath.erase(i + 1, 1);
            else
                ++i;
        }

        // Handle . and .. components
        std::vector<AmOsString> components;
        AmSize start = 0;
        bool canPop = false;
        for (AmSize i = 0; i <= resolvedPath.length(); ++i)
        {
            if (i == resolvedPath.length() || resolvedPath[i] == '/')
            {
                if (i > start)
                {
                    AmOsString component = resolvedPath.substr(start, i - start);
                    if (component == AM_OS_STRING(".."))
                    {
                        if (!components.empty() && canPop)
                        {
                            components.pop_back();
                            canPop = !components.empty() && components.back() != AM_OS_STRING("..");
                        }
                        else
                        {
                            components.push_back(component);
                        }
                    }
                    else if (component != AM_OS_STRING("."))
                    {
                        components.push_back(component);
                        canPop = true;
                    }
                }
                start = i + 1;
            }
        }

        // Reconstruct the path
        if (components.empty())
        {
            resolvedPath = AM_OS_STRING(".");
        }
        else
        {
            resolvedPath.clear();
            for (size_t i = 0; i < components.size(); ++i)
            {
                if (i > 0)
                    resolvedPath += AM_OS_STRING("/");

                resolvedPath += components[i];
            }
        }

        return resolvedPath;
    }

    bool O3DEFileSystem::Exists(const AmOsString& path) const
    {
        return AZ::IO::FileIOBase::GetInstance()->Exists(AM_OS_STRING_TO_STRING(ResolvePath(path)));
    }

    bool O3DEFileSystem::IsDirectory(const AmOsString& path) const
    {
        return AZ::IO::FileIOBase::GetInstance()->IsDirectory(AM_OS_STRING_TO_STRING(ResolvePath(path)));
    }

    AmOsString O3DEFileSystem::Join(const std::vector<AmOsString>& parts) const
    {
        if (parts.empty())
            return AM_OS_STRING("");

        AmOsString joined(parts[0]);

        for (AmSize i = 1, l = parts.size(); i < l; i++)
            joined += AM_OS_STRING("/") + parts[i];

        return ResolvePath(joined);
    }

    std::shared_ptr<File> O3DEFileSystem::OpenFile(const AmOsString& path, eFileOpenMode mode) const
    {
        auto fileHandle = AZ::IO::InvalidHandle;
        auto openMode = AZ::IO::OpenMode::ModeBinary;

        switch (mode)
        {
        case eFileOpenMode_Read:
            openMode |= AZ::IO::OpenMode::ModeRead;
            break;
        case eFileOpenMode_Write:
            openMode |= AZ::IO::OpenMode::ModeWrite;
            break;
        case eFileOpenMode_Append:
            openMode |= AZ::IO::OpenMode::ModeAppend;
            break;
        case eFileOpenMode_ReadWrite:
            openMode |= AZ::IO::OpenMode::ModeWrite | AZ::IO::OpenMode::ModeUpdate;
            break;
        case eFileOpenMode_ReadAppend:
            openMode |= AZ::IO::OpenMode::ModeAppend | AZ::IO::OpenMode::ModeUpdate;
            break;
        }

        auto resolvedPath = ResolvePath(path);
        AZ::IO::FileIOBase::GetInstance()->Open(AM_OS_STRING_TO_STRING(resolvedPath), openMode, fileHandle);
        return AmSharedPtr<O3DEFile, eMemoryPoolKind_IO>::Make(fileHandle, resolvedPath);
    }

    void O3DEFileSystem::StartOpenFileSystem()
    {
        // noop
    }

    bool O3DEFileSystem::TryFinalizeOpenFileSystem()
    {
        return true;
    }

    void O3DEFileSystem::StartCloseFileSystem()
    {
        // noop
    }

    bool O3DEFileSystem::TryFinalizeCloseFileSystem()
    {
        return true;
    }
} // namespace SparkyStudios::Audio::Amplitude
