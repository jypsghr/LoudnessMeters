/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "loudnessCode/support/SignalBank.h"
#include "loudnessCode/models/DynamicLoudnessGM2002.h"

//==============================================================================

class LoudnessMeterAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    LoudnessMeterAudioProcessor();
    ~LoudnessMeterAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    int getNumParameters() override;
    float getParameter (int index) override;
    void setParameter (int index, float newValue) override;

    const String getParameterName (int index) override;
    const String getParameterText (int index) override;

    const String getInputChannelName (int channelIndex) const override;
    const String getOutputChannelName (int channelIndex) const override;
    bool isInputChannelStereoPair (int index) const override;
    bool isOutputChannelStereoPair (int index) const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool silenceInProducesSilenceOut() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    int hopSize, numEars;
    int samplesNeeded, writePos;
    bool pluginInitialised;
    AudioSampleBuffer analysisBuffer;
    loudness::SignalBank inputSignalBank;
    loudness::DynamicLoudnessGM2002 model;
    const loudness::SignalBank* instantaneousLoudnessSignalBankPtr, *shortTermLoudnessSignalBankPtr;
    const loudness::SignalBank* longTermLoudnessSignalBankPtr, *specificLoudnessSignalBankPtr;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoudnessMeterAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED