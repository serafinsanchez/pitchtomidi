#include "audio/AudioCapture.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <chrono>
#include <thread>

namespace ptm {

namespace {
    // Common sample rates to test for support
    const std::vector<double> kCommonSampleRates = {
        8000.0, 11025.0, 16000.0, 22050.0, 32000.0, 44100.0, 48000.0, 88200.0, 96000.0
    };

    constexpr double kMaxAllowedLatency = 0.020;  // 20ms maximum latency
    constexpr unsigned int kMinBufferSize = 64;    // Minimum safe buffer size
    constexpr unsigned int kMaxBufferSize = 2048;  // Maximum safe buffer size
    
    // Struct to track stream performance
    struct StreamStats {
        std::atomic<double> currentLatency{0.0};
        std::atomic<uint32_t> underruns{0};
        std::atomic<uint32_t> overruns{0};
    };

    // Helper function to test if a sample rate is supported
    bool testSampleRate(PaStreamParameters* inputParams, double sampleRate) {
        return Pa_IsFormatSupported(inputParams, nullptr, sampleRate) == paFormatIsSupported;
    }

    // Helper function to get supported sample rates for a device
    std::set<double> getSupportedRates(const PaDeviceInfo* deviceInfo, PaDeviceIndex deviceIndex) {
        std::set<double> supported;
        
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
}

AudioCapture::AudioCapture() 
    : stream_(nullptr)
    , currentDevice_(paNoDevice)
    , isInitialized_(false)
    , audioBuffer_(kDefaultBufferSize) {
    
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        setState(StreamState::Error);
        lastError_ = Pa_GetErrorText(err);
        throw AudioCaptureException(std::string("Failed to initialize PortAudio: ") + lastError_);
    }
    isInitialized_ = true;
    setState(StreamState::Closed);
    spdlog::info("PortAudio initialized successfully");

    // Cache initial device list
    lastKnownDevices_ = enumerateDevices();
}

AudioCapture::~AudioCapture() {
    if (isInitialized_) {
        if (stream_) {
            shutdownStream();
        }
        Pa_Terminate();
        spdlog::info("PortAudio terminated");
    }
}

std::vector<AudioDevice> AudioCapture::enumerateDevices() {
    std::vector<AudioDevice> devices;
    int numDevices = Pa_GetDeviceCount();
    
    if (numDevices < 0) {
        throw AudioCaptureException(std::string("Error getting device count: ") + 
                                  Pa_GetErrorText(numDevices));
    }

    PaDeviceIndex defaultInput = Pa_GetDefaultInputDevice();

    for (int i = 0; i < numDevices; i++) {
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo && deviceInfo->maxInputChannels > 0) {
            AudioDevice device;
            device.index = i;
            device.name = deviceInfo->name;
            device.hostApi = Pa_GetHostApiInfo(deviceInfo->hostApi)->name;
            device.defaultSampleRate = deviceInfo->defaultSampleRate;
            device.sampleRates = getSupportedRates(deviceInfo, i);
            device.maxInputChannels = deviceInfo->maxInputChannels;
            device.defaultLatency = deviceInfo->defaultLowInputLatency;
            device.minLatency = deviceInfo->defaultLowInputLatency;
            device.isDefaultInput = (i == defaultInput);
            
            devices.push_back(device);
            
            spdlog::debug("Found input device: {} ({})", device.name, device.hostApi);
            for (double rate : device.sampleRates) {
                spdlog::debug("  Supported rate: {} Hz", rate);
            }
        }
    }

    return devices;
}

void AudioCapture::checkDeviceChanges() {
    auto currentDevices = enumerateDevices();
    
    // Check if the device list has changed
    bool hasChanged = currentDevices.size() != lastKnownDevices_.size();
    if (!hasChanged) {
        for (size_t i = 0; i < currentDevices.size(); ++i) {
            if (currentDevices[i].index != lastKnownDevices_[i].index ||
                currentDevices[i].name != lastKnownDevices_[i].name) {
                hasChanged = true;
                break;
            }
        }
    }

    if (hasChanged) {
        spdlog::info("Audio device list has changed");
        lastKnownDevices_ = currentDevices;
        
        // Check if current device is still available
        if (currentDevice_ != paNoDevice) {
            bool deviceStillExists = false;
            for (const auto& device : currentDevices) {
                if (device.index == currentDevice_) {
                    deviceStillExists = true;
                    break;
                }
            }
            
            if (!deviceStillExists && stream_) {
                spdlog::warn("Current audio device has been disconnected");
                stop();  // Stop the stream as the device is no longer available
                currentDevice_ = paNoDevice;
            }
        }

        // Notify callback if registered
        if (deviceChangeCallback_) {
            deviceChangeCallback_(currentDevices);
        }
    }
}

void AudioCapture::setDeviceChangeCallback(DeviceListCallback callback) {
    deviceChangeCallback_ = std::move(callback);
}

PaDeviceIndex AudioCapture::getDefaultInputDevice() const {
    PaDeviceIndex defaultDevice = Pa_GetDefaultInputDevice();
    if (defaultDevice == paNoDevice) {
        throw AudioCaptureException("No default input device available");
    }
    return defaultDevice;
}

const AudioDevice* AudioCapture::getCurrentDevice() const {
    if (currentDevice_ == paNoDevice) {
        return nullptr;
    }
    return &currentDeviceInfo_;
}

bool AudioCapture::isValidSampleRate(double sampleRate) const {
    if (currentDevice_ == paNoDevice) {
        return false;
    }
    return currentDeviceInfo_.supportsSampleRate(sampleRate);
}

std::vector<double> AudioCapture::getSupportedSampleRates() const {
    if (currentDevice_ == paNoDevice) {
        return {};
    }
    return std::vector<double>(currentDeviceInfo_.sampleRates.begin(), 
                              currentDeviceInfo_.sampleRates.end());
}

void AudioCapture::setDevice(PaDeviceIndex deviceIndex) {
    if (stream_) {
        throw AudioCaptureException("Cannot change device while stream is active");
    }

    const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(deviceIndex);
    if (!deviceInfo) {
        throw AudioCaptureException("Invalid device index");
    }

    // Cache device information
    currentDeviceInfo_.index = deviceIndex;
    currentDeviceInfo_.name = deviceInfo->name;
    currentDeviceInfo_.hostApi = Pa_GetHostApiInfo(deviceInfo->hostApi)->name;
    currentDeviceInfo_.defaultSampleRate = deviceInfo->defaultSampleRate;
    currentDeviceInfo_.sampleRates = getSupportedRates(deviceInfo, deviceIndex);
    currentDeviceInfo_.maxInputChannels = deviceInfo->maxInputChannels;
    currentDeviceInfo_.defaultLatency = deviceInfo->defaultLowInputLatency;
    currentDeviceInfo_.minLatency = deviceInfo->defaultLowInputLatency;
    currentDeviceInfo_.isDefaultInput = (deviceIndex == Pa_GetDefaultInputDevice());

    currentDevice_ = deviceIndex;
    spdlog::info("Selected audio device: {} ({})", currentDeviceInfo_.name, 
                 currentDeviceInfo_.hostApi);
}

void AudioCapture::start(double sampleRate, unsigned int framesPerBuffer,
                        std::function<void(const float*, unsigned long)> callback) {
    if (currentDevice_ == paNoDevice) {
        setState(StreamState::Error);
        lastError_ = "No device selected";
        throw AudioCaptureException(lastError_);
    }

    if (stream_) {
        setState(StreamState::Error);
        lastError_ = "Stream already active";
        throw AudioCaptureException(lastError_);
    }

    setState(StreamState::Opening);

    try {
        // Validate sample rate
        if (!isValidSampleRate(sampleRate)) {
            throw AudioCaptureException("Unsupported sample rate: " + std::to_string(sampleRate));
        }

        // Validate buffer size
        if (framesPerBuffer < kMinBufferSize || framesPerBuffer > kMaxBufferSize) {
            throw AudioCaptureException("Invalid buffer size. Must be between " + 
                                      std::to_string(kMinBufferSize) + " and " + 
                                      std::to_string(kMaxBufferSize));
        }

        // Calculate expected latency
        double expectedLatency = static_cast<double>(framesPerBuffer) / sampleRate;
        if (expectedLatency > kMaxAllowedLatency) {
            spdlog::warn("Buffer size may introduce latency ({:.1f}ms) above target (20ms)", 
                        expectedLatency * 1000.0);
        }

        userCallback_ = callback;
        streamStats_ = std::make_unique<StreamStats>();

        PaStreamParameters inputParameters;
        inputParameters.device = currentDevice_;
        inputParameters.channelCount = 1;  // Mono input
        inputParameters.sampleFormat = paFloat32;
        inputParameters.suggestedLatency = std::min(currentDeviceInfo_.defaultLatency, 
                                                  kMaxAllowedLatency);
        inputParameters.hostApiSpecificStreamInfo = nullptr;

        PaError err = Pa_OpenStream(&stream_,
                                   &inputParameters,
                                   nullptr,  // No output
                                   sampleRate,
                                   framesPerBuffer,
                                   paClipOff | paDitherOff,
                                   AudioCapture::paCallback,
                                   this);

        if (err != paNoError) {
            stream_ = nullptr;
            streamStats_.reset();
            setState(StreamState::Error);
            lastError_ = Pa_GetErrorText(err);
            throw AudioCaptureException(std::string("Failed to open stream: ") + lastError_);
        }

        err = Pa_StartStream(stream_);
        if (err != paNoError) {
            Pa_CloseStream(stream_);
            stream_ = nullptr;
            streamStats_.reset();
            setState(StreamState::Error);
            lastError_ = Pa_GetErrorText(err);
            throw AudioCaptureException(std::string("Failed to start stream: ") + lastError_);
        }

        setState(StreamState::Running);
        // Log stream info
        const PaStreamInfo* streamInfo = Pa_GetStreamInfo(stream_);
        if (streamInfo) {
            spdlog::info("Audio stream started: {:.1f} Hz, {} frames/buffer, {:.1f}ms latency", 
                         streamInfo->sampleRate, framesPerBuffer, 
                         streamInfo->inputLatency * 1000.0);
        } else {
            spdlog::info("Audio stream started: {:.1f} Hz, {} frames/buffer", 
                         sampleRate, framesPerBuffer);
        }
    } catch (...) {
        setState(StreamState::Error);
        throw;
    }
}

bool AudioCapture::shutdownStream() {
    if (!stream_) return true;

    shutdownRequested_ = true;
    setState(StreamState::Stopping);

    // Wait for any pending callbacks to complete
    if (!waitForState(StreamState::Stopping, kShutdownTimeout)) {
        spdlog::error("Timeout waiting for stream to stop");
        return false;
    }

    PaError err = Pa_StopStream(stream_);
    if (err != paNoError) {
        lastError_ = Pa_GetErrorText(err);
        spdlog::error("Error stopping stream: {}", lastError_);
        setState(StreamState::Error);
        return false;
    }

    err = Pa_CloseStream(stream_);
    if (err != paNoError) {
        lastError_ = Pa_GetErrorText(err);
        spdlog::error("Error closing stream: {}", lastError_);
        setState(StreamState::Error);
        return false;
    }

    stream_ = nullptr;
    clearAudioBuffer();
    setState(StreamState::Closed);
    shutdownRequested_ = false;
    spdlog::info("Audio stream stopped and closed");
    return true;
}

void AudioCapture::stop() {
    if (!stream_) return;
    
    if (!shutdownStream()) {
        // If graceful shutdown fails, force cleanup
        if (stream_) {
            Pa_AbortStream(stream_);
            Pa_CloseStream(stream_);
            stream_ = nullptr;
            clearAudioBuffer();
            setState(StreamState::Closed);
            spdlog::warn("Forced stream shutdown after graceful shutdown failed");
        }
    }
}

bool AudioCapture::isStreamHealthy() const {
    if (!stream_) return false;
    
    StreamState state = streamState_.load();
    if (state == StreamState::Error) return false;
    if (state != StreamState::Running) return true; // Not an error if not running
    
    // Check if stream is actually active
    if (Pa_IsStreamActive(stream_) != 1) return false;
    
    // Check performance metrics
    if (streamStats_) {
        if (streamStats_->currentLatency.load() > kMaxAllowedLatency) return false;
        if (streamStats_->overruns.load() > 10) return false;  // Too many overruns
        if (streamStats_->underruns.load() > 10) return false; // Too many underruns
    }
    
    return true;
}

void AudioCapture::setState(StreamState newState) {
    streamState_.store(newState);
}

bool AudioCapture::waitForState(StreamState expectedState, 
                              std::chrono::milliseconds timeout) const {
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start < timeout) {
        if (streamState_.load() == expectedState) {
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return false;
}

bool AudioCapture::isActive() const {
    return stream_ != nullptr && 
           streamState_.load() == StreamState::Running &&
           Pa_IsStreamActive(stream_) == 1;
}

int AudioCapture::paCallback(const void* inputBuffer, void* outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void* userData) {
    (void)outputBuffer;  // Unused
    
    auto* instance = static_cast<AudioCapture*>(userData);
    
    // Check for shutdown request
    if (instance->shutdownRequested_.load()) {
        return paComplete;
    }

    const float* input = static_cast<const float*>(inputBuffer);

    // Performance monitoring
    auto startTime = std::chrono::high_resolution_clock::now();

    // Update performance metrics
    if (instance->streamStats_) {
        // Calculate actual latency
        if (timeInfo) {
            double currentLatency = timeInfo->currentTime - timeInfo->inputBufferAdcTime;
            instance->streamStats_->currentLatency.store(currentLatency);
            
            // Log high latency events with more detail
            if (currentLatency > kMaxAllowedLatency) {
                spdlog::warn("High latency detected: {:.1f}ms (ADC: {:.6f}, Current: {:.6f})", 
                            currentLatency * 1000.0,
                            timeInfo->inputBufferAdcTime,
                            timeInfo->currentTime);
            }
        }

        // Enhanced buffer under/overrun logging
        if (statusFlags & paInputUnderflow) {
            instance->streamStats_->underruns++;
            spdlog::warn("Audio input underrun #{} detected", 
                        instance->streamStats_->underruns.load());
        }
        if (statusFlags & paInputOverflow) {
            instance->streamStats_->overruns++;
            spdlog::warn("Audio input overflow #{} detected", 
                        instance->streamStats_->overruns.load());
        }

        // Log other status flags for debugging
        if (statusFlags & paOutputUnderflow) spdlog::debug("Output underflow");
        if (statusFlags & paOutputOverflow) spdlog::debug("Output overflow");
        if (statusFlags & paPrimingOutput) spdlog::debug("Priming output");
    }

    // Write audio data to ring buffer
    if (input) {
        size_t written = instance->audioBuffer_.write(input, framesPerBuffer);
        if (written < framesPerBuffer) {
            spdlog::warn("Ring buffer overflow - dropped {} samples", framesPerBuffer - written);
            instance->streamStats_->overruns++;
        }

        // Call user callback if provided
        if (instance->userCallback_) {
            instance->userCallback_(input, framesPerBuffer);
        }
        
        // Monitor callback execution time
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        
        // Log if processing takes too long
        if (duration.count() > 1000) { // More than 1ms
            spdlog::warn("Audio callback processing took {:.2f}ms", duration.count() / 1000.0);
        }
    } else {
        spdlog::error("Null input buffer in audio callback");
        instance->setState(StreamState::Error);
        instance->lastError_ = "Null input buffer in audio callback";
        return paAbort;
    }

    return paContinue;
}

StreamStats AudioCapture::getStreamStats() const {
    if (!streamStats_) {
        return StreamStats{};
    }
    return *streamStats_;
}

// New methods for audio data access
size_t AudioCapture::getAudioData(float* buffer, size_t count) {
    return audioBuffer_.read(buffer, count);
}

size_t AudioCapture::getAvailableSamples() const {
    return audioBuffer_.available();
}

void AudioCapture::clearAudioBuffer() {
    audioBuffer_.clear();
}

std::string AudioCapture::getLastError() const {
    return lastError_;
}
} 