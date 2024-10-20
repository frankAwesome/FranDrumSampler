/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class FranDrumSamplerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    FranDrumSamplerAudioProcessorEditor (FranDrumSamplerAudioProcessor&);
    ~FranDrumSamplerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    FranDrumSamplerAudioProcessor& audioProcessor;

    juce::MidiKeyboardState keyboardState;
    juce::MidiKeyboardComponent midiKeyboard; 

    juce::Image backgroundImage;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FranDrumSamplerAudioProcessorEditor)
};
