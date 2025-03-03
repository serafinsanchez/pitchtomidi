#include "audio/AudioInput.hpp"
#include <iostream>
#include <algorithm>

namespace ptm {

namespace {
    // Common sample rates to test for support
    const std::vector<double> kCommonSampleRates = {
        8000.0, 11025.0, 16000.0, 22050.0, 32000.0, 44100.0, 48000.0, 88200.0, 96000.0
    };

    // Helper function to test if a sample rate is supported
    bool testSampleRate(PaStreamParameters* inputParams, double sampleRate) {
        return Pa_IsFormatSupported(inputParams, nullptr, sampleRate) == paFormatIsSupported;
    }

    // Helper function to get supported sample rates for a device
    std::set<double> getSupportedRatesInternal(const PaDeviceInfo* deviceInfo, PaDeviceIndex deviceIndex) {
        std::set<double> supported;
        
        if (!deviceInfo || deviceInfo->maxInputChannels <= 0) {
            return supported;
        }
        
        PaStreamParameters inputParams;
        inputParams.device = deviceIndex;
        inputParams.channelCount = 1;  // Mono input
        inputParams.sampleFormat = paFloat32;
        inputParams.suggestedLatency = deviceInfo->defaultLowInputLatency;
        inputParams.hostApiSpecificStreamInfo = nullptr;

        for (double rate : kCommonSampleRates) {
            if (testSampleRate(&inputParams, rate)) {
                supported.insert(rate);
            }
        }

        return supported;
    }

    // Initialize PortAudio if needed
    bool ensurePortAudioInitialized() {
        static bool initialized = false;
        static std::string lastError;
        
        if (!initialized) {
            PaError err = Pa_Initialize();
            if (err != paNoError) {
                lastError = Pa_GetErrorText(err);
                std::cerr << "Failed to initialize PortAudio: " << lastError << std::endl;
                return false;
            }
            initialized = true;
            std::cout << "PortAudio initialized successfully" << std::endl;
            
            // Register termination at program exit
            std::atexit([]() {
                Pa_Terminate();
                std::cout << "PortAudio terminated" << std::endl;
            });
        }
        return initialized;
    }
}

std::vector<AudioDeviceInfo> enumerateAudioDevices() {
    std::vector<AudioDeviceInfo> devices;
    
    // Ensure PortAudio is initialized
    if (!ensurePortAudioInitialized()) {
        throw AudioInputException("Failed to initialize PortAudio");
    }
    
    int numDevices = Pa_GetDeviceCount();
    if (numDevices < 0) {
        throw AudioInputException(std::string("Error getting device count: ") + 
                                Pa_GetErrorText(numDevices));
    }

    PaDeviceIndex defaultInput = Pa_GetDefaultInputDevice();
    std::cout << "Found " << numDevices << " audio devices, default input device index: " 
              << defaultInput << std::endl;

    for (int i = 0; i < numDevices; i++) {
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo && deviceInfo->maxInputChannels > 0) {
            AudioDeviceInfo device;
            device.audioDeviceId = i;
            device.name = deviceInfo->name;
            device.hostApi = Pa_GetHostApiInfo(deviceInfo->hostApi)->name;
            device.sampleRate = deviceInfo->defaultSampleRate;
            device.sampleRates = getSupportedRatesInternal(deviceInfo, i);
            device.maxInputChannels = deviceInfo->maxInputChannels;
            device.defaultLatency = deviceInfo->defaultLowInputLatency;
            device.minLatency = deviceInfo->defaultLowInputLatency;
            device.isDefaultInput = (i == defaultInput);
            
            devices.push_back(device);
            
            std::cout << "Found input device: " << device.name << " (" << device.hostApi << ")" << std::endl;
            std::cout << "  Supported rates: ";
            for (double rate : device.sampleRates) {
                std::cout << rate << " Hz ";
            }
            std::cout << std::endl;
        }
    }

    if (devices.empty()) {
        std::cerr << "No audio input devices found" << std::endl;
    }

    return devices;
}

int getDefaultAudioInputDevice() {
    // Ensure PortAudio is initialized
    if (!ensurePortAudioInitialized()) {
        throw AudioInputException("Failed to initialize PortAudio");
    }
    
    PaDeviceIndex defaultInput = Pa_GetDefaultInputDevice();
    if (defaultInput == paNoDevice) {
        std::cerr << "No default input device found" << std::endl;
        return -1;
    }
    
    return defaultInput;
}

bool isSampleRateSupported(int deviceId, double sampleRate) {
    // Ensure PortAudio is initialized
    if (!ensurePortAudioInitialized()) {
        throw AudioInputException("Failed to initialize PortAudio");
    }
    
    const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(deviceId);
    if (!deviceInfo || deviceInfo->maxInputChannels <= 0) {
        std::cerr << "Invalid device ID: " << deviceId << std::endl;
        return false;
    }
    
    PaStreamParameters inputParams;
    inputParams.device = deviceId;
    inputParams.channelCount = 1;  // Mono input
    inputParams.sampleFormat = paFloat32;
    inputParams.suggestedLatency = deviceInfo->defaultLowInputLatency;
    inputParams.hostApiSpecificStreamInfo = nullptr;
    
    return testSampleRate(&inputParams, sampleRate);
}

std::set<double> getSupportedSampleRates(int deviceId) {
    // Ensure PortAudio is initialized
    if (!ensurePortAudioInitialized()) {
        throw AudioInputException("Failed to initialize PortAudio");
    }
    
    const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(deviceId);
    if (!deviceInfo || deviceInfo->maxInputChannels <= 0) {
        std::cerr << "Invalid device ID: " << deviceId << std::endl;
        return {};
    }
    
    return getSupportedRatesInternal(deviceInfo, deviceId);
}

} // namespace ptm 