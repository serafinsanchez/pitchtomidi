#pragma once

#include <JuceHeader.h>
#include "MainWindow.h"

class PitchToMidiApplication : public juce::JUCEApplication
{
public:
    PitchToMidiApplication() {}

    const juce::String getApplicationName() override { return "PitchToMidi"; }
    const juce::String getApplicationVersion() override { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override { return false; }

    void initialise(const juce::String& commandLine) override
    {
        mainWindow.reset(new MainWindow(getApplicationName()));
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted(const juce::String& commandLine) override {}

private:
    std::unique_ptr<MainWindow> mainWindow;
}; 