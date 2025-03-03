Below is a detailed, step-by-step plan to build the Real-Time Monophonic Pitch-to-MIDI Conversion Application. This plan breaks down the work into manageable phases, from setting up the environment to final integration and testing.

---

### 1. **Project Setup and Planning**

1. **Kickoff & Requirement Review:**
   - Gather the engineering team and review the PRD in detail.
   - Ensure all team members understand the features, dependencies, and coding standards.
   
2. **Environment Setup:**
   - Set up version control (e.g., Git) and project management tools.
   - Prepare the development environment for cross-platform builds (Windows and macOS).
   
3. **Build System Configuration:**
   - Create a CMake-based project structure.
   - Define project directories (e.g., `src/`, `include/`, `tests/`, `docs/`).
   - Add configuration files for dependencies and set up continuous integration (CI) pipelines.

---

### 2. **Dependency Integration**

1. **Audio and MIDI Libraries:**
   - Integrate **PortAudio** for audio capture.
   - Integrate **RtMidi** for MIDI output.
   - Validate that both libraries compile and work on the target platforms.
   
2. **GUI Framework:**
   - Choose and set up either **Qt** or **JUCE** for the GUI.
   - Create a basic window with menus to verify the framework setup.
   
3. **Optional DSP & FFT Libraries:**
   - Integrate **FFTW** or **KissFFT** if you plan on using FFT-based pitch detection.
   
4. **Additional Tools:**
   - Set up **spdlog** for logging.
   - Integrate **GoogleTest** or **Catch2** for unit tests.
   - Optionally, integrate a configuration library (e.g., JSON for Modern C++).

---

### 3. **Module Development**

#### 3.1. **Audio Input Module**

1. **Device Enumeration:**
   - Implement a function `enumerateAudioDevices()` using PortAudio.
   - Store devices in a structure using variables like `audioDeviceId` and `sampleRate`.
   
2. **Audio Capture:**
   - Write `captureAudio()` to initialize the selected device with a configurable `bufferSize` (default 256–512 samples).
   - Validate real-time audio capture on test signals.

3. **Error Handling:**
   - Implement error messages for missing or failed devices.
   - Log errors using spdlog.

---

#### 3.2. **Signal Processing Module**

1. **Pitch Detection:**
   - Implement a module `processAudioFrame()` that performs pitch detection.
   - Use the chosen algorithm (YIN, autocorrelation, or FFT-based) with configurable parameters:
     - `windowSize` (default 1024 samples)
     - `hopSize` (default 256 samples)
   - Map detected frequency (`frequencyDetected`) to MIDI note (`midiNote`) using:
     \[
     \texttt{midiNote} = 69 + 12 \times \log_2(\texttt{frequencyDetected}/440)
     \]
   - Add hysteresis/debounce logic using `debounceThreshold`.

2. **Amplitude Detection:**
   - Implement amplitude calculation (RMS or peak) in the same processing loop.
   - Map `currentAmplitude` to a MIDI velocity (`midiVelocity`).
   - Ensure the mapping is configurable (e.g., linear or logarithmic).

3. **Note Event Detection:**
   - Develop logic to detect note-on and note-off events based on:
     - An `amplitudeThreshold` for triggering events.
     - A `smoothingWindow` to filter transient noise.
   - Provide clear transitions between note events.

---

#### 3.3. **MIDI Output Module**

1. **MIDI Message Generation:**
   - Implement functions `sendNoteOn()`, `sendNoteOff()`, and (if in continuous mode) `sendPitchBend()`.
   - Use variables:
     - `midiChannel` (1–16)
     - `pitchBendValue` (14-bit)
   
2. **Transmission:**
   - Use RtMidi for sending the generated MIDI messages.
   - Test the MIDI output using virtual MIDI tools or connected hardware.

---

#### 3.4. **GUI Module**

1. **Device Selection Panels:**
   - Create dropdown menus for audio input (showing `audioDeviceId`, `sampleRate`) and MIDI output ports.
   
2. **Status Display:**
   - Implement a real-time display showing:
     - Detected note (`currentNote`, e.g., "C4")
     - Velocity (`midiVelocity`)
     - Pitch deviation indicators.
   
3. **Settings Panel:**
   - Create controls (sliders/numeric fields) for:
     - `amplitudeThreshold`
     - `windowSize` and `hopSize`
     - `debounceThreshold`
     - `midiChannel`
     - Mode selection (discrete vs. continuous)
   - Add a Start/Stop toggle button to control processing.

4. **Real-Time Update:**
   - Ensure the GUI updates at least 30 FPS.
   - Use lock-free queues or thread-safe buffers to communicate between the audio thread and the GUI thread.

---

#### 3.5. **Configuration and Persistence Module**

1. **Settings Management:**
   - Implement load and save functions to read/write configuration files (using JSON or Boost.Program_options).
   - Use a variable like `configFilePath` to store the location.
   
2. **User Preferences:**
   - Persist settings such as selected audio device, MIDI channel, thresholds, and mode between sessions.

---

### 4. **Integration and Inter-Module Communication**

1. **Threading & Communication:**
   - Ensure that audio processing runs on a high-priority thread.
   - Use lock-free queues to transfer note events and amplitude values from the audio thread to the GUI thread.
   
2. **Module APIs:**
   - Define clear function names and API boundaries (e.g., `captureAudio()`, `processAudioFrame()`, `sendMidiMessage()`).
   - Document these APIs in the developer guide.

3. **Timing & Synchronization:**
   - Integrate high-resolution timers to measure latency and ensure timely dispatch of MIDI messages.

---

### 5. **Testing and Validation**

1. **Unit Testing:**
   - Write tests for each module using GoogleTest or Catch2:
     - Verify pitch detection accuracy.
     - Validate amplitude-to-velocity mapping.
     - Confirm correct MIDI message formatting.
   
2. **Integration Testing:**
   - Test the complete pipeline (audio capture → signal processing → MIDI output).
   - Use simulated input signals to measure latency (target <20 ms).
   
3. **Performance Testing:**
   - Benchmark the application under load to ensure it meets real-time requirements.
   
4. **Cross-Platform Testing:**
   - Validate functionality on Windows 10 and macOS 10.14 or later.
   - Test with both hardware and virtual MIDI devices.

---

### 6. **Documentation and Finalization**

1. **Code Documentation:**
   - Add inline comments and developer documentation.
   - Document all module interfaces and configuration options.
   
2. **User Documentation:**
   - Create a user manual for setting up audio/MIDI devices, calibrating settings, and using the GUI.
   
3. **Release Preparation:**
   - Perform final integration tests and bug fixes.
   - Package the application with an installer or executable for each supported platform.
   - Set up deployment and versioning protocols.

---

### 7. **Project Milestones and Timeline**

- **Week 1–2:** Environment setup, dependency integration, and project structure establishment.
- **Week 3–4:** Develop Audio Input Module and basic GUI window.
- **Week 5–6:** Implement Signal Processing (pitch and amplitude detection) and note event detection.
- **Week 7:** Integrate MIDI Output Module and test message transmission.
- **Week 8:** Expand GUI to include full settings and real-time feedback.
- **Week 9:** Implement configuration/persistence module.
- **Week 10:** Integration testing, performance benchmarking, and cross-platform validation.
- **Week 11–12:** Documentation, bug fixes, and final release preparation.

---

Following this step-by-step plan will help ensure that each module is built, tested, and integrated in a clear and organized manner, leaving minimal ambiguity for the engineering team.