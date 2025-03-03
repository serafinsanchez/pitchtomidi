juce_add_gui_app(PitchToMidi
    PRODUCT_NAME "PitchToMidi"
    VERSION "1.0.0"
    COMPANY_NAME "YourCompany"
    MICROPHONE_PERMISSION_ENABLED TRUE
)

juce_generate_juce_header(PitchToMidi)

# Create audio input library
add_library(audio_input_lib STATIC
    audio/AudioInput.cpp
)

target_include_directories(audio_input_lib
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${PORTAUDIO_INCLUDE_DIRS}
)

target_link_libraries(audio_input_lib
    PUBLIC
        ${PORTAUDIO_LIB}
)

if(APPLE)
    target_link_libraries(audio_input_lib
        PUBLIC
            "-framework CoreAudio"
            "-framework AudioToolbox"
            "-framework AudioUnit"
            "-framework CoreFoundation"
            "-framework CoreServices"
    )
endif()

target_sources(PitchToMidi
    PRIVATE
        gui/MainComponent.cpp
        gui/MainComponent.h
        gui/MainWindow.h
        gui/PitchToMidiApplication.h
        gui/PitchToMidiApplication.cpp
        audio/AudioCapture.cpp
)

target_compile_definitions(PitchToMidi
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:PitchToMidi,PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:PitchToMidi,VERSION>"
        $<$<CONFIG:Debug>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG>
        $<$<CONFIG:Release>:SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO>
)

target_include_directories(PitchToMidi
    PRIVATE
        ${CMAKE_SOURCE_DIR}/JUCE/modules
        ${CMAKE_BINARY_DIR}/juce_build_tools/JuceHeader
        ${PORTAUDIO_INCLUDE_DIRS}
        ${RTMIDI_INCLUDE_DIRS}
        ${FFTW3_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(PitchToMidi
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        ${PORTAUDIO_LIB}
        ${RTMIDI_LIB}
        Qt6::Widgets
        audio_input_lib
)

find_library(PORTAUDIO_LIB portaudio PATHS /opt/homebrew/lib REQUIRED)
target_link_libraries(PitchToMidi PRIVATE ${PORTAUDIO_LIB})

find_library(RTMIDI_LIB rtmidi PATHS /opt/homebrew/lib REQUIRED)
target_link_libraries(PitchToMidi PRIVATE ${RTMIDI_LIB})

find_library(FFTW3_LIB fftw3 PATHS /opt/homebrew/lib REQUIRED)
target_link_libraries(PitchToMidi PRIVATE ${FFTW3_LIB})

if(USE_FFTW)
    target_link_libraries(PitchToMidi PRIVATE ${FFTW3_LIB})
endif() 