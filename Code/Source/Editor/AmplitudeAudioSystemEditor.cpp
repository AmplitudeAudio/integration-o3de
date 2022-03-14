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

#include <Editor/AmplitudeAudioSystemEditor.h>

#include <AzCore/Math/Crc.h>
#include <AzCore/StringFunc/StringFunc.h>
#include <AzCore/Utils/Utils.h>
#include <AzCore/std/smart_ptr/make_shared.h>
#include <AzCore/std/string/conversions.h>

#include <ACETypes.h>

#include <Config.h>
#include <Editor/AmplitudeAudioSystemControl.h>
#include <Engine/Common.h>

#include <QDir>

void InitAmplitudeResources()
{
    Q_INIT_RESOURCE(AmplitudeEditor);
}

namespace SparkyStudios::Audio::Amplitude
{
    static TImplControlType TagToType(const AZStd::string_view tag)
    {
        if (tag == XmlTags::EventTag)
        {
            return eAMCT_AMPLITUDE_EVENT;
        }
        else if (tag == XmlTags::RtpcTag)
        {
            return eAMCT_AMPLITUDE_RTPC;
        }
        else if (tag == XmlTags::BusTag)
        {
            return eAMCT_AMPLITUDE_BUS;
        }
        else if (tag == XmlTags::FileTag)
        {
            return eAMCT_AMPLITUDE_SOUND_BANK;
        }
        else if (tag == XmlTags::SwitchTag)
        {
            return eAMCT_AMPLITUDE_SWITCH;
        }
        else if (tag == XmlTags::StateTag)
        {
            return eAMCT_AMPLITUDE_SWITCH;
        }
        else if (tag == XmlTags::EnvironmentTag)
        {
            return eAMCT_AMPLITUDE_EFFECT;
        }

        return eAMCT_INVALID;
    }

    static const AZStd::string_view TypeToTag(const TImplControlType type)
    {
        switch (type)
        {
        case eAMCT_AMPLITUDE_EVENT:
            return XmlTags::EventTag;
        case eAMCT_AMPLITUDE_RTPC:
            return XmlTags::RtpcTag;
        case eAMCT_AMPLITUDE_SWITCH:
            return XmlTags::SwitchTag;
        case eAMCT_AMPLITUDE_BUS:
            return XmlTags::BusTag;
        case eAMCT_AMPLITUDE_SOUND_BANK:
            return XmlTags::FileTag;
        case eAMCT_AMPLITUDE_SWITCH_STATE:
            return XmlTags::SwitchStateTag;
        case eAMCT_AMPLITUDE_EFFECT:
            return XmlTags::EnvironmentTag;
        }

        return "";
    }

    AmplitudeAudioSystemEditor::AmplitudeAudioSystemEditor()
    {
        InitAmplitudeResources();
    }

    void AmplitudeAudioSystemEditor::Reload()
    {
        // set all the controls as placeholder as we don't know if
        // any of them have been removed but still have connections to them
        for (const auto& idControlPair : m_controls)
        {
            TControlPtr control = idControlPair.second;
            if (control)
            {
                control->SetPlaceholder(true);
            }
        }

        // reload data
        m_loader.Load(this);

        m_connectionsByID.clear();
        UpdateConnectedStatus();
    }

    IAudioSystemControl* AmplitudeAudioSystemEditor::CreateControl(const SControlDef& controlDefinition)
    {
        AZStd::string fullName = controlDefinition.m_name;
        IAudioSystemControl* parent = controlDefinition.m_parentControl;
        if (parent)
        {
            AZ::StringFunc::Path::Join(controlDefinition.m_parentControl->GetName().c_str(), fullName.c_str(), fullName);
        }

        if (!controlDefinition.m_path.empty())
        {
            AZ::StringFunc::Path::Join(controlDefinition.m_path.c_str(), fullName.c_str(), fullName);
        }

        CID id = GetID(fullName);

        IAudioSystemControl* control = GetControl(id);
        if (control)
        {
            if (control->IsPlaceholder())
            {
                control->SetPlaceholder(false);
                if (parent && parent->IsPlaceholder())
                {
                    parent->SetPlaceholder(false);
                }
            }
            return control;
        }
        else
        {
            TControlPtr newControl =
                AZStd::make_shared<AmplitudeAudioSystemControl>(controlDefinition.m_name, id, controlDefinition.m_type);

            if (!parent)
            {
                parent = &m_rootControl;
            }

            parent->AddChild(newControl.get());
            newControl->SetParent(parent);
            newControl->SetLocalized(controlDefinition.m_isLocalized);
            m_controls[id] = newControl;
            return newControl.get();
        }
    }

    IAudioSystemControl* AmplitudeAudioSystemEditor::GetControl(CID id) const
    {
        if (id != ACE_INVALID_CID)
        {
            auto it = m_controls.find(id);
            if (it != m_controls.end())
            {
                return it->second.get();
            }
        }

        return nullptr;
    }

    IAudioSystemControl* AmplitudeAudioSystemEditor::GetControlByName(
        AZStd::string name, bool isLocalized, IAudioSystemControl* parent) const
    {
        if (parent)
        {
            AZ::StringFunc::Path::Join(parent->GetName().c_str(), name.c_str(), name);
        }

        if (isLocalized)
        {
            AZ::StringFunc::Path::Join(m_loader.GetLocalizationFolder().c_str(), name.c_str(), name);
        }

        return GetControl(GetID(name));
    }

    TConnectionPtr AmplitudeAudioSystemEditor::CreateConnectionToControl(
        EACEControlType atlControlType, IAudioSystemControl* middlewareControl)
    {
        if (middlewareControl)
        {
            middlewareControl->SetConnected(true);
            ++m_connectionsByID[middlewareControl->GetId()];

            if (middlewareControl->GetType() == eAMCT_AMPLITUDE_RTPC)
            {
                switch (atlControlType)
                {
                case EACEControlType::eACET_RTPC:
                    {
                        return AZStd::make_shared<CRtpcConnection>(middlewareControl->GetId());
                    }
                case EACEControlType::eACET_SWITCH_STATE:
                    {
                        return AZStd::make_shared<CSwitchStateConnection>(middlewareControl->GetId());
                    }
                case EACEControlType::eACET_ENVIRONMENT:
                    {
                        return AZStd::make_shared<CEffectConnection>(middlewareControl->GetId());
                    }
                }
            }

            return AZStd::make_shared<IAudioConnection>(middlewareControl->GetId());
        }

        return nullptr;
    }

    TConnectionPtr AmplitudeAudioSystemEditor::CreateConnectionFromXMLNode(
        AZ::rapidxml::xml_node<char>* node, EACEControlType atlControlType)
    {
        if (node)
        {
            AZStd::string_view element(node->name());
            TImplControlType type = TagToType(element);
            if (type != AUDIO_IMPL_INVALID_TYPE)
            {
                AZStd::string name;
                AZStd::string_view localized;

                if (auto nameAttr = node->first_attribute(XmlTags::NameAttribute, 0, false); nameAttr != nullptr)
                {
                    name = nameAttr->value();
                }

                if (auto localizedAttr = node->first_attribute(XmlTags::LocalizedAttribute, 0, false); localizedAttr != nullptr)
                {
                    localized = localizedAttr->value();
                }

                bool isLocalized = AZ::StringFunc::Equal(localized, "true");

                // If the control wasn't found, create a placeholder.
                // We want to see that connection even if it's not in the middleware.
                // User could be viewing the editor without a middleware project.
                IAudioSystemControl* control = GetControlByName(name, isLocalized);
                if (!control)
                {
                    control = CreateControl(SControlDef(name, type));
                    if (control)
                    {
                        control->SetPlaceholder(true);
                        control->SetLocalized(isLocalized);
                    }
                }

                // If it's a switch we connect to one of the states within the switch
                if (type == eAMCT_AMPLITUDE_SWITCH)
                {
                    if (auto childNode = node->first_node(); childNode != nullptr)
                    {
                        AZStd::string childName;
                        if (auto childNameAttr = childNode->first_attribute(XmlTags::NameAttribute, 0, false); childNameAttr != nullptr)
                        {
                            childName = childNameAttr->value();
                        }

                        IAudioSystemControl* childControl = GetControlByName(childName, false, control);
                        if (!childControl)
                        {
                            childControl = CreateControl(SControlDef(childName, eAMCT_AMPLITUDE_SWITCH_STATE, false, control));
                        }

                        control = childControl;
                    }
                }

                if (control)
                {
                    control->SetConnected(true);
                    ++m_connectionsByID[control->GetId()];

                    if (type == eAMCT_AMPLITUDE_RTPC)
                    {
                        switch (atlControlType)
                        {
                        case EACEControlType::eACET_RTPC:
                            {
                                TRtpcConnectionPtr connection = AZStd::make_shared<CRtpcConnection>(control->GetId());

                                float mult = 1.0f;
                                float shift = 0.0f;

                                if (auto multAttr = node->first_attribute(XmlTags::MultiplierAttribute, 0, false); multAttr != nullptr)
                                {
                                    mult = AZStd::stof(AZStd::string(multAttr->value()));
                                }

                                if (auto shiftAttr = node->first_attribute(XmlTags::ShiftAttribute, 0, false); shiftAttr != nullptr)
                                {
                                    shift = AZStd::stof(AZStd::string(shiftAttr->value()));
                                }

                                connection->m_mult = mult;
                                connection->m_shift = shift;

                                return connection;
                            }
                        case EACEControlType::eACET_SWITCH_STATE:
                            {
                                TSwitchStateConnectionPtr connection = AZStd::make_shared<CSwitchStateConnection>(control->GetId());

                                AmObjectID state = 0.0f;
                                if (auto valueAttr = node->first_attribute(XmlTags::ValueAttribute, 0, false); valueAttr != nullptr)
                                {
                                    state = AZStd::stoi(AZStd::string(valueAttr->value()));
                                }
                                connection->m_state = state;

                                return connection;
                            }
                        }
                    }
                    else
                    {
                        return AZStd::make_shared<IAudioConnection>(control->GetId());
                    }
                }
            }
        }

        return nullptr;
    }

    AZ::rapidxml::xml_node<char>* AmplitudeAudioSystemEditor::CreateXMLNodeFromConnection(
        const TConnectionPtr connection, const EACEControlType atlControlType)
    {
        const auto* control = static_cast<AmplitudeAudioSystemControl*>(GetControl(connection->GetID()));

        if (control)
        {
            XmlAllocator& xmlAllocator(AudioControls::s_xmlAllocator);

            switch (control->GetType())
            {
            case AudioControls::eAMCT_AMPLITUDE_SWITCH_STATE:
                [[fallthrough]];
            case AudioControls::eAMCT_AMPLITUDE_SWITCH:
                {
                    const auto* parent = static_cast<AmplitudeAudioSystemControl*>(control->GetParent());

                    if (parent)
                    {
                        AZStd::string_view parentType = TypeToTag(parent->GetType());
                        auto switchNode =
                            xmlAllocator.allocate_node(AZ::rapidxml::node_element, xmlAllocator.allocate_string(parentType.data()));

                        auto switchNameAttr = xmlAllocator.allocate_attribute(
                            XmlTags::NameAttribute, xmlAllocator.allocate_string(parent->GetName().c_str()));

                        auto switchIdAttr = xmlAllocator.allocate_attribute(
                            XmlTags::IdAttribute, xmlAllocator.allocate_string(AZStd::to_string(parent->GetAmplitudeId()).c_str()));

                        auto stateNode = xmlAllocator.allocate_node(AZ::rapidxml::node_element, XmlTags::SwitchStateTag);

                        auto stateNameAttr = xmlAllocator.allocate_attribute(
                            XmlTags::NameAttribute, xmlAllocator.allocate_string(control->GetName().c_str()));

                        auto stateIdAttr = xmlAllocator.allocate_attribute(
                            XmlTags::IdAttribute, xmlAllocator.allocate_string(AZStd::to_string(control->GetAmplitudeId()).c_str()));

                        switchNode->append_attribute(switchIdAttr);
                        switchNode->append_attribute(switchNameAttr);

                        stateNode->append_attribute(stateNameAttr);
                        stateNode->append_attribute(stateIdAttr);

                        switchNode->append_node(stateNode);

                        return switchNode;
                    }
                    break;
                }

            case AudioControls::eAMCT_AMPLITUDE_RTPC:
                {
                    auto connectionNode = xmlAllocator.allocate_node(
                        AZ::rapidxml::node_element, xmlAllocator.allocate_string(TypeToTag(control->GetType()).data()));

                    auto idAttr = xmlAllocator.allocate_attribute(
                        XmlTags::IdAttribute, xmlAllocator.allocate_string(AZStd::to_string(control->GetAmplitudeId()).c_str()));

                    auto nameAttr =
                        xmlAllocator.allocate_attribute(XmlTags::NameAttribute, xmlAllocator.allocate_string(control->GetName().c_str()));

                    connectionNode->append_attribute(idAttr);
                    connectionNode->append_attribute(nameAttr);

                    if (atlControlType == eACET_RTPC)
                    {
                        AZStd::shared_ptr<const CRtpcConnection> rtpcConnection =
                            AZStd::static_pointer_cast<const CRtpcConnection>(connection);
                        if (rtpcConnection->m_mult != 1.f)
                        {
                            auto multAttr = xmlAllocator.allocate_attribute(
                                XmlTags::MultiplierAttribute,
                                xmlAllocator.allocate_string(AZStd::to_string(rtpcConnection->m_mult).c_str()));

                            connectionNode->append_attribute(multAttr);
                        }

                        if (rtpcConnection->m_shift != 0.f)
                        {
                            auto shiftAttr = xmlAllocator.allocate_attribute(
                                XmlTags::ShiftAttribute, xmlAllocator.allocate_string(AZStd::to_string(rtpcConnection->m_shift).c_str()));

                            connectionNode->append_attribute(shiftAttr);
                        }
                    }
                    else if (atlControlType == eACET_SWITCH_STATE)
                    {
                        AZStd::shared_ptr<const CSwitchStateConnection> stateConnection =
                            AZStd::static_pointer_cast<const CSwitchStateConnection>(connection);

                        auto valueAttr = xmlAllocator.allocate_attribute(
                            XmlTags::ValueAttribute, xmlAllocator.allocate_string(AZStd::to_string(stateConnection->m_state).c_str()));

                        connectionNode->append_attribute(valueAttr);
                    }

                    return connectionNode;
                }

            case AudioControls::eAMCT_AMPLITUDE_EVENT:
                [[fallthrough]];
            case AudioControls::eAMCT_AMPLITUDE_BUS:
                {
                    auto connectionNode = xmlAllocator.allocate_node(
                        AZ::rapidxml::node_element, xmlAllocator.allocate_string(TypeToTag(control->GetType()).data()));

                    auto idAttr = xmlAllocator.allocate_attribute(
                        XmlTags::IdAttribute, xmlAllocator.allocate_string(AZStd::to_string(control->GetAmplitudeId()).c_str()));

                    auto nameAttr =
                        xmlAllocator.allocate_attribute(XmlTags::NameAttribute, xmlAllocator.allocate_string(control->GetName().c_str()));

                    connectionNode->append_attribute(idAttr);
                    connectionNode->append_attribute(nameAttr);

                    return connectionNode;
                }

            case AudioControls::eAMCT_AMPLITUDE_EFFECT:
                {
                    AZ_Assert(atlControlType == eACET_ENVIRONMENT, "Control Type do not match");

                    auto connectionNode = xmlAllocator.allocate_node(
                        AZ::rapidxml::node_element, xmlAllocator.allocate_string(TypeToTag(control->GetType()).data()));

                    AZStd::shared_ptr<const CEffectConnection> effectConnection = AZStd::static_pointer_cast<const CEffectConnection>(connection);

                    auto idAttr = xmlAllocator.allocate_attribute(
                        XmlTags::IdAttribute, xmlAllocator.allocate_string(AZStd::to_string(control->GetAmplitudeId()).c_str()));

                    auto effectAttr = xmlAllocator.allocate_attribute(
                        XmlTags::ValueAttribute, xmlAllocator.allocate_string(AZStd::to_string(effectConnection->m_effect).c_str()));

                    auto nameAttr =
                        xmlAllocator.allocate_attribute(XmlTags::NameAttribute, xmlAllocator.allocate_string(control->GetName().c_str()));

                    connectionNode->append_attribute(idAttr);
                    connectionNode->append_attribute(effectAttr);
                    connectionNode->append_attribute(nameAttr);

                    return connectionNode;
                }

            case AudioControls::eAMCT_AMPLITUDE_SOUND_BANK:
                {
                    auto connectionNode = xmlAllocator.allocate_node(
                        AZ::rapidxml::node_element, xmlAllocator.allocate_string(TypeToTag(control->GetType()).data()));

                    auto idAttr = xmlAllocator.allocate_attribute(
                        XmlTags::IdAttribute, xmlAllocator.allocate_string(AZStd::to_string(control->GetAmplitudeId()).c_str()));

                    auto nameAttr =
                        xmlAllocator.allocate_attribute(XmlTags::NameAttribute, xmlAllocator.allocate_string(control->GetName().c_str()));

                    connectionNode->append_attribute(idAttr);
                    connectionNode->append_attribute(nameAttr);

                    if (control->IsLocalized())
                    {
                        auto locAttr = xmlAllocator.allocate_attribute(XmlTags::LocalizedAttribute, xmlAllocator.allocate_string("true"));

                        connectionNode->append_attribute(locAttr);
                    }

                    return connectionNode;
                }
            }
        }

        return nullptr;
    }

    const AZStd::string_view AmplitudeAudioSystemEditor::GetTypeIcon(TImplControlType type) const
    {
        switch (type)
        {
        case eAMCT_AMPLITUDE_EVENT:
            return ":/Editor/Icons/event_normal.svg";
        case eAMCT_AMPLITUDE_RTPC:
            return ":/Editor/Icons/rtpc_normal.svg";
        case eAMCT_AMPLITUDE_SWITCH:
            return ":/Editor/Icons/switch_normal.svg";
        case eAMCT_AMPLITUDE_BUS:
            return ":/Editor/Icons/bus_normal.svg";
        case eAMCT_AMPLITUDE_SOUND_BANK:
            return ":/Editor/Icons/soundbank_normal.svg";
        case eAMCT_AMPLITUDE_SWITCH_STATE:
            return ":/Editor/Icons/switch_state_normal.svg";
        default:
            return ":/Editor/Icons/control_normal.svg";
        }
    }

    const AZStd::string_view AmplitudeAudioSystemEditor::GetTypeIconSelected(TImplControlType type) const
    {
        switch (type)
        {
        case eAMCT_AMPLITUDE_EVENT:
            return ":/Editor/Icons/event_selected.svg";
        case eAMCT_AMPLITUDE_RTPC:
            return ":/Editor/Icons/rtpc_selected.svg";
        case eAMCT_AMPLITUDE_SWITCH:
            return ":/Editor/Icons/switch_selected.svg";
        case eAMCT_AMPLITUDE_BUS:
            return ":/Editor/Icons/bus_selected.svg";
        case eAMCT_AMPLITUDE_SOUND_BANK:
            return ":/Editor/Icons/soundbank_selected.svg";
        case eAMCT_AMPLITUDE_SWITCH_STATE:
            return ":/Editor/Icons/switch_state_selected.svg";
        default:
            return ":/Editor/Icons/control_selected.svg";
        }
    }

    EACEControlType AmplitudeAudioSystemEditor::ImplTypeToATLType(TImplControlType type) const
    {
        switch (type)
        {
        case eAMCT_AMPLITUDE_EVENT:
            return eACET_TRIGGER;
        case eAMCT_AMPLITUDE_RTPC:
            return eACET_RTPC;
        case eAMCT_AMPLITUDE_SWITCH_STATE:
            return eACET_SWITCH_STATE;
        case eAMCT_AMPLITUDE_EFFECT:
        case eAMCT_AMPLITUDE_BUS:
            return eACET_ENVIRONMENT;
        case eAMCT_AMPLITUDE_SOUND_BANK:
            return eACET_PRELOAD;
        case eAMCT_AMPLITUDE_SWITCH:
            return eACET_SWITCH;
        }

        return eACET_NUM_TYPES;
    }

    TImplControlTypeMask AmplitudeAudioSystemEditor::GetCompatibleTypes(EACEControlType atlControlType) const
    {
        switch (atlControlType)
        {
        case eACET_TRIGGER:
            return eAMCT_AMPLITUDE_EVENT;
        case eACET_RTPC:
            return eAMCT_AMPLITUDE_RTPC;
        case eACET_SWITCH:
            return eAMCT_AMPLITUDE_SWITCH;
        case eACET_SWITCH_STATE:
            return eAMCT_AMPLITUDE_SWITCH_STATE;
        case eACET_ENVIRONMENT:
            return (eAMCT_AMPLITUDE_BUS | eAMCT_AMPLITUDE_SWITCH_STATE | eAMCT_AMPLITUDE_EFFECT);
        case eACET_PRELOAD:
            return eAMCT_AMPLITUDE_SOUND_BANK;
        }

        return AUDIO_IMPL_INVALID_TYPE;
    }

    CID AmplitudeAudioSystemEditor::GetID(const AZStd::string_view name) const
    {
        return ::Audio::AudioStringToID<CID>(name.data());
    }

    AZStd::string AmplitudeAudioSystemEditor::GetName() const
    {
        return "Amplitude";
    }

    void AmplitudeAudioSystemEditor::UpdateConnectedStatus()
    {
        for (const auto& idCountPair : m_connectionsByID)
        {
            if (idCountPair.second > 0)
            {
                IAudioSystemControl* control = GetControl(idCountPair.first);
                if (control)
                {
                    control->SetConnected(true);
                }
            }
        }
    }

    void AmplitudeAudioSystemEditor::ConnectionRemoved(IAudioSystemControl* control)
    {
        int connectionCount = m_connectionsByID[control->GetId()] - 1;
        if (connectionCount <= 0)
        {
            connectionCount = 0;
            control->SetConnected(false);
        }
        m_connectionsByID[control->GetId()] = connectionCount;
    }

    AZ::IO::FixedMaxPath AmplitudeAudioSystemEditor::GetDataPath() const
    {
        auto projectPath = AZ::IO::FixedMaxPath{ AZ::Utils::GetProjectPath() };
        return (projectPath / "sounds" / "amplitude_project");
    }
} // namespace SparkyStudios::Audio::Amplitude