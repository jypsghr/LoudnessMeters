#ifndef CALIBRATION_SCREEN_H_INCLUDED
#define CALIBRATION_SCREEN_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

class CalibrationScreen : public Component,
                          public ComboBox::Listener,
                          public TextEditor::Listener
{
public:
    CalibrationScreen ();
    ~CalibrationScreen();

    void paint (Graphics&);
    void resized();

    TextButton submitButton, measureButton;

    void comboBoxChanged (ComboBox *boxThatChanged) override;
    void textEditorTextChanged (TextEditor& editorThatChanged) override;

    void updateBasedOnCalibrationMeasurement (int measurementChannel, 
                                              double userLevel,
                                              double calibrationLevel);

    void setCalibrationLevelsToUnity ();
    int getCurrentUserInputChannel() const;
    double getCurrentUserInputLevel() const;
    double getCalibrationLevel (int channel) const;

private:

    ComboBox channelSelector;
    TextEditor calibrationEditor, gainEditor;
    TextEditor::LengthAndCharacterRestriction calibrationEditorTextFilter;
    Label calibrationEditorLabel, gainEditorLabel;

    static const int boxHeight = 20, boxWidth = 80, boxY = 60, boxSpacing = 10;

    bool currentUserInputChannel;
    double userInputLevels[2], calibrationLevels[2];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CalibrationScreen)
};


#endif  // CALIBRATION_SCREEN_H_INCLUDED
