# Try to find RtMidi
include(FindPackageHandleStandardArgs)

if(APPLE)
    # On macOS, try to find Homebrew's installation first
    execute_process(
        COMMAND brew --prefix rtmidi
        OUTPUT_VARIABLE HOMEBREW_RTMIDI_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()

find_path(RTMIDI_INCLUDE_DIR
    NAMES rtmidi/RtMidi.h
    PATHS
        ${HOMEBREW_RTMIDI_PREFIX}/include
        /opt/homebrew/include
        /usr/local/include
        /usr/include
    DOC "The RtMidi include directory"
)

find_library(RTMIDI_LIBRARY
    NAMES rtmidi
    PATHS
        ${HOMEBREW_RTMIDI_PREFIX}/lib
        /opt/homebrew/lib
        /usr/local/lib
        /usr/lib
    DOC "The RtMidi library"
)

# Handle the QUIETLY and REQUIRED arguments and set RTMIDI_FOUND to TRUE if
# all listed variables are TRUE
find_package_handle_standard_args(RtMidi
    REQUIRED_VARS 
        RTMIDI_LIBRARY 
        RTMIDI_INCLUDE_DIR
)

if(RTMIDI_FOUND AND NOT TARGET RtMidi::RtMidi)
    add_library(RtMidi::RtMidi UNKNOWN IMPORTED)
    set_target_properties(RtMidi::RtMidi PROPERTIES
        IMPORTED_LOCATION "${RTMIDI_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${RTMIDI_INCLUDE_DIR}"
    )
endif()

mark_as_advanced(RTMIDI_INCLUDE_DIR RTMIDI_LIBRARY) 