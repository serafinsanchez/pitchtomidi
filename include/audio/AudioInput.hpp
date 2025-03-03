#pragma once

#include <portaudio.h>
#include <string>
#include <vector>
#include <set>
#include <functional>
#include <stdexcept>

namespace ptm {

/**
 * Exception class for audio input errors
 */
class AudioInputException : public std::runtime_error {
public:
    explicit AudioInputException(const std::string& message) 
        : std::runtime_error(message) {}
};

/**
 * Structure to hold audio device information
 */
struct AudioDeviceInfo {
    int audioDeviceId;              // Device index
    std::string name;               // Device name
    std::string hostApi;            // Host API name
    double sampleRate;              // Default sample rate
    std::set<double> sampleRates;   // Supported sample rates
    int maxInputChannels;           // Maximum input channels
    double defaultLatency;          // Default latency
    double minLatency;              // Minimum latency
    bool isDefaultInput;            // Is this the default input device?

    // Helper method to check sample rate support
    bool supportsSampleRate(double rate) const {
        // If no specific rates are listed, assume default rate only
        if (sampleRates.empty()) {
            return rate == sampleRate;
        }
        return sampleRates.find(rate) != sampleRates.end();
    }
};

/**
 * Callback type for device list changes
 */
using DeviceChangeCallback = std::function<void(const std::vector<AudioDeviceInfo>&)>;

/**
 * Enumerates available audio input devices using PortAudio
 * 
 * @return Vector of AudioDeviceInfo structures containing device information
 * @throws AudioInputException if there's an error enumerating devices
 */
std::vector<AudioDeviceInfo> enumerateAudioDevices();

/**
 * Gets the default audio input device
 * 
 * @return The default audio input device ID or -1 if no default device
 */
int getDefaultAudioInputDevice();

/**
 * Checks if a sample rate is supported by a device
 * 
 * @param deviceId The device ID to check
 * @param sampleRate The sample rate to check
 * @return true if the sample rate is supported, false otherwise
 */
bool isSampleRateSupported(int deviceId, double sampleRate);

/**
 * Gets the list of supported sample rates for a device
 * 
 * @param deviceId The device ID to check
 * @return Set of supported sample rates
 */
std::set<double> getSupportedSampleRates(int deviceId);

/**
 * Captures audio from the default input device for a short period
 * 
 * @param bufferSize The size of the buffer to use for audio capture
 * @return true if audio was successfully captured, false otherwise
 */
bool captureAudio(unsigned int bufferSize = 512);

} // namespace ptm