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

#include <ATLEntityData.h>
#include <AudioAllocators.h>
#include <IAudioInterfacesCommonData.h>

#include <AzCore/std/containers/map.h>
#include <AzCore/std/containers/vector.h>

#include <SparkyStudios/Audio/Amplitude/Amplitude.h>

namespace Audio
{
    using namespace SparkyStudios::Audio::Amplitude;

    using TAmUniqueIDVector = AZStd::vector<AmObjectID, AudioImplStdAllocator>;

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    struct ATLAudioObjectData_Amplitude : IATLAudioObjectData
    {
        // convert to ATLMapLookupType
        using TEnvironmentImplMap = AZStd::map<AmEnvironmentID, float, AZStd::less<AmEnvironmentID>, AudioImplStdAllocator>;

        ATLAudioObjectData_Amplitude(const AmEntityID entityId, const bool hasPosition)
            : mNeedsToUpdateEnvironments(false)
            , mHasPosition(hasPosition)
            , mPositionEntities()
            , mAmID(entityId)
        {}

        ~ATLAudioObjectData_Amplitude() override = default;

        bool mNeedsToUpdateEnvironments;
        bool mHasPosition;
        AZStd::vector<AmEntityID> mPositionEntities;
        AmEntityID mAmID;
        TEnvironmentImplMap cEnvironmentImplAmounts;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    struct ATLListenerData_Amplitude : IATLListenerData
    {
        explicit ATLListenerData_Amplitude(const AmListenerID listenerId)
            : mAmID(listenerId)
        {}

        ~ATLListenerData_Amplitude() override = default;

        AmListenerID mAmID;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    struct ATLTriggerImplData_Amplitude : IATLTriggerImplData
    {
        explicit ATLTriggerImplData_Amplitude(const AmEventID eventId)
            : mAmID(eventId)
        {}

        ~ATLTriggerImplData_Amplitude() override = default;

        AmEventID mAmID;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    struct ATLRtpcImplData_Amplitude : IATLRtpcImplData
    {
        explicit ATLRtpcImplData_Amplitude(const AmRtpcID rtpcId)
            : mAmID(rtpcId)
        {}

        ~ATLRtpcImplData_Amplitude() override = default;

        AmRtpcID mAmID;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    struct ATLSwitchStateImplData_Amplitude : IATLSwitchStateImplData
    {
        ATLSwitchStateImplData_Amplitude(const AmSwitchID switchId, const AmObjectID stateId)
            : mAmSwitchID(switchId)
            , mAmStateID(stateId)
        {}

        ~ATLSwitchStateImplData_Amplitude() override = default;

        AmSwitchID mAmSwitchID;
        AmObjectID mAmStateID;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    struct ATLEnvironmentImplData_Amplitude : IATLEnvironmentImplData
    {
        explicit ATLEnvironmentImplData_Amplitude()
        {}

        ATLEnvironmentImplData_Amplitude(const AmObjectID environmentId, const AmEffectID effectId)
            : mAmEnvID(environmentId)
            , nAmEffectID(effectId)
        {}

        ~ATLEnvironmentImplData_Amplitude() override = default;

        AmObjectID mAmEnvID = kAmInvalidObjectId;
        AmEffectID nAmEffectID = kAmInvalidObjectId;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    struct ATLEventData_Amplitude : IATLEventData
    {
        explicit ATLEventData_Amplitude(const TAudioEventID eventId)
            : mAudioEventState(eAES_NONE)
            , mEventCanceler(nullptr)
            , mATLID(eventId)
            , mSourceId(INVALID_AUDIO_SOURCE_ID)
        {}

        ~ATLEventData_Amplitude() override = default;

        EAudioEventState mAudioEventState;
        EventCanceler mEventCanceler;
        TAudioEventID mATLID;
        TAudioSourceId mSourceId;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    struct ATLAudioFileEntryData_Amplitude : IATLAudioFileEntryData
    {
        ATLAudioFileEntryData_Amplitude()
            : mAmID(kAmInvalidObjectId)
        {}

        explicit ATLAudioFileEntryData_Amplitude(AmBankID bankId)
            : mAmID(bankId)
        {}

        ~ATLAudioFileEntryData_Amplitude() override = default;

        AmBankID mAmID;
    };
} // namespace Audio
