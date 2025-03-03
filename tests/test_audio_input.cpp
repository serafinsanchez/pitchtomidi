#include "audio/AudioInput.hpp"
#include <iostream>
#include <iomanip>

int main() {
    std::cout << "Testing Audio Input Module" << std::endl;
    
    try {
        // Enumerate audio devices
        auto devices = ptm::enumerateAudioDevices();
        
        std::cout << "Found " << devices.size() << " audio input devices:" << std::endl;
        
        // Print device information
        for (const auto& device : devices) {
            std::cout << "Device ID: " << device.audioDeviceId << std::endl;
            std::cout << "  Name: " << device.name << std::endl;
            std::cout << "  Host API: " << device.hostApi << std::endl;
            std::cout << "  Default Sample Rate: " << device.sampleRate << " Hz" << std::endl;
            std::cout << "  Max Input Channels: " << device.maxInputChannels << std::endl;
            std::cout << "  Default Latency: " << std::fixed << std::setprecision(3) 
                      << (device.defaultLatency * 1000) << " ms" << std::endl;
            
            std::cout << "  Supported Sample Rates: ";
            for (const auto& rate : device.sampleRates) {
                std::cout << rate << " ";
            }
            std::cout << std::endl;
            
            if (device.isDefaultInput) {
                std::cout << "  ** DEFAULT INPUT DEVICE **" << std::endl;
            }
            std::cout << std::endl;
        }
        
        // Get default device
        int defaultDeviceId = ptm::getDefaultAudioInputDevice();
        if (defaultDeviceId >= 0) {
            std::cout << "Default input device ID: " << defaultDeviceId << std::endl;
            
            // Test sample rate support for default device
            double testRate = 48000.0;
            bool supported = ptm::isSampleRateSupported(defaultDeviceId, testRate);
            std::cout << "Sample rate " << testRate << " Hz is " 
                      << (supported ? "supported" : "not supported") 
                      << " by the default device." << std::endl;
        } else {
            std::cout << "No default input device found." << std::endl;
        }
        
    } catch (const ptm::AudioInputException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 2;
    }
    
    return 0;
} 