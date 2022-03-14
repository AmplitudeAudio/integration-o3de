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

# Currently we are in the Code folder: ${CMAKE_CURRENT_LIST_DIR}
# Get the platform specific folder ${pal_dir} for the current folder: ${CMAKE_CURRENT_LIST_DIR}/Platform/${PAL_PLATFORM_NAME}
# Note: ly_get_list_relative_pal_filename will take care of the details for us, as this may be a restricted platform
#       in which case it will see if that platform is present here or in the restricted folder.
#       i.e. It could here in our gem : Gems/SSAmplitudeAudio/Code/Platform/<platorm_name>  or
#            <restricted_folder>/<platform_name>/Gems/SSAmplitudeAudio/Code
o3de_pal_dir(pal_dir ${CMAKE_CURRENT_LIST_DIR}/Platform/${PAL_PLATFORM_NAME} ${gem_restricted_path} "${gem_path}" "${gem_name}")

# Now that we have the platform abstraction layer (PAL) folder for this folder, thats where we will find the
# traits for this platform. Traits for a platform are defines for things like whether or not something in this gem
# is supported by this platform.
include(${pal_dir}/PAL_${PAL_PLATFORM_NAME_LOWERCASE}.cmake)

set(AMPLITUDEAUDIOSDK_COMPILEDEFINITIONS
    $<IF:$<CONFIG:Release>,AMPLITUDE_RELEASE,AMPLITUDE_DEBUG>
)

if(NOT PAL_TRAIT_SSAMPLITUDEAUDIO_SUPPORTED)
    include(ssamplitudeaudio_unsuported.cmake)
    return()
endif()

if (PAL_TRAIT_BUILD_SERVER_SUPPORTED OR PAL_TRAIT_SSAMPLITUDEAUDIO_USE_STUB)
    # Stub gem for server and unsupported platforms. Audio Engine Wwise is client only
    ly_add_target(
        NAME SSAmplitudeAudio.Stub ${PAL_TRAIT_MONOLITHIC_DRIVEN_MODULE_TYPE}
        NAMESPACE SparkyStudios::Audio
        OUTPUT_NAME SparkyStudios.Audio.Amplitude.Stub
        FILES_CMAKE
            ssamplitudeaudio_stub_files.cmake
        BUILD_DEPENDENCIES
            PRIVATE
                AZ::AzCore
    )
endif()

if (PAL_TRAIT_AMPLITUDE_AUDIO_USE_STUB)
    # setup aliases so stubs will be used if something references SSAmplitudeAudio(.Editor)
    add_library(SparkyStudios::Audio::SSAmplitudeAudio ALIAS SSAmplitudeAudio.Stub)
    add_library(SparkyStudios::Audio::SSAmplitudeAudio.Editor ALIAS SSAmplitudeAudio.Stub)
    return()
endif()

# Add the SSAmplitudeAudio.Static target
# Note: We include the common files and the platform specific files which are set in ssamplitudeaudio_common_files.cmake
# and in ${pal_dir}/ssamplitudeaudio_${PAL_PLATFORM_NAME_LOWERCASE}_files.cmake
ly_add_target(
    NAME SSAmplitudeAudio.Static STATIC
    NAMESPACE SparkyStudios::Audio
    OUTPUT_NAME SparkyStudios.Audio.Amplitude.Static
    FILES_CMAKE
        ssamplitudeaudio_files.cmake
        ${pal_dir}/ssamplitudeaudio_${PAL_PLATFORM_NAME_LOWERCASE}_files.cmake
    INCLUDE_DIRECTORIES
        PUBLIC
            Include
        PRIVATE
            .
            Source
    COMPILE_DEFINITIONS
        PUBLIC
            ${AMPLITUDEAUDIOSDK_COMPILEDEFINITIONS}
    BUILD_DEPENDENCIES
        PUBLIC
            3rdParty::AmplitudeAudioSDK
            Legacy::CryCommon
            Gem::AudioSystem.Static
            AZ::AzCore
)

# Here add SSAmplitudeAudio target, it depends on the SSAmplitudeAudio.Static
ly_add_target(
    NAME SSAmplitudeAudio ${PAL_TRAIT_MONOLITHIC_DRIVEN_MODULE_TYPE}
    NAMESPACE SparkyStudios::Audio
    OUTPUT_NAME SparkyStudios.Audio.Amplitude
    FILES_CMAKE
        ssamplitudeaudio_shared_files.cmake
        ${pal_dir}/ssamplitudeaudio_shared_${PAL_PLATFORM_NAME_LOWERCASE}_files.cmake
    INCLUDE_DIRECTORIES
        PUBLIC
            Include
        PRIVATE
            Source
    COMPILE_DEFINITIONS
        PUBLIC
            ${AMPLITUDEAUDIOSDK_COMPILEDEFINITIONS}
    BUILD_DEPENDENCIES
        PRIVATE
            SparkyStudios::Audio::SSAmplitudeAudio.Static
)

# By default, we will specify that the above target SSAmplitudeAudio would be used by
# Client and Server type targets when this gem is enabled.  If you don't want it
# active in Clients or Servers by default, delete one of both of the following lines:
ly_create_alias(NAME SSAmplitudeAudio.Clients NAMESPACE Gem TARGETS SparkyStudios::Audio::SSAmplitudeAudio)

# If we are on a host platform, we want to add the host tools targets like the SSAmplitudeAudio.Editor target which
# will also depend on SSAmplitudeAudio.Static
if(PAL_TRAIT_BUILD_HOST_TOOLS)
    ly_add_target(
        NAME SSAmplitudeAudio.Editor.Static STATIC
        NAMESPACE SparkyStudios::Audio
        OUTPUT_NAME SparkyStudios.Audio.Amplitude.Editor.Static
        AUTORCC
        FILES_CMAKE
            ssamplitudeaudio_editor_files.cmake
        INCLUDE_DIRECTORIES
            PRIVATE
                Source
            PUBLIC
                Include
        BUILD_DEPENDENCIES
            PRIVATE
                Legacy::EditorCore
            PUBLIC
                AZ::AssetBuilderSDK
                Gem::AudioSystem.Editor
                SparkyStudios::Audio::SSAmplitudeAudio.Static
        RUNTIME_DEPENDENCIES
            Gem::AudioSystem.Editor
    )

    ly_add_target(
        NAME SSAmplitudeAudio.Editor GEM_MODULE
        NAMESPACE SparkyStudios::Audio
        OUTPUT_NAME SparkyStudios.Audio.Amplitude.Editor
        FILES_CMAKE
            ssamplitudeaudio_editor_shared_files.cmake
        INCLUDE_DIRECTORIES
            PRIVATE
                Source
            PUBLIC
                Include
        BUILD_DEPENDENCIES
            PUBLIC
            SparkyStudios::Audio::SSAmplitudeAudio.Editor.Static
        RUNTIME_DEPENDENCIES
            Gem::AudioSystem.Editor
    )

    # By default, we will specify that the above target SSAmplitudeAudio would be used by
    # Tool and Builder type targets when this gem is enabled.  If you don't want it
    # active in Tools or Builders by default, delete one of both of the following lines:
    ly_create_alias(NAME SSAmplitudeAudio.Tools    NAMESPACE Gem TARGETS SparkyStudios::Audio::SSAmplitudeAudio.Editor)
    ly_create_alias(NAME SSAmplitudeAudio.Builders NAMESPACE Gem TARGETS SparkyStudios::Audio::SSAmplitudeAudio.Editor)
endif()

################################################################################
# Tests
################################################################################
# See if globally, tests are supported
if(PAL_TRAIT_BUILD_TESTS_SUPPORTED)
    # We globally support tests, see if we support tests on this platform for SSAmplitudeAudio.Static
    if(PAL_TRAIT_SSAMPLITUDEAUDIO_TEST_SUPPORTED)
        # We support SSAmplitudeAudio.Tests on this platform, add SSAmplitudeAudio.Tests target which depends on SSAmplitudeAudio.Static
        ly_add_target(
            NAME SSAmplitudeAudio.Tests ${PAL_TRAIT_TEST_TARGET_TYPE}
            NAMESPACE SparkyStudios::Audio
            OUTPUT_NAME SparkyStudios.Audio.Amplitude.Tests
            FILES_CMAKE
                ssamplitudeaudio_files.cmake
                ssamplitudeaudio_tests_files.cmake
            INCLUDE_DIRECTORIES
                PRIVATE
                    Tests
                    .
                    Source
            BUILD_DEPENDENCIES
                PRIVATE
                    AZ::AzTest
                    AZ::AzFramework
                    SparkyStudios::Audio::SSAmplitudeAudio.Static
        )

        # Add SSAmplitudeAudio.Tests to googletest
        ly_add_googletest(
            NAME SparkyStudios::Audio::SSAmplitudeAudio.Tests
        )
    endif()
endif()
