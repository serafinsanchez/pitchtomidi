add_executable(${PROJECT_NAME}
    main.cpp
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        PortAudio::PortAudio
        RtMidi::RtMidi
        Qt6::Widgets
        spdlog::spdlog
        nlohmann_json::nlohmann_json
)

if(USE_FFTW)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            FFTW3::FFTW3
    )
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
            USE_FFTW
    )
endif() 