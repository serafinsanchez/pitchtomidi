if(CMAKE_BUILD_TYPE MATCHES Debug)
    enable_testing()

    add_executable(unit_tests
        main_test.cpp
    )

    target_include_directories(unit_tests
        PRIVATE
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/src
            ${PORTAUDIO_INCLUDE_DIRS}
            ${RTMIDI_INCLUDE_DIRS}
    )

    # Find libraries with explicit paths
    find_library(PORTAUDIO_LIB portaudio PATHS /opt/homebrew/lib REQUIRED)
    find_library(RTMIDI_LIB rtmidi PATHS /opt/homebrew/lib REQUIRED)

    target_link_libraries(unit_tests
        PRIVATE
            GTest::gtest
            GTest::gtest_main
            ${PORTAUDIO_LIB}
            ${RTMIDI_LIB}
            spdlog::spdlog
            nlohmann_json::nlohmann_json
    )

    if(USE_FFTW)
        find_library(FFTW3_LIB fftw3 PATHS /opt/homebrew/lib REQUIRED)
        target_link_libraries(unit_tests
            PRIVATE
                ${FFTW3_LIB}
        )
        target_compile_definitions(unit_tests
            PRIVATE
                USE_FFTW
        )
    endif()

    include(GoogleTest)
    gtest_discover_tests(unit_tests)
endif()

# Audio/MIDI test executable
add_executable(test_audio_midi test_audio_midi.cpp)

target_include_directories(test_audio_midi
    PRIVATE
        ${PORTAUDIO_INCLUDE_DIRS}
        ${RTMIDI_INCLUDE_DIRS}
)

target_link_libraries(test_audio_midi
    PRIVATE
        ${PORTAUDIO_LIB}
        ${RTMIDI_LIB}
)

# Audio Input test executable
add_executable(test_audio_input test_audio_input.cpp)

target_include_directories(test_audio_input
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${PORTAUDIO_INCLUDE_DIRS}
)

target_link_libraries(test_audio_input
    PRIVATE
        audio_input_lib
)

# Audio Capture test executable
add_executable(test_capture_audio test_capture_audio.cpp)

target_include_directories(test_capture_audio
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${PORTAUDIO_INCLUDE_DIRS}
)

target_link_libraries(test_capture_audio
    PRIVATE
        audio_input_lib
)

if(APPLE)
    target_link_libraries(test_audio_midi
        PRIVATE
            "-framework CoreAudio"
            "-framework AudioToolbox"
            "-framework AudioUnit"
            "-framework CoreFoundation"
            "-framework CoreServices"
    )

    target_compile_definitions(test_audio_midi
        PRIVATE
            __MACOSX_CORE__
    )
    
    target_link_libraries(test_audio_input
        PRIVATE
            "-framework CoreAudio"
            "-framework AudioToolbox"
            "-framework AudioUnit"
            "-framework CoreFoundation"
            "-framework CoreServices"
    )
    
    target_link_libraries(test_capture_audio
        PRIVATE
            "-framework CoreAudio"
            "-framework AudioToolbox"
            "-framework AudioUnit"
            "-framework CoreFoundation"
            "-framework CoreServices"
    )
endif()

add_test(NAME TestAudioMidi COMMAND test_audio_midi)
add_test(NAME TestAudioInput COMMAND test_audio_input)
add_test(NAME TestCaptureAudio COMMAND test_capture_audio)