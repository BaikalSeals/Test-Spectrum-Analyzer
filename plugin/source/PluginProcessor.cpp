#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
  // Use this method as the place to do any pre-playback
  // initialisation that you need..
  juce::ignoreUnused (sampleRate, samplesPerBlock);

  //We have to prepare the filters before they can be used
  //A process spec object is passed to the chains which is then passed to 
  //each link in the chain
  juce::dsp::ProcessSpec spec; 
  //The object needs to know max number of samples per block, the number of
  //channels being handled and the sample rate
  spec.maximumBlockSize = samplesPerBlock; 
  spec.numChannels = 1; 
  spec.sampleRate = sampleRate; 

  //Using the spec object for to prepare the chains
  leftChain.prepare(spec);
  rightChain.prepare(spec); 

  auto chainSettings = getChainSettings(apvts); 

  updatePeakFilter(chainSettings); 

  //Setting up the low and high cut filter coefficients
  //These coefficients are set by determining the slope enum
  auto lowcutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(
    chainSettings.lowCutFreq, sampleRate, (chainSettings.lowCutSlope +1)*2
  ); 

  auto highcutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(
    chainSettings.lowCutFreq, sampleRate, (chainSettings.lowCutSlope +1)*2
  ); 

  //First a variable telling what filter the chain is set to, in this case low cut and high cut
  //We set the variable to then be used in the updateCutFilter function 
  auto& leftLowCut = leftChain.get<ChainPositions::LowCut>(); 
  updateCutFilter(leftLowCut, lowcutCoefficients, chainSettings.lowCutSlope); 

  auto& rightLowCut = rightChain.get<ChainPositions::LowCut>(); 
  updateCutFilter(rightLowCut, lowcutCoefficients, chainSettings.lowCutSlope); 

  auto& leftHighCut = leftChain.get<ChainPositions::HighCut>(); 
  updateCutFilter(leftHighCut, highcutCoefficients, chainSettings.highCutSlope); 

  auto& rightHighCut = rightChain.get<ChainPositions::HighCut>(); 
  updateCutFilter(rightHighCut, highcutCoefficients, chainSettings.highCutSlope); 
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
//Removed the variable for juce::MidiBuffer type. This is because we can 
//remove variable names in a function if those variables are not being used 
//This keeps there from being an error compiling on windows (unused parameter error)
void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& )
{
  juce::ScopedNoDenormals noDenormals; 

  //Gets the rms levels using the getRmsLevel function 
  rmsLevelLeft = Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples())); 
  rmsLevelRight = Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));
  
  //Gets the peak levels using getMagnitude 
  peakLeft = Decibels::gainToDecibels(buffer.getMagnitude(0, 0, buffer.getNumSamples())); 
  peakRight = Decibels::gainToDecibels(buffer.getMagnitude(1, 0, buffer.getNumSamples())); 
 
  //This is necessary set up for manipulation of the audio signal 
  auto totalNumInputChannels = getTotalNumInputChannels(); 
  auto totalNumOutputChannels = getTotalNumOutputChannels(); 

  for(auto i = totalNumInputChannels; i < totalNumOutputChannels; i++)
    buffer.clear (i, 0, buffer.getNumSamples()); 

  //Now in order to make use of the chains to have them processed
  //First create an audio block for the buffer
  juce::dsp::AudioBlock<float> block(buffer); 

  //Now we can extract individual channels from the bugger with getSingleChannelBlock
  //function
  auto leftBlock = block.getSingleChannelBlock(0); 
  auto rightBlock = block.getSingleChannelBlock(1); 
  
  //Now we create processing contexts that wrap each individual block so that the
  //chain can use it
  juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock); 
  juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

  //Now we pass these contexts to our mono filter chains
  leftChain.process(leftContext); 
  rightChain.process(rightContext); 

  auto chainSettings = getChainSettings(apvts); 

  updatePeakFilter(chainSettings); 

  
  auto lowcutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(
    chainSettings.lowCutFreq, getSampleRate(), (chainSettings.lowCutSlope +1)*2
  ); 

  auto highcutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
    chainSettings.highCutFreq, getSampleRate(), (chainSettings.highCutSlope + 1)*2
  ); 

  auto& leftLowCut = leftChain.get<ChainPositions::LowCut>(); 
  updateCutFilter(leftLowCut, lowcutCoefficients, chainSettings.lowCutSlope); 

  auto& rightLowCut = rightChain.get<ChainPositions::LowCut>(); 
  updateCutFilter(rightLowCut, lowcutCoefficients, chainSettings.lowCutSlope); 

  auto& leftHighCut = leftChain.get<ChainPositions::HighCut>(); 
  updateCutFilter(leftHighCut, highcutCoefficients, chainSettings.highCutSlope); 

  auto& rightHighCut = rightChain.get<ChainPositions::HighCut>(); 
  updateCutFilter(rightHighCut, highcutCoefficients, chainSettings.highCutSlope); 


}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}



juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new WrappedAudioPluginAudioProcessorEditor (*this);
  //return new juce::GenericAudioProcessorEditor(*this); 
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    //Here we get the state information for the filters (highcut, lowcut,peak)
    //If there is info saved, then it will set the parameters to how they were
    //last time the plugin was opened
    juce::MemoryOutputStream mos(destData, true); 
    apvts.state.writeToStream(mos); 

    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    auto tree = juce::ValueTree::readFromData(data, sizeInBytes); 
    if(tree.isValid()){
      apvts.replaceState(tree); 
      updateFilters(); 
    }

    juce::ignoreUnused (data, sizeInBytes);
}

//Added ==========================================================
//
//Gets Rms Value in order for the meter to display volume
float AudioPluginAudioProcessor::getRmsValue(const int channel) const {
  //Determines if it is channel 1 or 0 and returns the correct value
      jassert(channel == 0 || channel == 1); 
      if(channel == 0){
        return rmsLevelLeft; 
      }
      if(channel == 1){
        return rmsLevelRight; 
      }
      return 0.f; 
  }

//Gets true peak value for the meter to display true peak
float AudioPluginAudioProcessor::getPeakValue(const int channel) const {
  //Determines if it is channel 1 or 0 and returns the correct value
  jassert(channel == 0 || channel == 1); 
  if(channel == 0){
    return peakLeft; 
  }
  if(channel == 1){
    return peakRight; 
  }
  return 0.f; 
}

ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts){
  ChainSettings settings; 
  //We have to get raw paremeters since this is needed in real world values and
  //not in normalized values
  settings.lowCutFreq = apvts.getRawParameterValue("LowCut Freq")->load(); 
  settings.lowCutSlope = static_cast<Slope>(apvts.getRawParameterValue("LowCut Slope")->load()); 
  settings.highCutFreq = apvts.getRawParameterValue("HighCut Freq")->load(); 
  settings.highCutSlope = static_cast<Slope>(apvts.getRawParameterValue("HighCut Slope")->load()); 
  settings.peak1Freq = apvts.getRawParameterValue("Peak1 Freq")->load(); 
  settings.peak1GainInDecibels = apvts.getRawParameterValue("Peak1 Gain")->load(); 
  settings.peak1Quality = apvts.getRawParameterValue("Peak1 Quality")->load(); 
  settings.peak2Freq = apvts.getRawParameterValue("Peak2 Freq")->load(); 
  settings.peak2GainInDecibels = apvts.getRawParameterValue("Peak2 Gain")->load(); 
  settings.peak2Quality = apvts.getRawParameterValue("Peak2 Quality")->load(); 

  return settings; 
}

//Implementation of processor function
void AudioPluginAudioProcessor::updatePeakFilter(const ChainSettings &chainSettings){
  //First the coefficients are set using the peak gain and quality numbers found
  //in the chain settings variable. 
  auto peak1Coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
    getSampleRate(), chainSettings.peak1Freq, chainSettings.peak1Quality,
    juce::Decibels::decibelsToGain(chainSettings.peak1GainInDecibels)); 

  auto peak2Coefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
    getSampleRate(), chainSettings.peak2Freq, chainSettings.peak2Quality,
    juce::Decibels::decibelsToGain(chainSettings.peak2GainInDecibels)); 

  //Possible Error here. I went ahead and removed the * at the beginning of the line
  //and at the beginning of the ending variable. I looked into it and this should 
  //work and be better considering that JUCE has been updated so that you do not have
  //do allocate on the heap for coefficient objects during the audio callback. 
  //This means I have written it so that it is not dereferencing since it should no
  //longer have to do that. 

  //Now the left chain and right chain peak coefficients are set
  //We have to set this for both left and right since the chain is in mono otherwise
  //note that chain settings only applies to mono
  leftChain.get<ChainPositions::Peak1>().coefficients = peak1Coefficients;   //<==== POSSIBLE ERROR
  rightChain.get<ChainPositions::Peak1>().coefficients = peak1Coefficients; 
  leftChain.get<ChainPositions::Peak2>().coefficients = peak2Coefficients; 
  rightChain.get<ChainPositions::Peak2>().coefficients = peak2Coefficients; 
}


juce::AudioProcessorValueTreeState::ParameterLayout
AudioPluginAudioProcessor::createParameterLayout(){
  juce::AudioProcessorValueTreeState::ParameterLayout layout; 

  //Here we are creating the lowcut parameter and setting up things such as skew factor
  //we create a unique pointer of type juce audioparameter float
  //next we set the variable of this to its name, parameter name, the normalisable range which
  //sets the range of the hertz (in this case 20 to 20000) and then the value of steps when
  //the slider is moved is 1. Next is the skew factor. The closer the skew factor to 0, the 
  //more of the mouse movements are covering the lower part of the hertz range. Then we have
  //the default value at 20 since we do not want the parameter to do anything unless it is moved. 


  layout.add(std::make_unique<juce::AudioParameterFloat>(
    "LowCut Freq", "Lowcut Freq", juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),20.f)); 

  //We do all of what did above but for all the other parameters
  //The high cut
  layout.add(std::make_unique<juce::AudioParameterFloat>(
    "HighCut Freq", "Highcut Freq", juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),20000.f)); 

  //EQ nodes (three parts each)
  //EQ node 1: 
  layout.add(std::make_unique<juce::AudioParameterFloat>(
    "Peak1 Freq", "Peak1 Freq", juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),750.f)); 

  //Do not mess up this one unless you want your eardrums blown out. Ensure the rnage is only from
  //-24 to 24 on this one
  layout.add(std::make_unique<juce::AudioParameterFloat>(
    "Peak1 Gain", "Peak1 Gain", juce::NormalisableRange<float>(-24.f, 24.f, 0.1f, 1.f), 0.f)); 

  layout.add(std::make_unique<juce::AudioParameterFloat>(
    "Peak1 Quality", "Peak1 Quality", juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f),1.f)); 

  //EQ node 2: 
  layout.add(std::make_unique<juce::AudioParameterFloat>(
    "Peak2 Freq", "Peak2 Freq", juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),2750.f)); 

  layout.add(std::make_unique<juce::AudioParameterFloat>(
    "Peak2 Gain", "Peak2 Gain", juce::NormalisableRange<float>(-24.f, 24.f, 0.1f, 1.f), 0.f)); 

  layout.add(std::make_unique<juce::AudioParameterFloat>(
    "Peak2 Quality", "Peak2 Quality", juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f),1.f)); 

  //This part is for creating the choices for the low and high cut (how hard of a cut)
  juce::StringArray stringArray; 
  for(int i = 0; i<4; i++){
    juce::String str;
    str << (12+i*12); 
    str << " db/Oct"; 
    stringArray.add(str); 
  }

  layout.add(std::make_unique<juce::AudioParameterChoice>(
    "LowCut Slope", "LowCut Slope", stringArray, 0));
  layout.add(std::make_unique<juce::AudioParameterChoice>(
    "HighCut Slope", "HighCut Slope", stringArray, 0));

  return layout; 
}

void AudioPluginAudioProcessor::updateFilters(){
  auto chainSettings = getChainSettings(apvts); 

  updatePeakFilter(chainSettings); 
  auto& leftHighCut = leftChain.get<ChainPositions::HighCut>(); 
  auto& leftLowCut = leftChain.get<ChainPositions::HighCut>(); 
  auto& rightHighCut = rightChain.get<ChainPositions::HighCut>(); 
  auto& rightLowCut = rightChain.get<ChainPositions::HighCut>(); 

  auto lowcutCoefficients = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
    chainSettings.lowCutFreq, getSampleRate(), 2*(chainSettings.lowCutSlope +1)); 

  auto highcutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(
    chainSettings.highCutFreq, getSampleRate(), 2*(chainSettings.highCutSlope +1)); 

  updateCutFilter(leftLowCut, lowcutCoefficients, chainSettings.lowCutSlope); 
  updateCutFilter(leftHighCut, lowcutCoefficients, chainSettings.lowCutSlope); 
  updateCutFilter(rightLowCut, lowcutCoefficients, chainSettings.lowCutSlope); 
  updateCutFilter(rightHighCut, lowcutCoefficients, chainSettings.lowCutSlope); 
}


//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
