/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "../../loudnessCode/support/AuditoryTools.h"

//==============================================================================
LoudnessMeterAudioProcessor::LoudnessMeterAudioProcessor()
    : model(),
      inputSignalBank(),
      pluginInitialised (false),
      copyLoudnessValues (1),
      settingsFlag (OkToDoStuff)
{
    loudnessParameters.modelRate = 250;
    loudnessParameters.camSpacing = 0.5;
    loudnessParameters.compression = 0.3;
    loudnessParameters.filter = "";
}

LoudnessMeterAudioProcessor::~LoudnessMeterAudioProcessor()
{
}

//==============================================================================
const String LoudnessMeterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int LoudnessMeterAudioProcessor::getNumParameters()
{
    return 0;
}

float LoudnessMeterAudioProcessor::getParameter (int index)
{
    return 0.0f;
}

void LoudnessMeterAudioProcessor::setParameter (int index, float newValue)
{
}

const String LoudnessMeterAudioProcessor::getParameterName (int index)
{
    return String();
}

const String LoudnessMeterAudioProcessor::getParameterText (int index)
{
    return String();
}

const String LoudnessMeterAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String LoudnessMeterAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool LoudnessMeterAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool LoudnessMeterAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool LoudnessMeterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool LoudnessMeterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool LoudnessMeterAudioProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

double LoudnessMeterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int LoudnessMeterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int LoudnessMeterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void LoudnessMeterAudioProcessor::setCurrentProgram (int index)
{
}

const String LoudnessMeterAudioProcessor::getProgramName (int index)
{
    return String();
}

void LoudnessMeterAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void LoudnessMeterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    fs = sampleRate;
    
    Logger::outputDebugString ("prepareToPlay: numInputChannels:" + String (getNumInputChannels()) + "\n");
    
    if(!pluginInitialised)//Ardour hack
    {
        initialiseLoudness (loudnessParameters);
    }
}

void LoudnessMeterAudioProcessor::initialiseLoudness (const LoudnessParameters &newLoudnessParameters)
{
    loudnessParameters = newLoudnessParameters;
    
    /*
    * Model configuration
    */

    //limit to two channels (ears)
    numEars = 2;

    //set up an input buffer with a default hop size of 4ms
    //use something larger for debugging, e.g. 32ms
    hopSize = static_cast <int> (round (fs / loudnessParameters.modelRate));
    samplesNeeded = hopSize;
    writePos = 0;

    //loudness model configuration and initialisation
    inputSignalBank.initialize (numEars, 1, hopSize, (int) fs);
    model.configureModelParameters ("recentAndFaster");
    model.setPresentationDiotic (false); //required for left and right output

    //should be faster
    model.setRate (loudnessParameters.modelRate);
    model.setFilterSpacingInCams (loudnessParameters.camSpacing);
    model.setCompressionCriterionInCams (loudnessParameters.compression);
    model.setExcitationPatternInterpolated (false);

    //initialise the model
    model.initialize (inputSignalBank);
    
    /*
    * Pointers to loudness measures
    */

    // STL
    const loudness::SignalBank* bank = &model.getOutputSignalBank ("ShortTermLoudness");
    pointerToSTLLeft = bank -> getSingleSampleReadPointer (0, 0);
    pointerToSTLRight = bank -> getSingleSampleReadPointer (1, 0);

    // LTL
    bank = &model.getOutputSignalBank ("LongTermLoudness");
    pointerToLTLLeft = bank -> getSingleSampleReadPointer (0, 0);
    pointerToLTLRight = bank -> getSingleSampleReadPointer (1, 0);

    // Specific loudness (loudness as a function of frequency)
    bank = &model.getOutputSignalBank ("SpecificLoudnessPattern");
    pointerToSpecificLeft = bank -> getSingleSampleReadPointer (0, 0);
    pointerToSpecificRight = bank -> getSingleSampleReadPointer (1, 0);
    
    numAuditoryChannels = bank->getNChannels();
    
    copyLoudnessValues.set (1);
    loudnessValues.leftSpecificLoudness.clear();
    loudnessValues.rightSpecificLoudness.clear();
    loudnessValues.centreFrequencies.clear();
    
    const loudness::Real* ptr = bank -> getCentreFreqsReadPointer(0);

    for (int i = 0; i < numAuditoryChannels; ++i)
    {
        loudnessValues.leftSpecificLoudness.add (0);
        loudnessValues.rightSpecificLoudness.add (0);
        loudnessValues.centreFrequencies.add (loudness::freqToCam (ptr[i]));
    }

    pluginInitialised = true;
}

void LoudnessMeterAudioProcessor::setLoudnessParameters (const LoudnessParameters &newLoudnessParameters)
{
    while (! settingsFlag.compareAndSetBool (NotOkToDoStuff, OkToDoStuff))
    {
        Thread::sleep (10);
    }

    initialiseLoudness (newLoudnessParameters);
    settingsFlag.set (OkToDoStuff);
}

LoudnessParameters LoudnessMeterAudioProcessor::getLoudnessParameters()
{
    return loudnessParameters;
}


void LoudnessMeterAudioProcessor::releaseResources()
{
    if(pluginInitialised)
    {
        pluginInitialised = false;
    }
}

void LoudnessMeterAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    // analyse audio if settings aren't being changed
    if (settingsFlag.compareAndSetBool (NotOkToDoStuff, OkToDoStuff))
    {
        /*
         * Buffer -> SignalBank -> Model
         */

        //Logger::outputDebugString("processBlock: numInputChannels:" + String (getNumInputChannels()) + "\n");

        int numSamples = buffer.getNumSamples();
        int numInputChannels = buffer.getNumChannels();
        int remainingSamples = numSamples;
        int readPos = 0;

        // deal with any samples in the input which will create full hop buffers for us
        while (remainingSamples >= samplesNeeded)
        {
            //fill the SignalBank to be processed
            for (int ear = 0; ear < numEars; ++ear)
            {
                int idx = ear;
                if (numInputChannels == 1)
                    idx = 0;
                const float* signalToCopy = buffer.getReadPointer (idx);
                inputSignalBank.copySamples (ear, 0, writePos, signalToCopy + readPos, samplesNeeded);
            }

            //model process call
            model.process (inputSignalBank);

            remainingSamples -= samplesNeeded;
            readPos += samplesNeeded;
            samplesNeeded = hopSize;
            writePos = 0;
        }

        // grab any samples we need to save for the next processBlock call
        if (remainingSamples != 0)
        {
            for (int ear = 0; ear < numEars; ++ear)
            {
                int idx = ear;
                if (numInputChannels == 1)
                    idx = 0;
                const float* signalToCopy = buffer.getReadPointer (ear);
                inputSignalBank.copySamples (ear, 0, writePos, signalToCopy + readPos, samplesNeeded);
            }

            samplesNeeded -= remainingSamples;
            writePos += remainingSamples;
        }

        /*
         * Output
         */
        //Logger::outputDebugString("processBlock: STL (left):" + String (*pointerToSTLLeft) + "\n");
        //Logger::outputDebugString("processBlock: STL (right):" + String (*pointerToSTLRight) + "\n");
        
        if (copyLoudnessValues.get() == 1)
        {
            loudnessValues.leftSTL = loudness::soneToPhonMGB1997 (*pointerToSTLLeft, true);
            loudnessValues.rightSTL = loudness::soneToPhonMGB1997 (*pointerToSTLRight, true);
            loudnessValues.leftLTL = loudness::soneToPhonMGB1997 (*pointerToLTLLeft, true);
            loudnessValues.rightLTL = loudness::soneToPhonMGB1997 (*pointerToLTLRight, true);
            
            for (int i = 0; i < numAuditoryChannels; ++i)
            {
                //haven't made my mind up what to do with these yet...
                loudnessValues.leftSpecificLoudness.set (i, pointerToSpecificLeft [i]);
                loudnessValues.rightSpecificLoudness.set (i, pointerToSpecificRight [i]);
            }
                    
            copyLoudnessValues.set (0);
        }

        settingsFlag.set (OkToDoStuff);
    }
}

//==============================================================================
bool LoudnessMeterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* LoudnessMeterAudioProcessor::createEditor()
{
    return new LoudnessMeterAudioProcessorEditor (*this);
}

//==============================================================================
void LoudnessMeterAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void LoudnessMeterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
bool LoudnessMeterAudioProcessor::loudnessValuesReady()
{
    return (copyLoudnessValues.get() == 0);
}

void LoudnessMeterAudioProcessor::updateLoudnessValues()
{
    copyLoudnessValues.set (1);
}

LoudnessValues* LoudnessMeterAudioProcessor::getPointerToLoudnessValues()
{
    return &loudnessValues;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LoudnessMeterAudioProcessor();
}
