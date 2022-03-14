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

#include <Editor/AmplitudeAudioLoader.h>

namespace SparkyStudios::Audio::Amplitude
{
    //-------------------------------------------------------------------------------------------//
    class CRtpcConnection : public IAudioConnection
    {
    public:
        explicit CRtpcConnection(CID id)
            : IAudioConnection(id)
            , m_mult(1.0f)
            , m_shift(0.0f)
        {
        }

        ~CRtpcConnection() override = default;

        bool HasProperties() override
        {
            return true;
        }

        float m_mult;
        float m_shift;
    };

    using TRtpcConnectionPtr = AZStd::shared_ptr<CRtpcConnection>;

    //-------------------------------------------------------------------------------------------//
    class CSwitchStateConnection : public IAudioConnection
    {
    public:
        explicit CSwitchStateConnection(CID id)
            : IAudioConnection(id)
            , m_state(0)
        {
        }

        ~CSwitchStateConnection() override = default;

        bool HasProperties() override
        {
            return true;
        }

        AmObjectID m_state;
    };

    using TSwitchStateConnectionPtr = AZStd::shared_ptr<CSwitchStateConnection>;

    //-------------------------------------------------------------------------------------------//
    class CEffectConnection : public IAudioConnection
    {
    public:
        explicit CEffectConnection(CID id)
            : IAudioConnection(id)
            , m_effect(0)
        {
        }

        ~CEffectConnection() override = default;

        bool HasProperties() override
        {
            return true;
        }

        AmEffectID m_effect;
    };

    using TEffectConnectionPtr = AZStd::shared_ptr<CEffectConnection>;

    //-------------------------------------------------------------------------------------------//
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
            return &m_rootControl;
        }
        IAudioSystemControl* GetControl(CID id) const override;
        EACEControlType ImplTypeToATLType(TImplControlType type) const override;
        TImplControlTypeMask GetCompatibleTypes(EACEControlType atlControlType) const override;
        TConnectionPtr CreateConnectionToControl(EACEControlType atlControlType, IAudioSystemControl* middlewareControl) override;
        TConnectionPtr CreateConnectionFromXMLNode(AZ::rapidxml::xml_node<char>* node, EACEControlType atlControlType) override;
        AZ::rapidxml::xml_node<char>* CreateXMLNodeFromConnection(
            const TConnectionPtr connection, const EACEControlType atlControlType) override;
        const AZStd::string_view GetTypeIcon(TImplControlType type) const override;
        const AZStd::string_view GetTypeIconSelected(TImplControlType type) const override;
        AZStd::string GetName() const override;
        AZ::IO::FixedMaxPath GetDataPath() const override;
        void DataSaved() override
        {
        }
        void ConnectionRemoved(IAudioSystemControl* control) override;
        //////////////////////////////////////////////////////////

    private:
        IAudioSystemControl* GetControlByName(AZStd::string name, bool isLocalized = false, IAudioSystemControl* parent = nullptr) const;

        // Gets the ID of the control given its name. As controls can have the same name
        // if they're under different parents, the name of the parent is also needed (if there is one)
        CID GetID(const AZStd::string_view name) const;

        void UpdateConnectedStatus();

        IAudioSystemControl m_rootControl;

        using TControlPtr = AZStd::shared_ptr<IAudioSystemControl>;
        using TControlMap = AZStd::unordered_map<CID, TControlPtr>;
        TControlMap m_controls;

        using TConnectionsMap = AZStd::unordered_map<CID, int>;
        TConnectionsMap m_connectionsByID;
        AmplitudeAudioControlLoader m_loader;
    };
} // namespace SparkyStudios::Audio::Amplitude
