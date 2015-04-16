/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "StereoLoudnessBarGraph.h"


//==============================================================================
/**
*/
class LoudnessMeterAudioProcessorEditor  : public AudioProcessorEditor,
                                           private Timer
{
public:
    LoudnessMeterAudioProcessorEditor (LoudnessMeterAudioProcessor&);
    ~LoudnessMeterAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    LoudnessMeterAudioProcessor& processor;
    
    StereoLoudnessBarGraph barGraph;
    
    void timerCallback();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoudnessMeterAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
