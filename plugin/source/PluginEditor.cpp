
#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
AudioPluginComponent::AudioPluginComponent (AudioPluginAudioProcessor& p) : processorRef (p)
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addAndMakeVisible(verticalMeterL); 
    addAndMakeVisible(verticalMeterR);

    //Here we have startTimer in the constructor 
    //startTimer() accepts time in ms and startTimerHz accepts inverse of that 
    //Here we want to render it 24 times a second 
    startTimerHz(24); 
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

    g.setColour (juce::Colours::lightgreen);
    g.setFont (15.0f);
    g.drawFittedText ("Baikal Seals!\nThey're here!", getLocalBounds(), juce::Justification::centred, 1);
}

void AudioPluginComponent::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    //
    //for setbounds the first two arguments are the x and y pixel position and the 3rd argument is 
    //the width and then the last argument is the height
    verticalMeterL.setBounds(30, 190, 15, 200); 
    verticalMeterR.setBounds(10,190, 15, 200); 
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