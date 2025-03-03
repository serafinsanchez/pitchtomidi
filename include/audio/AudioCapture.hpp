#pragma once

#include <portaudio.h>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <stdexcept>
#include <set>
#include <atomic>
#include <chrono>
#include "audio/RingBuffer.hpp"

namespace ptm {

class AudioCaptureException : public std::runtime_error {
public:
    explicit AudioCaptureException(const std::string& message) 
        : std::runtime_error(message) {}
};

struct AudioDevice {
    PaDeviceIndex index;
    std::string name;
    std::string hostApi;           // Added: Host API name
    double defaultSampleRate;
    std::set<double> sampleRates;  // Added: Supported sample rates
    int maxInputChannels;
    double defaultLatency;
    double minLatency;
    bool isDefaultInput;           // Added: Is this the default input device?

    // Added: Helper method to check sample rate support
    bool supportsSampleRate(double rate) const {
        // If no specific rates are listed, assume default rate only
        if (sampleRates.empty()) {
            return rate == defaultSampleRate;
        }
        return sampleRates.find(rate) != sampleRates.end();
    }
};

// Added: Callback type for device list changes
using DeviceListCallback = std::function<void(const std::vector<AudioDevice>&)>;

// Forward declare StreamStats (defined in cpp file)
struct StreamStats;

class AudioCapture {
public:
    static constexpr size_t kDefaultBufferSize = 8192; // Ring buffer size in samples
    static constexpr std::chrono::milliseconds kShutdownTimeout{1000}; // 1 second timeout

    enum class StreamState {
        Closed,
        Opening,
        Running,
        Stopping,
        Error
    };

    AudioCapture();
    ~AudioCapture();

    // Prevent copying
    AudioCapture(const AudioCapture&) = delete;
    AudioCapture& operator=(const AudioCapture&) = delete;

    // Device management
    std::vector<AudioDevice> enumerateDevices();
    void setDevice(PaDeviceIndex deviceIndex);
    
    // Added: Get current device info
    const AudioDevice* getCurrentDevice() const;
    
    // Added: Default device handling
    PaDeviceIndex getDefaultInputDevice() const;
    
    // Added: Device monitoring
    void setDeviceChangeCallback(DeviceListCallback callback);
    
    // Stream control
    void start(double sampleRate = 44100.0,
              unsigned int framesPerBuffer = 256,
              std::function<void(const float*, unsigned long)> callback = nullptr);
    void stop();
    bool isActive() const;
    StreamState getState() const { return streamState_.load(); }
    
    // Stream health monitoring
    bool isStreamHealthy() const;
    std::string getLastError() const;

    // Added: Sample rate validation
    bool isValidSampleRate(double sampleRate) const;
    std::vector<double> getSupportedSampleRates() const;

    // Add new method to get stream statistics
    StreamStats getStreamStats() const;

    // Audio data access
    size_t getAudioData(float* buffer, size_t count);
    size_t getAvailableSamples() const;
    void clearAudioBuffer();

private:
    static int paCallback(const void* inputBuffer, void* outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void* userData);

    // Added: Device monitoring implementation
    void checkDeviceChanges();
    std::vector<AudioDevice> lastKnownDevices_;
    DeviceListCallback deviceChangeCallback_;
    
    PaStream* stream_;
    PaDeviceIndex currentDevice_;
    std::function<void(const float*, unsigned long)> userCallback_;
    bool isInitialized_;
    AudioDevice currentDeviceInfo_;  // Added: Cache current device info
    std::unique_ptr<StreamStats> streamStats_;  // Add stream statistics member
    RingBuffer<float> audioBuffer_;

    // Enhanced stream management
    bool waitForState(StreamState expectedState, 
                     std::chrono::milliseconds timeout = kShutdownTimeout) const;
    void setState(StreamState newState);
    bool shutdownStream();  // Returns true if shutdown was successful
    
    std::atomic<StreamState> streamState_{StreamState::Closed};
    std::string lastError_;
    std::atomic<bool> shutdownRequested_{false};
}; 