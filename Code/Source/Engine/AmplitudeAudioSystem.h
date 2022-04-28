// Copyright (c) 2021-present Sparky Studios. All rights reserved.
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

#include <Engine/ATLEntities_amplitude.h>

#include <SparkyStudios/Audio/Amplitude/Amplitude.h>

namespace Audio
{
    class AmplitudeAudioSystem : public AudioSystemImplementation
    {
    public:
        AZ_RTTI(AmplitudeAudioSystem, "{4A44DC87-7082-4220-809C-F5C829552F4F}", AudioSystemImplementation);
        AUDIO_IMPL_CLASS_ALLOCATOR(AmplitudeAudioSystem);

        explicit AmplitudeAudioSystem(const char* assetsPlatformName);
        ~AmplitudeAudioSystem() override;

        // AudioSystemImplementationNotificationBus
        void OnAudioSystemLoseFocus() override;
        void OnAudioSystemGetFocus() override;
        void OnAudioSystemMuteAll() override;
        void OnAudioSystemUnmuteAll() override;
        void OnAudioSystemRefresh() override;
        // ~AudioSystemImplementationNotificationBus

        // AudioSystemImplementationRequestBus
        void Update(float updateIntervalMS) override;

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
            const SATLSourceData* pSourceData) override;
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

        SATLAudioObjectData_Amplitude* NewGlobalAudioObjectData(TAudioObjectID objectId) override;
        SATLAudioObjectData_Amplitude* NewAudioObjectData(TAudioObjectID objectId) override;
        void DeleteAudioObjectData(IATLAudioObjectData* oldObjectData) override;

        SATLListenerData_Amplitude* NewDefaultAudioListenerObjectData(TATLIDType objectId) override;
        SATLListenerData_Amplitude* NewAudioListenerObjectData(TATLIDType objectId) override;
        void DeleteAudioListenerObjectData(IATLListenerData* oldListenerData) override;

        SATLEventData_Amplitude* NewAudioEventData(TAudioEventID eventId) override;
        void DeleteAudioEventData(IATLEventData* oldEventData) override;
        void ResetAudioEventData(IATLEventData* eventData) override;

        [[nodiscard]] const char* const GetImplSubPath() const override;
        void SetLanguage(const char* language) override;

        // Functions below are only used when AMPLITUDE_RELEASE is not defined
        [[nodiscard]] const char* const GetImplementationNameString() const override;
        void GetMemoryInfo(SAudioImplMemoryInfo& memoryInfo) const override;
        AZStd::vector<AudioImplMemoryPoolInfo> GetMemoryPoolInfo() override;

        bool CreateAudioSource(const SAudioInputConfig& sourceConfig) override;
        void DestroyAudioSource(TAudioSourceId sourceId) override;

        void SetPanningMode(PanningMode mode) override;
        // ~AudioSystemImplementationRequestBus

    protected:
        void SetBankPaths();

        AZStd::string m_soundbankFolder;
        AZStd::string m_localizedSoundbankFolder;
        AZStd::string m_assetsPlatform;

    private:
        static const char* const AmplitudeImplSubPath;
        static const char* const AmplitudeGlobalAudioObjectName;
        static const float ObstructionOcclusionMin;
        static const float ObstructionOcclusionMax;

        struct SEnvPairCompare
        {
            bool operator()(const AZStd::pair<const AmBusID, float>& pair1, const AZStd::pair<const AmBusID, float>& pair2) const;
        };

        // SATLSwitchStateImplData_Amplitude* ParseWwiseSwitchOrState(const AZ::rapidxml::xml_node<char>* node, EWwiseSwitchType type);
        // SATLSwitchStateImplData_Amplitude* ParseWwiseRtpcSwitch(const AZ::rapidxml::xml_node<char>* node);
        // void ParseRtpcImpl(const AZ::rapidxml::xml_node<char>* node, AmRtpcID& akRtpcId, float& mult, float& shift);

        // EAudioRequestStatus PrepUnprepTriggerSync(const IATLTriggerImplData* const triggerData, bool prepare);
        // EAudioRequestStatus PrepUnprepTriggerAsync(
        //     const IATLTriggerImplData* const triggerData, IATLEventData* const eventData, bool prepare);

        // EAudioRequestStatus PostEnvironmentAmounts(IATLAudioObjectData* const audioObjectData);

        Entity m_globalGameObject;
        AmEntityID m_globalGameObjectID;

        AmEntityID m_defaultListenerGameObjectID;

        AmBankID m_initBankID;

        SparkyStudios::Audio::Amplitude::FileLoader _fileLoader;

        SparkyStudios::Audio::Amplitude::Engine* _engine;

#if !defined(AMPLITUDE_RELEASE)
        bool m_isCommSystemInitialized;
        AZStd::vector<AudioImplMemoryPoolInfo> m_debugMemoryInfo;
        AZStd::string m_fullImplString;
        AZStd::string m_speakerConfigString;
#endif // !AMPLITUDE_RELEASE
    };
} // namespace Audio