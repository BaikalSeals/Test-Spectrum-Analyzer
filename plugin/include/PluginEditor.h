#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Components/VerticalMeter.h"
#include "Components/KnobLook.h"
//#include "../Assets/BinaryData.h"
#include "Components/Knobs.h"

//==============================================================================
//In order for the wrapper to work the class originally was structured as 
//class AudioPluginAudioProcessorEditor  : public juce::AudioProcessorEditor, public Timer
//but now we remove it as a child class of the audio processor editor class and have it as a
//child of the regular component class. We also renamed the class from AudioPluginAudioProcessorEditor
//to AudioPluginComponent since that is what it is now
class AudioPluginComponent  : public Component, public Timer
{
public:
    AudioPluginComponent (AudioPluginAudioProcessor&);
    ~AudioPluginComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    //We add this function in order to be able to refresh the horizontal meters
    void timerCallback() override; 

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    Gui::VerticalMeter verticalMeterL, verticalMeterR;
    //Creating the two look and feel of the knobs
    TTCOLook green;
    TTCOLook blue; 

    //Creating the knobs
    TTCOKnob 
    highCutFreq,
    lowCutFreq,
    lowCutShelf, 
    highCutShelf, 
    EQ1Freq,
    EQ1Gain, 
    EQ1Quality, 
    EQ2Freq,
    EQ2Gain, 
    EQ2Quality; 

    //
    using APVTS = juce::AudioProcessorValueTreeState; 
    using Attachment = APVTS::SliderAttachment; 

    Attachment highCutFreqAttachment,
    lowCutFreqAttachment,
    lowCutShelfAttachment, 
    highCutShelfAttachment, 
    EQ1FreqAttachment,
    EQ1GainAttachment, 
    EQ1QualityAttachment, 
    EQ2FreqAttachment,
    EQ2GainAttachment, 
    EQ2QualityAttachment; 


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginComponent)
};

//Root wrapper

class WrappedAudioPluginAudioProcessorEditor : public AudioProcessorEditor{
    public: 
        WrappedAudioPluginAudioProcessorEditor(AudioPluginAudioProcessor&); 
        void resized() override; 
    private: 
        //The only child component this should have is the previous audio processor editor
        AudioPluginComponent audioPluginComponent; 
        //Set original width and height of plugin. Here is where you will put the
        //size from the size function when you are changing the plugin
        //This can also be determined by size of assets. For instance, if you had an asset
        //like a  png that was the background of your plugin, then you'd set the width and
        //height here of the png
        static constexpr int originalWidth{700}; 
        static constexpr int originalHeight{400}; 

}; 