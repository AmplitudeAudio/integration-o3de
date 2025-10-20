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

#include <Engine/O3DEFile.h>

namespace SparkyStudios::Audio::Amplitude
{
    O3DEFile::O3DEFile(AZ::IO::HandleType fileHandle, AmOsString filePath)
        : _fileHandle(fileHandle)
        , _filePath(AZStd::move(filePath))
    {}

    O3DEFile::~O3DEFile()
    {
        if (!IsValid())
            return;

        Close();
    }

    AmOsString O3DEFile::GetPath() const
    {
        return _filePath;
    }

    bool O3DEFile::Eof() const
    {
        return AZ::IO::FileIOBase::GetInstance()->Eof(_fileHandle);
    }

    AmSize O3DEFile::Read(AmUInt8Buffer dst, AmSize bytes) const
    {
        AZ::u64 readSize = 0;
        AZ::IO::FileIOBase::GetInstance()->Read(_fileHandle, dst, bytes, false, &readSize);
        return readSize;
    }

    AmSize O3DEFile::Write(AmConstUInt8Buffer src, AmSize bytes)
    {
        AZ::u64 writeSize = 0;
        AZ::IO::FileIOBase::GetInstance()->Write(_fileHandle, src, bytes, &writeSize);
        return writeSize;
    }

    AmSize O3DEFile::Length() const
    {
        AZ::u64 size = 0;
        AZ::IO::FileIOBase::GetInstance()->Size(_fileHandle, size);
        return size;
    }

    void O3DEFile::Seek(AmInt64 offset, eFileSeekOrigin origin)
    {
        auto seekType = AZ::IO::SeekType::SeekFromStart;

        switch (origin)
        {
        case eFileSeekOrigin_Start:
            seekType = AZ::IO::SeekType::SeekFromStart;
            break;
        case eFileSeekOrigin_Current:
            seekType = AZ::IO::SeekType::SeekFromCurrent;
            break;
        case eFileSeekOrigin_End:
            seekType = AZ::IO::SeekType::SeekFromEnd;
            break;
        }

        AZ::IO::FileIOBase::GetInstance()->Seek(_fileHandle, offset, seekType);
    }

    AmSize O3DEFile::Position() const
    {
        AZ::u64 offset = 0;
        AZ::IO::FileIOBase::GetInstance()->Tell(_fileHandle, offset);
        return offset;
    }

    AmVoidPtr O3DEFile::GetPtr() const
    {
        return const_cast<AZ::u32*>(&_fileHandle);
    }

    bool O3DEFile::IsValid() const
    {
        return _fileHandle != AZ::IO::InvalidHandle;
    }

    void O3DEFile::Close()
    {
        AZ::IO::FileIOBase::GetInstance()->Close(_fileHandle);
    }
} // namespace SparkyStudios::Audio::Amplitude
