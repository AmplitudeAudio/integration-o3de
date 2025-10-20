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

#include <AudioAllocators.h>
#include <IAudioSystemImplementation.h>

#include <Engine/ATLEntities_Amplitude.h>
#include <Engine/O3DEFileSystem.h>
#include <Engine/O3DELogger.h>
#include <Engine/O3DEMemoryAllocator.h>

#include <SparkyStudios/Audio/Amplitude/Amplitude.h>

namespace Audio
{
    //! Audio System Middleware implementation using Amplitude Audio as the backend engine.
    class CAudioSystemImpl_Amplitude final : public AudioSystemImplementation
    {
    public:
        AZ_RTTI(CAudioSystemImpl_Amplitude, "{4A44DC87-7082-4220-809C-F5C829552F4F}", AudioSystemImplementation)
        AUDIO_IMPL_CLASS_ALLOCATOR(CAudioSystemImpl_Amplitude)

        explicit CAudioSystemImpl_Amplitude(const char* projectBuildPath, const char* engineConfig);
        ~CAudioSystemImpl_Amplitude() override;

        // AudioSystemImplementationNotificationBus
        void OnAudioSystemLoseFocus() override;
        void OnAudioSystemGetFocus() override;
        void OnAudioSystemMuteAll() override;
        void OnAudioSystemUnmuteAll() override;
        void OnAudioSystemRefresh() override;

        // AudioSystemImplementationRequestBus
        void Update(float updateIntervalMs) override;

        EAudioRequestStatus Initialize() override;
        EAudioRequestStatus ShutDown() override;
        EAudioRequestStatus Release() override;

        EAudioRequestStatus StopAllSounds() override;

        EAudioRequestStatus RegisterAudioObject(IATLAudioObjectData* audioObjectData, const char* objectName) override;
        EAudioRequestStatus UnregisterAudioObject(IATLAudioObjectData* audioObjectData) override;
        EAudioRequestStatus ResetAudioObject(IATLAudioObjectData* audioObjectData) override;
        EAudioRequestStatus UpdateAudioObject(IATLAudioObjectData* audioObjectData) override;

        EAudioRequestStatus PrepareTriggerSync(IATLAudioObjectData* audioObjectData, const IATLTriggerImplData* triggerData) override;
        EAudioRequestStatus UnprepareTriggerSync(IATLAudioObjectData* audioObjectData, const IATLTriggerImplData* triggerData) override;
        EAudioRequestStatus PrepareTriggerAsync(
            IATLAudioObjectData* audioObjectData, const IATLTriggerImplData* triggerData, IATLEventData* eventData) override;
        EAudioRequestStatus UnprepareTriggerAsync(
            IATLAudioObjectData* audioObjectData, const IATLTriggerImplData* triggerData, IATLEventData* eventData) override;
        EAudioRequestStatus ActivateTrigger(
            IATLAudioObjectData* audioObjectData,
            const IATLTriggerImplData* triggerData,
            IATLEventData* eventData,
            const SATLSourceData* sourceData) override;
        EAudioRequestStatus StopEvent(IATLAudioObjectData* audioObjectData, const IATLEventData* eventData) override;
        EAudioRequestStatus StopAllEvents(IATLAudioObjectData* audioObjectData) override;
        EAudioRequestStatus SetPosition(IATLAudioObjectData* audioObjectData, const SATLWorldPosition& worldPosition) override;
        EAudioRequestStatus SetMultiplePositions(
            IATLAudioObjectData* audioObjectData, const MultiPositionParams& multiPositionParams) override;
        EAudioRequestStatus SetEnvironment(
            IATLAudioObjectData* audioObjectData, const IATLEnvironmentImplData* environmentData, float amount) override;
        EAudioRequestStatus SetRtpc(IATLAudioObjectData* audioObjectData, const IATLRtpcImplData* rtpcData, float value) override;
        EAudioRequestStatus SetSwitchState(IATLAudioObjectData* audioObjectData, const IATLSwitchStateImplData* switchStateData) override;
        EAudioRequestStatus SetObstructionOcclusion(IATLAudioObjectData* audioObjectData, float obstruction, float occlusion) override;
        EAudioRequestStatus SetListenerPosition(IATLListenerData* listenerData, const SATLWorldPosition& newPosition) override;
        EAudioRequestStatus ResetRtpc(IATLAudioObjectData* audioObjectData, const IATLRtpcImplData* rtpcData) override;

        EAudioRequestStatus RegisterInMemoryFile(SATLAudioFileEntryInfo* audioFileEntry) override;
        EAudioRequestStatus UnregisterInMemoryFile(SATLAudioFileEntryInfo* audioFileEntry) override;

        EAudioRequestStatus ParseAudioFileEntry(
            const AZ::rapidxml::xml_node<char>* audioFileEntryNode, SATLAudioFileEntryInfo* fileEntryInfo) override;
        void DeleteAudioFileEntryData(IATLAudioFileEntryData* oldAudioFileEntryData) override;
        const char* const GetAudioFileLocation(SATLAudioFileEntryInfo* fileEntryInfo) override;

        IATLTriggerImplData* NewAudioTriggerImplData(const AZ::rapidxml::xml_node<char>* audioTriggerNode) override;
        void DeleteAudioTriggerImplData(IATLTriggerImplData* oldTriggerImplData) override;

        IATLRtpcImplData* NewAudioRtpcImplData(const AZ::rapidxml::xml_node<char>* audioRtpcNode) override;
        void DeleteAudioRtpcImplData(IATLRtpcImplData* oldRtpcImplData) override;

        IATLSwitchStateImplData* NewAudioSwitchStateImplData(const AZ::rapidxml::xml_node<char>* audioSwitchStateNode) override;
        void DeleteAudioSwitchStateImplData(IATLSwitchStateImplData* oldSwitchStateImplData) override;

        IATLEnvironmentImplData* NewAudioEnvironmentImplData(const AZ::rapidxml::xml_node<char>* audioEnvironmentNode) override;
        void DeleteAudioEnvironmentImplData(IATLEnvironmentImplData* oldEnvironmentImplData) override;

        ATLAudioObjectData_Amplitude* NewGlobalAudioObjectData(TAudioObjectID objectId) override;
        ATLAudioObjectData_Amplitude* NewAudioObjectData(TAudioObjectID objectId) override;
        void DeleteAudioObjectData(IATLAudioObjectData* oldObjectData) override;

        ATLListenerData_Amplitude* NewDefaultAudioListenerObjectData(TATLIDType listenerId) override;
        ATLListenerData_Amplitude* NewAudioListenerObjectData(TATLIDType listenerId) override;
        void DeleteAudioListenerObjectData(IATLListenerData* oldListenerData) override;

        ATLEventData_Amplitude* NewAudioEventData(TAudioEventID eventId) override;
        void DeleteAudioEventData(IATLEventData* oldEventData) override;
        void ResetAudioEventData(IATLEventData* eventData) override;

        [[nodiscard]] const char* const GetImplSubPath() const override;
        void SetLanguage(const char* language) override;

        // Functions below are only used when AM_RELEASE is not defined
        [[nodiscard]] const char* const GetImplementationNameString() const override;
        void GetMemoryInfo(SAudioImplMemoryInfo& memoryInfo) const override;
        AZStd::vector<AudioImplMemoryPoolInfo> GetMemoryPoolInfo() override;

        bool CreateAudioSource(const SAudioInputConfig& sourceConfig) override;
        void DestroyAudioSource(TAudioSourceId sourceId) override;

        void SetPanningMode(PanningMode mode) override;

    protected:
        AZStd::string m_projectBasePath;
        AZStd::string m_engineConfig;
        AZStd::string m_soundbanksPath;

    private:
        static constexpr char kAmplitudeImplSubPath[] = "amplitude";
        static constexpr char kAmplitudeGlobalAudioObjectName[] = "AM-GlobalAudioObject";
        static constexpr float kObstructionOcclusionMin = 0.0f;
        static constexpr float kObstructionOcclusionMax = 1.0f;

        struct SEnvPairCompare
        {
            bool operator()(const AZStd::pair<const AmBusID, float>& pair1, const AZStd::pair<const AmBusID, float>& pair2) const;
        };

        EAudioRequestStatus UpdateEnvironmentAmounts(const ATLAudioObjectData_Amplitude* audioObjectData);

        Entity _globalGameObject;
        AmEntityID _globalGameObjectId;

        AmEntityID _defaultListenerGameObjectId;

        AmBankID _initBankId;

        std::shared_ptr<O3DEFileSystem> _fileLoader;

#if !defined(AM_RELEASE)
        bool _isProfilerInitialized;
        AZStd::vector<AudioImplMemoryPoolInfo> _debugMemoryInfo;
        AZStd::string _fullImplString;
        AZStd::string _speakerConfigString;
#endif // !AM_RELEASE
    };
} // namespace Audio
