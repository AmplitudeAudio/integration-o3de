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
#include <Engine/ATLEntities_amplitude.h>
// #include <FileIOHandler_wwise.h>
#include <IAudioSystemImplementation.h>

#include <SparkyStudios/Audio/Amplitude/Amplitude.h>

namespace Audio
{
    class AmplitudeAudioSystem : public AudioSystemImplementation
    {
    public:
        AUDIO_IMPL_CLASS_ALLOCATOR(AmplitudeAudioSystem)

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
        void Update(const float updateIntervalMS) override;

        EAudioRequestStatus Initialize() override;
        EAudioRequestStatus ShutDown() override;
        EAudioRequestStatus Release() override;

        EAudioRequestStatus StopAllSounds() override;

        EAudioRequestStatus RegisterAudioObject(IATLAudioObjectData* const audioObjectData, const char* const objectName) override;
        EAudioRequestStatus UnregisterAudioObject(IATLAudioObjectData* const audioObjectData) override;
        EAudioRequestStatus ResetAudioObject(IATLAudioObjectData* const audioObjectData) override;
        EAudioRequestStatus UpdateAudioObject(IATLAudioObjectData* const audioObjectData) override;

        EAudioRequestStatus PrepareTriggerSync(
            IATLAudioObjectData* const audioObjectData, const IATLTriggerImplData* const triggerData) override;
        EAudioRequestStatus UnprepareTriggerSync(
            IATLAudioObjectData* const audioObjectData, const IATLTriggerImplData* const triggerData) override;
        EAudioRequestStatus PrepareTriggerAsync(
            IATLAudioObjectData* const audioObjectData,
            const IATLTriggerImplData* const triggerData,
            IATLEventData* const eventData) override;
        EAudioRequestStatus UnprepareTriggerAsync(
            IATLAudioObjectData* const audioObjectData,
            const IATLTriggerImplData* const triggerData,
            IATLEventData* const eventData) override;
        EAudioRequestStatus ActivateTrigger(
            IATLAudioObjectData* const audioObjectData,
            const IATLTriggerImplData* const triggerData,
            IATLEventData* const eventData,
            const SATLSourceData* const pSourceData) override;
        EAudioRequestStatus StopEvent(IATLAudioObjectData* const audioObjectData, const IATLEventData* const eventData) override;
        EAudioRequestStatus StopAllEvents(IATLAudioObjectData* const audioObjectData) override;
        EAudioRequestStatus SetPosition(IATLAudioObjectData* const audioObjectData, const SATLWorldPosition& worldPosition) override;
        EAudioRequestStatus SetMultiplePositions(
            IATLAudioObjectData* const audioObjectData, const MultiPositionParams& multiPositionParams) override;
        EAudioRequestStatus SetEnvironment(
            IATLAudioObjectData* const audioObjectData, const IATLEnvironmentImplData* const environmentData, const float amount) override;
        EAudioRequestStatus SetRtpc(
            IATLAudioObjectData* const audioObjectData, const IATLRtpcImplData* const rtpcData, const float value) override;
        EAudioRequestStatus SetSwitchState(
            IATLAudioObjectData* const audioObjectData, const IATLSwitchStateImplData* const switchStateData) override;
        EAudioRequestStatus SetObstructionOcclusion(
            IATLAudioObjectData* const audioObjectData, const float obstruction, const float occlusion) override;
        EAudioRequestStatus SetListenerPosition(IATLListenerData* const listenerData, const SATLWorldPosition& newPosition) override;
        EAudioRequestStatus ResetRtpc(IATLAudioObjectData* const audioObjectData, const IATLRtpcImplData* const rtpcData) override;

        EAudioRequestStatus RegisterInMemoryFile(SATLAudioFileEntryInfo* const audioFileEntry) override;
        EAudioRequestStatus UnregisterInMemoryFile(SATLAudioFileEntryInfo* const audioFileEntry) override;

        EAudioRequestStatus ParseAudioFileEntry(
            const AZ::rapidxml::xml_node<char>* audioFileEntryNode, SATLAudioFileEntryInfo* const fileEntryInfo) override;
        void DeleteAudioFileEntryData(IATLAudioFileEntryData* const oldAudioFileEntryData) override;
        const char* const GetAudioFileLocation(SATLAudioFileEntryInfo* const fileEntryInfo) override;

        IATLTriggerImplData* NewAudioTriggerImplData(const AZ::rapidxml::xml_node<char>* audioTriggerNode) override;
        void DeleteAudioTriggerImplData(IATLTriggerImplData* const oldTriggerImplData) override;

        IATLRtpcImplData* NewAudioRtpcImplData(const AZ::rapidxml::xml_node<char>* audioRtpcNode) override;
        void DeleteAudioRtpcImplData(IATLRtpcImplData* const oldRtpcImplData) override;

        IATLSwitchStateImplData* NewAudioSwitchStateImplData(const AZ::rapidxml::xml_node<char>* audioSwitchStateNode) override;
        void DeleteAudioSwitchStateImplData(IATLSwitchStateImplData* const oldSwitchStateImplData) override;

        IATLEnvironmentImplData* NewAudioEnvironmentImplData(const AZ::rapidxml::xml_node<char>* audioEnvironmentNode) override;
        void DeleteAudioEnvironmentImplData(IATLEnvironmentImplData* const oldEnvironmentImplData) override;

        SATLAudioObjectData_Amplitude* NewGlobalAudioObjectData(const TAudioObjectID objectId) override;
        SATLAudioObjectData_Amplitude* NewAudioObjectData(const TAudioObjectID objectId) override;
        void DeleteAudioObjectData(IATLAudioObjectData* const oldObjectData) override;

        SATLListenerData_Amplitude* NewDefaultAudioListenerObjectData(const TATLIDType objectId) override;
        SATLListenerData_Amplitude* NewAudioListenerObjectData(const TATLIDType objectId) override;
        void DeleteAudioListenerObjectData(IATLListenerData* const oldListenerData) override;

        SATLEventData_Amplitude* NewAudioEventData(const TAudioEventID eventId) override;
        void DeleteAudioEventData(IATLEventData* const oldEventData) override;
        void ResetAudioEventData(IATLEventData* const eventData) override;

        const char* const GetImplSubPath() const override;
        void SetLanguage(const char* const language) override;

        // Functions below are only used when AMPLITUDE_RELEASE is not defined
        const char* const GetImplementationNameString() const override;
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