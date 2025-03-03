include(FetchContent)

# Find pkg-config for system-wide library detection
find_package(PkgConfig REQUIRED)

# PortAudio
pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)
if(PORTAUDIO_FOUND)
    message(STATUS "Found PortAudio: ${PORTAUDIO_LIBRARIES}")
endif()

# RtMidi
pkg_check_modules(RTMIDI REQUIRED rtmidi)
if(RTMIDI_FOUND)
    message(STATUS "Found RtMidi: ${RTMIDI_LIBRARIES}")
endif()

# FFTW (Optional)
option(USE_FFTW "Use FFTW for FFT-based pitch detection" ON)
if(USE_FFTW)
    pkg_check_modules(FFTW3 REQUIRED fftw3)
    if(FFTW3_FOUND)
        message(STATUS "Found FFTW3: ${FFTW3_LIBRARIES}")
    endif()
endif()

# Qt6 (for GUI)
find_package(Qt6 COMPONENTS Widgets REQUIRED)

# spdlog (for logging)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.12.0
)
FetchContent_MakeAvailable(spdlog)

# GoogleTest (for testing)
if(CMAKE_BUILD_TYPE MATCHES Debug)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    FetchContent_MakeAvailable(googletest)
endif()

# JSON for Modern C++ (for configuration)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

# JUCE Framework
add_subdirectory(${CMAKE_SOURCE_DIR}/JUCE) 