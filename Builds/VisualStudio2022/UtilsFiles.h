/*
  ==============================================================================

    UtilsFiles.h

    Copyright (C) 2022 Francois Decourcelle

  ==============================================================================
*/

#pragma once
#include <memory>
#include <juce_core/juce_core.h>

inline std::unique_ptr<juce::InputStream> createSamplesInputStream(const char* resourcePath)
{
    auto samplesDir = juce::File::getSpecialLocation(juce::File::commonApplicationDataDirectory)
        .getChildFile("Application Support")
        .getChildFile("Ibekso")
        .getChildFile("ibkSampledInstrument")
        .getChildFile("Samples");

    auto resourceFile = samplesDir.getChildFile(resourcePath);

    jassert(resourceFile.existsAsFile());

    return resourceFile.createInputStream();
}
