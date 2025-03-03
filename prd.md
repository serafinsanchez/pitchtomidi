Below is a sample PRD structure that not only outlines the project overview and features but also provides explicit requirements for each feature—including dependencies, module interactions, and even suggested variable names—to ensure there are no ambiguities for the engineering team.

---

# Product Requirements Document (PRD)  
**Real-Time Monophonic Pitch-to-MIDI Conversion Application**

---

## 1. Project Overview

### 1.1. Objective  
Develop a real-time C++ application that converts monophonic audio (e.g., a single instrument or voice) into MIDI data. The system must offer high accuracy, low latency (under 20 ms total from audio input to MIDI output), and a user-friendly interface for musicians.

### 1.2. Key Goals  
- **Accuracy:** Precisely map audio pitches to MIDI notes (C1 to C8) using reliable pitch detection algorithms.
- **Low Latency:** Ensure real-time operation, processing input and output within 20 milliseconds.
- **Usability:** Provide an intuitive GUI for device selection, configuration, and live feedback.

### 1.3. High-Level Architecture  
- **Audio Input Module:** Captures audio using PortAudio.
- **Signal Processing Module:**  
  - **Pitch Detection:** Uses algorithms (e.g., YIN, FFT-based, or autocorrelation) to determine the fundamental frequency.
  - **Amplitude Detection:** Computes RMS/peak values to map amplitude to MIDI velocity.
  - **Event Detection:** Determines note-on/note-off events, and in continuous mode, pitch bend events.
- **MIDI Output Module:** Transmits MIDI messages via RtMidi.
- **GUI Module:** Developed in Qt or JUCE to allow configuration and display real-time data.
- **Configuration Module:** Manages persistent settings (e.g., via JSON or Boost.Program_options).

---

## 2. Features and Detailed Requirements

### 2.1. Audio Input Selection  
- **Description:**  
  - Enumerate available audio devices at startup using PortAudio.
  - Allow users to select a device via a dropdown in the GUI.
- **Functional Requirements:**  
  - **Device Enumeration:**  
    - *Variable:* `audioDeviceId` (unique identifier for each device)
    - *Variable:* `sampleRate` (e.g., 44.1 kHz minimum)
  - **Buffering:**  
    - *Variable:* `bufferSize` (default: 256–512 samples; configurable)
  - **Error Handling:**  
    - Display error if no device is found or if device initialization fails.
- **Dependencies:**  
  - **PortAudio**

---

### 2.2. Pitch Detection  
- **Description:**  
  - Detect the fundamental frequency from the monophonic audio input.
- **Functional Requirements:**  
  - **Algorithm:**  
    - Use YIN, autocorrelation, or FFT-based approach.
    - *Variables:*  
      - `windowSize` (e.g., 1024 samples by default)  
      - `hopSize` (e.g., 256 samples by default)  
      - `frequencyDetected` (floating-point value in Hz)
    - **Note Mapping:**  
      - Use formula:  
        \( \texttt{midiNote} = 69 + 12 \times \log_2(\texttt{frequencyDetected} / 440) \)
      - Round to the nearest integer.
      - *Variable:* `midiNote` (integer value)
  - **Hysteresis/Debounce Logic:**  
    - Prevent spurious note changes due to minor fluctuations.
    - *Variable:* `debounceThreshold` (e.g., 20 ms)
- **Dependencies:**  
  - **Optional:** FFT library (e.g., FFTW or KissFFT) if using FFT-based detection.

---

### 2.3. Amplitude Detection  
- **Description:**  
  - Compute the amplitude of the audio signal to map to MIDI velocity.
- **Functional Requirements:**  
  - **Measurement:**  
    - Use RMS or peak amplitude calculation.
    - *Variable:* `currentAmplitude` (float value)
  - **Mapping:**  
    - Map `currentAmplitude` to a MIDI velocity (0–127).
    - *Variable:* `midiVelocity` (integer value)
    - The mapping function should be configurable (linear or logarithmic).
- **Dependencies:**  
  - Standard math libraries available in C++.

---

### 2.4. Note Event Detection  
- **Description:**  
  - Detect note onsets and offsets based on amplitude thresholds and pitch changes.
- **Functional Requirements:**  
  - **Note-On/Note-Off:**  
    - Detect a note-on when `currentAmplitude` exceeds a threshold.
      - *Variable:* `amplitudeThreshold` (configurable value)
    - Detect note-off when `currentAmplitude` falls below the threshold or a new note is reliably detected.
  - **Debounce and Smoothing:**  
    - Implement temporal smoothing to filter out transient noise.
    - *Variable:* `smoothingWindow` (time in ms or number of samples)
- **Dependencies:**  
  - Relies on values from Pitch Detection and Amplitude Detection modules.

---

### 2.5. MIDI Output  
- **Description:**  
  - Generate and transmit MIDI messages using RtMidi.
- **Functional Requirements:**  
  - **MIDI Message Generation:**  
    - **Note-On Message:**  
      - Use `midiNote` and `midiVelocity` to generate a message.
      - *Variable:* `midiChannel` (range 1–16, configurable)
    - **Note-Off Message:**  
      - Sent when the note ceases.
    - **Pitch Bend (Continuous Mode):**  
      - Continuously send pitch bend messages with a configurable range (default ±2 semitones).
      - *Variable:* `pitchBendValue` (14-bit value)
  - **Transmission:**  
    - Use RtMidi to send messages.
- **Dependencies:**  
  - **RtMidi**

---

### 2.6. Visual Feedback and GUI  
- **Description:**  
  - Provide a real-time display of the detected note, velocity, and pitch deviation.
- **Functional Requirements:**  
  - **Components:**  
    - **Input Device Dropdown:**  
      - Shows `audioDeviceId` and `sampleRate`.
    - **MIDI Output Dropdown:**  
      - List of available MIDI ports.
    - **Status Display:**  
      - Displays `currentNote` (e.g., "C4"), `midiVelocity`, and a pitch deviation indicator.
    - **Settings Panel:**  
      - Controls for `amplitudeThreshold`, `windowSize`, `hopSize`, `debounceThreshold`, `midiChannel`, and mode (discrete vs. continuous).
    - **Start/Stop Toggle Button:**  
      - To control the processing loop.
  - **Update Rate:**  
    - GUI should update at least 30 FPS.
- **Dependencies:**  
  - **Qt or JUCE**

---

### 2.7. Configuration and Persistence  
- **Description:**  
  - Allow users to adjust settings and save them for future sessions.
- **Functional Requirements:**  
  - **Settings Management:**  
    - Save configuration values to a file (e.g., JSON format).
    - *Variable:* `configFilePath`
  - **Loading/Resetting Settings:**  
    - Provide functions to load and reset settings at startup.
- **Dependencies:**  
  - **Optional:** JSON for Modern C++ or Boost.Program_options

---

## 3. Explicit Technical Dependencies & Naming Conventions

### 3.1. External Libraries and Frameworks  
- **Audio Input:** PortAudio  
- **MIDI Output:** RtMidi  
- **GUI:** Qt or JUCE  
- **Build System:** CMake  
- **DSP (Optional):** FFTW or KissFFT  
- **Logging:** spdlog (or similar)  
- **Testing:** GoogleTest or Catch2  
- **Configuration Management (Optional):** JSON for Modern C++ or Boost.Program_options

### 3.2. Coding and Variable Naming Conventions  
- **General:**  
  - Use modern C++ (C++17 or later).  
  - Follow CamelCase or snake_case consistently (e.g., `audioDeviceId` or `audio_device_id`).
- **Suggested Variables:**  
  - `audioDeviceId` – Unique identifier for the audio device.  
  - `sampleRate` – Audio capture sample rate.  
  - `bufferSize` – Number of samples per audio buffer.  
  - `windowSize` – Size of the analysis window for pitch detection.  
  - `hopSize` – Step size between consecutive analysis windows.  
  - `frequencyDetected` – Floating-point variable storing the detected frequency in Hz.  
  - `midiNote` – Mapped MIDI note number (integer).  
  - `currentAmplitude` – RMS or peak amplitude of the current audio frame.  
  - `midiVelocity` – Calculated MIDI velocity (integer 0–127).  
  - `amplitudeThreshold` – Threshold value for triggering note events.  
  - `debounceThreshold` – Time/samples for hysteresis in note detection.  
  - `pitchBendValue` – 14-bit value for pitch bend messages.  
  - `midiChannel` – Configurable MIDI channel (1–16).  
  - `configFilePath` – Path to the configuration file for persistence.

---

## 4. Implementation Roadmap

### 4.1. Module Breakdown  
1. **Audio Input Module:**  
   - Interface with PortAudio, manage device enumeration, capture audio buffers.
2. **Signal Processing Module:**  
   - Pitch detection (with configurable parameters).  
   - Amplitude calculation.  
   - Note event detection (note-on/note-off and pitch bend).
3. **MIDI Output Module:**  
   - Use RtMidi to transmit MIDI messages.
4. **GUI Module:**  
   - Develop with Qt/JUCE to display device lists, real-time feedback, and settings.
5. **Configuration Module:**  
   - Read/write settings from/to a configuration file.

### 4.2. Inter-Module Communication  
- Use lock-free queues or thread-safe buffers to transfer audio data from the real-time processing thread to the GUI thread.
- Maintain clear API boundaries with explicit function names (e.g., `captureAudio()`, `processAudioFrame()`, `sendMidiMessage()`, etc.).

### 4.3. Testing and Calibration  
- **Unit Tests:**  
  - Validate pitch detection accuracy, amplitude mapping, and MIDI message formatting using frameworks such as GoogleTest.
- **Performance Benchmarks:**  
  - Test the complete pipeline to ensure the latency remains under 20 ms.
- **Calibration Tools:**  
  - Provide a mode to calibrate the amplitude-to-velocity mapping and pitch detection thresholds.

---

## 5. Acceptance Criteria  
- **Accuracy:**  
  - The application must correctly map input frequencies to MIDI notes (C1 to C8) with minimal error.
- **Latency:**  
  - Achieve less than 20 ms delay from audio capture to MIDI output on target hardware.
- **Stability:**  
  - Continuous operation for at least 2 hours without crashes or significant performance degradation.
- **Usability:**  
  - The GUI must enable easy device selection, real-time monitoring, and configuration adjustments.
- **Code Quality:**  
  - Adhere to modern C++ practices, ensure thread safety, and provide comprehensive documentation and unit tests.

---

This structured PRD—with its explicit sections on project overview, features, detailed requirements, dependencies, and even variable naming conventions—should leave no ambiguity for the engineering team as they move forward with implementation.