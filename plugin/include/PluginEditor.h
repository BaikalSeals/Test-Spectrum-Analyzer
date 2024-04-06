#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Components/VerticalMeter.h"

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