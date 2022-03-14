set(AMPLITUDEAUDIOSDK_LIBS ${BASE_PATH}/sdk/lib/win/$<IF:$<CONFIG:Debug>,libAmplitude_d.so,libAmplitude.so>)
