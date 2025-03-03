#include "MainComponent.h"

MainComponent::MainComponent()
{
    // Initialize audio device selector
    deviceManager = std::make_unique<juce::AudioDeviceManager>();
    deviceManager->initialise(0, 2, nullptr, true);
    
    audioDeviceSelector = std::make_unique<juce::AudioDeviceSelectorComponent>(
        *deviceManager,
        0,  // minAudioInputChannels
        2,  // maxAudioInputChannels
        0,  // minAudioOutputChannels
        2,  // maxAudioOutputChannels
        false,  // showMidiInputOptions
        false,  // showMidiOutputSelector
        false,  // showChannelsAsStereoPairs
        false   // hideAdvancedOptionsWithButton
    );
    addAndMakeVisible(audioDeviceSelector.get());

    // Initialize MIDI output selector
    addAndMakeVisible(midiOutputLabel);
    midiOutputLabel.setText("MIDI Output:", juce::dontSendNotification);
    addAndMakeVisible(midiOutputSelector);
    // TODO: Populate MIDI outputs

    // Initialize status display
    addAndMakeVisible(currentNoteLabel);
    currentNoteLabel.setText("Current Note: --", juce::dontSendNotification);
    
    addAndMakeVisible(velocityLabel);
    velocityLabel.setText("Velocity: 0", juce::dontSendNotification);
    
    addAndMakeVisible(pitchDeviationLabel);
    pitchDeviationLabel.setText("Pitch Deviation: 0 cents", juce::dontSendNotification);

    // Initialize settings controls
    addAndMakeVisible(amplitudeThresholdLabel);
    amplitudeThresholdLabel.setText("Amplitude Threshold:", juce::dontSendNotification);
    addAndMakeVisible(amplitudeThresholdSlider);
    amplitudeThresholdSlider.setRange(0.0, 1.0);
    amplitudeThresholdSlider.setValue(0.1);
    
    addAndMakeVisible(windowSizeLabel);
    windowSizeLabel.setText("Window Size:", juce::dontSendNotification);
    addAndMakeVisible(windowSizeSlider);
    windowSizeSlider.setRange(256, 4096, 256);
    windowSizeSlider.setValue(1024);
    
    addAndMakeVisible(hopSizeLabel);
    hopSizeLabel.setText("Hop Size:", juce::dontSendNotification);
    addAndMakeVisible(hopSizeSlider);
    hopSizeSlider.setRange(64, 1024, 64);
    hopSizeSlider.setValue(256);
    
    addAndMakeVisible(debounceThresholdLabel);
    debounceThresholdLabel.setText("Debounce (ms):", juce::dontSendNotification);
    addAndMakeVisible(debounceThresholdSlider);
    debounceThresholdSlider.setRange(0, 100);
    debounceThresholdSlider.setValue(20);
    
    addAndMakeVisible(midiChannelLabel);
    midiChannelLabel.setText("MIDI Channel:", juce::dontSendNotification);
    addAndMakeVisible(midiChannelSelector);
    for (int i = 1; i <= 16; ++i)
        midiChannelSelector.addItem(juce::String(i), i);
    midiChannelSelector.setSelectedId(1);
    
    addAndMakeVisible(discreteModeButton);
    discreteModeButton.setButtonText("Discrete Mode");
    discreteModeButton.setToggleState(true, juce::dontSendNotification);
    
    addAndMakeVisible(startStopButton);
    startStopButton.setButtonText("Start");
    
    setSize(800, 600);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced(20);
    
    // Audio device selector takes up top portion
    audioDeviceSelector->setBounds(area.removeFromTop(200));
    area.removeFromTop(20); // spacing
    
    // MIDI output selector
    auto midiRow = area.removeFromTop(30);
    midiOutputLabel.setBounds(midiRow.removeFromLeft(100));
    midiOutputSelector.setBounds(midiRow.removeFromLeft(200));
    area.removeFromTop(20);
    
    // Status display
    currentNoteLabel.setBounds(area.removeFromTop(30));
    velocityLabel.setBounds(area.removeFromTop(30));
    pitchDeviationLabel.setBounds(area.removeFromTop(30));
    area.removeFromTop(20);
    
    // Settings controls - arrange in two columns
    auto leftColumn = area.removeFromLeft(area.getWidth() / 2).reduced(10);
    auto rightColumn = area.reduced(10);
    
    // Left column
    auto setupSliderRow = [](juce::Label& label, juce::Slider& slider, juce::Rectangle<int>& bounds)
    {
        auto row = bounds.removeFromTop(50);
        label.setBounds(row.removeFromTop(20));
        slider.setBounds(row);
    };
    
    setupSliderRow(amplitudeThresholdLabel, amplitudeThresholdSlider, leftColumn);
    setupSliderRow(windowSizeLabel, windowSizeSlider, leftColumn);
    setupSliderRow(hopSizeLabel, hopSizeSlider, leftColumn);
    
    // Right column
    setupSliderRow(debounceThresholdLabel, debounceThresholdSlider, rightColumn);
    
    auto channelRow = rightColumn.removeFromTop(30);
    midiChannelLabel.setBounds(channelRow.removeFromLeft(100));
    midiChannelSelector.setBounds(channelRow.removeFromLeft(100));
    
    rightColumn.removeFromTop(10);
    discreteModeButton.setBounds(rightColumn.removeFromTop(30));
    
    rightColumn.removeFromTop(20);
    startStopButton.setBounds(rightColumn.removeFromTop(30));
} 