/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FranDrumSamplerAudioProcessorEditor::FranDrumSamplerAudioProcessorEditor(FranDrumSamplerAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), midiKeyboard(p.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.


    addAndMakeVisible(midiKeyboard);
    setSize(600, 400);
}

FranDrumSamplerAudioProcessorEditor::~FranDrumSamplerAudioProcessorEditor()
{
}

//==============================================================================
void FranDrumSamplerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void FranDrumSamplerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto area = getLocalBounds();
    auto keyboardArea = area.removeFromBottom(100);
    midiKeyboard.setBounds(keyboardArea);
}
