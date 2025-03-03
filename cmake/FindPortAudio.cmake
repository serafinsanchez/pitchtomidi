# Try to find PortAudio
include(FindPackageHandleStandardArgs)

if(APPLE)
    # On macOS, try to find Homebrew's installation first
    execute_process(
        COMMAND brew --prefix portaudio
        OUTPUT_VARIABLE HOMEBREW_PORTAUDIO_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

find_path(PORTAUDIO_INCLUDE_DIR
    NAMES portaudio.h
    PATHS
        ${HOMEBREW_PORTAUDIO_PREFIX}/include
        /opt/homebrew/include
        /usr/local/include
        /usr/include
    DOC "The PortAudio include directory"
)

find_library(PORTAUDIO_LIBRARY
    NAMES portaudio
    PATHS
        ${HOMEBREW_PORTAUDIO_PREFIX}/lib
        /opt/homebrew/lib
        /usr/local/lib
        /usr/lib
    DOC "The PortAudio library"
)

# Handle the QUIETLY and REQUIRED arguments and set PORTAUDIO_FOUND to TRUE if
# all listed variables are TRUE
find_package_handle_standard_args(PortAudio
    REQUIRED_VARS 
        PORTAUDIO_LIBRARY 
        PORTAUDIO_INCLUDE_DIR
)

if(PORTAUDIO_FOUND AND NOT TARGET PortAudio::PortAudio)
    add_library(PortAudio::PortAudio UNKNOWN IMPORTED)
    set_target_properties(PortAudio::PortAudio PROPERTIES
        IMPORTED_LOCATION "${PORTAUDIO_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${PORTAUDIO_INCLUDE_DIR}"
    )
endif()

mark_as_advanced(PORTAUDIO_INCLUDE_DIR PORTAUDIO_LIBRARY) 