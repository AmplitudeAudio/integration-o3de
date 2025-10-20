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

o3de_pal_dir(pal_dir ${CMAKE_CURRENT_LIST_DIR}/Platform/${PAL_PLATFORM_NAME} "${gem_restricted_path}" "${gem_path}" "${gem_parent_relative_path}")

include(${pal_dir}/PAL_${PAL_PLATFORM_NAME_LOWERCASE}.cmake)

set(AMPLITUDE_COMPILE_DEFINITIONS
    $<$<CONFIG:RELEASE>:AM_NO_ASSERTS>
    $<$<CONFIG:PROFILE>:AM_NO_ASSERTS>
    $<$<CONFIG:RELEASE>:AM_NO_MEMORY_STATS>
    $<$<CONFIG:PROFILE>:AM_NO_MEMORY_STATS>
)

find_package(AmplitudeAudioSDK MODULE)

include(DetectAmplitudeVersion)
include(DetectPlatform)
include(DetectCPUArchitecture)

# Check to see if building the Gem Modules are supported for the current platform
if(NOT PAL_TRAIT_AMPLITUDEAUDIO_SUPPORTED OR NOT AmplitudeAudioSDK_FOUND)
    return()
endif()

if (PAL_TRAIT_BUILD_SERVER_SUPPORTED OR PAL_TRAIT_AMPLITUDEAUDIO_USE_STUB)
    # Stub gem for server and unsupported platforms. Amplitude Audio is client only
    ly_add_target(
        NAME ${gem_name}.Stub ${PAL_TRAIT_MONOLITHIC_DRIVEN_MODULE_TYPE}
        NAMESPACE Gem
        FILES_CMAKE
            amplitudeaudio_stub_files.cmake
        BUILD_DEPENDENCIES
            PRIVATE
                AZ::AzCore
    )
endif()

ly_add_target(
    NAME ${gem_name}.API INTERFACE
    NAMESPACE Gem
    FILES_CMAKE
        amplitudeaudio_api_files.cmake
        ${pal_dir}/amplitudeaudio_api_files.cmake
    INCLUDE_DIRECTORIES
        INTERFACE
            Include
    BUILD_DEPENDENCIES
        INTERFACE
           AZ::AzCore
)

ly_add_target(
    NAME ${gem_name}.Private.Object STATIC
    NAMESPACE Gem
    FILES_CMAKE
        amplitudeaudio_private_files.cmake
        ${pal_dir}/amplitudeaudio_private_files.cmake
    TARGET_PROPERTIES
        O3DE_PRIVATE_TARGET TRUE
    INCLUDE_DIRECTORIES
        PRIVATE
            Include
            Source
            ${pal_dir}
    COMPILE_DEFINITIONS
        PUBLIC
            ${AMPLITUDE_COMPILE_DEFINITIONS}
    BUILD_DEPENDENCIES
        PUBLIC
            SparkyStudios::Audio::Amplitude::SDK::Shared
            Legacy::CryCommon
            Gem::AudioSystem.API
)

am_apply_detected_platform_defines(${gem_name}.Private.Object)

ly_add_target(
    NAME ${gem_name} ${PAL_TRAIT_MONOLITHIC_DRIVEN_MODULE_TYPE}
    NAMESPACE Gem
    FILES_CMAKE
        amplitudeaudio_shared_files.cmake
        ${pal_dir}/amplitudeaudio_shared_files.cmake
    INCLUDE_DIRECTORIES
        PUBLIC
            Include
        PRIVATE
            Source
    BUILD_DEPENDENCIES
        PUBLIC
            Gem::${gem_name}.API
        PRIVATE
            Gem::${gem_name}.Private.Object
)

ly_add_source_properties(
    SOURCES
        Source/Runtime/AmplitudeAudioModule.cpp
    PROPERTY COMPILE_DEFINITIONS
        VALUES
            O3DE_GEM_NAME=${gem_name}
            O3DE_GEM_VERSION=${gem_version}
)

ly_create_alias(NAME ${gem_name}.Clients NAMESPACE Gem TARGETS Gem::${gem_name})
ly_create_alias(NAME ${gem_name}.Unified NAMESPACE Gem TARGETS Gem::${gem_name})

ly_create_alias(NAME ${gem_name}.Clients.API NAMESPACE Gem TARGETS Gem::${gem_name}.API)
ly_create_alias(NAME ${gem_name}.Unified.API NAMESPACE Gem TARGETS Gem::${gem_name}.API)

o3de_add_variant_dependencies_for_gem_dependencies(GEM_NAME ${gem_name} VARIANTS Clients Unified)

# If we are on a host platform, we want to add the host tools targets like the ${gem_name}.Editor MODULE target
if(PAL_TRAIT_BUILD_HOST_TOOLS)
    ly_add_target(
        NAME ${gem_name}.Editor.API INTERFACE
        NAMESPACE Gem
        FILES_CMAKE
            amplitudeaudio_editor_api_files.cmake
            ${pal_dir}/amplitudeaudio_editor_api_files.cmake
        INCLUDE_DIRECTORIES
            INTERFACE
                Include
        BUILD_DEPENDENCIES
            INTERFACE
                AZ::AzToolsFramework
    )

    ly_add_target(
        NAME ${gem_name}.Editor.Private.Object STATIC
        NAMESPACE Gem
        AUTOMOC
        AUTORCC
        FILES_CMAKE
            amplitudeaudio_editor_private_files.cmake
            # amplitudeaudio_builder_private_files.cmake
        TARGET_PROPERTIES
            O3DE_PRIVATE_TARGET TRUE
        COMPILE_DEFINITIONS
            PUBLIC
                ${AMPLITUDE_COMPILE_DEFINITIONS}
        INCLUDE_DIRECTORIES
            PRIVATE
                Include
                Source
        BUILD_DEPENDENCIES
            PUBLIC
                Gem::AudioSystem.Editor.API
                Legacy::CryCommon
                AZ::AssetBuilderSDK
                AZ::AzToolsFramework
                AZ::AzFramework
                Gem::LmbrCentral.Static
                Gem::${gem_name}.Private.Object
            PRIVATE
                Legacy::EditorCore
        RUNTIME_DEPENDENCIES
            Gem::AudioSystem.Editor
    )

    ly_add_target(
        NAME ${gem_name}.Editor GEM_MODULE
        NAMESPACE Gem
        AUTOMOC
        FILES_CMAKE
            amplitudeaudio_editor_shared_files.cmake
        COMPILE_DEFINITIONS
            PRIVATE
                AMPLITUDE_AUDIO_EDITOR
        INCLUDE_DIRECTORIES
            PRIVATE
                Source
            PUBLIC
                Include
        BUILD_DEPENDENCIES
            PUBLIC
                Gem::AudioSystem.API
                Gem::${gem_name}.Editor.API
            PRIVATE
                Gem::${gem_name}.Editor.Private.Object
        RUNTIME_DEPENDENCIES
            Gem::AudioSystem.Editor
    )

    ly_create_alias(NAME ${gem_name}.Tools    NAMESPACE Gem TARGETS Gem::${gem_name}.Editor)
    ly_create_alias(NAME ${gem_name}.Builders NAMESPACE Gem TARGETS Gem::${gem_name}.Editor)

    ly_create_alias(NAME ${gem_name}.Tools.API    NAMESPACE Gem TARGETS Gem::${gem_name}.Editor.API)
    ly_create_alias(NAME ${gem_name}.Builders.API NAMESPACE Gem TARGETS Gem::${gem_name}.Editor.API)

    o3de_add_variant_dependencies_for_gem_dependencies(GEM_NAME ${gem_name} VARIANTS Tools)
endif()

################################################################################
# Tests
################################################################################
# See if globally, tests are supported
if(PAL_TRAIT_BUILD_TESTS_SUPPORTED)
    # We globally support tests, see if we support tests on this platform for ${gem_name}.Tests
    if(PAL_TRAIT_AMPLITUDEAUDIO_TEST_SUPPORTED)
        # We support ${gem_name}.Tests on this platform, add dependency on the Private Object target
        ly_add_target(
            NAME ${gem_name}.Tests ${PAL_TRAIT_TEST_TARGET_TYPE}
            NAMESPACE Gem
            FILES_CMAKE
                amplitudeaudio_tests_files.cmake
            INCLUDE_DIRECTORIES
                PRIVATE
                    Tests
                    Source/Engine
                    Include
            BUILD_DEPENDENCIES
                PUBLIC
                    Gem::AudioSystem.API
                PRIVATE
                    AZ::AzTest
                    Gem::${gem_name}.Private.Object
        )

        # Add ${gem_name}.Tests to googletest
        ly_add_googletest(
            NAME Gem::${gem_name}.Tests
        )
    endif()

    # If we are a host platform we want to add tools test like editor tests here
    if(PAL_TRAIT_BUILD_HOST_TOOLS)
        # We are a host platform, see if Editor tests are supported on this platform
        if(PAL_TRAIT_AMPLITUDEAUDIO_EDITOR_TEST_SUPPORTED)
            # We support ${gem_name}.Editor.Tests on this platform, add ${gem_name}.Editor.Tests target which depends on
            # private ${gem_name}.Editor.Private.Object target
            ly_add_target(
                NAME ${gem_name}.Editor.Tests ${PAL_TRAIT_TEST_TARGET_TYPE}
                NAMESPACE Gem
                FILES_CMAKE
                    amplitudeaudio_editor_tests_files.cmake
                INCLUDE_DIRECTORIES
                    PRIVATE
                        Tests
                        Source
                        Include
                BUILD_DEPENDENCIES
                    PUBLIC
                        Gem::AudioSystem.API
                    PRIVATE
                        AZ::AzTest
                        Gem::${gem_name}.Editor.Private.Object
                        Legacy::CryCommon
            )

            # Add ${gem_name}.Editor.Tests to googletest
            ly_add_googletest(
                NAME Gem::${gem_name}.Editor.Tests
            )
        endif()
    endif()
endif()
