/*
  ==============================================================================

    PluginProcessor.cpp

    Copyright (C) 2022 Francois Decourcelle

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "../Builds/VisualStudio2022/UtilsFiles.h"

//==============================================================================
FranDrumSamplerAudioProcessor::FranDrumSamplerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        //.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
{
    // Setup voices
    for (int i = 0; i < mNumVoices; i++)
    {
        mSampledInstrument.addVoice(new juce::SamplerVoice());
    }

    // Setup sampled sound
    mSampledInstrument.clearSounds();
    juce::WavAudioFormat wavFormat;

    std::unique_ptr<juce::AudioFormatReader> audioReader(wavFormat.createReaderFor(createSamplesInputStream("C:/1.wav").release(), true));
    std::unique_ptr<juce::AudioFormatReader> audioReaderSnare(wavFormat.createReaderFor(createSamplesInputStream("C:/3.wav").release(), true));
    std::unique_ptr<juce::AudioFormatReader> audioReaderHat(wavFormat.createReaderFor(createSamplesInputStream("C:/4.wav").release(), true));
    

    juce::BigInteger allNotes;
    allNotes.setRange(60, 1, true);

    mSampledInstrument.clearSounds();
    mSampledInstrument.addSound(new juce::SamplerSound("vox",         // name
        *audioReader,  // source
        allNotes,      // midi notes range
        60,            // root midi note
        0.0,           // attack time (sec)
        0.1,           // release time (sec)
        2.0           // maximum sample length (sec)
    ));



    juce::BigInteger allNotesSnare;
    allNotesSnare.setRange(62, 1, true);

    //mSampledInstrument.clearSounds();
    mSampledInstrument.addSound(new juce::SamplerSound("vox2",         // name
        *audioReaderSnare,  // source
        allNotesSnare,      // midi notes range
        62,            // root midi note
        0.0,           // attack time (sec)
        0.1,           // release time (sec)
        2.0           // maximum sample length (sec)
    ));


    

    juce::BigInteger allNotesHat;
    allNotesHat.setRange(64, 1, true);

    //mSampledInstrument.clearSounds();
    mSampledInstrument.addSound(new juce::SamplerSound("vox3",         // name
        *audioReaderHat,  // source
        allNotesHat,      // midi notes range
        64,            // root midi note
        0.0,           // attack time (sec)
        0.1,           // release time (sec)
        2.0           // maximum sample length (sec)
    ));
}

FranDrumSamplerAudioProcessor::~FranDrumSamplerAudioProcessor()
{
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

void FranDrumSamplerAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String FranDrumSamplerAudioProcessor::getProgramName(int index)
{
    return {};
}

void FranDrumSamplerAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void FranDrumSamplerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    mSampledInstrument.setCurrentPlaybackSampleRate(sampleRate);
}

void FranDrumSamplerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FranDrumSamplerAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
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

    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);



    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    mSampledInstrument.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool FranDrumSamplerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FranDrumSamplerAudioProcessor::createEditor()
{
    return new FranDrumSamplerAudioProcessorEditor(*this);
}

//==============================================================================
void FranDrumSamplerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FranDrumSamplerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FranDrumSamplerAudioProcessor();
}
