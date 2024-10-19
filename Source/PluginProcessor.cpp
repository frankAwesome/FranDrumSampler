/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FranDrumSamplerAudioProcessor::FranDrumSamplerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    formatManager.registerBasicFormats();

    juce::String waveFilePath = "C:\/2.WAV";
    loadWaveFile(waveFilePath);
}

FranDrumSamplerAudioProcessor::~FranDrumSamplerAudioProcessor()
{
    transportSource.setSource(nullptr);
}

//==============================================================================
const juce::String FranDrumSamplerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FranDrumSamplerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FranDrumSamplerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FranDrumSamplerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FranDrumSamplerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FranDrumSamplerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FranDrumSamplerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FranDrumSamplerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String FranDrumSamplerAudioProcessor::getProgramName (int index)
{
    return {};
}

void FranDrumSamplerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FranDrumSamplerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    transportSource.prepareToPlay(samplesPerBlock, sampleRate);

}

void FranDrumSamplerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FranDrumSamplerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void FranDrumSamplerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // Clear the audio buffer if necessary
    buffer.clear();

    // Let the MidiKeyboardState process the MIDI buffer
    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);

    // Process incoming MIDI messages to trigger playback
    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();

        if (message.isNoteOn() && message.getNoteNumber() == 60) // MIDI note 60 is C3
        {
            //transportSource.stop();
            transportSource.setPosition(0.0); 
            transportSource.start();

            //if (!transportSource.isPlaying())
            //{
            //    transportSource.setPosition(0.0);
            //    transportSource.start();
            //}
        }
        //else if (message.isNoteOff() && message.getNoteNumber() == 60)
        //{
        //    // Stop playback when C3 is released
        //    transportSource.stop();
        //}
    }

    // Resize the internal buffer if needed
    if (internalBuffer.getNumSamples() != buffer.getNumSamples())
    {
        internalBuffer.setSize(buffer.getNumChannels(), buffer.getNumSamples(), false, true, true);
    }

    // Clear the internal buffer before processing
    internalBuffer.clear();

    // Provide the transport source with the audio block information using the internal buffer
    juce::AudioSourceChannelInfo channelInfo(internalBuffer);
    transportSource.getNextAudioBlock(channelInfo);

    // Copy from the internal buffer to the output buffer
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        buffer.addFrom(channel, 0, internalBuffer, channel, 0, buffer.getNumSamples());
    }
}




//==============================================================================
bool FranDrumSamplerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FranDrumSamplerAudioProcessor::createEditor()
{
    return new FranDrumSamplerAudioProcessorEditor (*this);
}

//==============================================================================
void FranDrumSamplerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FranDrumSamplerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void FranDrumSamplerAudioProcessor::loadWaveFile(const juce::File& file)
{
    auto* reader = formatManager.createReaderFor(file);

    // Check if reader is null or if the sample rate is zero
    if (reader == nullptr)
    {
        DBG("Failed to create reader for the file: " << file.getFullPathName());
        return; // Exit if the file cannot be read
    }

    if (reader->sampleRate <= 0.0)
    {
        DBG("Sample rate is zero or negative for file: " << file.getFullPathName());
        delete reader; // Clean up the reader if necessary
        return; // Exit if the sample rate is invalid
    }

    auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
    transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
    readerSource.reset(newSource.release());
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FranDrumSamplerAudioProcessor();
}
