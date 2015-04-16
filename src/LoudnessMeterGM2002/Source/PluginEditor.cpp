/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LoudnessMeterAudioProcessorEditor::LoudnessMeterAudioProcessorEditor (LoudnessMeterAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p),
      loudnessValues (processor.getPointerToLoudnessValues())
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    addAndMakeVisible (barGraph);
    barGraph.setGraduationColour (Colours::black);
    barGraph.setPhonsRange (20, 80);
    barGraph.setBounds (300, 20, 80, 260);
    
    addAndMakeVisible (specificLoudness);
    specificLoudness.setGraduationColour (Colours::black);
    specificLoudness.setBounds (10, 20, 280, 260);
    
    startTimer (50);
}

LoudnessMeterAudioProcessorEditor::~LoudnessMeterAudioProcessorEditor()
{
}

//==============================================================================
void LoudnessMeterAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::hotpink);
}

void LoudnessMeterAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void LoudnessMeterAudioProcessorEditor::timerCallback()
{
    if (processor.loudnessValuesReady())
    {
        barGraph.setMeterLevels (loudnessValues->leftSTL, loudnessValues->leftLTL, loudnessValues->rightSTL, loudnessValues->rightLTL);
        specificLoudness.setSpecificLoudnessValues (loudnessValues->centreFrequencies, loudnessValues->leftSpecificLoudness, loudnessValues->rightSpecificLoudness);
        
        processor.updateLoudnessValues();
    }
}
