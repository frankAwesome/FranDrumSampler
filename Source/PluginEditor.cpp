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
    midiKeyboard.setMidiChannel(1);

    backgroundImage = juce::ImageCache::getFromFile(juce::File("C:\/Users\/User\/Pictures\/3.png"));

    setSize(420, 180);
}

FranDrumSamplerAudioProcessorEditor::~FranDrumSamplerAudioProcessorEditor()
{
}

//==============================================================================
void FranDrumSamplerAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.drawImage(backgroundImage, 0, 0, getWidth(), 110, 0, 0, backgroundImage.getWidth(), backgroundImage.getHeight());
}

void FranDrumSamplerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto area = getLocalBounds();
    auto keyboardArea = area.removeFromBottom(70);
    midiKeyboard.setBounds(keyboardArea);
}
