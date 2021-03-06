/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "../../GUI/StereoLoudnessBarGraph.h"
#include "../../GUI/StereoSpecificLoudnessGraph.h"
#include "../../GUI/SettingsButton.h"
#include "../../GUI/SettingsScreen.h"
#include "../../GUI/CalibrationScreen.h"
#include "../../GUI/LoudnessValueDisplay.h"


//==============================================================================
/**
*/
class LoudnessMeterAudioProcessorEditor  : public AudioProcessorEditor,
                                           private Timer,
                                           public Button::Listener
{
public:
    LoudnessMeterAudioProcessorEditor (LoudnessMeterAudioProcessor&);
    ~LoudnessMeterAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    void buttonClicked (Button *buttonThatWasClicked) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    LoudnessMeterAudioProcessor& processor;
    LoudnessValues *loudnessValues;
    
    Colour textColour;
    
    //graphs
    StereoLoudnessBarGraph barGraph;
    StereoSpecificLoudnessGraph specificLoudness;
    
    //Buttons
    SettingsButton settingsButton;
    SettingsButton calibrationButton;

    //Screens
    SettingsScreen settingsScreen;
    CalibrationScreen calibrationScreen;
    int screenHeight;

    //labels for displays loudness values
    LoudnessValueDisplay displayForLTL, displayForPeakSTL, displayForSPL;

    LoudnessParameters oldLoudnessParameters;
    
    ComponentAnimator animator;
    void showSettings (Component& screenToShow);
    void hideSettings (Component& screenToHide);
    
    void timerCallback();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LoudnessMeterAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
