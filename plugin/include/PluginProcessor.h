
#pragma once
 
#include <JuceHeader.h>


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

    

private:
    //Gets rms levels for the volume meter
    float rmsLevelLeft, rmsLevelRight; 
    //values for peak value 
    float peakLeft, peakRight; 

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

    //In order to do stereo we need two instances of this mono chain
    MonoChain leftChain, rightChain; 

    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
