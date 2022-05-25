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

#include <platform.h>

#include <AzCore/Console/ILogger.h>
#include <AzCore/Debug/Profiler.h>
#include <AzCore/IO/FileIO.h>
#include <AzCore/PlatformIncl.h>
#include <AzCore/StringFunc/StringFunc.h>
#include <AzCore/Utils/Utils.h>
#include <AzCore/std/string/conversions.h>

#include <AudioAllocators.h>
#include <IAudioSystem.h>

#include <Config.h>
#include <Engine/AmplitudeAudioSystem.h>
#include <Engine/Common.h>

using namespace SparkyStudios::Audio::Amplitude;

#define AM_MEM_POOLS_COUNT static_cast<size_t>(MemoryPoolKind::COUNT)

namespace Audio
{
    namespace Amplitude
    {
        namespace Memory
        {
            AmVoidPtr Malloc(MemoryPoolKind pool, AmSize size)
            {
                return AZ::AllocatorInstance<AudioImplAllocator>::Get().Allocate(
                    size, 0, 0, gMemoryManagerPools[static_cast<AmUInt8>(pool)]);
            }

            AmVoidPtr Malign(MemoryPoolKind pool, AmSize size, AmUInt32 alignment)
            {
                return AZ::AllocatorInstance<AudioImplAllocator>::Get().Allocate(
                    size, alignment, 0, gMemoryManagerPools[static_cast<AmUInt8>(pool)]);
            }

            AmVoidPtr Realloc([[maybe_unused]] MemoryPoolKind pool, AmVoidPtr address, AmSize size)
            {
                return AZ::AllocatorInstance<AudioImplAllocator>::Get().ReAllocate(address, size, 0);
            }

            AmVoidPtr Realign([[maybe_unused]] MemoryPoolKind pool, AmVoidPtr address, AmSize size, AmUInt32 alignment)
            {
                return AZ::AllocatorInstance<AudioImplAllocator>::Get().ReAllocate(address, size, alignment);
            }

            void Free([[maybe_unused]] MemoryPoolKind pool, AmVoidPtr address)
            {
                AZ::AllocatorInstance<AudioImplAllocator>::Get().DeAllocate(address);
            }

            AmSize TotalMemorySize()
            {
                return AZ::AllocatorInstance<AudioImplAllocator>::Get().Capacity();
            }

            AmSize SizeOfMemory([[maybe_unused]] MemoryPoolKind pool, AmVoidPtr address)
            {
                return AZ::AllocatorInstance<AudioImplAllocator>::Get().AllocationSize(address);
            }
        } // namespace Memory

        namespace Log
        {
            static void Write(const char* format, va_list args)
            {
                if (format && format[0] != '\0')
                {
                    constexpr size_t bufferLen = 1024;
                    char buffer[bufferLen] = "[Amplitude] ";

                    azvsnprintf(buffer + 12, bufferLen - 12, format, args);

                    buffer[bufferLen - 1] = '\0';

                    AZLOG_NOTICE("%s", buffer);
                }
            }
        } // namespace Log
    } // namespace Amplitude

    static bool gAudioDeviceInitializationEvent = false;

    static int GetAssetType(const SATLSourceData* sourceData)
    {
        if (!sourceData)
        {
            return eAAT_NONE;
        }

        return sourceData->m_sourceInfo.m_codecType == eACT_STREAM_PCM ? eAAT_STREAM : eAAT_SOURCE;
    }

    AmplitudeAudioSystem::AmplitudeAudioSystem(const char* assetsPlatformName)
        : _globalGameObjectId(GLOBAL_AUDIO_OBJECT_ID)
        , _defaultListenerGameObjectId(kAmInvalidObjectId)
        , _initBankId(kAmInvalidObjectId)
        , _fileLoader()
        , _engine(Engine::GetInstance())
#if !defined(AMPLITUDE_RELEASE)
        , _isCommSystemInitialized(false)
#endif // !AMPLITUDE_RELEASE
    {
        if (assetsPlatformName && assetsPlatformName[0] != '\0')
        {
            m_assetsPlatform = assetsPlatformName;
        }

        SetBankPaths();

#if !defined(AMPLITUDE_RELEASE)
        _fullImplString =
            AZStd::string::format("%s (%s)", SparkyStudios::Audio::Amplitude::Version().text.c_str(), m_soundbankFolder.c_str());

        // Set up memory categories for debug tracking, do this early before initializing Amplitude, so they are available
        // before any allocations through hooks occur.
        AZLOG_DEBUG("Memory Categories:");
        _debugMemoryInfo.reserve(AM_MEM_POOLS_COUNT);

        for (AZ::s32 memId = 0; memId < AM_MEM_POOLS_COUNT; ++memId)
        {
            AudioImplMemoryPoolInfo memInfo;
            azstrcpy(memInfo.m_poolName, sizeof(memInfo.m_poolName), gMemoryManagerPools[memId]);
            memInfo.m_poolId = memId;

            _debugMemoryInfo.push_back(memInfo);

            AZLOG_DEBUG("Memory category ID: %u - '%s'", memId, gMemoryManagerPools[memId]);
        }

        // Add one more category for global stats.
        AudioImplMemoryPoolInfo memInfo;
        azstrcpy(memInfo.m_poolName, sizeof(memInfo.m_poolName), "Global");
        _debugMemoryInfo.push_back(memInfo);
#endif

        AudioSystemImplementationRequestBus::Handler::BusConnect();
        AudioSystemImplementationNotificationBus::Handler::BusConnect();
    }

    AmplitudeAudioSystem::~AmplitudeAudioSystem()
    {
        AudioSystemImplementationRequestBus::Handler::BusDisconnect();
        AudioSystemImplementationNotificationBus::Handler::BusDisconnect();
    }

    void AmplitudeAudioSystem::OnAudioSystemLoseFocus()
    {
#if defined(AMPLITUDE_RELEASE)
        if (_engine->IsInitialized())
        {
            _engine->Pause(true);
        }
#endif // defined(AMPLITUDE_RELEASE)
    }

    void AmplitudeAudioSystem::OnAudioSystemGetFocus()
    {
#if defined(AMPLITUDE_RELEASE)
        if (_engine->IsInitialized())
        {
            _engine->Pause(false);
        }
#endif // defined(AMPLITUDE_RELEASE)
    }

    void AmplitudeAudioSystem::OnAudioSystemMuteAll()
    {
        if (_engine->IsInitialized())
        {
            _engine->SetMute(true);
        }
    }

    void AmplitudeAudioSystem::OnAudioSystemUnmuteAll()
    {
        if (_engine->IsInitialized())
        {
            _engine->SetMute(false);
        }
    }

    void AmplitudeAudioSystem::OnAudioSystemRefresh()
    {
        if (_engine->IsInitialized())
        {
            if (_initBankId != kAmInvalidObjectId)
            {
                _engine->UnloadSoundBank(_initBankId);
            }

            if (!_engine->LoadSoundBank(AM_STRING_TO_OS_STRING(kInitBankFile), _initBankId))
            {
                AZLOG_ERROR("Amplitude failed to load %s", kInitBankFile);
                _initBankId = kAmInvalidObjectId;
                AZ_Assert(false, "[Amplitude] Failed to load %s !", kInitBankFile);
            }
        }
        else
        {
            Initialize();
        }
    }

    void AmplitudeAudioSystem::Update(const float updateIntervalMs)
    {
        AZ_PROFILE_FUNCTION(Audio);

        if (_engine->IsInitialized())
        {
            _engine->AdvanceFrame(static_cast<AmTime>(updateIntervalMs) / kAmSecond);
        }
    }

    EAudioRequestStatus AmplitudeAudioSystem::Initialize()
    {
        RegisterLogFunc(Amplitude::Log::Write);

        const AZ::IO::FixedMaxPath projectPath(AZ::Utils::GetProjectPath());
        _fileLoader.SetBasePath((projectPath / kAssetsRootPath).Native().c_str());

        MemoryManagerConfig amMemConfig;
        amMemConfig.malloc = Amplitude::Memory::Malloc;
        amMemConfig.alignedMalloc = Amplitude::Memory::Malign;
        amMemConfig.realloc = Amplitude::Memory::Realloc;
        amMemConfig.alignedRealloc = Amplitude::Memory::Realign;
        amMemConfig.free = Amplitude::Memory::Free;
        amMemConfig.totalReservedMemorySize = Amplitude::Memory::TotalMemorySize;
        amMemConfig.sizeOf = Amplitude::Memory::SizeOfMemory;

        MemoryManager::Initialize(amMemConfig);

        if (amMemory == nullptr)
        {
            AZLOG_ERROR("Amplitude::MemoryManager::Init() has failed.");
            ShutDown();
            return EAudioRequestStatus::Failure;
        }

        // TODO: Audio device status callback

        _engine->SetFileLoader(_fileLoader);

        if (!_engine->Initialize(AM_OS_STRING("audio_config.amconfig")))
        {
            AZLOG_ERROR("Amplitude Engine has failed to initialize.");
            ShutDown();
            return EAudioRequestStatus::Failure;
        }

        _globalGameObject = _engine->AddEntity(_globalGameObjectId);
        if (!_globalGameObject.Valid())
        {
            AZLOG_WARN("Amplitude::Engine::AddEntity() failed.");
        }

        const bool result = _engine->LoadSoundBank(AM_STRING_TO_OS_STRING(kInitBankFile));
        _initBankId = 1;

        if (!result)
        {
            AZLOG_ERROR("Amplitude failed to load %s", kInitBankFile);
            _initBankId = kAmInvalidObjectId;
            AZ_Assert(false, "<Amplitude> Failed to load %s !", kInitBankFile);
        }

        return EAudioRequestStatus::Success;
    }

    EAudioRequestStatus AmplitudeAudioSystem::ShutDown()
    {
        // TODO: Audio device status callback

        if (_engine->IsInitialized())
        {
            // UnRegister the DummyGameObject
            _engine->RemoveEntity(&_globalGameObject);

            if (_globalGameObject.Valid())
            {
                AZLOG_WARN("Amplitude::Engine::RemoveEntity() failed.");
            }

            _engine->UnloadSoundBanks();

            _engine->Deinitialize();
        }

        // Terminate the Memory Manager
        if (MemoryManager::IsInitialized())
        {
            MemoryManager::Deinitialize();
        }

        return EAudioRequestStatus::Success;
    }

    EAudioRequestStatus AmplitudeAudioSystem::Release()
    {
        // Deleting this object and destroying the allocator has been moved to AmplitudeAudioSystemComponent
        return EAudioRequestStatus::Success;
    }

    EAudioRequestStatus AmplitudeAudioSystem::StopAllSounds()
    {
        if (_engine->IsInitialized())
        {
            _engine->StopAll();
            return EAudioRequestStatus::Success;
        }

        return EAudioRequestStatus::Failure;
    }

    EAudioRequestStatus AmplitudeAudioSystem::RegisterAudioObject(
        IATLAudioObjectData* const audioObjectData, [[maybe_unused]] const char* const objectName)
    {
        if (audioObjectData && _engine->IsInitialized())
        {
            const auto* const implObjectData = dynamic_cast<SATLAudioObjectData_Amplitude*>(audioObjectData);

            const Entity entity = _engine->AddEntity(implObjectData->nAmID);

            if (!entity.Valid())
            {
                AZLOG_WARN("Amplitude::Engine::AddEntity() failed.");
            }

            return BoolToARS(entity.Valid());
        }

        AZLOG_WARN("Amplitude::Engine::AddEntity() failed, audio object data was null.");
        return EAudioRequestStatus::Failure;
    }

    EAudioRequestStatus AmplitudeAudioSystem::UnregisterAudioObject(IATLAudioObjectData* const audioObjectData)
    {
        if (audioObjectData && _engine->IsInitialized())
        {
            const auto* const implObjectData = dynamic_cast<SATLAudioObjectData_Amplitude*>(audioObjectData);

            _engine->RemoveEntity(implObjectData->nAmID);
            const Entity entity = _engine->GetEntity(implObjectData->nAmID);

            if (entity.Valid())
            {
                AZLOG_WARN("Amplitude::Engine::RemoveEntity() failed.");
            }

            return BoolToARS(!entity.Valid());
        }

        AZLOG_WARN("Amplitude::Engine::AddEntity() failed, audio object data was null.");
        return EAudioRequestStatus::Failure;
    }

    EAudioRequestStatus AmplitudeAudioSystem::ResetAudioObject(IATLAudioObjectData* const audioObjectData)
    {
        if (audioObjectData)
        {
            auto* const implObjectData = dynamic_cast<SATLAudioObjectData_Amplitude*>(audioObjectData);

            implObjectData->cEnvironmentImplAmounts.clear();
            implObjectData->bNeedsToUpdateEnvironments = false;

            return EAudioRequestStatus::Success;
        }

        AZLOG_WARN("Amplitude::Engine::AddEntity() failed, audio object data was null.");
        return EAudioRequestStatus::Failure;
    }

    EAudioRequestStatus AmplitudeAudioSystem::UpdateAudioObject(IATLAudioObjectData* const audioObjectData)
    {
        AZ_PROFILE_FUNCTION(Audio);

        auto result = EAudioRequestStatus::Failure;

        if (audioObjectData)
        {
            const auto* const implObjectData = dynamic_cast<SATLAudioObjectData_Amplitude*>(audioObjectData);

            if (implObjectData->bNeedsToUpdateEnvironments)
            {
                // result = PostEnvironmentAmounts(implObjectData);
            }
        }

        return result;
    }

    EAudioRequestStatus AmplitudeAudioSystem::PrepareTriggerSync(
        [[maybe_unused]] IATLAudioObjectData* const audioObjectData, [[maybe_unused]] const IATLTriggerImplData* const triggerData)
    {
        CallLogFunc("Prep trigger sync");

        // return PrepUnprepTriggerSync(triggerData, true);
        return EAudioRequestStatus::Success;
    }

    EAudioRequestStatus AmplitudeAudioSystem::UnprepareTriggerSync(
        [[maybe_unused]] IATLAudioObjectData* const audioObjectData, [[maybe_unused]] const IATLTriggerImplData* const triggerData)
    {
        CallLogFunc("Unprep trigger sync");

        // return PrepUnprepTriggerSync(triggerData, false);
        return EAudioRequestStatus::Success;
    }

    EAudioRequestStatus AmplitudeAudioSystem::PrepareTriggerAsync(
        [[maybe_unused]] IATLAudioObjectData* const audioObjectData,
        [[maybe_unused]] const IATLTriggerImplData* const triggerData,
        [[maybe_unused]] IATLEventData* const eventData)
    {
        CallLogFunc("Prep trigger async");

        // return PrepUnprepTriggerAsync(triggerData, eventData, true);
        return EAudioRequestStatus::Success;
    }

    EAudioRequestStatus AmplitudeAudioSystem::UnprepareTriggerAsync(
        [[maybe_unused]] IATLAudioObjectData* const audioObjectData,
        [[maybe_unused]] const IATLTriggerImplData* const triggerData,
        [[maybe_unused]] IATLEventData* const eventData)
    {
        CallLogFunc("Unprep trigger async");

        // return PrepUnprepTriggerAsync(triggerData, eventData, false);
        return EAudioRequestStatus::Success;
    }

    EAudioRequestStatus AmplitudeAudioSystem::ActivateTrigger(
        IATLAudioObjectData* const audioObjectData,
        const IATLTriggerImplData* const triggerData,
        IATLEventData* const eventData,
        const SATLSourceData* const sourceData)
    {
        auto result = EAudioRequestStatus::Failure;

        const auto* const implObjectData = dynamic_cast<SATLAudioObjectData_Amplitude*>(audioObjectData);
        const auto* const implTriggerData = dynamic_cast<const SATLTriggerImplData_Amplitude*>(triggerData);

        if (auto* const implEventData = dynamic_cast<SATLEventData_Amplitude*>(eventData);
            implObjectData && implTriggerData && implEventData)
        {
            AmEntityID entityId;

            if (implObjectData->bHasPosition)
            {
                entityId = implObjectData->nAmID;
            }
            else
            {
                entityId = _globalGameObjectId;
            }

            switch (GetAssetType(sourceData))
            {
            case eAAT_SOURCE:
                {
                    AZ_Assert(sourceData, "SourceData not provided for source type!");
                    break;
                }

            case eAAT_STREAM:
                [[fallthrough]];
            case eAAT_NONE:
                [[fallthrough]];
            default:
                {
                    const Entity entity = _engine->GetEntity(entityId);
#if !defined(AMPLITUDE_RELEASE)
                    if (entityId != kAmInvalidObjectId && !entity.Valid())
                    {
                        CallLogFunc("Unable to find an entity with ID: %ul", entityId);
                    }
#endif

                    if (const EventHandle event = _engine->GetEventHandle(implTriggerData->nAmID))
                    {
                        if (const EventCanceler canceler = _engine->Trigger(event, entity); canceler.Valid())
                        {
                            implEventData->audioEventState = eAES_PLAYING;
                            implEventData->eventCanceler = canceler;
                            result = EAudioRequestStatus::Success;
                        }
                    }
                    else
                    {
                        AZLOG_WARN(
                            "[Amplitude] Unable to activate a trigger, the associated Amplitude event with ID %l has not been found in "
                            "loaded banks.",
                            implTriggerData->nAmID);
                    }
                    break;
                }
            }
        }
        else
        {
            AZLOG_ERROR("Invalid AudioObjectData, ATLTriggerData or EventData passed to the Amplitude implementation of ActivateTrigger.");
        }

        return result;
    }

    EAudioRequestStatus AmplitudeAudioSystem::StopEvent(
        [[maybe_unused]] IATLAudioObjectData* const audioObjectData, const IATLEventData* const eventData)
    {
        auto result = EAudioRequestStatus::Failure;

        if (auto* const implEventData = dynamic_cast<const SATLEventData_Amplitude*>(eventData))
        {
            switch (implEventData->audioEventState)
            {
            case eAES_PLAYING:
                {
                    if (implEventData->eventCanceler.Valid())
                    {
                        implEventData->eventCanceler.Cancel();
                        result = EAudioRequestStatus::Success;
                    }
                    else
                    {
                        AZLOG_ERROR("[Amplitude] Encountered a running event without a valid canceler.");
                    }
                    break;
                }
            default:
                {
                    AZLOG_ERROR("[Amplitude] Stopping an event of this type is not supported yet");
                    break;
                }
            }
        }
        else
        {
            AZLOG_ERROR("[Amplitude] Invalid EventData passed to StopEvent.");
        }

        return result;
    }

    EAudioRequestStatus AmplitudeAudioSystem::StopAllEvents([[maybe_unused]] IATLAudioObjectData* const audioObjectData)
    {
        // TODO: Cancel all events
        return EAudioRequestStatus::Success;
    }

    EAudioRequestStatus AmplitudeAudioSystem::SetPosition(
        IATLAudioObjectData* const audioObjectData, const SATLWorldPosition& worldPosition)
    {
        auto result = EAudioRequestStatus::Failure;

        if (const auto* implObjectData = dynamic_cast<SATLAudioObjectData_Amplitude*>(audioObjectData))
        {
            if (Entity entity = _engine->GetEntity(implObjectData->nAmID); entity.Valid())
            {
                entity.SetLocation(ATLVec3ToAmVec3(worldPosition.GetPositionVec()));
                entity.SetOrientation(
                    ATLVec3ToAmVec3(worldPosition.GetForwardVec().GetNormalized()),
                    ATLVec3ToAmVec3(worldPosition.GetUpVec().GetNormalized()));
            }
            else
            {
                AZLOG_ERROR("[Amplitude] Invalid AudioObjectData passed to SetPosition.");
            }

            result = EAudioRequestStatus::Success;
        }
        else
        {
            AZLOG_ERROR("[Amplitude] Invalid AudioObjectData passed to SetPosition.");
        }

        return result;
    }

    EAudioRequestStatus AmplitudeAudioSystem::SetMultiplePositions(
        [[maybe_unused]] IATLAudioObjectData* const audioObjectData, [[maybe_unused]] const MultiPositionParams& multiPositionParams)
    {
        // TODO: Not yet supported
        return EAudioRequestStatus::Success;
    }

    EAudioRequestStatus AmplitudeAudioSystem::SetEnvironment(
        IATLAudioObjectData* const audioObjectData, const IATLEnvironmentImplData* const environmentData, const float amount)
    {
        auto result = EAudioRequestStatus::Failure;

        const auto* implObjectData = dynamic_cast<SATLAudioObjectData_Amplitude*>(audioObjectData);

        if (const auto* implEnvironmentData = dynamic_cast<const SATLEnvironmentImplData_Amplitude*>(environmentData);
            implObjectData && implEnvironmentData)
        {
            switch (implEnvironmentData->eType)
            {
            case eAAET_BUS:
                {
                    if (const Bus bus = _engine->FindBus(implEnvironmentData->nAmEnvID); bus.Valid())
                    {
                        // TODO: Set gain? Per object bus gain?
                    }

                    result = EAudioRequestStatus::Success;
                    break;
                }
            case eAAET_SWITCH:
                {
                    if (amount > 0)
                    {
                        _engine->SetSwitchState(implEnvironmentData->nAmEnvID, implEnvironmentData->nAmStateID);
                        // TODO: Per object switch state?
                    }

                    result = EAudioRequestStatus::Success;
                    break;
                }
            case eAAET_EFFECT:
                {
                    const Environment env = _engine->GetEnvironment(implEnvironmentData->nAmEnvID);
                    if (!env.Valid())
                        break;

                    Entity entity = _engine->GetEntity(implObjectData->nAmID);
                    if (!entity.Valid())
                        break;

                    entity.SetEnvironmentFactor(env.GetId(), amount);

                    result = EAudioRequestStatus::Success;
                    break;
                }
            default:
                {
                    AZ_Assert(false, "[Amplitude] Unknown AudioEnvironmentImplementation type!");
                }
            }
        }
        else
        {
            AZLOG_ERROR("[Amplitude] Invalid AudioObjectData or EnvironmentData passed to SetEnvironment");
        }

        return result;
    }

    EAudioRequestStatus AmplitudeAudioSystem::SetRtpc(
        [[maybe_unused]] IATLAudioObjectData* const audioObjectData, const IATLRtpcImplData* const rtpcData, const float value)
    {
        auto result = EAudioRequestStatus::Failure;

        if (const auto* const implRtpcData = dynamic_cast<const SATLRtpcImplData_Amplitude*>(rtpcData))
        {
            _engine->SetRtpcValue(implRtpcData->nAmID, value);
            result = EAudioRequestStatus::Success;
        }
        else
        {
            AZLOG_ERROR("[Amplitude] Invalid AudioObjectData or RtpcData passed to SetRtpc");
        }

        return result;
    }

    EAudioRequestStatus AmplitudeAudioSystem::SetSwitchState(
        [[maybe_unused]] IATLAudioObjectData* const audioObjectData, const IATLSwitchStateImplData* const switchStateData)
    {
        auto result = EAudioRequestStatus::Failure;

        if (const auto* const implSwitchData = dynamic_cast<const SATLSwitchStateImplData_Amplitude*>(switchStateData))
        {
            _engine->SetSwitchState(implSwitchData->nAmSwitchID, implSwitchData->nAmStateID);
            result = EAudioRequestStatus::Success;
        }
        else
        {
            AZLOG_ERROR("[Amplitude] Invalid AudioObjectData or RtpcData passed to SetRtpc");
        }

        return result;
    }

    EAudioRequestStatus AmplitudeAudioSystem::SetObstructionOcclusion(
        IATLAudioObjectData* const audioObjectData, const float obstruction, const float occlusion)
    {
        if (audioObjectData)
        {
            const auto* const implObjectData = dynamic_cast<SATLAudioObjectData_Amplitude*>(audioObjectData);

            Entity entity = _engine->GetEntity(implObjectData->nAmID);

            if (!entity.Valid())
            {
                AZLOG_WARN("[Amplitude] Amplitude::Engine::GetEntity() failed with entity ID %lu", implObjectData->nAmID);
            }

            entity.SetObstruction(obstruction);
            entity.SetOcclusion(occlusion);

            return EAudioRequestStatus::Success;
        }

        AZLOG_WARN("[Amplitude] Amplitude::Engine::GetEntity() failed, audio object data was null.");
        return EAudioRequestStatus::Failure;
    }

    EAudioRequestStatus AmplitudeAudioSystem::SetListenerPosition(
        IATLListenerData* const listenerData, const SATLWorldPosition& newPosition)
    {
        auto result = EAudioRequestStatus::Failure;

        if (const auto* const implObjectData = dynamic_cast<SATLListenerData_Amplitude*>(listenerData))
        {
            if (Listener listener = _engine->GetListener(implObjectData->nAmListenerObjectId); listener.Valid())
            {
                listener.SetLocation(ATLVec3ToAmVec3(newPosition.GetPositionVec()));
                listener.SetOrientation(
                    ATLVec3ToAmVec3(newPosition.GetForwardVec().GetNormalized()), ATLVec3ToAmVec3(newPosition.GetUpVec().GetNormalized()));

                result = EAudioRequestStatus::Success;
            }
            else
            {
                AZLOG_ERROR("[Amplitude] Invalid ListenerData passed to SetPosition.");
            }
        }
        else
        {
            AZLOG_ERROR("[Amplitude] Invalid IATLListenerData passed to SetPosition.");
        }

        return result;
    }

    EAudioRequestStatus AmplitudeAudioSystem::ResetRtpc(
        [[maybe_unused]] IATLAudioObjectData* const audioObjectData, const IATLRtpcImplData* const rtpcData)
    {
        auto result = EAudioRequestStatus::Failure;

        if (const auto* const implRtpcData = dynamic_cast<const SATLRtpcImplData_Amplitude*>(rtpcData))
        {
            if (RtpcHandle rtpc = _engine->GetRtpcHandle(implRtpcData->nAmID))
            {
                rtpc->Reset();
                result = EAudioRequestStatus::Success;
            }
            else
            {
                CallLogFunc("Unable to get RTPC handle for ID %u", implRtpcData->nAmID);
            }
        }
        else
        {
            AZLOG_ERROR("[Amplitude] Invalid AudioObjectData or RtpcData passed to SetRtpc");
        }

        return result;
    }

    EAudioRequestStatus AmplitudeAudioSystem::RegisterInMemoryFile(SATLAudioFileEntryInfo* const audioFileEntry)
    {
        auto result = EAudioRequestStatus::Failure;

        if (audioFileEntry)
        {
            if (auto* const implFileEntryData = dynamic_cast<SATLAudioFileEntryData_Amplitude*>(audioFileEntry->pImplData))
            {
                if (AmBankID bankId = kAmInvalidObjectId;
                    _engine->LoadSoundBankFromMemoryView(audioFileEntry->pFileData, aznumeric_cast<AmSize>(audioFileEntry->nSize), bankId))
                {
                    implFileEntryData->nAmBankID = bankId;
                    result = EAudioRequestStatus::Success;
                }
                else
                {
                    implFileEntryData->nAmBankID = kAmInvalidObjectId;
                    AZLOG_ERROR("Amplitude failed to load soundbank '%s'\n", audioFileEntry->sFileName);
                }
            }
            else
            {
                AZLOG_ERROR("Invalid AudioFileEntryData passed to RegisterInMemoryFile");
            }
        }

        return result;
    }

    EAudioRequestStatus AmplitudeAudioSystem::UnregisterInMemoryFile(SATLAudioFileEntryInfo* const audioFileEntry)
    {
        auto result = EAudioRequestStatus::Failure;

        if (audioFileEntry)
        {
            if (const auto* const implFileEntryData = dynamic_cast<SATLAudioFileEntryData_Amplitude*>(audioFileEntry->pImplData))
            {
                _engine->UnloadSoundBank(implFileEntryData->nAmBankID);

                // TODO: Always success ?
                result = EAudioRequestStatus::Success;
            }
            else
            {
                AZLOG_ERROR("Invalid AudioFileEntryData passed to UnregisterInMemoryFile");
            }
        }

        return result;
    }

    EAudioRequestStatus AmplitudeAudioSystem::ParseAudioFileEntry(
        const AZ::rapidxml::xml_node<char>* audioFileEntryNode, SATLAudioFileEntryInfo* const fileEntryInfo)
    {
        auto result = EAudioRequestStatus::Failure;

        if (audioFileEntryNode && azstricmp(audioFileEntryNode->name(), XmlTags::kFileTag) == 0 && fileEntryInfo)
        {
            const char* audioFileEntryName = nullptr;
            if (const auto* fileEntryNameAttr = audioFileEntryNode->first_attribute(XmlTags::kNameAttribute, 0, false))
            {
                audioFileEntryName = fileEntryNameAttr->value();
            }

            AmBankID audioFileEntryId = kAmInvalidObjectId;
            if (const auto* fileEntryIdAttr = audioFileEntryNode->first_attribute(XmlTags::kIdAttribute, 0, false))
            {
                audioFileEntryId = AZStd::stoull(AZStd::string(fileEntryIdAttr->value()));
            }

            bool isLocalized = false;
            if (const auto* localizedAttr = audioFileEntryNode->first_attribute(XmlTags::kLocalizedAttribute, 0, false))
            {
                if (azstricmp(localizedAttr->value(), "true") == 0)
                {
                    isLocalized = true;
                }
            }

            if (audioFileEntryName && audioFileEntryName[0] != '\0')
            {
                fileEntryInfo->bLocalized = isLocalized;
                fileEntryInfo->sFileName = audioFileEntryName;
                fileEntryInfo->nMemoryBlockAlignment = AM_SIMD_ALIGNMENT;
                fileEntryInfo->pImplData = azcreate(
                    SATLAudioFileEntryData_Amplitude, (audioFileEntryId), Audio::AudioImplAllocator, "ATLAudioFileEntryData_Amplitude");
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

    void AmplitudeAudioSystem::DeleteAudioFileEntryData(IATLAudioFileEntryData* const oldAudioFileEntryData)
    {
        azdestroy(oldAudioFileEntryData, Audio::AudioImplAllocator, SATLAudioFileEntryData_Amplitude);
    }

    const char* const AmplitudeAudioSystem::GetAudioFileLocation(SATLAudioFileEntryInfo* const fileEntryInfo)
    {
        const char* location = nullptr;

        if (fileEntryInfo)
        {
            location = fileEntryInfo->bLocalized ? m_localizedSoundbankFolder.c_str() : m_soundbankFolder.c_str();
        }

        return location;
    }

    IATLTriggerImplData* AmplitudeAudioSystem::NewAudioTriggerImplData(const AZ::rapidxml::xml_node<char>* audioTriggerNode)
    {
        SATLTriggerImplData_Amplitude* newTriggerImpl = nullptr;

        if (audioTriggerNode && azstricmp(audioTriggerNode->name(), XmlTags::kEventTag) == 0)
        {
            if (const auto* eventNameAttr = audioTriggerNode->first_attribute(XmlTags::kNameAttribute, 0, false))
            {
                const char* eventName = eventNameAttr->value();

                if (const EventHandle amEvent = _engine->GetEventHandle(eventName); amEvent != nullptr)
                {
                    newTriggerImpl = azcreate(
                        SATLTriggerImplData_Amplitude, (amEvent->GetId()), Audio::AudioImplAllocator, "ATLTriggerImplData_Amplitude");
                }
            }
        }

        return newTriggerImpl;
    }

    void AmplitudeAudioSystem::DeleteAudioTriggerImplData(IATLTriggerImplData* const oldTriggerImplData)
    {
        azdestroy(oldTriggerImplData, Audio::AudioImplAllocator, SATLTriggerImplData_Amplitude);
    }

    IATLRtpcImplData* AmplitudeAudioSystem::NewAudioRtpcImplData(const AZ::rapidxml::xml_node<char>* audioRtpcNode)
    {
        SATLRtpcImplData_Amplitude* newRtpcImpl = nullptr;

        if (audioRtpcNode && azstricmp(audioRtpcNode->name(), XmlTags::kRtpcTag) == 0)
        {
            if (const auto* eventNameAttr = audioRtpcNode->first_attribute(XmlTags::kNameAttribute, 0, false))
            {
                const char* eventName = eventNameAttr->value();

                if (const RtpcHandle amRtpc = _engine->GetRtpcHandle(eventName); amRtpc != nullptr)
                {
                    newRtpcImpl =
                        azcreate(SATLRtpcImplData_Amplitude, (amRtpc->GetId()), Audio::AudioImplAllocator, "ATLRtpcImplData_Amplitude");
                }
            }
        }

        return newRtpcImpl;
    }

    void AmplitudeAudioSystem::DeleteAudioRtpcImplData(IATLRtpcImplData* const oldRtpcImplData)
    {
        azdestroy(oldRtpcImplData, Audio::AudioImplAllocator, SATLRtpcImplData_Amplitude);
    }

    IATLSwitchStateImplData* AmplitudeAudioSystem::NewAudioSwitchStateImplData(const AZ::rapidxml::xml_node<char>* audioSwitchStateNode)
    {
        SATLSwitchStateImplData_Amplitude* newSwitchStateImpl = nullptr;

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
                            SATLSwitchStateImplData_Amplitude,
                            (AZStd::stoull(AZStd::string(switchId)), AZStd::stoull(AZStd::string(stateId))), Audio::AudioImplAllocator,
                            "ATLSwitchStateImplData_Amplitude");
                    }
                }
            }
        }

        return newSwitchStateImpl;
    }

    void AmplitudeAudioSystem::DeleteAudioSwitchStateImplData(IATLSwitchStateImplData* const oldSwitchStateImplData)
    {
        azdestroy(oldSwitchStateImplData, Audio::AudioImplAllocator, SATLSwitchStateImplData_Amplitude);
    }

    IATLEnvironmentImplData* AmplitudeAudioSystem::NewAudioEnvironmentImplData(const AZ::rapidxml::xml_node<char>* audioEnvironmentNode)
    {
        if (audioEnvironmentNode == nullptr)
            return nullptr;

        SATLEnvironmentImplData_Amplitude* newEnvironmentImpl = nullptr;

        if (azstricmp(audioEnvironmentNode->name(), XmlTags::kBusTag) == 0)
        {
            if (const auto* auxBusNameAttr = audioEnvironmentNode->first_attribute(XmlTags::kNameAttribute, 0, false); auxBusNameAttr)
            {
                const char* auxBusName = auxBusNameAttr->value();

                if (const Bus amBus = _engine->FindBus(auxBusName); amBus.Valid())
                {
                    newEnvironmentImpl = azcreate(
                        SATLEnvironmentImplData_Amplitude, (eAAET_BUS, amBus.GetId()), Audio::AudioImplAllocator,
                        "ATLEnvironmentImplData_Amplitude");
                }
            }
        }
        else if (azstricmp(audioEnvironmentNode->name(), XmlTags::kSwitchTag) == 0)
        {
            if (const auto* switchIdAttr = audioEnvironmentNode->first_attribute(XmlTags::kIdAttribute, 0, false); switchIdAttr)
            {
                if (const auto* stateNode = audioEnvironmentNode->first_node(); stateNode)
                {
                    if (const auto* stateIdAttr = stateNode->first_attribute(XmlTags::kIdAttribute, 0, false); stateIdAttr)
                    {
                        const char* switchId = switchIdAttr->value();
                        const char* stateId = stateIdAttr->value();

                        newEnvironmentImpl = azcreate(
                            SATLEnvironmentImplData_Amplitude,
                            (eAAET_SWITCH, AZStd::stoull(AZStd::string(switchId)), AZStd::stoull(AZStd::string(stateId))),
                            Audio::AudioImplAllocator, "ATLEnvironmentImplData_Amplitude");
                    }
                }
            }
        }
        else if (azstricmp(audioEnvironmentNode->name(), XmlTags::kEnvironmentTag) == 0)
        {
            const auto* envIdAttr = audioEnvironmentNode->first_attribute(XmlTags::kIdAttribute, 0, false);
            const auto* valueAttr = audioEnvironmentNode->first_attribute(XmlTags::kValueAttribute, 0, false);

            if (envIdAttr && valueAttr)
            {
                const char* envId = envIdAttr->value();

                if (Environment env = _engine->AddEnvironment(AZStd::stoull(AZStd::string(envId))); env.Valid())
                {
                    const char* effectId = valueAttr->value();

                    if (const EffectHandle effect = _engine->GetEffectHandle(AZStd::stoull(AZStd::string(effectId))); effect != nullptr)
                    {
                        env.SetEffect(effect);

                        newEnvironmentImpl = azcreate(
                            SATLEnvironmentImplData_Amplitude, (eAAET_EFFECT, env.GetId(), effect->GetId()), Audio::AudioImplAllocator,
                            "ATLEnvironmentImplData_Amplitude");
                    }
                }
            }
        }

        return newEnvironmentImpl;
    }

    void AmplitudeAudioSystem::DeleteAudioEnvironmentImplData(IATLEnvironmentImplData* const oldEnvironmentImplData)
    {
        azdestroy(oldEnvironmentImplData, Audio::AudioImplAllocator, SATLEnvironmentImplData_Amplitude);
    }

    SATLAudioObjectData_Amplitude* AmplitudeAudioSystem::NewGlobalAudioObjectData(const TAudioObjectID objectId)
    {
        auto* newObjectData = azcreate(
            SATLAudioObjectData_Amplitude, (static_cast<AmObjectID>(objectId), false), Audio::AudioImplAllocator,
            "ATLAudioObjectData_Amplitude-Global");

        return newObjectData;
    }

    SATLAudioObjectData_Amplitude* AmplitudeAudioSystem::NewAudioObjectData(const TAudioObjectID objectId)
    {
        auto* newObjectData = azcreate(
            SATLAudioObjectData_Amplitude, (static_cast<AmObjectID>(objectId), true), Audio::AudioImplAllocator,
            "ATLAudioObjectData_Amplitude");

        return newObjectData;
    }

    void AmplitudeAudioSystem::DeleteAudioObjectData(IATLAudioObjectData* const oldObjectData)
    {
        azdestroy(oldObjectData, Audio::AudioImplAllocator, SATLAudioObjectData_Amplitude);
    }

    SATLListenerData_Amplitude* AmplitudeAudioSystem::NewDefaultAudioListenerObjectData(const TATLIDType listenerId)
    {
        auto* const newObjectData = azcreate(
            SATLListenerData_Amplitude, (static_cast<AmObjectID>(listenerId)), Audio::AudioImplAllocator,
            "ATLListenerData_Amplitude-Default");

        if (newObjectData)
        {
            const Listener listener = _engine->AddListener(newObjectData->nAmListenerObjectId);
            if (listener.Valid())
            {
                _engine->SetDefaultListener(&listener);
                _defaultListenerGameObjectId = newObjectData->nAmListenerObjectId;
            }
            else
            {
                AZLOG_WARN("Amplitude failed in registering a default Listener.");
            }
        }

        return newObjectData;
    }

    SATLListenerData_Amplitude* AmplitudeAudioSystem::NewAudioListenerObjectData(const TATLIDType listenerId)
    {
        auto* const newObjectData = azcreate(
            SATLListenerData_Amplitude, (static_cast<AmListenerID>(listenerId)), Audio::AudioImplAllocator, "ATLListenerData_Amplitude");

        if (newObjectData)
        {
            if (const Listener listener = _engine->AddListener(newObjectData->nAmListenerObjectId); !listener.Valid())
            {
                AZLOG_WARN("Amplitude failed in registering a Listener.");
            }
        }

        return newObjectData;
    }

    void AmplitudeAudioSystem::DeleteAudioListenerObjectData(IATLListenerData* const oldListenerData)
    {
        if (const auto* const listenerData = dynamic_cast<SATLListenerData_Amplitude*>(oldListenerData))
        {
            _engine->RemoveListener(listenerData->nAmListenerObjectId);
            if (listenerData->nAmListenerObjectId == _defaultListenerGameObjectId)
            {
                _defaultListenerGameObjectId = kAmInvalidObjectId;
            }
        }

        azdestroy(oldListenerData, Audio::AudioImplAllocator, SATLListenerData_Amplitude);
    }

    SATLEventData_Amplitude* AmplitudeAudioSystem::NewAudioEventData(const TAudioEventID eventId)
    {
        return azcreate(SATLEventData_Amplitude, (static_cast<AmEventID>(eventId)), Audio::AudioImplAllocator, "ATLEventData_Amplitude");
    }

    void AmplitudeAudioSystem::DeleteAudioEventData(IATLEventData* const oldEventData)
    {
        azdestroy(oldEventData, Audio::AudioImplAllocator, SATLEventData_Amplitude);
    }

    void AmplitudeAudioSystem::ResetAudioEventData(IATLEventData* const eventData)
    {
        if (auto* const implEventData = dynamic_cast<SATLEventData_Amplitude*>(eventData))
        {
            implEventData->audioEventState = eAES_NONE;
            implEventData->eventCanceler = EventCanceler(nullptr);
            implEventData->nSourceId = INVALID_AUDIO_SOURCE_ID;
        }
    }

    const char* const AmplitudeAudioSystem::GetImplSubPath() const
    {
        return AmplitudeImplSubPath;
    }

    void AmplitudeAudioSystem::SetLanguage([[maybe_unused]] const char* const language)
    {
        // TODO: Localized soundbanks not supported
    }

    // Functions below are only used when AMPLITUDE_RELEASE is not defined
    const char* const AmplitudeAudioSystem::GetImplementationNameString() const
    {
#if !defined(AMPLITUDE_RELEASE)
        return _fullImplString.c_str();
#else
        return nullptr;
#endif // !AMPLITUDE_RELEASE
    }

    void AmplitudeAudioSystem::GetMemoryInfo(SAudioImplMemoryInfo& memoryInfo) const
    {
        const auto& allocator = AZ::AllocatorInstance<Audio::AudioImplAllocator>::Get();

        memoryInfo.nPrimaryPoolSize = allocator.Capacity();
        memoryInfo.nPrimaryPoolUsedSize = memoryInfo.nPrimaryPoolSize - allocator.GetUnAllocatedMemory();
        memoryInfo.nPrimaryPoolAllocations = 0;
        memoryInfo.nSecondaryPoolSize = 0;
        memoryInfo.nSecondaryPoolUsedSize = 0;
        memoryInfo.nSecondaryPoolAllocations = 0;
    }

    AZStd::vector<AudioImplMemoryPoolInfo> AmplitudeAudioSystem::GetMemoryPoolInfo()
    {
#if !defined(AMPLITUDE_RELEASE)
        // Update memory category info...
        for (auto& memInfo : _debugMemoryInfo)
        {
            if (memInfo.m_poolId < 0)
            {
                break;
            }

            const MemoryPoolStats& poolStats = amMemory->GetStats(static_cast<MemoryPoolKind>(memInfo.m_poolId));

            memInfo.m_memoryUsed = static_cast<AZ::u32>(poolStats.maxMemoryUsed.load());
            memInfo.m_peakUsed = 0;
            memInfo.m_numAllocs = static_cast<AZ::u32>(poolStats.allocCount.load());
            memInfo.m_numFrees = static_cast<AZ::u32>(poolStats.freeCount.load());
        }

        // TODO: Global stats

        // return the memory infos...
        return _debugMemoryInfo;
#else
        return AZStd::vector<AudioImplMemoryPoolInfo>();
#endif // !AMPLITUDE_RELEASE
    }

    bool AmplitudeAudioSystem::CreateAudioSource([[maybe_unused]] const SAudioInputConfig& sourceConfig)
    {
        CallLogFunc("Create audio source: %s", sourceConfig.m_sourceFilename.c_str());
        return false;
    }

    void AmplitudeAudioSystem::DestroyAudioSource([[maybe_unused]] TAudioSourceId sourceId)
    {
    }

    void AmplitudeAudioSystem::SetPanningMode([[maybe_unused]] PanningMode mode)
    {
        // TODO: Panning mode not supported
    }

    void AmplitudeAudioSystem::SetBankPaths()
    {
        // Default...
        // "Sounds/amplitude_assets/soundbanks/"
        AZStd::string bankPath = kDefaultBanksPath;
        AZ::StringFunc::AssetDatabasePath::Join(bankPath.c_str(), "", bankPath);

        // "Sounds/amplitude_project/audio_config.json"
        const AZStd::string configFile = AZStd::string(kProjectRootPath) + kEngineConfigFile;

        if (AZ::IO::FileIOBase::GetInstance() && AZ::IO::FileIOBase::GetInstance()->Exists(configFile.c_str()))
        {
            // TODO: Support multi-platform soundbanks
            // Audio::Wwise::ConfigurationSettings configSettings;
            // if (configSettings.Load(configFile))
            // {
            //     for (const auto& platformMap : configSettings.m_platformMappings)
            //     {
            //         // May need to do a series of checks compare the data in the config settings to what's actually in the file system.
            //         // This is the most straightforward platform check.
            //         if (azstricmp(platformMap.m_enginePlatform.c_str(), AZ_TRAIT_OS_PLATFORM_NAME) == 0)
            //         {
            //             AZStd::string platformPath;
            //             // "sounds/wwise/windows"
            //             AZ::StringFunc::AssetDatabasePath::Join(bankPath.c_str(), platformMap.m_bankSubPath.c_str(), platformPath);

            //             AZStd::string initBankPath;
            //             // "sounds/wwise/windows/init.bnk"
            //             AZ::StringFunc::AssetDatabasePath::Join(platformPath.c_str(), Audio::Wwise::InitBank, initBankPath);
            //             if (AZ::IO::FileIOBase::GetInstance()->Exists(initBankPath.c_str()))
            //             {
            //                 if (!platformPath.ends_with(AZ_CORRECT_DATABASE_SEPARATOR))
            //                 {
            //                     platformPath.push_back(AZ_CORRECT_DATABASE_SEPARATOR);
            //                 }
            //                 bankPath = AZStd::move(platformPath);
            //                 break;
            //             }
            //         }
            //     }
            // }
        }

        if (!bankPath.ends_with(AZ_CORRECT_DATABASE_SEPARATOR))
        {
            bankPath.push_back(AZ_CORRECT_DATABASE_SEPARATOR);
        }

        m_soundbankFolder = bankPath;
        m_localizedSoundbankFolder = bankPath;
    }
} // namespace Audio
