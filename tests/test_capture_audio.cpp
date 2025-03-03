#include "audio/AudioInput.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "Audio Capture Test" << std::endl;
    
    unsigned int bufferSize = 512;
    if (argc > 1) {
        bufferSize = static_cast<unsigned int>(std::stoi(argv[1]));
    }
    
    std::cout << "Using buffer size: " << bufferSize << std::endl;
    
    bool result = ptm::captureAudio(bufferSize);
    
    if (result) {
        std::cout << "Audio capture test PASSED" << std::endl;
        return 0;
    } else {
        std::cout << "Audio capture test FAILED" << std::endl;
        return 1;
    }
}