# Copyright (c) 2021-present Sparky Studios. All rights reserved.
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

# Amplitude installation path
set(SS_AMPLITUDE_SDK_PATH "" CACHE PATH "The path to Amplitude Audio SDK libraries.")

# Check for a known file in the SDK path to verify the path
function(is_valid_sdk sdk_path is_valid)
    set(${is_valid} FALSE PARENT_SCOPE)
    if(EXISTS ${sdk_path})
        set(sdk_file ${sdk_path}/sdk/include/SparkyStudios/Audio/Amplitude/Amplitude.h)
        if(EXISTS ${sdk_file})
            set(${is_valid} TRUE PARENT_SCOPE)
        endif()
    endif()
endfunction()

# Paths that will be checked, in order:
# - CMake cache variable
# - A Environment Variable
set(AMPLITUDE_SDK_PATHS
    "${SS_AMPLITUDE_SDK_PATH}"
    "$ENV{SS_AMPLITUDE_ROOT_PATH}"
)

set(found_sdk FALSE)
foreach(candidate_path ${AMPLITUDE_SDK_PATHS})
    is_valid_sdk(${candidate_path} found_sdk)
    if(found_sdk)
        # Update the Amplitude installation path variable internally
        set(SS_AMPLITUDE_SDK_PATH "${candidate_path}")
        break()
    endif()
endforeach()

if(NOT found_sdk)
    # If we don't find a path that appears to be a valid Amplitude install, we can bail here.
    # No 3rdParty::AmplitudeAudioSDK target will exist, so that can be checked elsewhere.
    return()
endif()

message(STATUS "Using Amplitude Audio SDK at ${SS_AMPLITUDE_SDK_PATH}")

set(AMPLITUDE_COMPILE_DEFINITIONS
    $<IF:$<CONFIG:Release>,AMPLITUDE_NO_ASSERTS,>
    $<IF:$<CONFIG:Release>,AM_NO_MEMORY_STATS,>
)

# Use these to get the parent path and folder name before adding the external 3rd party target.
get_filename_component(AMPLITUDE_INSTALL_ROOT ${SS_AMPLITUDE_SDK_PATH} DIRECTORY)
get_filename_component(AMPLITUDE_FOLDER ${SS_AMPLITUDE_SDK_PATH} NAME)

ly_add_external_target(
    NAME AmplitudeAudioSDK
    VERSION ${AMPLITUDE_FOLDER}
    3RDPARTY_ROOT_DIRECTORY ${AMPLITUDE_INSTALL_ROOT}
    INCLUDE_DIRECTORIES sdk/include
    COMPILE_DEFINITIONS ${AMPLITUDE_COMPILE_DEFINITIONS}
)

set(AmplitudeAudioSDK_FOUND TRUE)
