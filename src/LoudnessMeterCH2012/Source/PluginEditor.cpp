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
    : AudioProcessorEditor (&p),
      processor (p),
      loudnessValues (processor.getPointerToLoudnessValues()),
      settingsButton ("Settings"),
      calibrationButton ("Calibrate SPL"),
      settingsScreen (processor.getLoudnessParameters()),
      screenHeight (120),
      displayForLTL("Long Term", Colours::lightgrey, Colours::yellow),
      displayForPeakSTL("Peak", Colours::lightgrey, Colours::yellow),
      displayForSPL("SPL (Z)", Colours::lightgrey, Colours::yellow)
{
    int generalSpacing = 10;
    int windowWidth = 700;
    int windowHeight = 380;
    setSize (windowWidth, windowHeight);
   
    int goldenPoint = windowWidth / 1.618;
    int specLoudGraphWidth = goldenPoint - 1.5 * generalSpacing;
    int specLoudGraphHeight = 300;
    addAndMakeVisible (specificLoudness);
    specificLoudness.setGraduationColour (Colours::lightgrey);
    specificLoudness.setGraphBounds (generalSpacing, 
                                generalSpacing,
                                specLoudGraphWidth,
                                specLoudGraphHeight);

    int barX = goldenPoint + 0.5 * generalSpacing;
    int barWidth = (windowWidth - goldenPoint - 2.5 * generalSpacing) / 2;
    addAndMakeVisible (barGraph);
    barGraph.setGraduationColour (Colours::lightgrey);
    barGraph.setRange (0, 34);
    barGraph.setMeterBounds (barX,
                        generalSpacing,
                        barWidth, 
                        specLoudGraphHeight);

    int valueDisplayWidth = barWidth / 1.5;
    int valueDisplayHeight = valueDisplayWidth;
    int displayForLTLX = barX + barWidth 
                         + (windowWidth - (barX + barWidth)
                         - valueDisplayWidth)/2;
    addAndMakeVisible (displayForLTL);
    displayForLTL.setBounds (displayForLTLX, generalSpacing, 
                             valueDisplayWidth, valueDisplayHeight);

    addAndMakeVisible (displayForPeakSTL);
    displayForPeakSTL.setBounds (displayForLTLX, 
                                 valueDisplayHeight + generalSpacing * 2,
                                 valueDisplayWidth, valueDisplayHeight);

    addAndMakeVisible (displayForSPL);
    displayForSPL.setBounds (displayForLTLX, 
                             valueDisplayHeight * 2 + generalSpacing * 3,
                             valueDisplayWidth, valueDisplayHeight);

    int settingsWidth = valueDisplayWidth, settingsHeight = 20;
    int settingsX = displayForLTLX;
    int settingsY = specLoudGraphHeight + generalSpacing;
    addAndMakeVisible (settingsButton);
    settingsButton.setBounds (settingsX, settingsY,
                              settingsWidth, settingsHeight);
    settingsButton.addListener (this);

    int calibrationX = settingsX;
    int calibrationY = settingsY + settingsHeight + generalSpacing;
    addAndMakeVisible (calibrationButton);
    calibrationButton.setBounds (calibrationX,
                                 calibrationY, settingsWidth,
                                 settingsHeight);
    calibrationButton.addListener (this);
    
    addAndMakeVisible (settingsScreen);
    settingsScreen.setBounds (0, getHeight(), getWidth(), screenHeight);
    settingsScreen.submitButton.addListener (this);
    oldLoudnessParameters = settingsScreen.getLoudnessParameters();

    addAndMakeVisible (calibrationScreen);
    calibrationScreen.setBounds (0, getHeight(), getWidth(), screenHeight);
    calibrationScreen.submitButton.addListener (this);
    calibrationScreen.measureButton.addListener (this);
    
    startTimer (50);
}

LoudnessMeterAudioProcessorEditor::~LoudnessMeterAudioProcessorEditor()
{
}

//==============================================================================
void LoudnessMeterAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::darkolivegreen);
}

void LoudnessMeterAudioProcessorEditor::resized()
{
}

void LoudnessMeterAudioProcessorEditor::buttonClicked (Button *buttonThatWasClicked)
{
    if (buttonThatWasClicked == &settingsButton)
    {
        //Show the settings screen to allow model config
        showSettings(settingsScreen);
    }
    else if (buttonThatWasClicked == &(settingsScreen.submitButton))
    {
        hideSettings (settingsScreen);

        //Get the new params from GUI
        LoudnessParameters newLoudnessParameters = settingsScreen.getLoudnessParameters();

        //Update only if a parameter has changed
        if (newLoudnessParameters.modelRate != oldLoudnessParameters.modelRate ||
            newLoudnessParameters.camSpacing != oldLoudnessParameters.camSpacing ||
            newLoudnessParameters.compression != oldLoudnessParameters.compression ||
            newLoudnessParameters.filter != oldLoudnessParameters.filter)
        {
            processor.setLoudnessParameters (newLoudnessParameters);
            oldLoudnessParameters = newLoudnessParameters;
        }
    }
    else if (buttonThatWasClicked == &calibrationButton)
    {
        //show calibraton screen to allow calibration
        showSettings(calibrationScreen);
    }
    else if (buttonThatWasClicked == &(calibrationScreen.measureButton))
    {
        calibrationScreen.measureButton.setButtonText("Please wait...");
        processor.setMeasurementChannel (calibrationScreen.getCurrentUserInputChannel());
        processor.setMeasurementLevel (calibrationScreen.getCurrentUserInputLevel());
        processor.setStartCalibrationMeasurement (true);
    }
    else if (buttonThatWasClicked == &(calibrationScreen.submitButton))
    {
        processor.calibrate (calibrationScreen.getCalibrationLevel(0),
                             calibrationScreen.getCalibrationLevel(1));
        calibrationScreen.setCalibrationLevelsToUnity();
        hideSettings (calibrationScreen);
    }
}

void LoudnessMeterAudioProcessorEditor::timerCallback()
{
    if (processor.loudnessValuesReady())
    {
        //update bar graphs
        barGraph.setMeterLevels (loudnessValues->leftSTL,
                                 loudnessValues->leftSTL,
                                 loudnessValues->rightSTL,
                                 loudnessValues->rightSTL);

        //update specific loudness graph
        specificLoudness.setSpecificLoudnessValues (loudnessValues->centreFrequencies, 
                                                    loudnessValues->leftSpecificLoudness,
                                                    loudnessValues->rightSpecificLoudness);

        //update number displays
        displayForLTL.setValueToDisplay (loudnessValues->overallLTL, 1);
        displayForPeakSTL.setValueToDisplay (loudnessValues->overallPeakSTL, 1);
        displayForSPL.setValueToDisplay (loudnessValues->averageSPL, 1);
        processor.updateLoudnessValues();
    }

    if (processor.isCalibrationMeasurementNew())
    {
        calibrationScreen.measureButton.setButtonText("Measure");
        calibrationScreen.updateBasedOnCalibrationMeasurement (processor.getMeasurementChannel(),
                                                               processor.getMeasurementLevel(),
                                                               (float)processor.getNewCalibrationLevel());
        processor.setCalibrationMeasurementNew(false);
    }
}

void LoudnessMeterAudioProcessorEditor::showSettings (Component &screenToShow)
{
    settingsButton.setEnabled (false);
    calibrationButton.setEnabled (false);
    
    animator.animateComponent (&screenToShow,
                               Rectangle <int> (0, getHeight() - screenHeight, getWidth(), screenHeight),
                               1.0f, 1000, false, 0.0, 0.0);
}

void LoudnessMeterAudioProcessorEditor::hideSettings (Component &screenToHide)
{
    settingsButton.setEnabled (true);
    calibrationButton.setEnabled (true);
    
    animator.animateComponent (&screenToHide,
                               Rectangle <int> (0, getHeight(), getWidth(), screenHeight),
                               1.0f, 1000, false, 0.0, 0.0);
    repaint();
}
