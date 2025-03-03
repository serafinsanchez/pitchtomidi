if(CMAKE_BUILD_TYPE MATCHES Debug)
    enable_testing()

    add_executable(unit_tests
        main_test.cpp
    )

    target_include_directories(unit_tests
        PRIVATE
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/src
    )

    target_link_libraries(unit_tests
        PRIVATE
            GTest::gtest
            GTest::gtest_main
            PortAudio::PortAudio
            RtMidi::RtMidi
            spdlog::spdlog
            nlohmann_json::nlohmann_json
    )

    if(USE_FFTW)
        target_link_libraries(unit_tests
            PRIVATE
                FFTW3::FFTW3
        )
        target_compile_definitions(unit_tests
            PRIVATE
                USE_FFTW
        )
    endif()

    include(GoogleTest)
    gtest_discover_tests(unit_tests)
endif() 