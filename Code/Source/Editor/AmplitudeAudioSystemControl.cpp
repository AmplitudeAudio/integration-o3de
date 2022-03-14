#include <Editor/AmplitudeAudioSystemControl.h>

namespace AudioControls
{
    //-------------------------------------------------------------------------------------------//
    AmplitudeAudioSystemControl::AmplitudeAudioSystemControl(const AZStd::string& name, CID id, TImplControlType type)
        : IAudioSystemControl(name, id, type)
    {
    }

    AmObjectID AmplitudeAudioSystemControl::GetAmplitudeId() const
    {
        return _id;
    }

    void AmplitudeAudioSystemControl::SetAmplitudeId(AmObjectID id)
    {
        _id = id;
    }
} // namespace AudioControls
