include(FetchContent)

# PortAudio
find_package(PortAudio REQUIRED)

# RtMidi
find_package(RtMidi REQUIRED)

# FFTW (Optional)
option(USE_FFTW "Use FFTW for FFT-based pitch detection" ON)
if(USE_FFTW)
    find_package(FFTW3 REQUIRED)
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