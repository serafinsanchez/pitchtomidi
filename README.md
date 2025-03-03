# PitchToMidi

Real-time monophonic pitch to MIDI conversion application.

## Prerequisites

- CMake 3.15 or higher
- C++17 compatible compiler
- Dependencies:
  - PortAudio
  - RtMidi
  - Qt6
  - FFTW3 (optional)
  - spdlog
  - nlohmann/json
  - GoogleTest (for testing)

## Building

```bash
# Clone the repository
git clone https://github.com/yourusername/pitchtomidi.git
cd pitchtomidi

# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
cmake --build .

# Run tests (Debug build only)
ctest --output-on-failure
```

## Project Structure

```
.
├── CMakeLists.txt
├── cmake/
│   └── Dependencies.cmake
├── include/
│   └── # Header files
├── src/
│   ├── CMakeLists.txt
│   └── # Source files
├── tests/
│   ├── CMakeLists.txt
│   └── # Test files
└── docs/
    └── # Documentation
```

## License

[Your chosen license]

## Contributing

[Your contribution guidelines] 