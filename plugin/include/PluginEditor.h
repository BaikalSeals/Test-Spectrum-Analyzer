#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Components/VerticalMeter.h"
#include "Components/KnobLook.h"
#include "Components/Knobs.h"

//==============================================================================
//In order for the wrapper to work the class originally was structured as 
//class AudioPluginAudioProcessorEditor  : public juce::AudioProcessorEditor, public Timer
//but now we remove it as a child class of the audio processor editor class and have it as a
//child of the regular component class. We also renamed the class from AudioPluginAudioProcessorEditor
//to AudioPluginComponent since that is what it is now
class AudioPluginComponent  : public Component, public Timer, public AudioProcessorValueTreeState::Listener
{
public:
    AudioPluginComponent (AudioPluginAudioProcessor&);
    ~AudioPluginComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    //We add this function in order to be able to refresh the horizontal meters
    void timerCallback() override; 

    //This function might seem pointlessly redudant, but right now it is needed in order
    //to have the response curve drawn correctly when the plugin first starts up 
    //It's essentially the same as parameterChanged, but executes all sections
    void Starter(){
        auto chainSettings = getChainSettings(processorRef.apvts); 

        auto sampleRate = processorRef.getSampleRate(); 

        auto peak1Coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            sampleRate, chainSettings.peak1Freq, chainSettings.peak1Quality,
            juce::Decibels::decibelsToGain(chainSettings.peak1GainInDecibels)); 

            monoChain.get<ChainPositions::Peak1>().coefficients = peak1Coefficients; 

        peak1Coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            sampleRate, chainSettings.peak2Freq, chainSettings.peak2Quality,
            juce::Decibels::decibelsToGain(chainSettings.peak2GainInDecibels)); 

            monoChain.get<ChainPositions::Peak2>().coefficients = peak1Coefficients; 

        auto cutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(
            chainSettings.lowCutFreq, sampleRate, 2*(chainSettings.lowCutSlope +1));

            auto& cutFilter = monoChain.get<ChainPositions::LowCut>(); 

            cutFilter.setBypassed<0>(true); 
            cutFilter.setBypassed<1>(true); 
            cutFilter.setBypassed<2>(true); 
            cutFilter.setBypassed<3>(true); 

            switch(chainSettings.lowCutSlope){
                case Slope12:
                    cutFilter.get<0>().coefficients = cutCoefficients[0]; 
                    cutFilter.setBypassed<0>(false); 
                    break; 
                case Slope24:
                    cutFilter.get<0>().coefficients = cutCoefficients[0]; 
                    cutFilter.setBypassed<0>(false); 
                    cutFilter.get<1>().coefficients = cutCoefficients[1]; 
                    cutFilter.setBypassed<1>(false); 
                    break; 
                case Slope36:
                    cutFilter.get<0>().coefficients = cutCoefficients[0]; 
                    cutFilter.setBypassed<0>(false); 
                    cutFilter.get<1>().coefficients = cutCoefficients[1]; 
                    cutFilter.setBypassed<1>(false); 
                    cutFilter.get<2>().coefficients = cutCoefficients[2]; 
                    cutFilter.setBypassed<2>(false); 
                    break; 
                case Slope48:
                    cutFilter.get<0>().coefficients = cutCoefficients[0]; 
                    cutFilter.setBypassed<0>(false); 
                    cutFilter.get<1>().coefficients = cutCoefficients[1]; 
                    cutFilter.setBypassed<1>(false); 
                    cutFilter.get<2>().coefficients = cutCoefficients[2]; 
                    cutFilter.setBypassed<2>(false); 
                    cutFilter.get<3>().coefficients = cutCoefficients[3]; 
                    cutFilter.setBypassed<3>(false); 
                    break; 
            }
        
        cutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
            chainSettings.highCutFreq, sampleRate, 2*(chainSettings.highCutSlope +1));

            auto& cutFilter2 = monoChain.get<ChainPositions::HighCut>(); 

            cutFilter2.setBypassed<0>(true); 
            cutFilter2.setBypassed<1>(true); 
            cutFilter2.setBypassed<2>(true); 
            cutFilter2.setBypassed<3>(true); 

            switch(chainSettings.highCutSlope){
                case Slope12:
                    cutFilter2.get<0>().coefficients = cutCoefficients[0]; 
                    cutFilter2.setBypassed<0>(false); 
                    break; 
                case Slope24:
                    cutFilter2.get<0>().coefficients = cutCoefficients[0]; 
                    cutFilter2.setBypassed<0>(false); 
                    cutFilter2.get<1>().coefficients = cutCoefficients[1]; 
                    cutFilter2.setBypassed<1>(false); 
                    break; 
                case Slope36:
                    cutFilter2.get<0>().coefficients = cutCoefficients[0]; 
                    cutFilter2.setBypassed<0>(false); 
                    cutFilter2.get<1>().coefficients = cutCoefficients[1]; 
                    cutFilter2.setBypassed<1>(false); 
                    cutFilter2.get<2>().coefficients = cutCoefficients[2]; 
                    cutFilter2.setBypassed<2>(false); 
                    break; 
                case Slope48:
                    cutFilter2.get<0>().coefficients = cutCoefficients[0]; 
                    cutFilter2.setBypassed<0>(false); 
                    cutFilter2.get<1>().coefficients = cutCoefficients[1]; 
                    cutFilter2.setBypassed<1>(false); 
                    cutFilter2.get<2>().coefficients = cutCoefficients[2]; 
                    cutFilter2.setBypassed<2>(false); 
                    cutFilter2.get<3>().coefficients = cutCoefficients[3]; 
                    cutFilter2.setBypassed<3>(false); 
                    break; 
            }

    }; 

    void parameterChanged(const String& parameterID, float newValue) override {
        juce::ignoreUnused(newValue); 
        auto chainSettings = getChainSettings(processorRef.apvts); 
        if(parameterID == "Peak1 Gain"||"Peak1 Quality"||"Peak1 Freq"){
            auto peak1Coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            processorRef.getSampleRate(), chainSettings.peak1Freq, chainSettings.peak1Quality,
            juce::Decibels::decibelsToGain(chainSettings.peak1GainInDecibels)); 

            monoChain.get<ChainPositions::Peak1>().coefficients = peak1Coefficients; 
            repaint(); 
        }

        if(parameterID == "Peak2 Gain"||"Peak2 Quality"||"Peak2 Freq"){
            auto peak1Coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            processorRef.getSampleRate(), chainSettings.peak2Freq, chainSettings.peak2Quality,
            juce::Decibels::decibelsToGain(chainSettings.peak2GainInDecibels)); 

            monoChain.get<ChainPositions::Peak2>().coefficients = peak1Coefficients; 
            repaint(); 
        }

        if(parameterID == "LowCut Slope"||"LowCut Freq"){
            auto cutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(
            chainSettings.lowCutFreq, processorRef.getSampleRate(), 2*(chainSettings.lowCutSlope +1));

            auto& cutFilter = monoChain.get<ChainPositions::LowCut>(); 

            cutFilter.setBypassed<0>(true); 
            cutFilter.setBypassed<1>(true); 
            cutFilter.setBypassed<2>(true); 
            cutFilter.setBypassed<3>(true); 

            switch(chainSettings.lowCutSlope){
                case Slope12:
                    cutFilter.get<0>().coefficients = cutCoefficients[0]; 
                    cutFilter.setBypassed<0>(false); 
                    break; 
                case Slope24:
                    cutFilter.get<0>().coefficients = cutCoefficients[0]; 
                    cutFilter.setBypassed<0>(false); 
                    cutFilter.get<1>().coefficients = cutCoefficients[1]; 
                    cutFilter.setBypassed<1>(false); 
                    break; 
                case Slope36:
                    cutFilter.get<0>().coefficients = cutCoefficients[0]; 
                    cutFilter.setBypassed<0>(false); 
                    cutFilter.get<1>().coefficients = cutCoefficients[1]; 
                    cutFilter.setBypassed<1>(false); 
                    cutFilter.get<2>().coefficients = cutCoefficients[2]; 
                    cutFilter.setBypassed<2>(false); 
                    break; 
                case Slope48:
                    cutFilter.get<0>().coefficients = cutCoefficients[0]; 
                    cutFilter.setBypassed<0>(false); 
                    cutFilter.get<1>().coefficients = cutCoefficients[1]; 
                    cutFilter.setBypassed<1>(false); 
                    cutFilter.get<2>().coefficients = cutCoefficients[2]; 
                    cutFilter.setBypassed<2>(false); 
                    cutFilter.get<3>().coefficients = cutCoefficients[3]; 
                    cutFilter.setBypassed<3>(false); 
                    break; 
            }

            repaint();
        }

        if(parameterID == "HighCut Slope"||"HighCut Freq"){
            auto cutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
            chainSettings.highCutFreq, processorRef.getSampleRate(), 2*(chainSettings.highCutSlope +1));

            auto& cutFilter = monoChain.get<ChainPositions::HighCut>(); 

            cutFilter.setBypassed<0>(true); 
            cutFilter.setBypassed<1>(true); 
            cutFilter.setBypassed<2>(true); 
            cutFilter.setBypassed<3>(true); 

            switch(chainSettings.highCutSlope){
                case Slope12:
                    cutFilter.get<0>().coefficients = cutCoefficients[0]; 
                    cutFilter.setBypassed<0>(false); 
                    break; 
                case Slope24:
                    cutFilter.get<0>().coefficients = cutCoefficients[0]; 
                    cutFilter.setBypassed<0>(false); 
                    cutFilter.get<1>().coefficients = cutCoefficients[1]; 
                    cutFilter.setBypassed<1>(false); 
                    break; 
                case Slope36:
                    cutFilter.get<0>().coefficients = cutCoefficients[0]; 
                    cutFilter.setBypassed<0>(false); 
                    cutFilter.get<1>().coefficients = cutCoefficients[1]; 
                    cutFilter.setBypassed<1>(false); 
                    cutFilter.get<2>().coefficients = cutCoefficients[2]; 
                    cutFilter.setBypassed<2>(false); 
                    break; 
                case Slope48:
                    cutFilter.get<0>().coefficients = cutCoefficients[0]; 
                    cutFilter.setBypassed<0>(false); 
                    cutFilter.get<1>().coefficients = cutCoefficients[1]; 
                    cutFilter.setBypassed<1>(false); 
                    cutFilter.get<2>().coefficients = cutCoefficients[2]; 
                    cutFilter.setBypassed<2>(false); 
                    cutFilter.get<3>().coefficients = cutCoefficients[3]; 
                    cutFilter.setBypassed<3>(false); 
                    break; 
            }

            repaint();
        }
    }; 

private:

    //IMPORTANT! Any variable that is created will be destructed in
    //reverse order from the way it is written here. This means you
    //want to declare your attachment variables after their non attached
    //counter parts

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

    MonoChain monoChain; 

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