#include <iostream>
#include <portaudio.h>
#include "RtMidi.h"
#include <thread>
#include <chrono>

int main() {
    bool success = true;
    
    // Test PortAudio
    std::cout << "Testing PortAudio..." << std::endl;
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio initialization failed: " << Pa_GetErrorText(err) << std::endl;
        success = false;
    } else {
        std::cout << "PortAudio initialized successfully" << std::endl;
        
        // List audio devices
        int numDevices = Pa_GetDeviceCount();
        std::cout << "Number of audio devices: " << numDevices << std::endl;
        
        if (numDevices < 0) {
            std::cerr << "Error getting device count!" << std::endl;
            success = false;
        } else {
            for (int i = 0; i < numDevices; i++) {
                const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
                if (deviceInfo != nullptr) {
                    std::cout << "Device " << i << ": " << deviceInfo->name 
                              << " (Input channels: " << deviceInfo->maxInputChannels
                              << ", Output channels: " << deviceInfo->maxOutputChannels << ")" 
                              << std::endl;
                }
            }
        }
        
        Pa_Terminate();
    }
    
    // Test RtMidi
    std::cout << "\nTesting RtMidi..." << std::endl;
    try {
        // Test MIDI Out
        RtMidiOut midiout;
        std::cout << "Number of MIDI output ports: " << midiout.getPortCount() << std::endl;
        
        for (unsigned int i = 0; i < midiout.getPortCount(); i++) {
            std::cout << "MIDI Output Port " << i << ": " << midiout.getPortName(i) << std::endl;
        }
        
        // Test MIDI In
        RtMidiIn midiin;
        std::cout << "Number of MIDI input ports: " << midiin.getPortCount() << std::endl;
        
        for (unsigned int i = 0; i < midiin.getPortCount(); i++) {
            std::cout << "MIDI Input Port " << i << ": " << midiin.getPortName(i) << std::endl;
        }
        
    } catch (RtMidiError &error) {
        std::cerr << "RtMidi error: " << error.getMessage() << std::endl;
        success = false;
    }
    
    if (success) {
        std::cout << "\nAll tests completed successfully!" << std::endl;
        return 0;
    } else {
        std::cerr << "\nSome tests failed!" << std::endl;
        return 1;
    }
} 