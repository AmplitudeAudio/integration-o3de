set(AMPLITUDEAUDIOSDK_LIBS ${BASE_PATH}/sdk/lib/win/$<IF:$<CONFIG:Debug>,Amplitude_d.lib,Amplitude.lib>)
