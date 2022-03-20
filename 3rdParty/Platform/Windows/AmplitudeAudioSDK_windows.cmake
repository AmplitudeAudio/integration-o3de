set(AMPLITUDEAUDIOSDK_LIBS
    ${BASE_PATH}/sdk/lib/win/$<IF:$<CONFIG:Debug>,Amplitude_d.lib,Amplitude.lib>
    ${BASE_PATH}/sdk/lib/win/$<IF:$<CONFIG:Debug>,samplerate_d.lib,samplerate.lib>
)
