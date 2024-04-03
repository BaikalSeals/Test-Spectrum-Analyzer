
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addAndMakeVisible(verticalMeterL); 
    addAndMakeVisible(verticalMeterR); 
    setSize (700, 400);

    //Here we have startTimer in the constructor 
    //startTimer() accepts time in ms and startTimerHz accepts inverse of that 
    //Here we want to render it 24 times a second 
    startTimerHz(24); 
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

void AudioPluginAudioProcessorEditor::timerCallback(){
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
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
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

        // g.strokePath(vmOutline, PathStrokeType(2.f));   
        // DropShadow vmOutlineShadow(Colours::grey, 1, juce::Point(-3,3)); 
        // vmOutlineShadow.drawForPath(g, vmOutline); 

}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    //
    //for setbounds the first two arguments are the x and y pixel position and the 3rd argument is 
    //the width and then the last argument is the height
    verticalMeterL.setBounds(30, 190, 15, 200); 
    verticalMeterR.setBounds(10,190, 15, 200); 
}
