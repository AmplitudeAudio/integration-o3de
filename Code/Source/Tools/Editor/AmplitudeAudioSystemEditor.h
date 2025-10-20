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

#include <IAudioConnection.h>
#include <IAudioSystem.h>
#include <IAudioSystemControl.h>
#include <IAudioSystemEditor.h>

#include <SparkyStudios/Audio/Amplitude/Core/Common.h>

#include <Tools/Editor/AmplitudeAudioLoader.h>

namespace SparkyStudios::Audio::Amplitude
{
    class RtpcConnection : public IAudioConnection
    {
    public:
        explicit RtpcConnection(CID id)
            : IAudioConnection(id)
        {}

        ~RtpcConnection() override = default;

        bool HasProperties() override
        {
            return true;
        }

        AmReal32 mMultiplier = 1.0f;
        AmReal32 mShift = 0.0f;
    };

    using TRtpcConnectionPtr = AZStd::shared_ptr<RtpcConnection>;

    class SwitchStateConnection : public IAudioConnection
    {
    public:
        explicit SwitchStateConnection(CID id)
            : IAudioConnection(id)
            , m_state(0)
        {}

        ~SwitchStateConnection() override = default;

        bool HasProperties() override
        {
            return true;
        }

        AmObjectID m_state;
    };

    using TSwitchStateConnectionPtr = AZStd::shared_ptr<SwitchStateConnection>;

    class EffectConnection : public IAudioConnection
    {
    public:
        explicit EffectConnection(CID id)
            : IAudioConnection(id)
            , mEffect(0)
        {}

        ~EffectConnection() override = default;

        bool HasProperties() override
        {
            return true;
        }

        AmEffectID mEffect;
    };

    using TEffectConnectionPtr = AZStd::shared_ptr<EffectConnection>;

    class AmplitudeAudioSystemEditor : public IAudioSystemEditor
    {
        friend class AmplitudeAudioControlLoader;

    public:
        AmplitudeAudioSystemEditor();
        ~AmplitudeAudioSystemEditor() override = default;

        //////////////////////////////////////////////////////////
        // IAudioSystemEditor implementation
        /////////////////////////////////////////////////////////
        void Reload() override;
        IAudioSystemControl* CreateControl(const SControlDef& controlDefinition) override;

        IAudioSystemControl* GetRoot() override
        {
            return &_rootControl;
        }

        [[nodiscard]] IAudioSystemControl* GetControl(CID id) const override;
        [[nodiscard]] EACEControlType ImplTypeToATLType(TImplControlType type) const override;
        [[nodiscard]] TImplControlTypeMask GetCompatibleTypes(EACEControlType atlControlType) const override;
        TConnectionPtr CreateConnectionToControl(EACEControlType atlControlType, IAudioSystemControl* middlewareControl) override;
        TConnectionPtr CreateConnectionFromXMLNode(AZ::rapidxml::xml_node<char>* node, EACEControlType atlControlType) override;
        AZ::rapidxml::xml_node<char>* CreateXMLNodeFromConnection(TConnectionPtr connection, EACEControlType atlControlType) override;
        [[nodiscard]] const AZStd::string_view GetTypeIcon(TImplControlType type) const override;
        [[nodiscard]] const AZStd::string_view GetTypeIconSelected(TImplControlType type) const override;
        [[nodiscard]] AZStd::string GetName() const override;
        [[nodiscard]] AZ::IO::FixedMaxPath GetDataPath() const override;

        void DataSaved() override
        {}

        void ConnectionRemoved(IAudioSystemControl* control) override;
        //////////////////////////////////////////////////////////

    private:
        IAudioSystemControl* GetControlByName(AZStd::string name, bool isLocalized = false, IAudioSystemControl* parent = nullptr) const;

        // Gets the ID of the control given its name. As controls can have the same name
        // if they're under different parents, the name of the parent is also needed (if there is one)
        [[nodiscard]] CID GetID(AZStd::string_view name) const;

        void UpdateConnectedStatus();

        IAudioSystemControl _rootControl;

        using TControlPtr = AZStd::shared_ptr<IAudioSystemControl>;
        using TControlMap = AZStd::unordered_map<CID, TControlPtr>;
        TControlMap _controls;

        using TConnectionsMap = AZStd::unordered_map<CID, int>;
        TConnectionsMap _connectionsByID;
        AmplitudeAudioControlLoader _loader;
    };
} // namespace SparkyStudios::Audio::Amplitude
