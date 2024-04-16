
#pragma once
 
#include <JuceHeader.h>

enum Slope{
    Slope12,
    Slope24,
    Slope36,
    Slope48
};

//We want to extract parameters from the audio processor value tree state
//A data structure is needed to keep this in order
struct ChainSettings{
    float peak1Freq {0}, peak1GainInDecibels{0}, peak1Quality {1.f};
    float peak2Freq {0}, peak2GainInDecibels{0}, peak2Quality {1.f}; 
    float lowCutFreq {0}, highCutFreq {0}; 
    Slope lowCutSlope {Slope::Slope12}, highCutSlope {Slope::Slope12};
}; 

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts); 

//Aliases created so that we do not have to deal with writing out all the
//nested namespaces
using Filter = juce::dsp::IIR::Filter<float>; 

//The IIR filter class has a response of 12db when it is configured as low or
//high pass filter. In order to get 48 db per octave we will need 4 of these filters.
//We can use 4 of these filters to pass through a processing chain in order to pass one
//context and have it run through 4 automatically. 
using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>; 

//This is a chain that represents the whole mono signal path. The first cutfilter is
//the low cut, then the Filter is 1st EQ (peak1), then the next filter is the 2nd EQ (peak2),
//and finally the last cutfilter is the highcut 
using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, Filter, CutFilter>; 

//enum for chain positions
enum ChainPositions{
    LowCut,
    Peak1,
    Peak2,
    HighCut
}; 

// using Coefficients = Filter::CoefficientsPtr; 
// void updateCoefficients(Coefficients& old, const Coefficients& replacements); 

// Coefficients makePeakFilter(const ChainSettings& chainSettings, double sampleRate); 

//==============================================================================
class AudioPluginAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //Added==========================================
    
    //rms values for the volume meter
    float getRmsValue(const int channel) const;
    
    //peak values for the volume meter 
    float getPeakValue(const int channel) const; 

    //This syncs parameters between the knobs on the gui and the parameters on the dsp
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout(); 

    juce::AudioProcessorValueTreeState apvts {
        *this, nullptr, "Parameters", createParameterLayout()}; 

    void updateCutFilter2(CutFilter& cut, const juce::ReferenceCountedArray<juce::dsp::IIR::Coefficients<float>> cutCoefficients, const Slope& slope); 

    
private:
    //Gets rms levels for the volume meter
    float rmsLevelLeft, rmsLevelRight; 
    //values for peak value for volume meter (not the EQ, just poor naming on my part)
    float peakLeft, peakRight; 

    //In order to do stereo we need two instances of this mono chain
    MonoChain leftChain, rightChain; 

    //Writing out functions for the filters
    //This will allow for more easily implementing updates to the peak filters
    void updatePeakFilter(const ChainSettings& chainSettings); 

    template<typename ChainType, typename CoefficientType>
    void updateCutFilter(ChainType& leftLowCut, 
    const CoefficientType& cutCoefficients, 
    const Slope& lowCutSlope){

        leftLowCut.template setBypassed<0>(true); 
        leftLowCut.template setBypassed<1>(true); 
        leftLowCut.template setBypassed<2>(true); 
        leftLowCut.template setBypassed<3>(true); 

        switch(lowCutSlope){
            case Slope12:
            leftLowCut.template get<0>().coefficients = cutCoefficients[0];
            leftLowCut.template setBypassed<0>(false); 
            break; 
            case Slope24:
            leftLowCut.template get<0>().coefficients = cutCoefficients[0];
            leftLowCut.template setBypassed<0>(false); 
            leftLowCut.template get<1>().coefficients = cutCoefficients[1];
            leftLowCut.template setBypassed<1>(false); 
            break;
            case Slope36:
            leftLowCut.template get<0>().coefficients = cutCoefficients[0];
            leftLowCut.template setBypassed<0>(false); 
            leftLowCut.template get<1>().coefficients = cutCoefficients[1];
            leftLowCut.template setBypassed<1>(false); 
            leftLowCut.template get<2>().coefficients = cutCoefficients[2];
            leftLowCut.template setBypassed<2>(false); 
            break;
            case Slope48:
            leftLowCut.template get<0>().coefficients = cutCoefficients[0];
            leftLowCut.template setBypassed<0>(false); 
            leftLowCut.template get<1>().coefficients = cutCoefficients[1];
            leftLowCut.template setBypassed<1>(false); 
            leftLowCut.template get<2>().coefficients = cutCoefficients[2];
            leftLowCut.template setBypassed<2>(false); 
            leftLowCut.template get<3>().coefficients = cutCoefficients[3];
            leftLowCut.template setBypassed<3>(false); 
            break; 
        }
    }


    //Set here as a way to update all filters at once
    //I could put this in the process block to cut down on reused code, but
    //since this is a test plugin to learn c++, juce and cmake, I want to leave
    //it as is to be able to go back and easily reference what it was I was doing
    //This function is here for being able to set the value tree state
    void updateFilters();
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
