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

#include <ATLEntityData.h>
#include <AudioAllocators.h>
#include <IAudioInterfacesCommonData.h>

#include <AzCore/std/containers/map.h>
#include <AzCore/std/containers/vector.h>

#include <SparkyStudios/Audio/Amplitude/Amplitude.h>

using namespace SparkyStudios::Audio::Amplitude;

namespace Audio
{
    using TAmUniqueIDVector = AZStd::vector<AmObjectID, AudioImplStdAllocator>;

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    struct SATLAudioObjectData_Amplitude : public IATLAudioObjectData
    {
        // convert to ATLMapLookupType
        using TEnvironmentImplMap = AZStd::map<AmBusID, float, AZStd::less<AmBusID>, AudioImplStdAllocator>;

        SATLAudioObjectData_Amplitude(const AmEntityID nPassedAmID, const bool bPassedHasPosition)
            : bNeedsToUpdateEnvironments(false)
            , bHasPosition(bPassedHasPosition)
            , nAmID(nPassedAmID)
        {
        }

        ~SATLAudioObjectData_Amplitude() override
        {
        }

        bool bNeedsToUpdateEnvironments;
        const bool bHasPosition;
        const AmEntityID nAmID;
        TEnvironmentImplMap cEnvironmentImplAmounts;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    struct SATLListenerData_Amplitude : public IATLListenerData
    {
        explicit SATLListenerData_Amplitude(const AmListenerID passedObjectId)
            : nAmListenerObjectId(passedObjectId)
        {
        }

        ~SATLListenerData_Amplitude() override
        {
        }

        const AmListenerID nAmListenerObjectId = kAmInvalidObjectId;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    struct SATLTriggerImplData_Amplitude : public IATLTriggerImplData
    {
        explicit SATLTriggerImplData_Amplitude(const AmEventID nPassedAmID)
            : nAmID(nPassedAmID)
        {
        }

        ~SATLTriggerImplData_Amplitude() override
        {
        }

        const AmEventID nAmID;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    struct SATLRtpcImplData_Amplitude : public IATLRtpcImplData
    {
        SATLRtpcImplData_Amplitude(const AmRtpcID nPassedAmID)
            : nAmID(nPassedAmID)
        {
        }

        ~SATLRtpcImplData_Amplitude() override
        {
        }

        const AmRtpcID nAmID;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    struct SATLSwitchStateImplData_Amplitude : public IATLSwitchStateImplData
    {
        SATLSwitchStateImplData_Amplitude(const AmSwitchID nPassedAmSwitchID, const AmObjectID nPassedAmStateID)
            : nAmSwitchID(nPassedAmSwitchID)
            , nAmStateID(nPassedAmStateID)
        {
        }

        ~SATLSwitchStateImplData_Amplitude() override
        {
        }

        const AmSwitchID nAmSwitchID;
        const AmObjectID nAmStateID;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    enum EAmplitudeAudioEnvironmentType : TATLEnumFlagsType
    {
        eAAET_NONE = 0,
        eAAET_BUS = 1,
        eAAET_SWITCH = 2,
        eAAET_EFFECT = 3,
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    struct SATLEnvironmentImplData_Amplitude : public IATLEnvironmentImplData
    {
        explicit SATLEnvironmentImplData_Amplitude(const EAmplitudeAudioEnvironmentType ePassedType)
            : eType(ePassedType)
        {
        }

        SATLEnvironmentImplData_Amplitude(const EAmplitudeAudioEnvironmentType ePassedType, const AmObjectID nPassedAmID)
            : eType(ePassedType)
            , nAmEnvID(nPassedAmID)
        {
        }

        SATLEnvironmentImplData_Amplitude(
            const EAmplitudeAudioEnvironmentType ePassedType, const AmObjectID nPassedAmID, const AmObjectID nPassedAmStateID)
            : eType(ePassedType)
            , nAmEnvID(nPassedAmID)
            , nAmStateID(nPassedAmStateID)
        {
        }

        ~SATLEnvironmentImplData_Amplitude() override
        {
        }

        const EAmplitudeAudioEnvironmentType eType;
        AmObjectID nAmEnvID;
        union
        {
            AmObjectID nAmStateID;
            AmEffectID nAmEffectID;
        };
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    struct SATLEventData_Amplitude : public IATLEventData
    {
        explicit SATLEventData_Amplitude(const TAudioEventID nPassedID)
            : audioEventState(eAES_NONE)
            , eventCanceler(nullptr)
            , nATLID(nPassedID)
            , nSourceId(INVALID_AUDIO_SOURCE_ID)
        {
        }

        ~SATLEventData_Amplitude() override
        {
        }

        EAudioEventState audioEventState;
        EventCanceler eventCanceler;
        const TAudioEventID nATLID;
        TAudioSourceId nSourceId;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    struct SATLAudioFileEntryData_Amplitude : public IATLAudioFileEntryData
    {
        SATLAudioFileEntryData_Amplitude()
            : nAmBankID(kAmInvalidObjectId)
        {
        }

        SATLAudioFileEntryData_Amplitude(AmBankID bankId)
            : nAmBankID(bankId)
        {
        }

        ~SATLAudioFileEntryData_Amplitude() override
        {
        }

        AmBankID nAmBankID;
    };
} // namespace Audio
