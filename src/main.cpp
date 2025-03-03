#include "utils/Logger.hpp"
#include <stdexcept>

int main(int argc, char* argv[]) {
    try {
        // Initialize the logger
        PitchToMidi::Logger::getInstance().init();

        // Example usage of different log levels
        LOG_INFO("Application started");
        LOG_DEBUG("Debug mode is enabled");
        
        // Example with formatted output
        int sampleRate = 44100;
        LOG_INFO("Audio initialized with sample rate: {}", sampleRate);
        
        // Example of logging in a try-catch block
        try {
            throw std::runtime_error("Example error");
        } catch (const std::exception& e) {
            LOG_ERROR("Caught exception: {}", e.what());
        }

        LOG_INFO("Application shutting down normally");
        return 0;
    } catch (const std::exception& e) {
        LOG_CRITICAL("Fatal error: {}", e.what());
        return 1;
    }
} 