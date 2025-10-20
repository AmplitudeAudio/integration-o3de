# Copyright (c) 2025-present Sparky Studios. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set(FILES
    Source/Assets/RoomWallMaterialDescriptor.cpp
    Source/Assets/RoomWallMaterialsAsset.cpp
    Source/Assets/RoomWallMaterialsAsset.h

    Source/Common/AmplitudeAudioModuleInterface.cpp
    Source/Common/AmplitudeAudioModuleInterface.h

    Source/Engine/ATLEntities_Amplitude.h
    Source/Engine/AudioSystemImpl_Amplitude.cpp
    Source/Engine/AudioSystemImpl_Amplitude.h
    Source/Engine/Common.h
    Source/Engine/O3DEFile.cpp
    Source/Engine/O3DEFile.h
    Source/Engine/O3DEFileSystem.cpp
    Source/Engine/O3DEFileSystem.h
    Source/Engine/O3DELogger.cpp
    Source/Engine/O3DELogger.h
    Source/Engine/O3DEMemoryAllocator.cpp
    Source/Engine/O3DEMemoryAllocator.h

    Source/Runtime/AmplitudeAudioSystemComponent.cpp
    Source/Runtime/AmplitudeAudioSystemComponent.h
    Source/Runtime/RoomComponent.cpp
    Source/Runtime/RoomComponent.h
)

# Skip the following file that is also used in the editor shared target so the compiler will recognize its the same symbol
set(SKIP_UNITY_BUILD_INCLUSION_FILES
    Source/Runtime/AmplitudeAudioSystemComponent.cpp
)
