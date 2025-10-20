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

#include <Engine/O3DELogger.h>

namespace SparkyStudios::Audio::Amplitude
{
    void O3DELogger::Log(eLogMessageLevel level, const char* file, int line, const AmString& message)
    {
        AZ_UNUSED(file, line)

        switch (level)
        {
        case eLogMessageLevel_Debug:
            AZLOG_DEBUG("[Amplitude] %s", message.c_str())
            break;
        case eLogMessageLevel_Info:
            AZLOG_INFO("[Amplitude] %s", message.c_str())
            break;
        case eLogMessageLevel_Warning:
            AZLOG_WARN("[Amplitude] %s", message.c_str())
            break;
        case eLogMessageLevel_Error:
            AZLOG_ERROR("[Amplitude] %s", message.c_str())
            break;
        case eLogMessageLevel_Critical:
            AZLOG_FATAL("[Amplitude] %s", message.c_str())
            break;
        case eLogMessageLevel_Success:
            AZLOG_NOTICE("[Amplitude] %s", message.c_str())
            break;
        }
    }
}; // namespace SparkyStudios::Audio::Amplitude
