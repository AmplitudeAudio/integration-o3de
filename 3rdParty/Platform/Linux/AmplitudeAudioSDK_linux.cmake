set(AMPLITUDEAUDIOSDK_LIBS
    ${BASE_PATH}/sdk/lib/linux/$<IF:$<CONFIG:Debug>,libAmplitude_d.a,libAmplitude.a>
    ${BASE_PATH}/sdk/lib/linux/$<IF:$<CONFIG:Debug>,libsamplerate_d.a,libsamplerate.a>
)
