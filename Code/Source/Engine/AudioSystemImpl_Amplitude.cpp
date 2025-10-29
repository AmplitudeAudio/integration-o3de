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

#include <AzCore/PlatformIncl.h>
#include <Engine/AudioSystemImpl_Amplitude.h>

#include <AzCore/AzCore_Traits_Platform.h>
#include <AzCore/Console/ILogger.h>
#include <AzCore/Debug/Profiler.h>
#include <AzCore/std/string/conversions.h>
#include <AzCore/StringFunc/StringFunc.h>
#include <AzCore/Utils/Utils.h>
#include <platform.h>

#include <AudioAllocators.h>
#include <IAudioSystem.h>

#include <Engine/Common.h>
#include <Tools/Config.h>

#define AM_MEM_POOLS_COUNT static_cast<AZ::s32>(SparkyStudios::Audio::Amplitude::eMemoryPoolKind_COUNT)

namespace Audio
{
    using namespace SparkyStudios::Audio::Amplitude;

    namespace
    {
        O3DELogger gLogger;

        bool gAudioDeviceInitializationEvent = false;

        int GetAssetType(const SATLSourceData* sourceData)
        {
            if (!sourceData)
                return eAAT_NONE;

            return sourceData->m_sourceInfo.m_codecType == eACT_STREAM_PCM ? eAAT_STREAM : eAAT_SOURCE;
        }

        void DeviceNotification(eDeviceNotification notification, const DeviceDescription& device, Driver* driver)
        {
            AZ_UNUSED(device, driver)

            if (notification == eDeviceNotification_Started)
                gAudioDeviceInitializationEvent = true;
        }
    } // namespace

    CAudioSystemImpl_Amplitude::CAudioSystemImpl_Amplitude(const char* projectBuildPath, const char* engineConfig)
        : _globalGameObjectId(GLOBAL_AUDIO_OBJECT_ID)
        , _defaultListenerGameObjectId(kAmInvalidObjectId)
        , _initBankId(kAmInvalidObjectId)
        , _fileLoader(nullptr)
        , m_projectBasePath(projectBuildPath)
        , m_engineConfig(engineConfig)
#if !defined(AM_RELEASE)
        , _isProfilerInitialized(false) // TODO: Profiler
#endif // !AM_RELEASE
    {
        m_soundbanksPath = m_projectBasePath + '/' + kSoundBanksFolder + '/';

#if !defined(AM_RELEASE) && !defined(AM_NO_MEMORY_STATS)
        _fullImplString = AZStd::string::format("%s (%s)", amVersion.text.c_str(), m_projectBasePath.c_str());

        // Set up memory categories for debug tracking, do this early before initializing Amplitude, so they are available
        // before any allocations through hooks occur.
        AZLOG_DEBUG("Memory Categories:")
        _debugMemoryInfo.reserve(AM_MEM_POOLS_COUNT);

        for (AZ::s32 memId = 0; memId < AM_MEM_POOLS_COUNT; ++memId)
        {
            AudioImplMemoryPoolInfo memInfo;
            azstrcpy(memInfo.m_poolName, sizeof(memInfo.m_poolName), gMemoryManagerPools[memId]);
            memInfo.m_poolId = memId;

            _debugMemoryInfo.push_back(memInfo);

            AZLOG_DEBUG("  Memory category ID: %d - '%s'", memId, gMemoryManagerPools[memId])
        }

        // Add one more category for global stats.
        AudioImplMemoryPoolInfo memInfo;
        azstrcpy(memInfo.m_poolName, sizeof(memInfo.m_poolName), "Global");
        _debugMemoryInfo.push_back(memInfo);
#endif

        AudioSystemImplementationRequestBus::Handler::BusConnect();
        AudioSystemImplementationNotificationBus::Handler::BusConnect();
    }

    CAudioSystemImpl_Amplitude::~CAudioSystemImpl_Amplitude()
    {
        AudioSystemImplementationRequestBus::Handler::BusDisconnect();
        AudioSystemImplementationNotificationBus::Handler::BusDisconnect();
    }

    void CAudioSystemImpl_Amplitude::OnAudioSystemLoseFocus()
    {
#if defined(AM_RELEASE)
        if (amEngine == nullptr)
            return;

        amEngine->Pause(true);
#endif // defined(AM_RELEASE)
    }

    void CAudioSystemImpl_Amplitude::OnAudioSystemGetFocus()
    {
#if defined(AM_RELEASE)
        if (amEngine == nullptr)
            return;

        amEngine->Pause(false);
#endif // defined(AM_RELEASE)
    }

    void CAudioSystemImpl_Amplitude::OnAudioSystemMuteAll()
    {
        if (amEngine == nullptr)
            return;

        amEngine->SetMute(true);
    }

    void CAudioSystemImpl_Amplitude::OnAudioSystemUnmuteAll()
    {
        if (amEngine == nullptr)
            return;

        amEngine->SetMute(false);
    }

    void CAudioSystemImpl_Amplitude::OnAudioSystemRefresh()
    {
        if (amEngine != nullptr && amEngine->IsInitialized())
        {
            if (_initBankId != kAmInvalidObjectId)
                amEngine->UnloadSoundBank(_initBankId);

            _initBankId = kAmInvalidObjectId;

            if (!amEngine->LoadSoundBank(AM_STRING_TO_OS_STRING(kInitBankFile), _initBankId))
            {
                amLogError("Amplitude failed to load %s", kInitBankFile);
                _initBankId = kAmInvalidObjectId;
                AZ_Assert(false, "[Amplitude] Failed to load %s !", kInitBankFile)
            }
        }
        else
        {
            Initialize();
        }
    }

    void CAudioSystemImpl_Amplitude::Update(const float updateIntervalMs)
    {
        AZ_PROFILE_FUNCTION(Audio);

        if (amEngine == nullptr || !amEngine->IsInitialized())
            return;

        amEngine->AdvanceFrame(updateIntervalMs);
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::Initialize()
    {
        Logger::SetLogger(&gLogger);

        MemoryManager::Initialize(std::make_unique<O3DEMemoryAllocator>());

        if (amMemory == nullptr)
        {
            amLogError("Amplitude::MemoryManager::Init() has failed.");
            ShutDown();
            return EAudioRequestStatus::Failure;
        }

        _fileLoader = AmSharedPtr<O3DEFileSystem, eMemoryPoolKind_IO>::Make();

        // TODO: Localization here? (not yet supported by the SDK)
        _fileLoader->SetBasePath(AM_STRING_TO_OS_STRING(m_projectBasePath.c_str()));

        RegisterDeviceNotificationCallback(DeviceNotification);

        amEngine->SetFileSystem(_fileLoader);

        // Open the file system
        amEngine->StartOpenFileSystem();
        while (!amEngine->TryFinalizeOpenFileSystem()) // While the file system is still opening
            Thread::Sleep(1); // Wait for the file system to open

        // Register all the default extensions shipped with the engine
        Engine::RegisterDefaultExtensions();

        // TODO: Support platform-specific engine initialization
        if (!amEngine->Initialize(AM_STRING_TO_OS_STRING(m_engineConfig.c_str())))
        {
            amLogError("Amplitude Engine has failed to initialize.");
            ShutDown();
            return EAudioRequestStatus::Failure;
        }

        _globalGameObject = amEngine->AddEntity(_globalGameObjectId);
        if (!_globalGameObject.Valid())
            amLogWarning("Amplitude::Engine::AddEntity() failed.");

        if (!amEngine->LoadSoundBank(AM_STRING_TO_OS_STRING(kInitBankFile), _initBankId))
        {
            amLogError("Amplitude failed to load %s", kInitBankFile);
            _initBankId = kAmInvalidObjectId;
            AZ_Assert(false, "<Amplitude> Failed to load %s !", kInitBankFile)
        }

        // Start loading sound files.
        amEngine->StartLoadSoundFiles();
        while (!amEngine->TryFinalizeLoadSoundFiles())
            Thread::Sleep(1);

        return EAudioRequestStatus::Success;
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::ShutDown()
    {
        UnregisterDeviceNotificationCallback();

        if (amEngine != nullptr)
        {
            if (amEngine->IsInitialized())
            {
                // UnRegister the DummyGameObject
                amEngine->RemoveEntity(&_globalGameObject);

                if (_globalGameObject.Valid())
                {
                    amLogWarning("Amplitude::Engine::RemoveEntity() failed.");
                }

                amEngine->UnloadSoundBanks();

                amEngine->Deinitialize();

                // Close the file system
                amEngine->StartCloseFileSystem();
                while (!amEngine->TryFinalizeCloseFileSystem())
                    Thread::Sleep(1);

                // Unregister all default extensions
                Engine::UnregisterDefaultExtensions();
            }

            // Destroy the Amplitude engine instance
            Engine::DestroyInstance();
        }

        // Terminate the Memory Manager
        if (MemoryManager::IsInitialized())
            MemoryManager::Deinitialize();

        return EAudioRequestStatus::Success;
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::Release()
    {
        // Deleting this object and destroying the allocator has been moved to AmplitudeAudioSystemComponent
        return EAudioRequestStatus::Success;
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::StopAllSounds()
    {
        if (amEngine != nullptr)
        {
            amEngine->StopAll();
            return EAudioRequestStatus::Success;
        }

        return EAudioRequestStatus::Failure;
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::RegisterAudioObject(
        IATLAudioObjectData* const audioObjectData, [[maybe_unused]] const char* const objectName)
    {
        if (audioObjectData && amEngine != nullptr)
        {
            const auto* const implObjectData = dynamic_cast<ATLAudioObjectData_Amplitude*>(audioObjectData);

            const Entity entity = amEngine->AddEntity(implObjectData->mAmID);

            if (!entity.Valid())
            {
                amLogWarning("Amplitude::Engine::AddEntity() failed.");
            }

            return BoolToARS(entity.Valid());
        }

        amLogWarning("Amplitude::Engine::AddEntity() failed, audio object data was null or Amplitude was not initialized.");
        return EAudioRequestStatus::Failure;
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::UnregisterAudioObject(IATLAudioObjectData* const audioObjectData)
    {
        if (audioObjectData && amEngine != nullptr)
        {
            const auto* const implObjectData = dynamic_cast<ATLAudioObjectData_Amplitude*>(audioObjectData);

            amEngine->RemoveEntity(implObjectData->mAmID);
            const Entity entity = amEngine->GetEntity(implObjectData->mAmID);

            if (entity.Valid())
            {
                amLogWarning("Amplitude::Engine::RemoveEntity() failed.");
            }

            return BoolToARS(!entity.Valid());
        }

        amLogWarning("Amplitude::Engine::AddEntity() failed, audio object data was null or Amplitude was not initialized.");
        return EAudioRequestStatus::Failure;
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::ResetAudioObject(IATLAudioObjectData* const audioObjectData)
    {
        if (audioObjectData)
        {
            auto* const implObjectData = dynamic_cast<ATLAudioObjectData_Amplitude*>(audioObjectData);

            implObjectData->cEnvironmentImplAmounts.clear();
            implObjectData->mNeedsToUpdateEnvironments = false;

            return EAudioRequestStatus::Success;
        }

        amLogWarning("Amplitude::Engine::AddEntity() failed, audio object data was null.");
        return EAudioRequestStatus::Failure;
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::UpdateAudioObject(IATLAudioObjectData* const audioObjectData)
    {
        AZ_PROFILE_FUNCTION(Audio);

        auto result = EAudioRequestStatus::Success;

        if (audioObjectData)
        {
            const auto* const implObjectData = dynamic_cast<ATLAudioObjectData_Amplitude*>(audioObjectData);

            if (implObjectData->mNeedsToUpdateEnvironments)
                result = UpdateEnvironmentAmounts(implObjectData);
        }

        return result;
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::PrepareTriggerSync(
        [[maybe_unused]] IATLAudioObjectData* const audioObjectData, [[maybe_unused]] const IATLTriggerImplData* const triggerData)
    {
        // Not needed in Amplitude
        return EAudioRequestStatus::Success;
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::UnprepareTriggerSync(
        [[maybe_unused]] IATLAudioObjectData* const audioObjectData, [[maybe_unused]] const IATLTriggerImplData* const triggerData)
    {
        // Not needed in Amplitude
        return EAudioRequestStatus::Success;
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::PrepareTriggerAsync(
        [[maybe_unused]] IATLAudioObjectData* const audioObjectData,
        [[maybe_unused]] const IATLTriggerImplData* const triggerData,
        [[maybe_unused]] IATLEventData* const eventData)
    {
        // Not needed in Amplitude
        return EAudioRequestStatus::Success;
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::UnprepareTriggerAsync(
        [[maybe_unused]] IATLAudioObjectData* const audioObjectData,
        [[maybe_unused]] const IATLTriggerImplData* const triggerData,
        [[maybe_unused]] IATLEventData* const eventData)
    {
        // Not needed in Amplitude
        return EAudioRequestStatus::Success;
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::ActivateTrigger(
        IATLAudioObjectData* const audioObjectData,
        const IATLTriggerImplData* const triggerData,
        IATLEventData* const eventData,
        const SATLSourceData* const sourceData)
    {
        auto result = EAudioRequestStatus::Failure;

        const auto* const implObjectData = dynamic_cast<ATLAudioObjectData_Amplitude*>(audioObjectData);
        const auto* const implTriggerData = dynamic_cast<const ATLTriggerImplData_Amplitude*>(triggerData);

        if (auto* const implEventData = dynamic_cast<ATLEventData_Amplitude*>(eventData);
            implObjectData && implTriggerData && implEventData)
        {
            AmEntityID entityId;

            if (implObjectData->mHasPosition)
                entityId = implObjectData->mAmID;
            else
                entityId = _globalGameObjectId;

            switch (GetAssetType(sourceData))
            {
            case eAAT_SOURCE:
                {
                    AZ_Assert(sourceData, "SourceData not provided for source type!") break;
                }

            case eAAT_STREAM:
                [[fallthrough]];
            case eAAT_NONE:
                [[fallthrough]];
            default:
                {
                    const Entity entity = amEngine->GetEntity(entityId);
#if !defined(AM_RELEASE)
                    if (entityId != kAmInvalidObjectId && !entity.Valid())
                    {
                        amLogDebug("Unable to find an entity with ID: " AM_ID_CHAR_FMT, entityId);
                    }
#endif

                    if (const EventHandle event = amEngine->GetEventHandle(implTriggerData->mAmID))
                    {
                        if (const EventCanceler canceler = amEngine->Trigger(event, entity); canceler.Valid())
                        {
                            implEventData->mAudioEventState = eAES_PLAYING;
                            implEventData->mEventCanceler = canceler;

                            // canceler.GetEvent()->OnFinish([this, eventData]
                            // {
                            //     DeleteAudioEventData(eventData);
                            // });

                            result = EAudioRequestStatus::Success;
                        }
                    }
                    else
                    {
                        amLogWarning(
                            "Unable to activate a trigger, the associated Amplitude event with ID " AM_ID_CHAR_FMT
                            " has not been found in loaded banks.",
                            implTriggerData->mAmID);
                    }
                    break;
                }
            }
        }
        else
        {
            amLogError("Invalid AudioObjectData, ATLTriggerData or EventData passed to the Amplitude implementation of ActivateTrigger.");
        }

        return result;
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::StopEvent(
        [[maybe_unused]] IATLAudioObjectData* const audioObjectData, const IATLEventData* const eventData)
    {
        auto result = EAudioRequestStatus::Failure;

        if (auto* const implEventData = dynamic_cast<const ATLEventData_Amplitude*>(eventData))
        {
            switch (implEventData->mAudioEventState)
            {
            case eAES_PLAYING:
                {
                    if (implEventData->mEventCanceler.GetEvent()->IsRunning())
                    {
                        if (implEventData->mEventCanceler.Valid())
                        {
                            implEventData->mEventCanceler.Cancel();
                            result = EAudioRequestStatus::Success;
                        }
                        else
                        {
                            amLogError("Encountered a running event without a valid canceler.");
                        }
                    }
                    break;
                }
            default:
                {
                    amLogError("Stopping an event of this type is not supported yet");
                    break;
                }
            }
        }
        else
        {
            amLogError("Invalid EventData passed to StopEvent.");
        }

        return result;
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::StopAllEvents([[maybe_unused]] IATLAudioObjectData* const audioObjectData)
    {
        amEngine->CancelAllEvents();

        return EAudioRequestStatus::Success;
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::SetPosition(
        IATLAudioObjectData* const audioObjectData, const SATLWorldPosition& worldPosition)
    {
        auto result = EAudioRequestStatus::Failure;

        if (const auto* implObjectData = dynamic_cast<ATLAudioObjectData_Amplitude*>(audioObjectData))
        {
            if (const Entity entity = amEngine->GetEntity(implObjectData->mAmID); entity.Valid())
            {
                entity.SetLocation(ATLVec3ToAmVec3(worldPosition.GetPositionVec()));
                entity.SetOrientation(
                    ATLFwdUpToAmOrientation(worldPosition.GetForwardVec().GetNormalized(), worldPosition.GetUpVec().GetNormalized()));
            }
            else
            {
                amLogError("Invalid AudioObjectData passed to SetPosition.");
            }

            result = EAudioRequestStatus::Success;
        }
        else
        {
            amLogError("Invalid AudioObjectData passed to SetPosition.");
        }

        return result;
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::SetMultiplePositions(
        [[maybe_unused]] IATLAudioObjectData* const audioObjectData, [[maybe_unused]] const MultiPositionParams& multiPositionParams)
    {
        auto result = EAudioRequestStatus::Failure;

        if ([[maybe_unused]] const auto* implObjectData = dynamic_cast<ATLAudioObjectData_Amplitude*>(audioObjectData))
        {
            // const auto currentPositionCount = implObjectData->mPositionEntities.size();
            // const auto newPositionCount = multiPositionParams.m_positions.size();

            // TODO: Unsupported by SDK
        }
        else
        {
            amLogError("Invalid AudioObjectData passed to SetPosition.");
        }

        return result;
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::SetEnvironment(
        IATLAudioObjectData* const audioObjectData, const IATLEnvironmentImplData* const environmentData, const float amount)
    {
        auto result = EAudioRequestStatus::Failure;

        const auto* implObjectData = dynamic_cast<ATLAudioObjectData_Amplitude*>(audioObjectData);

        if (const auto* implEnvironmentData = dynamic_cast<const ATLEnvironmentImplData_Amplitude*>(environmentData);
            implObjectData && implEnvironmentData)
        {
            const Environment env = amEngine->GetEnvironment(implEnvironmentData->mAmEnvID);
            if (!env.Valid())
                return result;

            const Entity entity = amEngine->GetEntity(implObjectData->mAmID);
            if (!entity.Valid())
                return result;

            env.SetEffect(implEnvironmentData->nAmEffectID);
            entity.SetEnvironmentFactor(env.GetId(), amount);

            result = EAudioRequestStatus::Success;
        }
        else
        {
            amLogError("Invalid AudioObjectData or EnvironmentData passed to SetEnvironment");
        }

        return result;
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::SetRtpc(
        [[maybe_unused]] IATLAudioObjectData* const audioObjectData, const IATLRtpcImplData* const rtpcData, const float value)
    {
        auto result = EAudioRequestStatus::Failure;

        if (const auto* const implRtpcData = dynamic_cast<const ATLRtpcImplData_Amplitude*>(rtpcData))
        {
            amEngine->SetRtpcValue(implRtpcData->mAmID, value);
            result = EAudioRequestStatus::Success;
        }
        else
        {
            amLogError("Invalid AudioObjectData or RtpcData passed to SetRtpc");
        }

        return result;
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::SetSwitchState(
        [[maybe_unused]] IATLAudioObjectData* const audioObjectData, const IATLSwitchStateImplData* const switchStateData)
    {
        auto result = EAudioRequestStatus::Failure;

        if (const auto* const implSwitchData = dynamic_cast<const ATLSwitchStateImplData_Amplitude*>(switchStateData))
        {
            amEngine->SetSwitchState(implSwitchData->mAmSwitchID, implSwitchData->mAmStateID);
            result = EAudioRequestStatus::Success;
        }
        else
        {
            amLogError("Invalid AudioObjectData or RtpcData passed to SetRtpc");
        }

        return result;
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::SetObstructionOcclusion(
        IATLAudioObjectData* const audioObjectData, const float obstruction, const float occlusion)
    {
        if (audioObjectData)
        {
            const auto* const implObjectData = dynamic_cast<ATLAudioObjectData_Amplitude*>(audioObjectData);

            const Entity entity = amEngine->GetEntity(implObjectData->mAmID);

            if (!entity.Valid())
            {
                amLogError("Amplitude::Engine::GetEntity() failed with entity ID " AM_ID_CHAR_FMT, implObjectData->mAmID);
                return EAudioRequestStatus::Failure;
            }

            entity.SetObstruction(obstruction);
            entity.SetOcclusion(occlusion);

            return EAudioRequestStatus::Success;
        }

        amLogError("Amplitude::Engine::GetEntity() failed, audio object data was null.");
        return EAudioRequestStatus::Failure;
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::SetListenerPosition(
        IATLListenerData* const listenerData, const SATLWorldPosition& newPosition)
    {
        auto result = EAudioRequestStatus::Failure;

        if (const auto* const implObjectData = dynamic_cast<ATLListenerData_Amplitude*>(listenerData))
        {
            if (const Listener listener = amEngine->AddListener(implObjectData->mAmID); listener.Valid())
            {
                listener.SetLocation(ATLVec3ToAmVec3(newPosition.GetPositionVec()));
                listener.SetOrientation(
                    ATLFwdUpToAmOrientation(newPosition.GetForwardVec().GetNormalized(), newPosition.GetUpVec().GetNormalized()));

                result = EAudioRequestStatus::Success;
            }
            else
            {
                amLogError("Invalid ListenerData passed to SetPosition.");
            }
        }
        else
        {
            amLogError("Invalid IATLListenerData passed to SetPosition.");
        }

        return result;
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::ResetRtpc(
        [[maybe_unused]] IATLAudioObjectData* const audioObjectData, const IATLRtpcImplData* const rtpcData)
    {
        auto result = EAudioRequestStatus::Failure;

        if (const auto* const implRtpcData = dynamic_cast<const ATLRtpcImplData_Amplitude*>(rtpcData))
        {
            if (RtpcHandle rtpc = amEngine->GetRtpcHandle(implRtpcData->mAmID))
            {
                rtpc->Reset();
                result = EAudioRequestStatus::Success;
            }
            else
            {
                amLogDebug("Unable to get RTPC handle for ID " AM_ID_CHAR_FMT, implRtpcData->mAmID);
            }
        }
        else
        {
            amLogError("Invalid AudioObjectData or RtpcData passed to SetRtpc");
        }

        return result;
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::RegisterInMemoryFile(SATLAudioFileEntryInfo* const audioFileEntry)
    {
        auto result = EAudioRequestStatus::Failure;

        if (audioFileEntry)
        {
            if (auto* const implFileEntryData = dynamic_cast<ATLAudioFileEntryData_Amplitude*>(audioFileEntry->pImplData))
            {
                if (AmBankID bankId = kAmInvalidObjectId;
                    amEngine->LoadSoundBankFromMemoryView(audioFileEntry->pFileData, audioFileEntry->nSize, bankId))
                {
                    implFileEntryData->mAmID = bankId;
                    result = EAudioRequestStatus::Success;
                }
                else
                {
                    implFileEntryData->mAmID = kAmInvalidObjectId;
                    amLogError("Failed to load sound bank '%s'\n", audioFileEntry->sFileName);
                }
            }
            else
            {
                amLogError("Invalid AudioFileEntryData passed to RegisterInMemoryFile");
            }
        }

        return result;
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::UnregisterInMemoryFile(SATLAudioFileEntryInfo* const audioFileEntry)
    {
        auto result = EAudioRequestStatus::Failure;

        if (audioFileEntry)
        {
            if (const auto* const implFileEntryData = dynamic_cast<ATLAudioFileEntryData_Amplitude*>(audioFileEntry->pImplData))
            {
                amEngine->UnloadSoundBank(implFileEntryData->mAmID);

                result = EAudioRequestStatus::Success;
            }
            else
            {
                amLogError("Invalid AudioFileEntryData passed to UnregisterInMemoryFile");
            }
        }

        return result;
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::ParseAudioFileEntry(
        const AZ::rapidxml::xml_node<char>* audioFileEntryNode, SATLAudioFileEntryInfo* const fileEntryInfo)
    {
        auto result = EAudioRequestStatus::Failure;

        if (audioFileEntryNode && azstricmp(audioFileEntryNode->name(), XmlTags::kFileTag) == 0 && fileEntryInfo)
        {
            const char* name = nullptr;
            if (const auto* nameAttribute = audioFileEntryNode->first_attribute(XmlTags::kNameAttribute, 0, false))
            {
                name = nameAttribute->value();
            }

            AmBankID id = kAmInvalidObjectId;
            if (const auto* idAttribute = audioFileEntryNode->first_attribute(XmlTags::kIdAttribute, 0, false))
            {
                id = AZStd::stoull(AZStd::string(idAttribute->value()));
            }

            bool isLocalized = false;
            if (const auto* localizedAttr = audioFileEntryNode->first_attribute(XmlTags::kLocalizedAttribute, 0, false))
            {
                if (azstricmp(localizedAttr->value(), "true") == 0)
                {
                    isLocalized = true;
                }
            }

            if (name && name[0] != '\0')
            {
                fileEntryInfo->bLocalized = isLocalized;
                fileEntryInfo->sFileName = name;
                fileEntryInfo->nMemoryBlockAlignment = AUDIO_MEMORY_ALIGNMENT;
                fileEntryInfo->pImplData = azcreate(ATLAudioFileEntryData_Amplitude, (id), Audio::AudioImplAllocator);
                result = EAudioRequestStatus::Success;
            }
            else
            {
                fileEntryInfo->sFileName = nullptr;
                fileEntryInfo->nMemoryBlockAlignment = 0;
                fileEntryInfo->pImplData = nullptr;
            }
        }

        return result;
    }

    void CAudioSystemImpl_Amplitude::DeleteAudioFileEntryData(IATLAudioFileEntryData* const oldAudioFileEntryData)
    {
        azdestroy(oldAudioFileEntryData, Audio::AudioImplAllocator, ATLAudioFileEntryData_Amplitude);
    }

    const char* const CAudioSystemImpl_Amplitude::GetAudioFileLocation(SATLAudioFileEntryInfo* const fileEntryInfo)
    {
        const char* location = nullptr;

        if (fileEntryInfo)
        {
            // TODO: Localization here? (not yet supported by SDK)
            location = m_soundbanksPath.c_str();
        }

        return location;
    }

    IATLTriggerImplData* CAudioSystemImpl_Amplitude::NewAudioTriggerImplData(const AZ::rapidxml::xml_node<char>* audioTriggerNode)
    {
        ATLTriggerImplData_Amplitude* newTriggerImpl = nullptr;

        if (audioTriggerNode && azstricmp(audioTriggerNode->name(), XmlTags::kEventTag) == 0)
        {
            if (const auto* nameAttribute = audioTriggerNode->first_attribute(XmlTags::kNameAttribute, 0, false))
            {
                const char* name = nameAttribute->value();

                if (const EventHandle amEvent = amEngine->GetEventHandle(name); amEvent != nullptr)
                {
                    newTriggerImpl = azcreate(ATLTriggerImplData_Amplitude, (amEvent->GetId()), Audio::AudioImplAllocator);
                }
            }
        }

        return newTriggerImpl;
    }

    void CAudioSystemImpl_Amplitude::DeleteAudioTriggerImplData(IATLTriggerImplData* const oldTriggerImplData)
    {
        azdestroy(oldTriggerImplData, Audio::AudioImplAllocator, ATLTriggerImplData_Amplitude);
    }

    IATLRtpcImplData* CAudioSystemImpl_Amplitude::NewAudioRtpcImplData(const AZ::rapidxml::xml_node<char>* audioRtpcNode)
    {
        ATLRtpcImplData_Amplitude* newRtpcImpl = nullptr;

        if (audioRtpcNode && azstricmp(audioRtpcNode->name(), XmlTags::kRtpcTag) == 0)
        {
            if (const auto* nameAttribute = audioRtpcNode->first_attribute(XmlTags::kNameAttribute, 0, false))
            {
                const char* name = nameAttribute->value();

                if (const RtpcHandle amRtpc = amEngine->GetRtpcHandle(name); amRtpc != nullptr)
                {
                    newRtpcImpl = azcreate(ATLRtpcImplData_Amplitude, (amRtpc->GetId()), Audio::AudioImplAllocator);
                }
            }
        }

        return newRtpcImpl;
    }

    void CAudioSystemImpl_Amplitude::DeleteAudioRtpcImplData(IATLRtpcImplData* const oldRtpcImplData)
    {
        azdestroy(oldRtpcImplData, Audio::AudioImplAllocator, ATLRtpcImplData_Amplitude);
    }

    IATLSwitchStateImplData* CAudioSystemImpl_Amplitude::NewAudioSwitchStateImplData(
        const AZ::rapidxml::xml_node<char>* audioSwitchStateNode)
    {
        ATLSwitchStateImplData_Amplitude* newSwitchStateImpl = nullptr;

        if (audioSwitchStateNode && azstricmp(audioSwitchStateNode->name(), XmlTags::kSwitchTag) == 0)
        {
            if (const auto* switchIdAttr = audioSwitchStateNode->first_attribute(XmlTags::kIdAttribute, 0, false); switchIdAttr)
            {
                if (const auto* stateNode = audioSwitchStateNode->first_node(); stateNode)
                {
                    if (const auto* stateIdAttr = stateNode->first_attribute(XmlTags::kIdAttribute, 0, false); stateIdAttr)
                    {
                        const char* switchId = switchIdAttr->value();
                        const char* stateId = stateIdAttr->value();

                        newSwitchStateImpl = azcreate(
                            ATLSwitchStateImplData_Amplitude,
                            (AZStd::stoull(AZStd::string(switchId)), AZStd::stoull(AZStd::string(stateId))), Audio::AudioImplAllocator);
                    }
                }
            }
        }

        return newSwitchStateImpl;
    }

    void CAudioSystemImpl_Amplitude::DeleteAudioSwitchStateImplData(IATLSwitchStateImplData* const oldSwitchStateImplData)
    {
        azdestroy(oldSwitchStateImplData, Audio::AudioImplAllocator, ATLSwitchStateImplData_Amplitude);
    }

    IATLEnvironmentImplData* CAudioSystemImpl_Amplitude::NewAudioEnvironmentImplData(
        const AZ::rapidxml::xml_node<char>* audioEnvironmentNode)
    {
        if (audioEnvironmentNode == nullptr)
            return nullptr;

        ATLEnvironmentImplData_Amplitude* newEnvironmentImpl = nullptr;

        if (azstricmp(audioEnvironmentNode->name(), XmlTags::kEnvironmentTag) == 0)
        {
            const auto* envIdAttr = audioEnvironmentNode->first_attribute(XmlTags::kIdAttribute, 0, false);
            const auto* valueAttr = audioEnvironmentNode->first_attribute(XmlTags::kValueAttribute, 0, false);

            if (envIdAttr && valueAttr)
            {
                const char* envId = envIdAttr->value();

                if (const Environment env = amEngine->AddEnvironment(AZStd::stoull(AZStd::string(envId))); env.Valid())
                {
                    const char* effectId = valueAttr->value();

                    if (const EffectHandle effect = amEngine->GetEffectHandle(AZStd::stoull(AZStd::string(effectId))); effect != nullptr)
                    {
                        env.SetEffect(effect);

                        newEnvironmentImpl =
                            azcreate(ATLEnvironmentImplData_Amplitude, (env.GetId(), effect->GetId()), Audio::AudioImplAllocator);
                    }
                }
            }
        }

        return newEnvironmentImpl;
    }

    void CAudioSystemImpl_Amplitude::DeleteAudioEnvironmentImplData(IATLEnvironmentImplData* const oldEnvironmentImplData)
    {
        azdestroy(oldEnvironmentImplData, Audio::AudioImplAllocator, ATLEnvironmentImplData_Amplitude);
    }

    ATLAudioObjectData_Amplitude* CAudioSystemImpl_Amplitude::NewGlobalAudioObjectData(const TAudioObjectID objectId)
    {
        auto* newObjectData = azcreate(ATLAudioObjectData_Amplitude, (static_cast<AmObjectID>(objectId), false), Audio::AudioImplAllocator);

        return newObjectData;
    }

    ATLAudioObjectData_Amplitude* CAudioSystemImpl_Amplitude::NewAudioObjectData(const TAudioObjectID objectId)
    {
        auto* newObjectData = azcreate(ATLAudioObjectData_Amplitude, (static_cast<AmObjectID>(objectId), true), Audio::AudioImplAllocator);

        return newObjectData;
    }

    void CAudioSystemImpl_Amplitude::DeleteAudioObjectData(IATLAudioObjectData* const oldObjectData)
    {
        azdestroy(oldObjectData, Audio::AudioImplAllocator, ATLAudioObjectData_Amplitude);
    }

    ATLListenerData_Amplitude* CAudioSystemImpl_Amplitude::NewDefaultAudioListenerObjectData(const TATLIDType listenerId)
    {
        auto* const newObjectData = azcreate(ATLListenerData_Amplitude, (static_cast<AmObjectID>(listenerId)), Audio::AudioImplAllocator);

        if (newObjectData)
        {
            const Listener listener = amEngine->AddListener(newObjectData->mAmID);
            if (listener.Valid())
            {
                amEngine->SetDefaultListener(&listener);
                _defaultListenerGameObjectId = newObjectData->mAmID;
            }
            else
            {
                amLogWarning("Amplitude failed in registering a default Listener.");
            }
        }

        return newObjectData;
    }

    ATLListenerData_Amplitude* CAudioSystemImpl_Amplitude::NewAudioListenerObjectData(const TATLIDType listenerId)
    {
        auto* const newObjectData = azcreate(ATLListenerData_Amplitude, (static_cast<AmListenerID>(listenerId)), Audio::AudioImplAllocator);

        if (newObjectData)
        {
            if (const Listener listener = amEngine->AddListener(newObjectData->mAmID); !listener.Valid())
            {
                amLogWarning("Amplitude failed in registering a Listener.");
            }
        }

        return newObjectData;
    }

    void CAudioSystemImpl_Amplitude::DeleteAudioListenerObjectData(IATLListenerData* const oldListenerData)
    {
        if (const auto* const listenerData = dynamic_cast<ATLListenerData_Amplitude*>(oldListenerData))
        {
            amEngine->RemoveListener(listenerData->mAmID);
            if (listenerData->mAmID == _defaultListenerGameObjectId)
            {
                _defaultListenerGameObjectId = kAmInvalidObjectId;
            }
        }

        azdestroy(oldListenerData, Audio::AudioImplAllocator, ATLListenerData_Amplitude);
    }

    ATLEventData_Amplitude* CAudioSystemImpl_Amplitude::NewAudioEventData(const TAudioEventID eventId)
    {
        return azcreate(ATLEventData_Amplitude, (static_cast<AmEventID>(eventId)), Audio::AudioImplAllocator);
    }

    void CAudioSystemImpl_Amplitude::DeleteAudioEventData(IATLEventData* const oldEventData)
    {
        azdestroy(oldEventData, Audio::AudioImplAllocator, ATLEventData_Amplitude);
    }

    void CAudioSystemImpl_Amplitude::ResetAudioEventData(IATLEventData* const eventData)
    {
        if (auto* const implEventData = dynamic_cast<ATLEventData_Amplitude*>(eventData))
        {
            implEventData->mAudioEventState = eAES_NONE;
            implEventData->mEventCanceler = EventCanceler(nullptr);
            implEventData->mSourceId = INVALID_AUDIO_SOURCE_ID;
        }
    }

    const char* const CAudioSystemImpl_Amplitude::GetImplSubPath() const
    {
        return kAmplitudeImplSubPath;
    }

    void CAudioSystemImpl_Amplitude::SetLanguage([[maybe_unused]] const char* const language)
    {
        // TODO: Localized sound banks not yet supported by SDK
    }

    // The functions below are only used when AM_RELEASE is not defined
    const char* const CAudioSystemImpl_Amplitude::GetImplementationNameString() const
    {
#if !defined(AM_RELEASE)
        return _fullImplString.c_str();
#else
        return amVersion.text.c_str();
#endif // !AM_RELEASE
    }

    void CAudioSystemImpl_Amplitude::GetMemoryInfo(SAudioImplMemoryInfo& memoryInfo) const
    {
        const auto& allocator = AZ::AllocatorInstance<Audio::AudioImplAllocator>::Get();

        memoryInfo.nPrimaryPoolSize = allocator.Capacity();
        memoryInfo.nPrimaryPoolUsedSize = memoryInfo.nPrimaryPoolSize - allocator.GetUnAllocatedMemory();
        memoryInfo.nPrimaryPoolAllocations = 0;
        memoryInfo.nSecondaryPoolSize = 0;
        memoryInfo.nSecondaryPoolUsedSize = 0;
        memoryInfo.nSecondaryPoolAllocations = 0;
    }

    AZStd::vector<AudioImplMemoryPoolInfo> CAudioSystemImpl_Amplitude::GetMemoryPoolInfo()
    {
#if !defined(AM_RELEASE) && !defined(AM_NO_MEMORY_STATS)
        // Update memory category info...
        for (auto& memInfo : _debugMemoryInfo)
        {
            if (memInfo.m_poolId < 0)
                break;

            const MemoryPoolStats& poolStats = amMemory->GetStats(static_cast<eMemoryPoolKind>(memInfo.m_poolId));

            memInfo.m_memoryUsed = static_cast<AZ::u32>(poolStats.maxMemoryUsed.load());
            memInfo.m_peakUsed = 0;
            memInfo.m_numAllocs = static_cast<AZ::u32>(poolStats.allocCount.load());
            memInfo.m_numFrees = static_cast<AZ::u32>(poolStats.freeCount.load());
        }

        // TODO: Global stats

        // return the memory infos...
        return _debugMemoryInfo;
#else
        return {};
#endif // !AM_RELEASE
    }

    bool CAudioSystemImpl_Amplitude::CreateAudioSource([[maybe_unused]] const SAudioInputConfig& sourceConfig)
    {
        amLogDebug("Create audio source: %s", sourceConfig.m_sourceFilename.c_str());
        return false;
    }

    void CAudioSystemImpl_Amplitude::DestroyAudioSource([[maybe_unused]] TAudioSourceId sourceId)
    {}

    void CAudioSystemImpl_Amplitude::SetPanningMode([[maybe_unused]] PanningMode mode)
    {
        // Panning mode set from engine config at initialization time
    }

    EAudioRequestStatus CAudioSystemImpl_Amplitude::UpdateEnvironmentAmounts(const ATLAudioObjectData_Amplitude* audioObjectData)
    {
        if (audioObjectData == nullptr)
            return EAudioRequestStatus::Failure;

        const auto entity = amEngine->GetEntity(audioObjectData->mAmID);
        if (!entity.Valid())
            return EAudioRequestStatus::Failure;

        for (const auto& envAmount : audioObjectData->cEnvironmentImplAmounts)
            entity.SetEnvironmentFactor(envAmount.first, envAmount.second);

        return EAudioRequestStatus::Success;
    }
} // namespace Audio
