#pragma once

#include <JuceHeader.h>

class MainComponent : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    // Audio device manager
    std::unique_ptr<juce::AudioDeviceManager> deviceManager;
    
    // Audio device selector
    std::unique_ptr<juce::AudioDeviceSelectorComponent> audioDeviceSelector;
    
    // MIDI output selector
    juce::ComboBox midiOutputSelector;
    juce::Label midiOutputLabel;
    
    // Status display
    juce::Label currentNoteLabel;
    juce::Label velocityLabel;
    juce::Label pitchDeviationLabel;
    
    // Settings controls
    juce::Slider amplitudeThresholdSlider;
    juce::Label amplitudeThresholdLabel;
    
    juce::Slider windowSizeSlider;
    juce::Label windowSizeLabel;
    
    juce::Slider hopSizeSlider;
    juce::Label hopSizeLabel;
    
    juce::Slider debounceThresholdSlider;
    juce::Label debounceThresholdLabel;
    
    juce::ComboBox midiChannelSelector;
    juce::Label midiChannelLabel;
    
    juce::ToggleButton discreteModeButton;
    juce::TextButton startStopButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
}; 