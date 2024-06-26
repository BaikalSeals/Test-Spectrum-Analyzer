
#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
AudioPluginComponent::AudioPluginComponent (AudioPluginAudioProcessor& p) : processorRef (p), 
//We now need to set up the "sliders" (in this case knobs), for taking the input from the processor
highCutFreqAttachment(processorRef.apvts, "HighCut Freq", highCutFreq),
highCutShelfAttachment(processorRef.apvts, "HighCut Slope", highCutShelf),
lowCutFreqAttachment(processorRef.apvts, "LowCut Freq", lowCutFreq), 
lowCutShelfAttachment(processorRef.apvts, "LowCut Slope", lowCutShelf), 
EQ1FreqAttachment(processorRef.apvts, "Peak1 Freq", EQ1Freq), 
EQ1GainAttachment(processorRef.apvts, "Peak1 Gain", EQ1Gain), 
EQ1QualityAttachment(processorRef.apvts, "Peak1 Quality", EQ1Quality),
EQ2FreqAttachment(processorRef.apvts, "Peak2 Freq", EQ2Freq), 
EQ2GainAttachment(processorRef.apvts, "Peak2 Gain", EQ2Gain), 
EQ2QualityAttachment(processorRef.apvts, "Peak2 Quality", EQ2Quality)
{
    juce::ignoreUnused (processorRef);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addAndMakeVisible(verticalMeterL); 
    addAndMakeVisible(verticalMeterR);
    //The knob is set to be made visible 
    //(Note that you cannot set bounds at resize unless this is written here)
    addAndMakeVisible(lowCutFreq); 
    addAndMakeVisible(lowCutShelf); 
    addAndMakeVisible(highCutFreq); 
    addAndMakeVisible(highCutShelf);
    addAndMakeVisible(EQ1Freq); 
    addAndMakeVisible(EQ1Gain); 
    addAndMakeVisible(EQ1Quality); 
    addAndMakeVisible(EQ2Freq); 
    addAndMakeVisible(EQ2Gain); 
    addAndMakeVisible(EQ2Quality); 

    processorRef.apvts.addParameterListener("Peak1 Gain", this); 
    processorRef.apvts.addParameterListener("Peak1 Quality", this); 
    processorRef.apvts.addParameterListener("Peak1 Freq", this); 
    processorRef.apvts.addParameterListener("Peak2 Gain", this); 
    processorRef.apvts.addParameterListener("Peak2 Quality", this); 
    processorRef.apvts.addParameterListener("Peak2 Freq", this); 
    processorRef.apvts.addParameterListener("HighCut Slope", this); 
    processorRef.apvts.addParameterListener("HighCut Freq", this); 
    processorRef.apvts.addParameterListener("LowCut Slope", this); 
    processorRef.apvts.addParameterListener("LowCut Freq", this); 

    auto chainSettings = getChainSettings(processorRef.apvts); 
    Starter(); 


    //Here we have startTimer in the constructor 
    //startTimer() accepts time in ms and startTimerHz accepts inverse of that 
    //Here we want to render it 30 times a second 
    startTimerHz(30); 
}

AudioPluginComponent::~AudioPluginComponent()
{
}

void AudioPluginComponent::timerCallback(){
  //Here the usage of processorRef is calling on the AudioProcessorEditor the processorRef is a quick
  //way to access the audioprocessoreditor object
  verticalMeterL.setLevel(processorRef.getRmsValue(0)); 
  verticalMeterR.setLevel(processorRef.getRmsValue(1)); 
  verticalMeterL.setPeak(processorRef.getPeakValue(0)); 
  verticalMeterR.setPeak(processorRef.getPeakValue(1));

  verticalMeterL.repaint(); 
  verticalMeterR.repaint(); 
}

//==============================================================================
void AudioPluginComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    ColourGradient background(
        Colour(static_cast<juce::uint8>(60),static_cast<juce::uint8>(60),static_cast<juce::uint8>(60)),260.f,0.f,
        Colour(static_cast<juce::uint8>(20),static_cast<juce::uint8>(20),static_cast<juce::uint8>(20)),400.f,400.f,
        true 
    ); 
    
    g.setGradientFill(background); 
    g.fillAll(); 

    auto bounds = getLocalBounds(); 
    auto responseArea = bounds.removeFromLeft(50); 
    responseArea = bounds.removeFromRight(50); 
    responseArea = bounds.removeFromTop(5);
    responseArea = bounds.removeFromTop((int)(bounds.getHeight()*0.40)); 

    auto w = responseArea.getWidth(); 

    auto& lowcut = monoChain.get<ChainPositions::LowCut>(); 
    auto& peak1 = monoChain.get<ChainPositions::Peak1>(); 
    auto& peak2 = monoChain.get<ChainPositions::Peak2>(); 
    auto& highcut = monoChain.get<ChainPositions::HighCut>(); 

    auto sampleRate = processorRef.getSampleRate(); 

    std::vector<double> mags; 

    mags.resize(w); 

    for(int i = 0; i< w; i++){
        double mag = 1.f; 

        auto freq = mapToLog10(double(i)/double(w), 20.0, 20000.0); 

        if(!monoChain.isBypassed<ChainPositions::Peak1>())
            mag *= peak1.coefficients->getMagnitudeForFrequency(freq, sampleRate); 
        if(!monoChain.isBypassed<ChainPositions::Peak2>())
            mag *= peak2.coefficients->getMagnitudeForFrequency(freq, sampleRate); 
        
        if(!lowcut.isBypassed<0>())
            mag *= lowcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate); 
        if(!lowcut.isBypassed<1>())
            mag *= lowcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate); 
        if(!lowcut.isBypassed<2>())
            mag *= lowcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate); 
        if(!lowcut.isBypassed<3>())
            mag *= lowcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate); 
 
        if(!highcut.isBypassed<0>())
            mag *= highcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate); 
        if(!highcut.isBypassed<1>())
            mag *= highcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate); 
        if(!highcut.isBypassed<2>())
            mag *= highcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate); 
        if(!highcut.isBypassed<3>())
            mag *= highcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate); 

        (Decibels::gainToDecibels(mag) < -24.0) ? mag = -24.0 : mag = Decibels::gainToDecibels(mag); 
        mags[i] = mag; 
    }

    Path responseCurve; 

    const double outputMin = responseArea.getBottom(); 
    const double outputMax = responseArea.getY(); 
    auto map = [outputMin, outputMax](double input){
        return jmap(input, -24.0, 24.0, outputMin, outputMax);
    }; 

    responseCurve.startNewSubPath((float)(responseArea.getX()), (float)(map(mags.front()))); 
    for( size_t i = 1; i < mags.size(); i++){
        responseCurve.lineTo((float)(responseArea.getX() + i), (float)(map(mags[i]))); 
    }

    g.setColour(Colour(187, 255, 0));
    g.strokePath(responseCurve, PathStrokeType(2.f)); 

    g.setColour(Colour(0, 255, 222));
    g.drawRoundedRectangle(responseArea.toFloat(), 4.f, 3.f); 

    g.setColour (juce::Colours::lightgreen);
    g.setFont (15.0f);
    auto sealTextBounds = getLocalBounds(); 
    sealTextBounds.removeFromBottom((int)(0.9*sealTextBounds.getY())); 
    sealTextBounds.removeFromLeft(600); 
    g.drawFittedText ("Baikal Seals!\nThey're here!", sealTextBounds, juce::Justification::centred, 1);
}

void AudioPluginComponent::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    //
    //for setbounds the first two arguments are the x and y pixel position and the 3rd argument is 
    //the width and then the last argument is the height
    verticalMeterL.setBounds(30, 10, 15, 380); 
    verticalMeterR.setBounds(10,10, 15, 380); 

    //First we set the Custom Look and feel object to green indicated by 0
    green.setLook(0); 
    blue.setLook(1); 
    //Next the knob1 is set to the green object (which in this case is green)
    highCutFreq.setLook(green); 
    highCutShelf.setLook(green); 
    lowCutFreq.setLook(blue); 
    lowCutShelf.setLook(blue); 
    EQ1Freq.setLook(blue); 
    EQ1Gain.setLook(blue); 
    EQ1Quality.setLook(blue); 
    EQ2Freq.setLook(green); 
    EQ2Gain.setLook(green); 
    EQ2Quality.setLook(green); 
    //Next the bounds are set
    //The bounds for the knobs need to have a 4:5 ration of 
    //width to height
    highCutShelf.setBounds(70, 325, 60,75);
    lowCutShelf.setBounds(70,250, 60,75);
    highCutFreq.setBounds(130,325,60,75);
    lowCutFreq.setBounds(130,250,60,75);

    //bounds for EQ1 knobs
    EQ1Freq.setBounds(300, 350, 40, 50); 
    EQ1Gain.setBounds(340, 350, 40, 50); 
    EQ1Quality.setBounds(380, 350, 40, 50); 

    //bounds for EQ2 knobs
    EQ2Freq.setBounds(300, 300, 40, 50); 
    EQ2Gain.setBounds(340, 300, 40, 50); 
    EQ2Quality.setBounds(380, 300, 40, 50); 

}

//Wrapper constructor
WrappedAudioPluginAudioProcessorEditor::WrappedAudioPluginAudioProcessorEditor(AudioPluginAudioProcessor& p) :
AudioProcessorEditor(p), 
audioPluginComponent(p) {
    //Adds and makes visible the audio plugin component similar to how this works with other components
    addAndMakeVisible(audioPluginComponent);
    //This is where a lot of the functionality of the wrapper comes from 
    //This function returns a pointer to a bounds constrainer
    if(auto* constraints = getConstrainer()){
        //Sets a fixed aspect ratio
        constraints->setFixedAspectRatio(static_cast<double>(originalWidth)/static_cast<double> (originalHeight));
        //Sets minimum size in terms of original sizes and then maximum size in terms of original size
        constraints->setSizeLimits(originalWidth/2, originalHeight/2, originalWidth*4, originalHeight*4); 
    }
    
    //Now we need to set resizable to true
    setResizable(true, true); 
    //Now we set size to be the original width and height when created
    setSize(originalWidth, originalHeight); 

}

//Now the function that allows for resizing must be written
void WrappedAudioPluginAudioProcessorEditor::resized(){
    //Here we set the scalefactor to be constant so that the width and height get adjusted
    //at the same time when the screen size is changed
    const auto scaleFactor = static_cast<float>(getWidth()) / originalWidth; 
    //Now a transform is applied using the scale factor 
    audioPluginComponent.setTransform(AffineTransform::scale(scaleFactor)); 
    //Sets the bounds of the audioplugin component. We set the bounds of the original width 
    //and height so that it is always full sized, but the scale scales it accordingly
    audioPluginComponent.setBounds(0,0, originalWidth, originalHeight); 
}